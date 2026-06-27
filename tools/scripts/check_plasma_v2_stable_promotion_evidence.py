"""Validate the Plasma v2 stable-promotion proof and evidence matrix."""

from __future__ import annotations

import json
import pathlib
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
EVIDENCE_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion"
MATRIX = EVIDENCE_DIR / "proof-matrix.json"
MATRIX_MD = MATRIX.with_suffix(".md")
BUNDLE = EVIDENCE_DIR / "proof-bundle-v1.json"
EVIDENCE_INDEX = EVIDENCE_DIR / "evidence-index.json"

REQUIRED_ROWS = {
    "reference-software-path",
    "gl11-candidate-path",
    "performance-envelope",
    "packc-v1-candidate",
    "visualintent-candidates",
    "manager-preview",
    "workbench-release-readiness",
    "final-artistic-stable-hold",
    "artifact-package-audit",
    "support-claims",
    "security-provenance",
}

REQUIRED_AXES = {
    "reference_execution",
    "gl11_candidate_comparison",
    "performance_envelope",
    "packc_v1_candidate",
    "visualintent_candidates",
    "manager_preview",
    "workbench_release_readiness",
    "artistic_decision",
    "artifact_package_audit",
    "support_claims",
    "security_provenance",
    "stable_promotion",
    "release_publication",
    "compatibility_certification",
}

REQUIRED_BUNDLE_AXES = {
    "reference_execution",
    "gl11_candidate_comparison",
    "performance_envelope",
    "packc_v1_candidate",
    "visualintent_candidates",
    "manager_preview",
    "workbench_release_readiness",
    "artistic_decision",
    "artifact_package_audit",
    "support_claims",
    "security_provenance",
    "stable_promotion",
    "release_publication",
    "compatibility_certification",
}


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def repo_ref(value: str) -> pathlib.Path:
    return ROOT / value


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def validate_matrix(errors: list[str]) -> None:
    require(MATRIX.exists(), f"Missing stable-promotion proof matrix: {MATRIX.relative_to(ROOT)}", errors)
    require(MATRIX_MD.exists(), f"Missing stable-promotion proof matrix markdown: {MATRIX_MD.relative_to(ROOT)}", errors)
    if not MATRIX.exists():
        return
    matrix = load_json(MATRIX)
    require(matrix.get("schema") == "screensave.plasma-v2.stable-promotion.proof-matrix.v1", "stable proof matrix schema mismatch.", errors)
    require(matrix.get("status") == "hold", "stable proof matrix status must be hold.", errors)
    require(matrix.get("candidate_id") == "plasma-v2-rc1", "stable proof matrix candidate id must be plasma-v2-rc1.", errors)
    require("blocked by final artistic acceptance hold" in matrix.get("claim_boundary", ""), "stable proof matrix must name the artistic hold.", errors)
    axes = set(matrix.get("claim_axes", {}))
    require(REQUIRED_AXES <= axes, f"stable proof matrix missing claim axes: {sorted(REQUIRED_AXES - axes)}", errors)
    require(matrix.get("claim_axes", {}).get("stable_promotion") == "blocked", "stable promotion axis must be blocked.", errors)
    require(matrix.get("claim_axes", {}).get("release_publication") == "not-performed", "release publication axis must be not-performed.", errors)
    require(matrix.get("claim_axes", {}).get("compatibility_certification") == "not-claimed", "compatibility certification axis must be not-claimed.", errors)
    require(matrix.get("claim_axes", {}).get("artistic_decision") == "request-changes", "artistic decision axis must be request-changes.", errors)
    rows = matrix.get("rows", [])
    row_ids = {row.get("id") for row in rows}
    require(REQUIRED_ROWS <= row_ids, f"stable proof matrix missing rows: {sorted(REQUIRED_ROWS - row_ids)}", errors)
    for row in rows:
        require(row.get("status") in {"pass", "pass-with-hold", "hold"}, f"stable proof matrix row has invalid status: {row.get('id')}", errors)
        ref = row.get("evidence_ref", "")
        require(bool(ref) and repo_ref(ref).exists(), f"stable proof matrix evidence ref missing: {ref}", errors)


