"""Validate the Plasma v2 PAW-E preview proof matrix."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
PROOF_PROFILES = ROOT / "catalog" / "proof_profiles.toml"
PROOF_REGISTRY = ROOT / "catalog" / "generated" / "proof_registry.json"
MATRIX_ROOT = ROOT / "validation" / "captures" / "plasma-v2" / "matrix"

REQUIRED_PROFILES = {
    "plasma.v2.reference.preview",
    "plasma.v2.materials.preview",
    "plasma.v2.controls.preview",
    "plasma.v2.visualintent.preview",
}
REQUIRED_MATRIX_FILES = {
    "material-distinctness.json",
    "control-influence.json",
    "visualintent-candidates.json",
    "proof-bundles/plasma-v2-reference-preview.json",
    "proof-bundles/plasma-v2-materials-preview.json",
    "proof-bundles/plasma-v2-controls-preview.json",
    "proof-bundles/plasma-v2-visualintent-preview.json",
    "contact-sheets/README.md",
}


def load_json(path: pathlib.Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def profile_map(data: dict) -> dict[str, dict]:
    return {item.get("key"): item for item in data.get("proof_profiles", [])}


def validate_profile(key: str, profile: dict, errors: list[str]) -> None:
    require(profile.get("status") == "implemented", f"{key} must be implemented.", errors)
    require(profile.get("product") == "plasma", f"{key} must target Plasma.", errors)
    require(profile.get("comparison_class") == "exact", f"{key} must remain exact.", errors)
    require("stable promotion" in profile.get("claim_boundary", ""), f"{key} must block stable promotion in its claim boundary.", errors)
    baseline_capture = ROOT / str(profile.get("baseline_capture", ""))
    baseline_proof = ROOT / str(profile.get("baseline_proof", ""))
    require(baseline_capture.is_file(), f"{key} baseline capture is missing: {profile.get('baseline_capture')}", errors)
    require(baseline_proof.is_file(), f"{key} baseline proof is missing: {profile.get('baseline_proof')}", errors)


def main() -> int:
    errors: list[str] = []
    catalog_profiles = profile_map(load_toml(PROOF_PROFILES))
    registry_profiles = profile_map(load_json(PROOF_REGISTRY))

    for key in REQUIRED_PROFILES:
        require(key in catalog_profiles, f"catalog/proof_profiles.toml missing {key}.", errors)
        require(key in registry_profiles, f"generated proof registry missing {key}.", errors)
        if key in catalog_profiles:
            validate_profile(key, catalog_profiles[key], errors)

    for relative in REQUIRED_MATRIX_FILES:
        require((MATRIX_ROOT / relative).exists(), f"Missing matrix evidence {relative}.", errors)

    material = load_json(MATRIX_ROOT / "material-distinctness.json") if (MATRIX_ROOT / "material-distinctness.json").exists() else {}
    require(material.get("status") == "pass", "material matrix summary must pass.", errors)
    require(material.get("material_count") == 5, "material matrix must cover five materials.", errors)
    require(material.get("treatment_count") == 3, "material matrix must cover three treatments.", errors)

    influence = load_json(MATRIX_ROOT / "control-influence.json") if (MATRIX_ROOT / "control-influence.json").exists() else {}
    require(influence.get("status") == "pass", "control influence matrix summary must pass.", errors)
    require(influence.get("materially_influencing_controls", 0) >= 10, "control influence matrix must retain material influence evidence.", errors)

    visualintent = load_json(MATRIX_ROOT / "visualintent-candidates.json") if (MATRIX_ROOT / "visualintent-candidates.json").exists() else {}
    require(visualintent.get("candidate_count") == 3, "VisualIntent matrix summary must contain three candidates.", errors)
    require(visualintent.get("model_calls") is False, "VisualIntent matrix summary must record no model calls.", errors)
    require(visualintent.get("network_calls") is False, "VisualIntent matrix summary must record no network calls.", errors)
    require(visualintent.get("automatic_promotion") is False, "VisualIntent matrix summary must record no automatic promotion.", errors)

    for slug in (
        "plasma-v2-reference-preview",
        "plasma-v2-materials-preview",
        "plasma-v2-controls-preview",
        "plasma-v2-visualintent-preview",
    ):
        bundle = load_json(MATRIX_ROOT / "proof-bundles" / f"{slug}.json") if (MATRIX_ROOT / "proof-bundles" / f"{slug}.json").exists() else {}
        axes = bundle.get("result_axes", {})
        require(bundle.get("proof_schema") == "proof-bundle-v1", f"{slug} must be a Proof Bundle v1 projection.", errors)
        require(axes.get("capture", {}).get("status") == "pass", f"{slug} capture axis must pass.", errors)
        require(axes.get("comparison", {}).get("status") == "pass", f"{slug} comparison axis must pass.", errors)
        require(axes.get("lifecycle", {}).get("status") == "pass", f"{slug} lifecycle axis must pass.", errors)
        require(axes.get("release_promotion", {}).get("status") == "blocked", f"{slug} release promotion axis must remain blocked.", errors)
        require("not compatibility certification" in bundle.get("claim_boundary", ""), f"{slug} bundle must block certification claims.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 matrix checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
