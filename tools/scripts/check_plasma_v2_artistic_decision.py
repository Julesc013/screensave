"""Validate the Plasma v2 final artistic decision packet scaffolding."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
DECISION_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision"
POLICY = DECISION_DIR / "decision-policy.md"
TEMPLATE = DECISION_DIR / "decision.template.toml"
RELEASE_CANDIDATE = DECISION_DIR / "decision.release-candidate.toml"
SUMMARY = DECISION_DIR / "decision-summary.md"
REVIEWER_NOTES = DECISION_DIR / "reviewer-notes" / "README.md"

REQUIRED_STATES = {
    "not-started",
    "preview-only",
    "stable-candidate-review",
    "accepted-for-release-candidate",
    "request-changes",
    "defer-to-labs",
    "rejected",
}

REQUIRED_EVIDENCE = {
    "proof_bundle",
    "stable_candidate_review",
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


def check_decision_packet(path: pathlib.Path, expected_state: str, errors: list[str]) -> None:
    data = load_toml(path)
    require(data.get("product") == "plasma", f"{path.relative_to(ROOT)} must target Plasma.", errors)
    require(data.get("profile") == "plasma.v2.reference.preview", f"{path.relative_to(ROOT)} must name the preview profile.", errors)
    require(data.get("decision_state") == expected_state, f"{path.relative_to(ROOT)} must use decision_state {expected_state}.", errors)

    evidence = data.get("evidence_refs", {})
    for key in REQUIRED_EVIDENCE:
        ref = evidence.get(key, "")
        require(ref != "", f"{path.relative_to(ROOT)} is missing evidence ref {key}.", errors)
        if ref:
            require((ROOT / ref).exists(), f"{path.relative_to(ROOT)} evidence ref does not exist: {ref}.", errors)

    boundaries = data.get("boundaries", {})
    for key in (
        "final_stable_artistic_acceptance",
        "stable_release",
        "release_publication",
        "compatibility_certification",
        "public_sdk_stability",
    ):
        require(boundaries.get(key) is False, f"{path.relative_to(ROOT)} boundary {key} must remain false.", errors)


def main() -> int:
    errors: list[str] = []
    for path in (POLICY, TEMPLATE, RELEASE_CANDIDATE, SUMMARY, REVIEWER_NOTES):
        require(path.exists(), f"Missing artistic decision packet file {path.relative_to(ROOT)}.", errors)

    if not errors:
        summary_text = SUMMARY.read_text(encoding="utf-8")
        for phrase in (
            "accepted-for-release-candidate",
            "not final stable artistic acceptance",
            "not stable release promotion",
            "not release publication",
            "not compatibility certification",
            "PAW-I",
        ):
            require(phrase in summary_text, f"decision-summary.md is missing phrase {phrase!r}.", errors)

        reviewer_text = REVIEWER_NOTES.read_text(encoding="utf-8")
        require("release-candidate preparation only" in reviewer_text, "reviewer notes must preserve release-candidate-only boundary.", errors)
        require("future stable acceptance" in reviewer_text, "reviewer notes must reserve future stable acceptance.", errors)

        policy_text = POLICY.read_text(encoding="utf-8")
        for state in REQUIRED_STATES:
            require(state in policy_text, f"decision-policy.md is missing state {state}.", errors)
        for phrase in (
            "does not promote a stable release",
            "not compatibility certification",
            "not final stable artistic acceptance",
        ):
            require(phrase in policy_text, f"decision-policy.md is missing boundary phrase {phrase!r}.", errors)

        template = load_toml(TEMPLATE)
        require(set(template.get("allowed_states", [])) == REQUIRED_STATES, "decision template must list the exact decision states.", errors)
        check_decision_packet(TEMPLATE, "not-started", errors)
        check_decision_packet(RELEASE_CANDIDATE, "accepted-for-release-candidate", errors)

        candidate = load_toml(RELEASE_CANDIDATE)
        require(
            "final stable artistic acceptance remains separate" in candidate.get("decision_summary", ""),
            "release-candidate decision must keep final stable artistic acceptance separate.",
            errors,
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 artistic decision checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
