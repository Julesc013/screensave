"""Validate the ScreenSave AIDE WorkUnit ledger and templates."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
LEDGER = ROOT / ".aide" / "work_units" / "index.toml"
TEMPLATE_DIR = ROOT / ".aide" / "work_units" / "templates"
WORKUNIT = ROOT / "tools" / "aideops" / "workunit.py"

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
    inspect = subprocess.run(
        [sys.executable, str(WORKUNIT), "inspect", "--task", "SS-G12"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    require(inspect.returncode == 0, f"workunit.py inspect failed: {inspect.stderr}", errors)


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
