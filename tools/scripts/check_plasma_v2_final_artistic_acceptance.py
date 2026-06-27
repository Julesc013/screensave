"""Validate the Plasma v2 final stable artistic decision packet."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
DECISION_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision"
DECISION = DECISION_DIR / "decision.stable.toml"
SUMMARY = DECISION_DIR / "decision-summary.md"
REVIEWER_NOTES = DECISION_DIR / "reviewer-notes" / "stable-review-required.md"

ALLOWED_STATES = {
    "accepted-for-stable",
    "request-changes",
    "defer-to-labs",
    "rejected",
}

REQUIRED_EVIDENCE = {
    "proof_bundle",
    "stable_candidate_review",
    "release_candidate_decision",
    "acceleration_matrix",
    "performance_envelope",
    "package_stage",
    "known_limits",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict:
    return tomllib.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    errors: list[str] = []
    for path in (DECISION, SUMMARY, REVIEWER_NOTES):
        require(path.exists(), f"Missing final stable artistic decision file: {path.relative_to(ROOT)}", errors)

    if DECISION.exists():
        decision = load_toml(DECISION)
        state = decision.get("decision_state")
        require(decision.get("product") == "plasma", "stable decision must target Plasma.", errors)
        require(decision.get("profile") == "plasma.v2.reference.preview", "stable decision must name the preview profile.", errors)
        require(state in ALLOWED_STATES, f"stable decision has invalid state: {state}", errors)
        require(decision.get("reviewer", "") != "", "stable decision must name the reviewer or required reviewer.", errors)

        evidence = decision.get("evidence_refs", {})
        for key in REQUIRED_EVIDENCE:
            ref = evidence.get(key, "")
            require(ref != "", f"stable decision is missing evidence ref {key}.", errors)
            if ref:
                require((ROOT / ref).exists(), f"stable decision evidence ref does not exist: {ref}", errors)

        boundaries = decision.get("boundaries", {})
        if state == "accepted-for-stable":
            require(boundaries.get("final_stable_artistic_acceptance") is True, "accepted stable decision must mark final_stable_artistic_acceptance true.", errors)
            require(decision.get("accepted_scope", {}).get("product") == "Plasma v2", "accepted stable decision must scope the accepted product.", errors)
            require(decision.get("accepted_scope", {}).get("profile") == "plasma.v2.reference.preview", "accepted stable decision must scope the accepted profile.", errors)
        else:
            require(boundaries.get("final_stable_artistic_acceptance") is False, "non-accepted stable decision must keep final_stable_artistic_acceptance false.", errors)
            require(decision.get("requested_changes", {}).get("blocking") is True, "non-accepted stable decision must record a blocking requested change.", errors)

        for key in (
            "stable_release",
            "release_publication",
            "compatibility_certification",
            "public_sdk_stability",
            "automatic_acceptance",
        ):
            require(boundaries.get(key) is False, f"stable decision boundary {key} must remain false.", errors)

    if SUMMARY.exists():
        summary = SUMMARY.read_text(encoding="utf-8")
        for phrase in (
            "Stable-Promotion Hold",
            "request-changes",
            "not automatic acceptance",
            "not release publication",
            "not compatibility certification",
        ):
            require(phrase in summary, f"decision summary is missing phrase {phrase!r}.", errors)

    if REVIEWER_NOTES.exists():
        notes = REVIEWER_NOTES.read_text(encoding="utf-8")
        require("final stable artistic acceptance was not supplied" in notes, "stable reviewer notes must record the missing final acceptance.", errors)
        require("accepted-for-stable" in notes, "stable reviewer notes must list the accepted-for-stable option.", errors)
        require("compatibility certification" in notes, "stable reviewer notes must preserve compatibility certification boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 final stable artistic decision checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
