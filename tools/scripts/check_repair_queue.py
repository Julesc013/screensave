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
BURN_DOWN = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "repair-burndown.json"
STABLE_BURN_DOWN = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "repair-burndown.json"

REPAIR_CLASSES = {
    "validator_failure",
    "doc_drift",
    "generated_catalog_drift",
    "proof_baseline_drift",
    "packc_schema_drift",
    "aide_evidence_drift",
    "evidence_index_drift",
    "ci_drift",
    "artifact_manifest_drift",
    "package_checksum_drift",
    "workbench_surface_drift",
    "instrument_architecture_gap",
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
        legacy_repair = next(
            (item for item in queue.get("repairs", []) if isinstance(item, dict) and item.get("id") == "SS-PLV2-IR-REPAIR-001"),
            None,
        )
        require(legacy_repair is not None, "repair queue must record SS-PLV2-IR-REPAIR-001.", errors)
        if legacy_repair is not None:
            require(
                legacy_repair.get("repair_class") == "instrument_architecture_gap",
                "SS-PLV2-IR-REPAIR-001 must be an instrument_architecture_gap repair.",
                errors,
            )
            require(
                legacy_repair.get("status") == "completed",
                "SS-PLV2-IR-REPAIR-001 must be recorded as completed evidence.",
                errors,
            )
            outputs = set(legacy_repair.get("evidence_outputs", []))
            for ref in [
                "validation/captures/plasma-v2/instrument-audit/legacy-authority-report.json",
                "validation/captures/plasma-v2/instrument-audit/migration-report.json",
                "validation/captures/plasma-v2/instrument-audit/workbench-inspection.json",
            ]:
                require(ref in outputs, f"SS-PLV2-IR-REPAIR-001 missing evidence output {ref}.", errors)
        visualintent_repair = next(
            (item for item in queue.get("repairs", []) if isinstance(item, dict) and item.get("id") == "SS-PLV2-IR-REPAIR-002"),
            None,
        )
        require(visualintent_repair is not None, "repair queue must record SS-PLV2-IR-REPAIR-002.", errors)
        if visualintent_repair is not None:
            require(
                visualintent_repair.get("repair_class") == "instrument_architecture_gap",
                "SS-PLV2-IR-REPAIR-002 must be an instrument_architecture_gap repair.",
                errors,
            )
            require(
                visualintent_repair.get("status") == "completed",
                "SS-PLV2-IR-REPAIR-002 must be recorded as completed evidence.",
                errors,
            )
            outputs = set(visualintent_repair.get("evidence_outputs", []))
            for ref in [
                "validation/captures/plasma-v2/instrument-audit/visualintent/spec-reduction-report.json",
                "validation/captures/plasma-v2/instrument-audit/visualintent/proof-summary.json",
                "validation/captures/plasma-v2/instrument-audit/workbench-inspection.json",
                "validation/captures/plasma-v2/instrument-audit/re-audit-report.json",
            ]:
                require(ref in outputs, f"SS-PLV2-IR-REPAIR-002 missing evidence output {ref}.", errors)
            validators = set(visualintent_repair.get("expected_validators", []))
            require(
                "py -3 tools/scripts/check_plasma_v2_visualintent_proof.py" in validators,
                "SS-PLV2-IR-REPAIR-002 must require the VisualIntent proof checker.",
                errors,
            )
        final_repair = next(
            (item for item in queue.get("repairs", []) if isinstance(item, dict) and item.get("id") == "SS-PLV2-I-REPAIR-001"),
            None,
        )
        require(final_repair is not None, "repair queue must record SS-PLV2-I-REPAIR-001.", errors)
        if final_repair is not None:
            require(
                final_repair.get("repair_class") == "release_readiness_gap",
                "SS-PLV2-I-REPAIR-001 must be a release_readiness_gap repair.",
                errors,
            )
            require(
                final_repair.get("status") == "blocking",
                "SS-PLV2-I-REPAIR-001 must remain blocking until accepted-for-stable is supplied.",
                errors,
            )
            require(
                final_repair.get("blocker_reasons") == [
                    "final_stable_artistic_acceptance_not_accepted",
                    "missing_project_owned_accepted_for_stable_verdict",
                ],
                "SS-PLV2-I-REPAIR-001 must record the Turn 4 request-changes blocker reasons.",
                errors,
            )
            require(
                "products/savers/plasma/src/v2/**" in final_repair.get("allowed_paths", []),
                "SS-PLV2-I-REPAIR-001 must allow only bounded Plasma v2 island product repair.",
                errors,
            )
            require(
                "platform/**" in final_repair.get("forbidden_paths", []),
                "SS-PLV2-I-REPAIR-001 must forbid platform edits.",
                errors,
            )
            final_outputs = set(final_repair.get("evidence_outputs", []))
            for ref in [
                "validation/captures/plasma-v2/final-artistic-decision/request-changes-intake.json",
                "validation/captures/plasma-v2/final-artistic-decision/request-changes-repair-plan.json",
                "validation/captures/plasma-v2/final-artistic-decision/intake.json",
                "validation/captures/plasma-v2/final-artistic-decision/review-evidence.json",
                "validation/captures/plasma-v2/final-artistic-decision/repair-evidence.json",
                "validation/captures/plasma-v2/final-artistic-decision/repair-summary.md",
                "validation/captures/plasma-v2/final-artistic-decision/decision.stable.toml",
                "validation/captures/plasma-v2/final-artistic-decision/aide-decision-summary.json",
                "validation/captures/plasma-v2/final-artistic-decision/aide-repair-summary.json",
                "validation/captures/plasma-v2/stable-promotion/hold-report.json",
            ]:
                require(ref in final_outputs, f"SS-PLV2-I-REPAIR-001 missing evidence output {ref}.", errors)
            final_boundary = str(final_repair.get("claim_boundary", ""))
            for phrase in ("ScreenSave/project authority owns the verdict", "no automatic acceptance", "compatibility certification"):
                require(phrase in final_boundary, f"SS-PLV2-I-REPAIR-001 boundary missing {phrase!r}.", errors)
        burn = queue.get("release_candidate_burndown", {})
        if burn:
            require(
                burn.get("status") == "no-blocking-release-candidate-repairs",
                "release-candidate repair burn-down status must be no-blocking-release-candidate-repairs.",
                errors,
            )
            require(burn.get("open_blocking_count") == 0, "release-candidate repair burn-down open_blocking_count must be 0.", errors)
            require("compatibility certification" in str(burn.get("claim_boundary", "")), "release-candidate repair burn-down must block certification.", errors)
        stable_burn = queue.get("stable_promotion_burndown", {})
        if stable_burn:
            require(
                stable_burn.get("status") in {
                    "no-blocking-stable-promotion-repairs",
                    "blocking-stable-promotion-repairs-open",
                },
                "stable-promotion repair burn-down status invalid.",
                errors,
            )
            require(
                stable_burn.get("open_blocking_count") in {0, 1},
                "stable-promotion repair burn-down open_blocking_count must be 0 or 1.",
                errors,
            )
            require("compatibility certification" in str(stable_burn.get("claim_boundary", "")), "stable-promotion repair burn-down must block certification.", errors)
            for repair_id in stable_burn.get("blocking_repairs", []):
                require(
                    any(item.get("id") == repair_id for item in queue.get("repairs", []) if isinstance(item, dict)),
                    f"stable-promotion blocking repair not listed: {repair_id}",
                    errors,
                )
            if stable_burn.get("open_blocking_count") == 1:
                require(
                    stable_burn.get("blocking_repairs") == ["SS-PLV2-I-REPAIR-001"],
                    "stable-promotion burn-down must name only SS-PLV2-I-REPAIR-001 when one repair is open.",
                    errors,
                )
        validate_repair(load_toml(TEMPLATE), "repair template", errors)
        policy_text = POLICY.read_text(encoding="utf-8")
        for needle in [
            "validator_failure",
            "generated_catalog_drift",
            "instrument_architecture_gap",
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
        if BURN_DOWN.exists():
            burn = json.loads(BURN_DOWN.read_text(encoding="utf-8"))
            require(
                burn.get("status") == "no-blocking-release-candidate-repairs",
                "repair burn-down evidence status must be no-blocking-release-candidate-repairs.",
                errors,
            )
            require(burn.get("open_blocking_count") == 0, "repair burn-down evidence open_blocking_count must be 0.", errors)
            require("certify compatibility" in burn.get("claim_boundary", ""), "repair burn-down evidence must block certification.", errors)
        if STABLE_BURN_DOWN.exists():
            burn = json.loads(STABLE_BURN_DOWN.read_text(encoding="utf-8"))
            require(
                burn.get("status") in {
                    "no-blocking-stable-promotion-repairs",
                    "blocking-stable-promotion-repairs-open",
                },
                "stable repair burn-down evidence status invalid.",
                errors,
            )
            require(burn.get("open_blocking_count") in {0, 1}, "stable repair burn-down evidence open_blocking_count must be 0 or 1.", errors)
            require("certify compatibility" in burn.get("claim_boundary", ""), "stable repair burn-down evidence must block certification.", errors)
            if burn.get("open_blocking_count") == 1:
                blocking_ids = {str(item.get("id")) for item in burn.get("blocking_repairs", [])}
                require(blocking_ids == {"SS-PLV2-I-REPAIR-001"}, "stable repair burn-down evidence must name SS-PLV2-I-REPAIR-001.", errors)
                reasons = " ".join(str(item.get("reason", "")) for item in burn.get("blocking_repairs", []))
                require("explicit final human artistic acceptance verdict" in reasons, "stable repair burn-down evidence must record the missing verdict reason.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Repair queue checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
