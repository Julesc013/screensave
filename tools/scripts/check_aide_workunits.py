"""Validate the ScreenSave AIDE WorkUnit ledger and templates."""

from __future__ import annotations

import json
import pathlib
import re
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
WORK_UNITS_DIR = ROOT / ".aide" / "work_units"
LEDGER = WORK_UNITS_DIR / "index.toml"
TEMPLATE_DIR = WORK_UNITS_DIR / "templates"
WORKUNIT = ROOT / "tools" / "aideops" / "workunit.py"
REFERENCE_SOURCES = [
    ROOT / "PROJECT_STATE.toml",
    ROOT / "docs" / "roadmap",
]
WORK_UNIT_REF = re.compile(r"(?P<path>\.aide[\\/]+work_units[\\/]+[A-Za-z0-9_.\-]+\.toml)")

REQUIRED_FIELDS = {
    "id",
    "title",
    "task_class",
    "objective",
    "allowed_paths",
    "forbidden_paths",
    "selected_validation_tier",
    "expected_validators",
    "evidence_outputs",
    "claim_boundary",
    "rollback_point",
    "human_review_required",
    "agent_allowed",
    "agent_mode",
}
TASK_CLASSES = {"implementation", "repair", "review", "release-readiness", "agent-session"}
AGENT_MODES = {"none", "assistive", "proposal-only", "fixture-worker", "future-worker"}
ALLOWED_AGENT_MODES = {"assistive", "proposal-only"}
REQUIRED_TASKS = {f"SS-G{index}" for index in range(13)}
REQUIRED_PRODUCT_TASKS = {
    "SS-PLV2-IR0",
    "SS-PLV2-IR-REPAIR-001",
    "SS-PLV2-IR-REPAIR-002",
    "SS-PLV2-IR-REPAIR-003",
}
TEMPLATES = {
    "implementation-task.toml",
    "repair-task.toml",
    "review-task.toml",
    "release-readiness-task.toml",
    "agent-session-task.toml",
}


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def referenced_work_unit_paths() -> list[pathlib.Path]:
    paths: list[pathlib.Path] = []
    for source in REFERENCE_SOURCES:
        if source.is_file():
            candidates = [source]
        elif source.is_dir():
            candidates = list(source.glob("*.md"))
        else:
            candidates = []
        for candidate in candidates:
            text = candidate.read_text(encoding="utf-8")
            for match in WORK_UNIT_REF.finditer(text):
                paths.append(ROOT / match.group("path").replace("\\", "/"))
    return paths


def work_unit_files() -> list[pathlib.Path]:
    paths = [LEDGER]
    paths.extend(sorted(path for path in WORK_UNITS_DIR.glob("*.toml") if path.name != "index.toml"))
    paths.extend(referenced_work_unit_paths())
    observed: set[pathlib.Path] = set()
    unique: list[pathlib.Path] = []
    for path in paths:
        resolved = path.resolve()
        if resolved in observed:
            continue
        observed.add(resolved)
        unique.append(path)
    return unique


def packet_units(path: pathlib.Path) -> list[dict[str, Any]]:
    payload = load_toml(path)
    return [item for item in payload.get("work_units", []) if isinstance(item, dict)]


def validate_unit(unit: dict[str, Any], label: str, errors: list[str]) -> None:
    missing = REQUIRED_FIELDS - set(unit)
    require(not missing, f"{label} missing fields: {', '.join(sorted(missing))}", errors)
    if missing:
        return
    require(unit.get("task_class") in TASK_CLASSES, f"{label} task_class invalid.", errors)
    require(unit.get("agent_mode") in AGENT_MODES, f"{label} agent_mode invalid.", errors)
    require(unit.get("selected_validation_tier") in {"T0", "T1", "T2", "T3"}, f"{label} selected_validation_tier invalid.", errors)
    for key in ["allowed_paths", "forbidden_paths", "expected_validators", "evidence_outputs"]:
        require(isinstance(unit.get(key), list), f"{label} {key} must be a list.", errors)
    require(bool(unit.get("objective")), f"{label} objective must not be empty.", errors)
    require(bool(unit.get("claim_boundary")), f"{label} claim_boundary must not be empty.", errors)
    if unit.get("agent_allowed") is True:
        require(unit.get("agent_mode") in ALLOWED_AGENT_MODES, f"{label} may only allow assistive/proposal-only agents.", errors)
    else:
        require(unit.get("agent_mode") in {"none", "future-worker", "fixture-worker"}, f"{label} blocked agents must not use assistive modes.", errors)
    boundary = str(unit.get("claim_boundary", "")).lower()
    for forbidden in ["stable = true", "compatibility certified", "automatic merge allowed"]:
        require(forbidden not in boundary, f"{label} claim boundary contains forbidden overclaim: {forbidden}", errors)


