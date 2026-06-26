"""Validate the Plasma v2 stable-candidate visual review round."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "visual_review_v1.md"
REVIEW_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-candidate-review"
SUMMARY_JSON = REVIEW_DIR / "review-summary.json"
SUMMARY_MD = REVIEW_DIR / "review-summary.md"
CONTACT_SHEETS = REVIEW_DIR / "contact-sheets" / "README.md"
DECISION_DIR = REVIEW_DIR / "reviewer-decisions"

DECISION_CLASSES = {"accepted-for-stable-candidate", "request-changes", "defer-to-labs", "reject"}
REQUIRED_DIMENSIONS = {
    "default_profile",
    "migrated_materials",
    "visualintent_candidates",
    "accelerated_path",
    "dark_room_comfort",
    "control_influence",
    "luminance_safety",
    "artifacting",
    "motion_quality",
    "fallback_degradation_honesty",
}
NON_DECISIONS = {
    "stable_release",
    "compatibility_certification",
    "core_promotion",
    "all_platform_support",
    "public_sdk_stability",
    "automatic_product_promotion",
    "final_artistic_acceptance",
}


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [CONTRACT, SUMMARY_JSON, SUMMARY_MD, CONTACT_SHEETS]:
        require(path.exists(), f"Missing stable-candidate review path: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        require("accepted-for-stable-candidate" in text, "Visual Review v1 must admit stable-candidate decision classes.", errors)
        require("does not mean stable release" in text, "Visual Review v1 must preserve stable-release boundary.", errors)

    decisions = sorted(DECISION_DIR.glob("*.toml")) if DECISION_DIR.exists() else []
    require(bool(decisions), "Stable-candidate review must include at least one reviewer decision.", errors)
    for decision_path in decisions:
        decision = load_toml(decision_path)
        require(decision.get("schema_version") == 1, f"{decision_path.relative_to(ROOT)} schema_version must be 1.", errors)
        require(decision.get("product") == "plasma", f"{decision_path.relative_to(ROOT)} product must be plasma.", errors)
        require(decision.get("profile") == "plasma.v2.reference.preview", f"{decision_path.relative_to(ROOT)} profile mismatch.", errors)
        require(decision.get("decision_class") in DECISION_CLASSES, f"{decision_path.relative_to(ROOT)} decision class invalid.", errors)
        require(set(decision.get("dimensions", {}).keys()) == REQUIRED_DIMENSIONS, f"{decision_path.relative_to(ROOT)} dimensions mismatch.", errors)
        require(NON_DECISIONS <= set(decision.get("explicit_non_decisions", [])), f"{decision_path.relative_to(ROOT)} non-decisions incomplete.", errors)
        require("not stable release" in decision.get("claim_boundary", ""), f"{decision_path.relative_to(ROOT)} must block stable release.", errors)
        for ref in decision.get("reviewed_capture_refs", []):
            require((ROOT / str(ref)).exists(), f"{decision_path.relative_to(ROOT)} review ref missing: {ref}", errors)

    if SUMMARY_JSON.exists():
        summary = load_json(SUMMARY_JSON)
        require(summary.get("status") == "pass", "review summary must pass.", errors)
        require(summary.get("decision_class") == "accepted-for-stable-candidate", "review summary decision mismatch.", errors)
        require(summary.get("stable_release") is False, "review summary must keep stable_release false.", errors)
        require(set(summary.get("reviewed_dimensions", [])) == REQUIRED_DIMENSIONS, "review summary dimensions mismatch.", errors)
        require("not stable release" in summary.get("claim_boundary", ""), "review summary must block stable release.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 stable-candidate visual review round checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
