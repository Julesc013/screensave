"""Validate Plasma v2 VisualIntent candidate proof evidence."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PROOF_SUMMARY = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit" / "visualintent" / "proof-summary.json"
SPEC_REDUCTION = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit" / "visualintent" / "spec-reduction-report.json"
MATRIX = ROOT / "validation" / "captures" / "plasma-v2" / "matrix" / "visualintent-candidates.json"
PROOF_BUNDLE = ROOT / "validation" / "captures" / "plasma-v2" / "matrix" / "proof-bundles" / "plasma-v2-visualintent-preview.json"
PROFILE_PROOF = ROOT / "validation" / "captures" / "plasma-v2" / "matrix" / "captures" / "visualintent-preview" / "profile-proof.json"
VISUALINTENT_CHECK = ROOT / "tools" / "scripts" / "check_visual_intent_resolver.py"
PACKC_CHECK = ROOT / "tools" / "scripts" / "check_packc.py"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def repo_ref_exists(ref: Any) -> bool:
    return isinstance(ref, str) and (ROOT / ref).exists()


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_check(path: pathlib.Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(path)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def main() -> int:
    errors: list[str] = []
    for path in [PROOF_SUMMARY, SPEC_REDUCTION, MATRIX, PROOF_BUNDLE, PROFILE_PROOF, VISUALINTENT_CHECK, PACKC_CHECK]:
        require(path.exists(), f"Missing VisualIntent proof input {repo_path(path)}.", errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    resolver = run_check(VISUALINTENT_CHECK)
    require(resolver.returncode == 0, f"VisualIntent resolver validation must pass: {resolver.stderr}", errors)
    packc = run_check(PACKC_CHECK)
    require(packc.returncode == 0, f"packc validation must pass for VisualIntent candidates: {packc.stderr}", errors)

    summary = load_json(PROOF_SUMMARY)
    reduction = load_json(SPEC_REDUCTION)
    matrix = load_json(MATRIX)
    proof_bundle = load_json(PROOF_BUNDLE)
    profile_proof = load_json(PROFILE_PROOF)

    require(summary.get("status") == "pass", "VisualIntent proof summary must pass.", errors)
    require(summary.get("fixtures_checked") == 4, "VisualIntent proof summary must cover four fixtures.", errors)
    require(summary.get("candidate_count_per_fixture") == 3, "VisualIntent proof summary must record three candidates per fixture.", errors)
    require(summary.get("candidate_specs_are_deterministic") is True, "VisualIntent proof summary must record deterministic specs.", errors)
    require(summary.get("candidate_specs_compile") is True, "VisualIntent proof summary must record compiling specs.", errors)
    require(summary.get("proof_status") == "packc-and-spec-proof-only", "VisualIntent proof summary must keep the packc/spec-only proof status.", errors)
    require(summary.get("all_candidate_capture_refs_generated") is False, "VisualIntent proof summary must not claim per-candidate captures.", errors)
    require(summary.get("proof_profile") == "plasma.v2.visualintent.preview", "VisualIntent proof summary must name the VisualIntent proof profile.", errors)
    require(repo_ref_exists(summary.get("proof_bundle_ref")), "VisualIntent proof summary must reference an existing proof bundle.", errors)
    require("not artistic acceptance" in summary.get("claim_boundary", ""), "VisualIntent proof summary must deny artistic acceptance.", errors)
    require("not stable promotion" in summary.get("claim_boundary", ""), "VisualIntent proof summary must deny stable promotion.", errors)

    require(reduction.get("status") == "pass", "VisualIntent spec reduction report must pass.", errors)
    require(reduction.get("visualintent_candidates_reduce_to_plasma_spec") is True, "VisualIntent spec reduction report must clear the reduction claim.", errors)
    require(reduction.get("all_candidates_packc_valid") is True, "VisualIntent spec reduction report must record packc-valid candidates.", errors)

    require(matrix.get("status") == "pass", "VisualIntent candidate matrix must pass.", errors)
    require(matrix.get("fixture_count") == 4, "VisualIntent candidate matrix must cover four fixtures.", errors)
    require(matrix.get("candidate_count") == 12, "VisualIntent candidate matrix must cover twelve candidates.", errors)
    require(matrix.get("proof_status") == "packc-and-spec-proof-only", "VisualIntent candidate matrix must keep packc/spec-only proof status.", errors)
    require(repo_ref_exists(matrix.get("proof_bundle_ref")), "VisualIntent candidate matrix must reference an existing proof bundle.", errors)
    require(matrix.get("model_calls") is False, "VisualIntent candidate matrix must record no model calls.", errors)
    require(matrix.get("network_calls") is False, "VisualIntent candidate matrix must record no network calls.", errors)
    require(matrix.get("automatic_promotion") is False, "VisualIntent candidate matrix must record no automatic promotion.", errors)
    for candidate in matrix.get("candidates", []):
        require(candidate.get("product") == "plasma", "VisualIntent matrix candidate must target Plasma.", errors)
        require(candidate.get("proof_profile") == "plasma.v2.visualintent.preview", "VisualIntent matrix candidate must use the preview proof profile.", errors)
        require(candidate.get("packc_status") == "pass", "VisualIntent matrix candidate must record packc pass.", errors)
        spec = candidate.get("plasma_v2_spec_summary", {})
        require(spec.get("schema_id") == "screensave.plasma.spec.v2", "VisualIntent matrix candidate must summarize a Plasma v2 spec.", errors)
        require(spec.get("quality_intent") == "safe", "VisualIntent matrix candidate must keep safe quality intent.", errors)
        require(spec.get("presentation") == "flat", "VisualIntent matrix candidate must keep flat presentation.", errors)

    require(proof_bundle.get("proof_schema") == "proof-bundle-v1", "VisualIntent proof bundle must be Proof Bundle v1.", errors)
    require(proof_bundle.get("subject", {}).get("profile") == "plasma.v2.visualintent.preview", "VisualIntent proof bundle must use the preview profile.", errors)
    capture_axis = proof_bundle.get("result_axes", {}).get("capture", {})
    require(capture_axis.get("status") == "pass", "VisualIntent proof bundle capture axis must pass.", errors)
    for ref in capture_axis.get("capture_refs", []):
        require(repo_ref_exists(ref), f"VisualIntent capture ref must exist: {ref}", errors)
    require(profile_proof.get("profile") == "plasma.v2.visualintent.preview", "VisualIntent profile proof must use the preview profile.", errors)
    require(profile_proof.get("status") == "pass", "VisualIntent profile proof must pass.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 VisualIntent proof checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
