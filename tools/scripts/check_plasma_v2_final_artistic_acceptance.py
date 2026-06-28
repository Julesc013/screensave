"""Validate the Plasma v2 final stable artistic decision packet."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
DECISION_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision"
DECISION = DECISION_DIR / "decision.stable.toml"
STABLE_TEMPLATE = DECISION_DIR / "decision.stable.template.toml"
REVIEW_INPUTS = DECISION_DIR / "review-inputs.json"
REVIEW_EVIDENCE = DECISION_DIR / "review-evidence.json"
REVIEW_SUMMARY = DECISION_DIR / "review-summary.md"
CONTACT_SHEET_INDEX = DECISION_DIR / "contact-sheet-index.json"
SUMMARY = DECISION_DIR / "decision-summary.md"
POLICY = DECISION_DIR / "decision-policy.md"
REVIEWER_NOTES = DECISION_DIR / "reviewer-notes" / "stable-review-required.md"
PRODUCT_PROTOCOL = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-final-artistic-acceptance.md"

ALLOWED_STATES = {
    "accepted-for-stable",
    "request-changes",
    "defer-to-labs",
    "rejected",
}

REQUIRED_CANONICAL_FIELDS = {
    "decision_id",
    "decision_date",
    "reviewer_or_project_authority",
    "source_commit",
    "release_candidate",
    "profile",
    "accepted_scope",
    "excluded_scope",
    "evidence_refs",
    "visual_review_summary",
    "instrument_architecture_summary",
    "known_limit_acceptance",
    "claim_boundary",
}

REQUIRED_TEMPLATE_EVIDENCE = {
    "proof_bundle",
    "instrument_architecture_audit",
    "instrument_reaudit",
    "legacy_authority",
    "visualintent_spec_reduction",
    "stable_candidate_review",
    "acceleration_matrix",
    "performance_envelope",
    "package_stage",
    "known_limits",
}

REQUIRED_DECISION_EVIDENCE = {
    "proof_bundle",
    "stable_candidate_review",
    "release_candidate_decision",
    "acceleration_matrix",
    "performance_envelope",
    "package_stage",
    "known_limits",
}

ACCEPTED_SCOPE_PHRASES = {
    "Plasma v2 release candidate rc1",
    "profile plasma.v2.reference.preview",
    "direct-control instrument architecture",
    "software/reference path",
    "GDI lower-band path",
    "GL11 optional candidate",
    "packc v1-candidate data packs",
    "VisualIntent-to-spec candidate flow",
    "current Windows release-candidate evidence",
}

EXCLUDED_SCOPE_PHRASES = {
    "final public release publication",
    "Windows preservation certification",
    "macOS/Linux support",
    "all Core saver migration",
    "public SDK stability",
    "runtime executable plugin system",
    "AIDE runtime dependency",
}

REQUIRED_REVIEW_EVIDENCE = {
    "reference_proof_bundle",
    "instrument_architecture_audit",
    "instrument_reaudit",
    "legacy_authority_repair",
    "visualintent_spec_reduction",
    "material_response",
    "control_influence",
    "field_pipeline",
    "runtime_report",
    "plan_report",
    "gl11_optionality_acceleration",
    "performance_envelope",
    "workbench_release_readiness_review",
    "manager_package_preview_review",
    "release_candidate_package_staging",
    "stable_promotion_package_staging",
    "release_candidate_support_claims",
    "stable_support_claims",
    "known_limits",
    "aide_evidence_index",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    return tomllib.loads(path.read_text(encoding="utf-8"))


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def repo_ref_exists(ref: str) -> bool:
    return bool(ref) and (ROOT / ref).exists()


def decision_state(payload: dict[str, Any]) -> str:
    return str(payload.get("decision") or payload.get("decision_state") or "")


def validate_ref_table(refs: dict[str, Any], required: set[str], label: str, errors: list[str]) -> None:
    for key in required:
        ref = str(refs.get(key, ""))
        require(ref != "", f"{label} is missing evidence ref {key}.", errors)
        if ref:
            require(repo_ref_exists(ref), f"{label} evidence ref does not exist: {ref}", errors)


def validate_template(errors: list[str]) -> None:
    template = load_toml(STABLE_TEMPLATE)
    require(template.get("template_id") == "screensave.plasma-v2.final-artistic-decision.stable.template", "stable template id mismatch.", errors)
    require(template.get("release_candidate") == "plasma-v2-rc1", "stable template must name plasma-v2-rc1.", errors)
    require(template.get("profile") == "plasma.v2.reference.preview", "stable template must name the preview profile.", errors)
    require(set(template.get("decision_options", [])) == ALLOWED_STATES, "stable template decision options mismatch.", errors)
    require(REQUIRED_CANONICAL_FIELDS <= set(template), "stable template is missing canonical decision fields.", errors)
    validate_ref_table(template.get("evidence_refs", {}), REQUIRED_TEMPLATE_EVIDENCE, "stable template", errors)
    excluded = template.get("excluded_scope", {})
    for key in (
        "final_public_release_publication",
        "windows_preservation_certification",
        "macos_linux_support",
        "all_core_saver_migration",
        "public_sdk_stability",
        "runtime_executable_plugin_system",
        "aide_runtime_dependency",
    ):
        require(excluded.get(key) is True, f"stable template excluded scope must include {key}.", errors)
    boundary = str(template.get("claim_boundary", ""))
    for phrase in ("ScreenSave/project authority", "compatibility certification", "AIDE automatic acceptance"):
        require(phrase in boundary, f"stable template claim boundary missing {phrase!r}.", errors)


def validate_review_inputs(errors: list[str]) -> None:
    inputs = load_json(REVIEW_INPUTS)
    require(inputs.get("status") == "protocol-defined", "review inputs must be protocol-defined.", errors)
    require(inputs.get("release_candidate") == "plasma-v2-rc1", "review inputs must name plasma-v2-rc1.", errors)
    require(inputs.get("profile") == "plasma.v2.reference.preview", "review inputs must name the preview profile.", errors)
    require(inputs.get("decision_owner") == "ScreenSave/project authority", "review inputs must name ScreenSave/project authority.", errors)
    require(inputs.get("aide_role") == "evidence-index-only", "review inputs must keep AIDE evidence-only.", errors)
    require(set(inputs.get("decision_options", [])) == ALLOWED_STATES, "review inputs decision options mismatch.", errors)
    require(REQUIRED_CANONICAL_FIELDS <= set(inputs.get("required_decision_fields", [])), "review inputs missing canonical field requirements.", errors)
    accepted_scope = set(inputs.get("accepted_scope", []))
    excluded_scope = set(inputs.get("excluded_scope", []))
    require(ACCEPTED_SCOPE_PHRASES <= accepted_scope, "review inputs accepted scope is incomplete.", errors)
    require(EXCLUDED_SCOPE_PHRASES <= excluded_scope, "review inputs excluded scope is incomplete.", errors)
    for ref in inputs.get("evidence_refs", []):
        require(repo_ref_exists(str(ref)), f"review input evidence ref does not exist: {ref}", errors)
    protocol = inputs.get("protocol", {})
    for key in ("proof_supports_decision", "review_records_decision", "screensave_owns_decision", "aide_indexes_decision"):
        require(protocol.get(key) is True, f"review input protocol must set {key} true.", errors)
    for key in ("validators_auto_accept", "aide_auto_accept", "publication_in_scope", "compatibility_certification_broadened"):
        require(protocol.get(key) is False, f"review input protocol must set {key} false.", errors)


def validate_decision(errors: list[str]) -> None:
    decision = load_toml(DECISION)
    state = decision_state(decision)
    require(decision.get("product") == "plasma", "stable decision must target Plasma.", errors)
    require(decision.get("profile") == "plasma.v2.reference.preview", "stable decision must name the preview profile.", errors)
    require(state in ALLOWED_STATES, f"stable decision has invalid state: {state}", errors)
    require(decision.get("reviewer", "") != "" or decision.get("reviewer_or_project_authority", "") != "", "stable decision must name the reviewer or project authority.", errors)

    evidence = decision.get("evidence_refs", {})
    validate_ref_table(evidence, REQUIRED_DECISION_EVIDENCE, "stable decision", errors)

    canonical_present = "decision_id" in decision or "decision" in decision
    if canonical_present:
        require(REQUIRED_CANONICAL_FIELDS <= set(decision), "stable decision has a partial canonical field set.", errors)
        require(decision.get("release_candidate") == "plasma-v2-rc1", "stable decision must name plasma-v2-rc1.", errors)
        require(decision.get("source_commit", "") != "", "stable decision must name source_commit.", errors)
        require(decision.get("decision_id", "") != "", "stable decision must name decision_id.", errors)
        require(decision.get("decision_date", "") != "", "stable decision must name decision_date.", errors)
        require(decision.get("claim_boundary", "") != "", "stable decision must include a claim boundary.", errors)

    boundaries = decision.get("boundaries", {})
    if state == "accepted-for-stable":
        require(boundaries.get("final_stable_artistic_acceptance") is True, "accepted stable decision must mark final_stable_artistic_acceptance true.", errors)
        require(decision.get("accepted_scope", {}).get("product") == "Plasma v2", "accepted stable decision must scope the accepted product.", errors)
        require(decision.get("accepted_scope", {}).get("profile") == "plasma.v2.reference.preview", "accepted stable decision must scope the accepted profile.", errors)
        require(decision.get("publication_decision") == "not-in-this-turn", "accepted stable decision must not publish in this turn.", errors)
        require(decision.get("compatibility_certification") == "not-broadened", "accepted stable decision must not broaden compatibility certification.", errors)
        require(decision.get("aide_authority") == "evidence-only", "accepted stable decision must keep AIDE evidence-only.", errors)
    else:
        require(boundaries.get("final_stable_artistic_acceptance") is False, "non-accepted stable decision must keep final_stable_artistic_acceptance false.", errors)
        require(decision.get("requested_changes", {}).get("blocking") is True, "non-accepted stable decision must record a blocking requested change.", errors)
        reason = str(decision.get("requested_changes", {}).get("reason", ""))
        require("explicit" in reason and "verdict" in reason, "non-accepted stable decision must include an exact missing-verdict reason.", errors)

    for key in (
        "stable_release",
        "release_publication",
        "compatibility_certification",
        "public_sdk_stability",
        "automatic_acceptance",
    ):
        require(boundaries.get(key) is False, f"stable decision boundary {key} must remain false.", errors)


def validate_review_evidence(errors: list[str]) -> None:
    evidence = load_json(REVIEW_EVIDENCE)
    require(evidence.get("status") == "ready-for-decision", "review evidence must be ready-for-decision.", errors)
    require(evidence.get("release_candidate") == "plasma-v2-rc1", "review evidence must name plasma-v2-rc1.", errors)
    require(evidence.get("profile") == "plasma.v2.reference.preview", "review evidence must name the preview profile.", errors)
    require(evidence.get("evidence_complete") is True, "review evidence must mark evidence_complete true.", errors)
    require(evidence.get("instrument_audit_status") == "pass", "review evidence must record instrument audit pass.", errors)
    require(evidence.get("instrument_audit_report_status") == "promotion-ready", "review evidence must record promotion-ready instrument audit.", errors)
    require(evidence.get("stable_promotion_precheck") == "pending", "review evidence must leave stable promotion precheck pending.", errors)
    for item in evidence.get("visual_review_inputs", []):
        require(repo_ref_exists(str(item.get("ref", ""))), f"review evidence visual input missing: {item.get('ref')}", errors)
    refs = evidence.get("evidence_refs", {})
    validate_ref_table(refs, REQUIRED_REVIEW_EVIDENCE, "review evidence", errors)
    require(len(evidence.get("known_limits", [])) >= 5, "review evidence must record known limits.", errors)
    require(len(evidence.get("excluded_claims", [])) >= 8, "review evidence must record excluded claims.", errors)
    require(
        evidence.get("claim_boundary") == "Final artistic decision input only; not release publication or compatibility certification.",
        "review evidence claim boundary mismatch.",
        errors,
    )


def validate_contact_sheets(errors: list[str]) -> None:
    index = load_json(CONTACT_SHEET_INDEX)
    require(index.get("status") == "ready-for-review", "contact sheet index must be ready-for-review.", errors)
    sheets = index.get("contact_sheets", [])
    require(len(sheets) >= 6, "contact sheet index must include the review contact sheets.", errors)
    for item in sheets:
        require(repo_ref_exists(str(item.get("ref", ""))), f"contact sheet ref missing: {item.get('ref')}", errors)
    boundary = str(index.get("claim_boundary", ""))
    require("do not accept stable promotion automatically" in boundary, "contact sheet boundary must block automatic acceptance.", errors)


def validate_text_files(errors: list[str]) -> None:
    decision = load_toml(DECISION)
    state = decision_state(decision)
    summary = SUMMARY.read_text(encoding="utf-8")
    summary_phrases = (
        (
            "Stable-Promotion Accepted",
            "accepted-for-stable",
            "not AIDE automatic promotion",
            "not release publication",
            "not compatibility certification",
        )
        if state == "accepted-for-stable"
        else (
            "Stable-Promotion Hold",
            "request-changes",
            "not automatic acceptance",
            "not release publication",
            "not compatibility certification",
        )
    )
    for phrase in summary_phrases:
        require(phrase in summary, f"decision summary is missing phrase {phrase!r}.", errors)

    notes = REVIEWER_NOTES.read_text(encoding="utf-8")
    if state == "accepted-for-stable":
        require("accepted-for-stable" in notes, "stable reviewer notes must record the accepted final verdict.", errors)
        require("publication remains `not-in-this-turn`" in notes, "stable reviewer notes must preserve publication boundary.", errors)
    else:
        require("final stable artistic acceptance was not supplied" in notes, "stable reviewer notes must record the missing final acceptance.", errors)
        require("accepted-for-stable" in notes, "stable reviewer notes must list the accepted-for-stable option.", errors)
    require("compatibility certification" in notes, "stable reviewer notes must preserve compatibility certification boundary.", errors)

    policy = POLICY.read_text(encoding="utf-8")
    for phrase in ("accepted-for-stable", "still not release publication", "AIDE automatic promotion"):
        require(phrase in policy, f"decision policy missing {phrase!r}.", errors)

    protocol = PRODUCT_PROTOCOL.read_text(encoding="utf-8")
    for phrase in (
        "ScreenSave owns the decision",
        "External evidence indexes support the decision without owning it.",
        "accepted-for-stable",
        "request-changes",
        "Windows preservation certification",
        "external automation runtime dependency",
    ):
        require(phrase in protocol, f"product protocol missing {phrase!r}.", errors)

    review_summary = REVIEW_SUMMARY.read_text(encoding="utf-8")
    review_phrases = (
        (
            "decision-recorded",
            "Instrument audit status: `promotion-ready`",
            "Stable-promotion precheck: `ready-for-gate`",
            "final stable artistic acceptance is accepted",
            "not release publication",
            "compatibility certification",
        )
        if state == "accepted-for-stable"
        else (
            "ready-for-decision",
            "Instrument audit status: `promotion-ready`",
            "Stable-promotion precheck: `pending`",
            "final stable artistic acceptance is not yet accepted-for-stable",
            "not release publication",
            "compatibility certification",
        )
    )
    for phrase in review_phrases:
        require(phrase in review_summary, f"review summary missing {phrase!r}.", errors)


def main() -> int:
    errors: list[str] = []
    for path in (
        DECISION,
        STABLE_TEMPLATE,
        REVIEW_INPUTS,
        REVIEW_EVIDENCE,
        REVIEW_SUMMARY,
        CONTACT_SHEET_INDEX,
        SUMMARY,
        POLICY,
        REVIEWER_NOTES,
        PRODUCT_PROTOCOL,
    ):
        require(path.exists(), f"Missing final stable artistic decision file: {path.relative_to(ROOT)}", errors)

    if not errors:
        validate_template(errors)
        validate_review_inputs(errors)
        validate_review_evidence(errors)
        validate_contact_sheets(errors)
        validate_decision(errors)
        validate_text_files(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 final stable artistic decision checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
