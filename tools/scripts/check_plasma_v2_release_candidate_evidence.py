"""Validate the Plasma v2 release-candidate proof and evidence matrix."""

from __future__ import annotations

import json
import pathlib
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
MATRIX = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "proof-matrix.json"
MATRIX_MD = MATRIX.with_suffix(".md")
BUNDLE = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "proof-bundle-v1.json"
EVIDENCE_INDEX = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "evidence-index.json"

REQUIRED_ROWS = {
    "reference-software-path",
    "gl11-candidate-path",
    "default-plasma-lava",
    "five-u09-materials",
    "restrained-dither",
    "restrained-crt",
    "visualintent-candidates",
    "packc-example-packs",
    "manager-preview",
    "workbench-release-readiness",
}

REQUIRED_AXES = {
    "deterministic_proof",
    "visual_review",
    "performance_envelope",
    "artifact_build_audit",
    "release_candidate_readiness",
    "stable_release_promotion",
    "compatibility_certification",
}

REQUIRED_BUNDLE_AXES = {
    "execution",
    "capture",
    "comparison",
    "lifecycle",
    "performance",
    "artifact_audit",
    "compatibility",
    "artistic_review",
    "release_promotion",
    "portable_v2_equivalence",
}


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def repo_ref(value: str) -> pathlib.Path:
    return ROOT / value


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def validate_matrix(errors: list[str]) -> None:
    require(MATRIX.exists(), f"Missing proof matrix: {MATRIX.relative_to(ROOT)}", errors)
    require(MATRIX_MD.exists(), f"Missing proof matrix markdown: {MATRIX_MD.relative_to(ROOT)}", errors)
    if not MATRIX.exists():
        return
    matrix = load_json(MATRIX)
    require(matrix.get("schema") == "screensave.plasma-v2.release-candidate.proof-matrix.v1", "proof matrix schema mismatch.", errors)
    require(matrix.get("status") == "pass", "proof matrix status must pass.", errors)
    require(matrix.get("candidate_id") == "plasma-v2-rc1", "proof matrix candidate id must be plasma-v2-rc1.", errors)
    require("not stable release" in matrix.get("claim_boundary", ""), "proof matrix must block stable release.", errors)
    axes = set(matrix.get("claim_axes", {}))
    require(REQUIRED_AXES <= axes, f"proof matrix missing claim axes: {sorted(REQUIRED_AXES - axes)}", errors)
    require(matrix.get("claim_axes", {}).get("stable_release_promotion") == "blocked", "stable release promotion axis must be blocked.", errors)
    require(matrix.get("claim_axes", {}).get("compatibility_certification") == "not-claimed", "compatibility certification axis must be not-claimed.", errors)
    rows = matrix.get("rows", [])
    row_ids = {row.get("id") for row in rows}
    require(REQUIRED_ROWS <= row_ids, f"proof matrix missing rows: {sorted(REQUIRED_ROWS - row_ids)}", errors)
    for row in rows:
        require(row.get("status") == "pass", f"proof matrix row must pass: {row.get('id')}", errors)
        ref = row.get("evidence_ref", "")
        require(bool(ref) and repo_ref(ref).exists(), f"proof matrix evidence ref missing: {ref}", errors)


def validate_bundle(errors: list[str]) -> None:
    require(BUNDLE.exists(), f"Missing release-candidate bundle: {BUNDLE.relative_to(ROOT)}", errors)
    if not BUNDLE.exists():
        return
    bundle = load_json(BUNDLE)
    require(bundle.get("proof_schema") == "proof-bundle-v1", "release-candidate bundle must use Proof Bundle v1.", errors)
    require(bundle.get("subject", {}).get("candidate_id") == "plasma-v2-rc1", "release-candidate bundle must name rc1.", errors)
    axes = bundle.get("result_axes", {})
    require(REQUIRED_BUNDLE_AXES <= set(axes), f"release-candidate bundle missing axes: {sorted(REQUIRED_BUNDLE_AXES - set(axes))}", errors)
    require(axes.get("capture", {}).get("status") == "pass", "bundle capture axis must pass.", errors)
    require(axes.get("comparison", {}).get("status") == "pass", "bundle comparison axis must pass.", errors)
    require(axes.get("performance", {}).get("status") == "pass-with-warnings", "bundle performance axis must preserve warnings.", errors)
    require(axes.get("compatibility", {}).get("certified") is False, "bundle compatibility axis must not certify OS support.", errors)
    require(axes.get("artistic_review", {}).get("status") == "accepted-for-release-candidate", "bundle artistic axis must be release-candidate only.", errors)
    require(axes.get("artistic_review", {}).get("final_stable_artistic_acceptance") is False, "bundle artistic axis must not final-accept stable.", errors)
    require(axes.get("release_promotion", {}).get("status") == "blocked", "bundle release promotion must remain blocked.", errors)
    require("not stable release promotion" in bundle.get("claim_boundary", ""), "bundle must block stable release promotion.", errors)
    for axis in axes.values():
        for ref in axis.get("evidence_refs", []):
            require(repo_ref(ref).exists(), f"bundle evidence ref missing: {ref}", errors)


def validate_index(errors: list[str]) -> None:
    require(EVIDENCE_INDEX.exists(), f"Missing release-candidate evidence index: {EVIDENCE_INDEX.relative_to(ROOT)}", errors)
    if not EVIDENCE_INDEX.exists():
        return
    index = load_json(EVIDENCE_INDEX)
    require(index.get("schema") == "screensave.plasma-v2.release-candidate.evidence-index.v1", "evidence index schema mismatch.", errors)
    require(index.get("candidate_id") == "plasma-v2-rc1", "evidence index candidate id must be rc1.", errors)
    require("not publication" in index.get("claim_boundary", ""), "evidence index must block publication.", errors)
    entries = index.get("entries", [])
    require(len(entries) >= 7, "evidence index must include release-candidate refs.", errors)
    for entry in entries:
        require(entry.get("status") == "tracked", f"evidence entry must be tracked: {entry.get('id')}", errors)
        ref = entry.get("ref", "")
        require(bool(ref) and repo_ref(ref).exists(), f"evidence index ref missing: {ref}", errors)


def main() -> int:
    errors: list[str] = []
    validate_matrix(errors)
    validate_bundle(errors)
    validate_index(errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 release-candidate evidence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