def validate_cli(errors: list[str]) -> None:
    result = subprocess.run(
        [sys.executable, str(WORKUNIT), "list"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(result.returncode == 0, f"workunit.py list failed: {result.stderr}", errors)
    if result.returncode == 0:
        payload = json.loads(result.stdout)
        require(payload.get("count") >= 13, "workunit.py list must expose PAW-G WorkUnits.", errors)
        ids = {item.get("id") for item in payload.get("work_units", [])}
        require(REQUIRED_PRODUCT_TASKS <= ids, "workunit.py list must expose Plasma instrument repair WorkUnits.", errors)
    inspect = subprocess.run(
        [sys.executable, str(WORKUNIT), "inspect", "--task", "SS-PLV2-IR-REPAIR-001"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(inspect.returncode == 0, f"workunit.py inspect failed: {inspect.stderr}", errors)
    status = subprocess.run(
        [sys.executable, str(WORKUNIT), "status", "--task", "SS-PLV2-IR-REPAIR-001"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(status.returncode == 0, f"workunit.py status failed: {status.stderr}", errors)
    missing = subprocess.run(
        [sys.executable, str(WORKUNIT), "inspect", "--task", "SS-PLV2-IR-MISSING"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(missing.returncode != 0, "missing WorkUnit inspect must return a nonzero status.", errors)
    require("Traceback" not in missing.stderr, "missing WorkUnit inspect must not traceback.", errors)
    if missing.stdout:
        payload = json.loads(missing.stdout)
        require(payload.get("status") == "blocked", "missing WorkUnit refusal must be typed as blocked.", errors)
        require(payload.get("reason") == "workunit_not_found", "missing WorkUnit refusal reason must be workunit_not_found.", errors)
        require(isinstance(payload.get("searched_roots"), list), "missing WorkUnit refusal must list searched_roots.", errors)


def main() -> int:
    errors: list[str] = []
    for path in [LEDGER, TEMPLATE_DIR, WORKUNIT]:
        require(path.exists(), f"Missing WorkUnit path: {path.relative_to(ROOT)}", errors)
    if not errors:
        ledger = load_toml(LEDGER)
        require(ledger.get("schema_version") == 1, "WorkUnit ledger schema_version must be 1.", errors)
        require(ledger.get("status") == "active", "WorkUnit ledger status must be active.", errors)
        require(ledger.get("source_mutation_by_aide") is False, "WorkUnit ledger must block AIDE source mutation.", errors)
        require(ledger.get("automatic_merge") is False, "WorkUnit ledger must block automatic merge.", errors)
        require(ledger.get("automatic_promotion") is False, "WorkUnit ledger must block automatic promotion.", errors)
        ids = {item.get("id") for item in ledger.get("work_units", [])}
        require(REQUIRED_TASKS <= ids, "WorkUnit ledger missing PAW-G tasks.", errors)
        for unit in ledger.get("work_units", []):
            if isinstance(unit, dict):
                validate_unit(unit, str(unit.get("id", "unknown")), errors)
        all_units: list[dict[str, Any]] = []
        strict_packets = {LEDGER, WORK_UNITS_DIR / "plasma-v2-instrument-repair.toml"}
        for path in work_unit_files():
            require(path.exists(), f"Referenced WorkUnit file is missing: {repo_path(path)}", errors)
            if path.exists():
                for unit in packet_units(path):
                    if path in strict_packets:
                        validate_unit(unit, f"{repo_path(path)}:{unit.get('id', 'unknown')}", errors)
                    all_units.append(unit)
        all_ids = {item.get("id") for item in all_units}
        require(REQUIRED_PRODUCT_TASKS <= all_ids, "Product-local Plasma repair WorkUnits must be discoverable.", errors)

        observed_templates = {path.name for path in TEMPLATE_DIR.glob("*.toml")}
        require(TEMPLATES <= observed_templates, "WorkUnit templates are incomplete.", errors)
        for template_name in TEMPLATES:
            validate_unit(load_toml(TEMPLATE_DIR / template_name), template_name, errors)

    if not errors:
        validate_cli(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("AIDE WorkUnit checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
