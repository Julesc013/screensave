"""Validate Plasma v2 release-candidate Manager and Workbench review receipts."""

from __future__ import annotations

import json
import pathlib
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
MANAGER = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "manager-review.json"
WORKBENCH = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "workbench-review.json"


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def check_refs(data: dict[str, Any], label: str, errors: list[str]) -> None:
    for key, ref in data.get("refs", {}).items():
        require((ROOT / ref).exists(), f"{label} ref missing: {key}={ref}", errors)
    source = data.get("source_ref", "")
    require(bool(source) and (ROOT / source).exists(), f"{label} source_ref missing: {source}", errors)


def main() -> int:
    errors: list[str] = []
    for path in (MANAGER, WORKBENCH):
        require(path.exists(), f"Missing release-candidate review receipt: {path.relative_to(ROOT)}", errors)

    if MANAGER.exists():
        manager = load_json(MANAGER)
        require(manager.get("schema") == "screensave.plasma-v2.release-candidate.manager-review.v1", "Manager review schema mismatch.", errors)
        require(manager.get("status") == "pass", "Manager review must pass.", errors)
        require(manager.get("candidate_id") == "plasma-v2-rc1", "Manager review must name rc1.", errors)
        for key in ("pack_manifest", "provenance", "license", "compatibility_refusal_status", "proof_refs", "rollback_notes", "installability_class"):
            require(manager.get("shows", {}).get(key) is True, f"Manager review must show {key}.", errors)
        for key in ("install_mutation", "release_publication", "stable_promotion"):
            require(manager.get("mutation", {}).get(key) is False, f"Manager review must keep {key} false.", errors)
        require("no install mutation" in manager.get("claim_boundary", ""), "Manager review must block install mutation.", errors)
        require("compatibility certification" in manager.get("claim_boundary", ""), "Manager review must block certification.", errors)
        check_refs(manager, "Manager review", errors)

    if WORKBENCH.exists():
        workbench = load_json(WORKBENCH)
        require(workbench.get("schema") == "screensave.plasma-v2.release-candidate.workbench-review.v1", "Workbench review schema mismatch.", errors)
        require(workbench.get("status") == "pass", "Workbench review must pass.", errors)
        require(workbench.get("candidate_id") == "plasma-v2-rc1", "Workbench review must name rc1.", errors)
        for key in ("proof_status", "performance_envelope", "visual_review", "package_stage", "release_candidate_readiness", "promotion_blocked"):
            require(workbench.get("shows", {}).get(key) is True, f"Workbench review must show {key}.", errors)
        for key in ("publishes_release", "promotes_stable", "certifies_compatibility", "duplicates_proof_runner"):
            require(workbench.get("authority", {}).get(key) is False, f"Workbench review must keep {key} false.", errors)
        require("no release publication" in workbench.get("claim_boundary", ""), "Workbench review must block publication.", errors)
        require("duplicate proof authority" in workbench.get("claim_boundary", ""), "Workbench review must block duplicate proof authority.", errors)
        check_refs(workbench, "Workbench review", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 release-candidate review checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