def validate_bundle(errors: list[str]) -> None:
    require(BUNDLE.exists(), f"Missing stable-promotion bundle: {BUNDLE.relative_to(ROOT)}", errors)
    if not BUNDLE.exists():
        return
    bundle = load_json(BUNDLE)
    require(bundle.get("proof_schema") == "proof-bundle-v1", "stable-promotion bundle must use Proof Bundle v1.", errors)
    require(bundle.get("status") == "hold", "stable-promotion bundle must preserve hold.", errors)
    require(bundle.get("subject", {}).get("candidate_id") == "plasma-v2-rc1", "stable-promotion bundle must name rc1.", errors)
    axes = bundle.get("result_axes", {})
    require(REQUIRED_BUNDLE_AXES <= set(axes), f"stable-promotion bundle missing axes: {sorted(REQUIRED_BUNDLE_AXES - set(axes))}", errors)
    require(axes.get("reference_execution", {}).get("status") == "pass", "bundle reference axis must pass.", errors)
    require(axes.get("gl11_candidate_comparison", {}).get("status") == "pass", "bundle GL11 comparison axis must pass.", errors)
    require(axes.get("performance_envelope", {}).get("status") == "pass-with-warnings", "bundle performance axis must preserve warnings.", errors)
    require(axes.get("artistic_decision", {}).get("status") == "request-changes", "bundle artistic decision axis must request changes.", errors)
    require(axes.get("artistic_decision", {}).get("final_stable_artistic_acceptance") is False, "bundle artistic axis must not final-accept stable.", errors)
    require(axes.get("stable_promotion", {}).get("status") == "blocked", "bundle stable promotion must remain blocked.", errors)
    require(axes.get("stable_promotion", {}).get("stable_release") is False, "bundle stable release must remain false.", errors)
    require(axes.get("release_publication", {}).get("status") == "not-performed", "bundle publication axis must be not-performed.", errors)
    require(axes.get("compatibility_certification", {}).get("certified") is False, "bundle compatibility axis must not certify OS support.", errors)
    require("stable release promotion is blocked" in bundle.get("claim_boundary", ""), "bundle must block stable release promotion.", errors)
    for axis in axes.values():
        for ref in axis.get("evidence_refs", []):
            require(repo_ref(ref).exists(), f"bundle evidence ref missing: {ref}", errors)


def validate_index(errors: list[str]) -> None:
    require(EVIDENCE_INDEX.exists(), f"Missing stable-promotion evidence index: {EVIDENCE_INDEX.relative_to(ROOT)}", errors)
    if not EVIDENCE_INDEX.exists():
        return
    index = load_json(EVIDENCE_INDEX)
    require(index.get("schema") == "screensave.plasma-v2.stable-promotion.evidence-index.v1", "stable evidence index schema mismatch.", errors)
    require(index.get("status") == "active-hold", "stable evidence index status must be active-hold.", errors)
    require(index.get("candidate_id") == "plasma-v2-rc1", "stable evidence index candidate id must be rc1.", errors)
    require("not publication" in index.get("claim_boundary", ""), "stable evidence index must block publication.", errors)
    require("automatic artistic acceptance" in index.get("claim_boundary", ""), "stable evidence index must block automatic acceptance.", errors)
    entries = index.get("entries", [])
    require(len(entries) >= 9, "stable evidence index must include stable-promotion refs.", errors)
    for entry in entries:
        require(entry.get("status") == "tracked", f"stable evidence entry must be tracked: {entry.get('id')}", errors)
        ref = entry.get("ref", "")
        require(bool(ref) and repo_ref(ref).exists(), f"stable evidence index ref missing: {ref}", errors)


def main() -> int:
    errors: list[str] = []
    validate_matrix(errors)
    validate_bundle(errors)
    validate_index(errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 stable-promotion evidence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
