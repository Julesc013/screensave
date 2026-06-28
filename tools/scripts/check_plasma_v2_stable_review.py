"""Validate Plasma v2 stable-promotion Manager and Workbench review receipts."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
MANAGER = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "manager-final-review.json"
WORKBENCH = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "workbench-final-review.json"
DECISION = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision" / "decision.stable.toml"


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def final_artistic_accepted() -> bool:
    if not DECISION.exists():
        return False
    decision = load_toml(DECISION)
    return str(decision.get("decision_state", decision.get("decision", ""))) == "accepted-for-stable"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def check_refs(data: dict[str, Any], label: str, errors: list[str]) -> None:
    for key, ref in data.get("refs", {}).items():
        if ref.endswith("stable-promotion/proof-matrix.json") or ref.endswith("stable-promotion/proof-bundle-v1.json"):
            require(ref != "", f"{label} pending proof ref must be declared: {key}", errors)
        else:
            require((ROOT / ref).exists(), f"{label} ref missing: {key}={ref}", errors)
    source = data.get("source_ref", "")
    require(bool(source) and (ROOT / source).exists(), f"{label} source_ref missing: {source}", errors)


def main() -> int:
    errors: list[str] = []
    accepted = final_artistic_accepted()
    expected_status = "pass" if accepted else "pass-with-hold"
    for path in (MANAGER, WORKBENCH):
        require(path.exists(), f"Missing stable-promotion review receipt: {path.relative_to(ROOT)}", errors)

    if MANAGER.exists():
        manager = load_json(MANAGER)
        require(manager.get("schema") == "screensave.plasma-v2.stable-promotion.manager-review.v1", "Manager stable review schema mismatch.", errors)
        require(manager.get("status") == expected_status, f"Manager stable review status must be {expected_status}.", errors)
        require(manager.get("candidate_id") == "plasma-v2-rc1", "Manager stable review must name rc1.", errors)
        for key in ("package_manifest", "pack_manifest", "provenance", "license", "support_class", "installability_refusal_status", "proof_refs", "rollback_notes"):
            require(manager.get("shows", {}).get(key) is True, f"Manager stable review must show {key}.", errors)
        for key in ("install_mutation", "release_publication", "stable_promotion", "compatibility_certification"):
            require(manager.get("mutation", {}).get(key) is False, f"Manager stable review must keep {key} false.", errors)
        require("no install mutation" in manager.get("claim_boundary", ""), "Manager stable review must block install mutation.", errors)
        require("compatibility certification" in manager.get("claim_boundary", ""), "Manager stable review must block certification.", errors)
        check_refs(manager, "Manager stable review", errors)

    if WORKBENCH.exists():
        workbench = load_json(WORKBENCH)
        require(workbench.get("schema") == "screensave.plasma-v2.stable-promotion.workbench-review.v1", "Workbench stable review schema mismatch.", errors)
        require(workbench.get("status") == expected_status, f"Workbench stable review status must be {expected_status}.", errors)
        require(workbench.get("candidate_id") == "plasma-v2-rc1", "Workbench stable review must name rc1.", errors)
        for key in ("proof_status", "performance_envelope", "visual_decision", "support_claims", "package_stage", "promotion_state", "publication_state"):
            require(workbench.get("shows", {}).get(key) is True, f"Workbench stable review must show {key}.", errors)
        require(
            workbench.get("shows", {}).get("stable_hold") is (not accepted),
            "Workbench stable review hold flag must match the final verdict.",
            errors,
        )
        for key in ("publishes_release", "promotes_stable", "certifies_compatibility", "duplicates_proof_runner", "overrides_artistic_decision"):
            require(workbench.get("authority", {}).get(key) is False, f"Workbench stable review must keep {key} false.", errors)
        require("no release publication" in workbench.get("claim_boundary", ""), "Workbench stable review must block publication.", errors)
        require("duplicate proof authority" in workbench.get("claim_boundary", ""), "Workbench stable review must block duplicate proof authority.", errors)
        require("artistic-decision override" in workbench.get("claim_boundary", ""), "Workbench stable review must block artistic override.", errors)
        check_refs(workbench, "Workbench stable review", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 stable review checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
