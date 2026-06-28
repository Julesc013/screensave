"""Validate the Plasma v2 final artistic request-changes repair plan."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
DECISION_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision"
DECISION = DECISION_DIR / "decision.stable.toml"
INTAKE = DECISION_DIR / "request-changes-intake.json"
PLAN = DECISION_DIR / "request-changes-repair-plan.json"
REPAIR_EVIDENCE = DECISION_DIR / "repair-evidence.json"
REPAIR_SUMMARY = DECISION_DIR / "repair-summary.md"
HOLD_REPORT = DECISION_DIR / "hold-report.json"
STABLE_HOLD_REPORT = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "hold-report.json"
TAXONOMY_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-artistic-repair-taxonomy.md"

TAXONOMY_CLASSES = {
    "field_shape_quality",
    "motion_quality",
    "material_response",
    "luminance_comfort",
    "contrast_balance",
    "treatment_restraint",
    "control_influence_visible_but_weak",
    "default_profile_quality",
    "visualintent_candidate_quality",
    "gl11_reference_mismatch",
    "dark_room_comfort",
    "preview_legibility",
    "composition_repetition",
    "artifacting_or_bandings",
    "documentation_or_evidence_gap",
}

EXPECTED_BLOCKERS = {
    "final_stable_artistic_acceptance_not_accepted",
    "missing_project_owned_accepted_for_stable_verdict",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    return tomllib.loads(path.read_text(encoding="utf-8"))


def repo_ref_exists(ref: str) -> bool:
    return bool(ref) and (ROOT / ref).exists()


def decision_blockers(decision: dict[str, Any], intake: dict[str, Any], stable_hold: dict[str, Any]) -> set[str]:
    blockers = {str(item) for item in decision.get("requested_changes", {}).get("blocker_reasons", [])}
    blockers.update(str(item.get("id")) for item in stable_hold.get("exact_blockers", []) if isinstance(item, dict))
    aliases = intake.get("decision_aliases", {})
    for key, value in aliases.items():
        if key in blockers:
            blockers.add(str(value))
    return {item for item in blockers if item}


def validate_taxonomy_doc(errors: list[str]) -> None:
    text = TAXONOMY_DOC.read_text(encoding="utf-8")
    for class_name in TAXONOMY_CLASSES:
        require(f"`{class_name}`" in text, f"taxonomy doc missing {class_name}.", errors)
    for phrase in (
        "direct controls -> plasma_v2_spec -> plasma_v2_plan -> plasma_v2_runtime",
        "documentation_or_evidence_gap",
        "does not record a field, motion, material",
        "does not make proof",
    ):
        require(phrase in text, f"taxonomy doc missing phrase {phrase!r}.", errors)


def validate_intake(intake: dict[str, Any], stable_hold: dict[str, Any], errors: list[str]) -> None:
    require(intake.get("status") == "parsed", "request-changes intake must be parsed.", errors)
    require(intake.get("work_unit") == "SS-PLV2-I-REPAIR-001", "intake must name SS-PLV2-I-REPAIR-001.", errors)
    require(intake.get("decision") == "request-changes", "intake must record request-changes.", errors)
    require(intake.get("accepted_for_stable_verdict_present") is False, "intake must record missing accepted-for-stable verdict.", errors)
    require(intake.get("visual_artistic_blockers_named") is False, "intake must not invent visual blockers.", errors)
    require(intake.get("visual_artistic_blockers") == [], "intake visual blocker list must be empty.", errors)
    blocker_ids = {str(item.get("id")) for item in intake.get("exact_blockers", []) if isinstance(item, dict)}
    require(EXPECTED_BLOCKERS <= blocker_ids, "intake must include the exact Turn 4 blockers.", errors)
    stable_ids = {str(item.get("id")) for item in stable_hold.get("exact_blockers", []) if isinstance(item, dict)}
    require(EXPECTED_BLOCKERS <= stable_ids, "stable hold report must include the exact Turn 4 blockers.", errors)
    for item in intake.get("exact_blockers", []):
        require(repo_ref_exists(str(item.get("source_ref", ""))), f"intake source ref missing: {item.get('source_ref')}", errors)
    boundary = str(intake.get("repair_boundary", ""))
    require("names no visual defect" in boundary, "intake repair boundary must say no visual defect is named.", errors)
    require("must not be changed" in boundary, "intake repair boundary must block source edits without named defects.", errors)


def validate_plan(plan: dict[str, Any], allowed_blockers: set[str], errors: list[str]) -> None:
    require(plan.get("status") == "active", "repair plan must be active.", errors)
    require(plan.get("source_decision") == "validation/captures/plasma-v2/final-artistic-decision/decision.stable.toml", "repair plan source decision mismatch.", errors)
    require(plan.get("decision") == "request-changes", "repair plan must be for request-changes.", errors)
    require(plan.get("stable_promotion_allowed_before_repair") is False, "stable promotion must be blocked before repair.", errors)
    require(plan.get("visual_artistic_blockers_named") is False, "repair plan must not invent visual blockers.", errors)
    require(plan.get("source_visual_defects") == [], "repair plan source visual defects must be empty.", errors)
    require(plan.get("product_source_repairs_allowed") is False, "repair plan must block product source repair when no visual defect is named.", errors)
    require(plan.get("product_source_repairs_applied") is False, "repair plan must record no product source repair applied.", errors)
    blockers = plan.get("blockers", [])
    require(isinstance(blockers, list) and len(blockers) == 2, "repair plan must contain the two exact blockers.", errors)
    blocker_ids = {str(item.get("id")) for item in blockers if isinstance(item, dict)}
    require(blocker_ids == EXPECTED_BLOCKERS, "repair plan blockers must match the held blockers exactly.", errors)
    require(blocker_ids <= allowed_blockers, "repair plan contains a blocker not derived from decision or supporting reports.", errors)
    for item in blockers:
        if not isinstance(item, dict):
            errors.append("repair plan blocker entries must be objects.")
            continue
        label = str(item.get("id", "blocker"))
        for key in ("id", "class", "source_ref", "repair_strategy", "allowed_files", "expected_evidence", "claim_boundary"):
            require(key in item, f"{label} missing {key}.", errors)
        require(item.get("class") in TAXONOMY_CLASSES, f"{label} uses unknown repair class.", errors)
        require(item.get("class") == "documentation_or_evidence_gap", f"{label} must be a documentation_or_evidence_gap.", errors)
        require(repo_ref_exists(str(item.get("source_ref", ""))), f"{label} source ref missing: {item.get('source_ref')}", errors)
        require(isinstance(item.get("allowed_files"), list) and item.get("allowed_files"), f"{label} must list allowed files.", errors)
        require(isinstance(item.get("expected_evidence"), list) and item.get("expected_evidence"), f"{label} must list expected evidence.", errors)
        strategy = str(item.get("repair_strategy", ""))
        require("Do not alter Plasma v2 visuals" in strategy or "explicit project-owned verdict" in strategy, f"{label} strategy must preserve the verdict boundary.", errors)
    boundary = str(plan.get("claim_boundary", ""))
    for phrase in ("does not invent visual defects", "compatibility certification", "promote stable"):
        require(phrase in boundary, f"repair plan boundary missing {phrase!r}.", errors)


def validate_repair_evidence(errors: list[str]) -> None:
    if not REPAIR_EVIDENCE.exists():
        return
    evidence = load_json(REPAIR_EVIDENCE)
    require(evidence.get("status") in {"ready-for-re-review", "held-for-project-verdict"}, "repair evidence status invalid.", errors)
    require(evidence.get("repair_workunit") == "SS-PLV2-I-REPAIR-001", "repair evidence must name the repair WorkUnit.", errors)
    require(evidence.get("source_decision") == "request-changes", "repair evidence must derive from request-changes.", errors)
    require(evidence.get("product_source_repairs_applied") is False, "repair evidence must record no source repair for missing-verdict blocker.", errors)
    require(evidence.get("visual_artistic_blockers_repaired") == [], "repair evidence must not claim visual blocker repairs.", errors)
    require(evidence.get("instrument_audit_status") in {"pass", "promotion-ready"}, "repair evidence must record instrument audit status.", errors)
    boundary = str(evidence.get("claim_boundary", ""))
    require("not stable promotion by itself" in boundary, "repair evidence boundary must block stable promotion.", errors)
    if REPAIR_SUMMARY.exists():
        summary = REPAIR_SUMMARY.read_text(encoding="utf-8")
        for phrase in ("No visual defect was recorded", "request-changes", "release-candidate-hold"):
            require(phrase in summary, f"repair summary missing {phrase!r}.", errors)
    if HOLD_REPORT.exists():
        hold = load_json(HOLD_REPORT)
        require(hold.get("status") == "hold", "final artistic hold report must be hold.", errors)
        require(hold.get("final_artistic_decision") == "request-changes", "final artistic hold report decision mismatch.", errors)
        ids = {str(item.get("id")) for item in hold.get("exact_blockers", []) if isinstance(item, dict)}
        require(EXPECTED_BLOCKERS <= ids, "final artistic hold report must include exact blockers.", errors)


def main() -> int:
    errors: list[str] = []
    for path in (DECISION, INTAKE, PLAN, STABLE_HOLD_REPORT, TAXONOMY_DOC):
        require(path.exists(), f"Missing artistic repair plan path: {path.relative_to(ROOT)}", errors)
    if not errors:
        decision = load_toml(DECISION)
        intake = load_json(INTAKE)
        plan = load_json(PLAN)
        stable_hold = load_json(STABLE_HOLD_REPORT)
        require(decision.get("decision") == "request-changes", "source decision must still be request-changes for this repair plan.", errors)
        validate_taxonomy_doc(errors)
        validate_intake(intake, stable_hold, errors)
        validate_plan(plan, decision_blockers(decision, intake, stable_hold), errors)
        validate_repair_evidence(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 artistic repair plan checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
