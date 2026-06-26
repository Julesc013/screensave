"""Validate the ScreenSave repair queue and deterministic repair tooling."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
QUEUE = ROOT / ".aide" / "repairs" / "index.toml"
TEMPLATE = ROOT / ".aide" / "repairs" / "templates" / "repair-task.toml"
POLICY = ROOT / ".aide" / "policies" / "screensave-repair.yaml"
TOOL = ROOT / "tools" / "aideops" / "repair_queue.py"
SCAN_OUT = ROOT / "out" / "aide" / "repairs" / "scan.json"

REPAIR_CLASSES = {
    "validator_failure",
    "doc_drift",
    "generated_catalog_drift",
    "proof_baseline_drift",
    "packc_schema_drift",
    "aide_evidence_drift",
    "ci_drift",
    "artifact_manifest_drift",
    "workbench_surface_drift",
    "release_readiness_gap",
}


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_tool(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(TOOL), *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def validate_repair(item: dict[str, Any], label: str, errors: list[str]) -> None:
    for key in [
        "id",
        "title",
        "repair_class",
        "status",
        "allowed_paths",
        "forbidden_paths",
        "expected_validators",
        "evidence_outputs",
        "claim_boundary",
        "rollback_point",
    ]:
        require(key in item, f"{label} missing {key}", errors)
    require(item.get("repair_class") in REPAIR_CLASSES, f"{label} repair_class invalid.", errors)
    require(isinstance(item.get("allowed_paths"), list), f"{label} allowed_paths must be a list.", errors)
    require(isinstance(item.get("forbidden_paths"), list), f"{label} forbidden_paths must be a list.", errors)
    boundary = str(item.get("claim_boundary", "")).lower()
    for word in ["release", "certification"]:
        require(word in boundary, f"{label} claim boundary must mention {word}.", errors)


def main() -> int:
    errors: list[str] = []
    for path in [QUEUE, TEMPLATE, POLICY, TOOL]:
        require(path.exists(), f"Missing repair queue path: {path.relative_to(ROOT)}", errors)
    if not errors:
        queue = load_toml(QUEUE)
        require(queue.get("schema_version") == 1, "repair queue schema_version must be 1.", errors)
        require(queue.get("status") == "active", "repair queue status must be active.", errors)
        require(queue.get("network_calls") is False, "repair queue must be network-free.", errors)
        require(queue.get("provider_or_model_calls") is False, "repair queue must be model-free.", errors)
        require(queue.get("source_mutation_by_aide") is False, "repair queue must not admit AIDE source mutation.", errors)
        require(set(queue.get("repair_classes", [])) == REPAIR_CLASSES, "repair classes mismatch.", errors)
        for repair in queue.get("repairs", []):
            if isinstance(repair, dict):
                validate_repair(repair, str(repair.get("id", "repair")), errors)
        validate_repair(load_toml(TEMPLATE), "repair template", errors)
        policy_text = POLICY.read_text(encoding="utf-8")
        for needle in [
            "validator_failure",
            "generated_catalog_drift",
            "release_readiness_gap",
            "source_mutation_by_aide: false",
            "automatic_merge: false",
            "automatic_promotion: false",
            "WorkUnit",
            "Repair class",
            "Boundary",
        ]:
            require(needle in policy_text, f"repair policy missing {needle!r}", errors)

    if not errors:
        scan = run_tool(["scan"])
        require(scan.returncode == 0, f"repair_queue.py scan failed: {scan.stderr}", errors)
        plan = run_tool(["plan", "--class", "validator_failure"])
        require(plan.returncode == 0, f"repair_queue.py plan failed: {plan.stderr}", errors)
        close = run_tool(["close", "--task", "SS-REPAIR-001"])
        require(close.returncode == 0, f"repair_queue.py close failed: {close.stderr}", errors)
        require(SCAN_OUT.exists(), "repair_queue.py scan must write scan.json.", errors)
        if SCAN_OUT.exists():
            payload = json.loads(SCAN_OUT.read_text(encoding="utf-8"))
            require(payload.get("status") == "pass", "repair scan status must pass.", errors)
            require(payload.get("repair_count", 0) >= 1, "repair scan must include repair tasks.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Repair queue checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
