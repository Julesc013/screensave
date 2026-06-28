"""Validate the deterministic VisualIntent-to-Plasma v2 spec resolver."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
RESOLVER = ROOT / "tools" / "visualintent" / "resolve_plasma.py"
SCHEMA = ROOT / "tools" / "visualintent" / "schemas" / "visual_intent_v1.schema.json"
EXAMPLES = ROOT / "tools" / "visualintent" / "examples" / "plasma"
CONTRACT = ROOT / "contracts" / "visual_intent_v1.md"
OUT_DIR = ROOT / "out" / "checks" / "visualintent"
SPEC_REDUCTION_REPORT = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit" / "visualintent" / "spec-reduction-report.json"
FIXTURES = [
    "calm_dark.toml",
    "warm_slow_lava.toml",
    "cool_aurora.toml",
    "high_contrast_phosphor.toml",
]
SPEC_KEYS = {
    "schema_id",
    "schema_version",
    "field_family",
    "scale",
    "complexity",
    "motion_speed",
    "warp_amount",
    "feedback_amount",
    "output_style",
    "material",
    "brightness",
    "contrast",
    "treatment",
    "quality_intent",
    "seed_policy",
    "presentation",
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def repo_ref_exists(ref: Any) -> bool:
    return isinstance(ref, str) and (ROOT / ref).exists()


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_resolver(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(RESOLVER), *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def stable_projection(summary: dict[str, Any]) -> dict[str, Any]:
    return {
        "schema_version": summary.get("schema_version"),
        "input_schema": summary.get("input_schema"),
        "target_product": summary.get("target_product"),
        "candidate_count": summary.get("candidate_count"),
        "candidate_count_default": summary.get("candidate_count_default"),
        "candidate_count_max": summary.get("candidate_count_max"),
        "candidate_policy": summary.get("candidate_policy"),
        "model_calls": summary.get("model_calls"),
        "network_calls": summary.get("network_calls"),
        "source_code_generation": summary.get("source_code_generation"),
        "runtime_mutation": summary.get("runtime_mutation"),
        "automatic_promotion": summary.get("automatic_promotion"),
        "compatibility_certification": summary.get("compatibility_certification"),
        "stable_release": summary.get("stable_release"),
        "normalized_intent": summary.get("normalized_intent"),
        "candidates": [
            {
                "candidate_id": candidate.get("candidate_id"),
                "variant": candidate.get("variant"),
                "product": candidate.get("product"),
                "profile": candidate.get("profile"),
                "plasma_v2_spec": candidate.get("plasma_v2_spec"),
                "rationale": candidate.get("rationale"),
                "constraints_applied": candidate.get("constraints_applied"),
                "warnings": candidate.get("warnings"),
                "refusals": candidate.get("refusals"),
                "claim_boundary": candidate.get("claim_boundary"),
            }
            for candidate in summary.get("candidates", [])
        ],
    }


def validate_summary(summary: dict[str, Any], errors: list[str], label: str) -> None:
    require(summary.get("schema_version") == "screensave.visualintent.plasma.resolution.v1", f"{label}: resolver summary schema mismatch.", errors)
    require(summary.get("input_schema") == "screensave.visual_intent.v1", f"{label}: resolver input schema mismatch.", errors)
    require(summary.get("target_product") == "plasma", f"{label}: resolver target_product must be plasma.", errors)
    require(summary.get("candidate_count") == 3, f"{label}: resolver must emit exactly three candidates.", errors)
    require(summary.get("candidate_count_default") == 3, f"{label}: resolver must record default candidate count.", errors)
    require(summary.get("candidate_count_max") == 5, f"{label}: resolver must record max candidate count.", errors)
    require(summary.get("model_calls") is False, f"{label}: resolver must record no model calls.", errors)
    require(summary.get("network_calls") is False, f"{label}: resolver must record no network calls.", errors)
    require(summary.get("source_code_generation") is False, f"{label}: resolver must record no source generation.", errors)
    require(summary.get("runtime_mutation") is False, f"{label}: resolver must record no runtime mutation.", errors)
    require(summary.get("automatic_promotion") is False, f"{label}: resolver must record no automatic promotion.", errors)
    require(summary.get("compatibility_certification") is False, f"{label}: resolver must record no compatibility certification.", errors)
    require(summary.get("stable_release") is False, f"{label}: resolver must record no stable release.", errors)

    candidates = summary.get("candidates", [])
    require(len(candidates) == 3, f"{label}: resolver candidate array must contain three records.", errors)
    seen_ids: set[str] = set()
    for candidate in candidates:
        candidate_id = candidate.get("candidate_id")
        require(isinstance(candidate_id, str) and candidate_id not in seen_ids, f"{label}: candidate IDs must be stable and unique.", errors)
        if isinstance(candidate_id, str):
            seen_ids.add(candidate_id)
        require(candidate.get("product") == "plasma", f"{label}: candidate product must be plasma.", errors)
        require(candidate.get("profile") == "plasma.v2.visualintent.preview", f"{label}: candidate profile must be the VisualIntent preview profile.", errors)
        require(candidate.get("warnings") == [], f"{label}: admitted fixture must not produce warnings.", errors)
        require(candidate.get("refusals") == [], f"{label}: admitted fixture must not produce refusals.", errors)
        require(isinstance(candidate.get("rationale"), str) and candidate.get("rationale"), f"{label}: candidate rationale must be textual.", errors)
        require("runtime truth" in candidate.get("claim_boundary", ""), f"{label}: candidate boundary must deny runtime truth.", errors)
        for key in ["pack_toml", "compiled_manifest", "hash_manifest", "proof_receipt", "spec_json"]:
            require(key in candidate, f"{label}: candidate record missing {key}.", errors)
            require(repo_ref_exists(candidate.get(key)), f"{label}: candidate ref does not exist: {candidate.get(key)}", errors)
        compiled_manifest_ref = candidate.get("compiled_manifest")
        if repo_ref_exists(compiled_manifest_ref):
            compiled_manifest_path = ROOT / str(compiled_manifest_ref)
            compiled_manifest = load_json(compiled_manifest_path)
            require(compiled_manifest.get("proof_profile") == "plasma.v2.visualintent.preview", f"{label}: compiled candidate manifest must retain the VisualIntent proof profile.", errors)
            compiled_spec_path = compiled_manifest_path.parent / "plasma-spec-v2.json"
            proof_ref_path = compiled_manifest_path.parent / "proof-profile-ref.json"
            require(compiled_spec_path.exists(), f"{label}: compiled candidate spec must exist.", errors)
            require(proof_ref_path.exists(), f"{label}: compiled candidate proof-profile ref must exist.", errors)
            if compiled_spec_path.exists():
                compiled_spec = load_json(compiled_spec_path)
                require(compiled_spec.get("proof_profile") == "plasma.v2.visualintent.preview", f"{label}: compiled candidate spec must retain the VisualIntent proof profile.", errors)
            if proof_ref_path.exists():
                proof_ref = load_json(proof_ref_path)
                require(proof_ref.get("proof_profile") == "plasma.v2.visualintent.preview", f"{label}: compiled proof ref must retain the VisualIntent proof profile.", errors)
        spec = candidate.get("plasma_v2_spec", {})
        require(set(spec) == SPEC_KEYS, f"{label}: candidate spec must carry exactly the admitted Plasma v2 spec keys.", errors)
        require(spec.get("schema_id") == "screensave.plasma.spec.v2", f"{label}: candidate spec must use Plasma v2 schema.", errors)
        require(spec.get("schema_version") == 2, f"{label}: candidate spec schema_version must be 2.", errors)
        require(spec.get("quality_intent") == "safe", f"{label}: candidate spec must keep safe quality intent.", errors)
        require(spec.get("seed_policy") == "fixed", f"{label}: candidate spec must keep fixed seed policy.", errors)
        require(spec.get("presentation") == "flat", f"{label}: candidate spec must keep flat presentation.", errors)


def main() -> int:
    errors: list[str] = []
    for path in [RESOLVER, SCHEMA, EXAMPLES, CONTRACT, SPEC_REDUCTION_REPORT]:
        require(path.exists(), f"Missing VisualIntent resolver input {path.relative_to(ROOT)}.", errors)
    for fixture in FIXTURES:
        require((EXAMPLES / fixture).exists(), f"Missing VisualIntent fixture tools/visualintent/examples/plasma/{fixture}.", errors)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    schema = load_json(SCHEMA)
    require(schema.get("properties", {}).get("candidate_count", {}).get("default") == 3, "VisualIntent schema must record candidate_count default 3.", errors)
    require(schema.get("properties", {}).get("candidate_count", {}).get("maximum") == 5, "VisualIntent schema must cap candidate_count at 5.", errors)
    report = load_json(SPEC_REDUCTION_REPORT)
    require(report.get("status") == "pass", "VisualIntent spec-reduction report must pass.", errors)
    require(report.get("visualintent_candidates_reduce_to_plasma_spec") is True, "VisualIntent spec-reduction report must clear the spec-reduction claim.", errors)
    require(report.get("fixtures_checked") == 4, "VisualIntent spec-reduction report must record four fixtures.", errors)
    require(report.get("candidate_count_per_fixture") == 3, "VisualIntent spec-reduction report must record three candidates per fixture.", errors)
    require(report.get("all_candidates_packc_valid") is True, "VisualIntent spec-reduction report must record packc-valid candidates.", errors)
    require(report.get("all_candidates_have_plasma_v2_spec") is True, "VisualIntent spec-reduction report must record explicit Plasma specs.", errors)
    require(report.get("executable_payloads_detected") is False, "VisualIntent spec-reduction report must deny executable payloads.", errors)
    require(report.get("network_or_model_calls") is False, "VisualIntent spec-reduction report must deny network/model calls.", errors)

    contract_text = CONTRACT.read_text(encoding="utf-8")
    for needle in [
        "Product Reduction Law",
        "VisualIntent cannot execute",
        "VisualIntent cannot bypass product-specific schemas",
        "explicit `plasma_v2_spec`",
        "plasma_v2_plan",
        "`candidate_count` defaults to `3`",
        "`candidate_count` must never exceed `5`",
        "candidate rationale must be textual evidence only",
        "generated candidates are review inputs",
    ]:
        require(needle in contract_text, f"VisualIntent contract missing resolver law text: {needle}", errors)

    if OUT_DIR.exists():
        shutil.rmtree(OUT_DIR)
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    for fixture in FIXTURES:
        source = EXAMPLES / fixture
        first_out = OUT_DIR / source.stem / "first"
        second_out = OUT_DIR / source.stem / "second"
        first_result = run_resolver(["resolve", str(source), "--out", str(first_out)])
        require(first_result.returncode == 0, f"{fixture}: resolver command form must pass: {first_result.stderr}", errors)
        second_result = run_resolver([str(source), "--out", str(second_out)])
        require(second_result.returncode == 0, f"{fixture}: resolver direct form must pass: {second_result.stderr}", errors)
        first_summary_path = first_out / "visualintent-candidates.json"
        second_summary_path = second_out / "visualintent-candidates.json"
        require(first_summary_path.exists(), f"{fixture}: resolver must emit first summary.", errors)
        require(second_summary_path.exists(), f"{fixture}: resolver must emit second summary.", errors)
        if first_summary_path.exists() and second_summary_path.exists():
            first_summary = load_json(first_summary_path)
            second_summary = load_json(second_summary_path)
            validate_summary(first_summary, errors, fixture)
            require(
                stable_projection(first_summary) == stable_projection(second_summary),
                f"{fixture}: resolver stable candidate content must be repeatable across output dirs.",
                errors,
            )

    parent_out = run_resolver(["resolve", str(EXAMPLES / FIXTURES[0]), "--out", str(pathlib.Path("..") / "visualintent-outside")])
    require(parent_out.returncode != 0, "VisualIntent resolver must reject parent traversal output outside out/.", errors)
    absolute_out = run_resolver(["resolve", str(EXAMPLES / FIXTURES[0]), "--out", str(pathlib.Path.home() / "visualintent-outside")])
    require(absolute_out.returncode != 0, "VisualIntent resolver must reject absolute output outside out/.", errors)

    base_lines = [
        "visual_intent_version = 1",
        'target_product = "plasma"',
        'mood = "safe test"',
        "candidate_count = 3",
        "[appearance]",
        "brightness = 0.4",
        "contrast = 0.5",
        "density = 0.5",
        'palette = ["#010203"]',
        'palette_hint = "neutral"',
        "warmth = 0.5",
        "softness = 0.5",
        "complexity = 0.5",
        "[motion]",
        'motion_character = "slow"',
        "motion_speed = 0.2",
        "[constraints]",
        "avoid_flashing = true",
        "deterministic = true",
    ]
    negative_cases = {
        "candidate_count_too_high.toml": [line if line != "candidate_count = 3" else "candidate_count = 6" for line in base_lines],
        "unknown_target_product.toml": [line if line != 'target_product = "plasma"' else 'target_product = "nocturne"' for line in base_lines],
        "missing_required_field.toml": [line for line in base_lines if line != "brightness = 0.4"],
        "invalid_palette_hint.toml": [line if line != 'palette_hint = "neutral"' else 'palette_hint = "shader magic"' for line in base_lines],
        "attempted_executable_payload.toml": [line if line != 'mood = "safe test"' else 'mood = "run payload.exe"' for line in base_lines],
        "network_url_payload.toml": [line if line != 'mood = "safe test"' else 'mood = "http://example.invalid"' for line in base_lines],
    }
    for name, lines in negative_cases.items():
        invalid = OUT_DIR / name
        invalid.write_text("\n".join(lines) + "\n", encoding="utf-8")
        invalid_result = run_resolver(["resolve", str(invalid), "--out", str(OUT_DIR / (invalid.stem + "_out"))])
        require(invalid_result.returncode != 0, f"VisualIntent resolver must reject negative fixture {name}.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("VisualIntent resolver checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
