"""Validate ScreenSave artifact-set and proof-profile catalog authority."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
PRODUCTS_PATH = ROOT / "catalog" / "products.toml"
PROFILES_PATH = ROOT / "catalog" / "artifact_profiles.toml"
ARTIFACT_SETS_PATH = ROOT / "catalog" / "artifact_sets.toml"
PROOF_PROFILES_PATH = ROOT / "catalog" / "proof_profiles.toml"
ARTIFACT_MANIFEST_TOOL = ROOT / "tools" / "artifactmanifest" / "artifactmanifest.py"
PE_AUDIT_TOOL = ROOT / "tools" / "scripts" / "audit_pe_artifacts.py"


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def validate_unique_keys(items: list[dict], label: str, errors: list[str]) -> set[str]:
    seen: set[str] = set()
    for item in items:
        key = item.get("key")
        require(isinstance(key, str) and bool(key), f"{label} entry must define a key.", errors)
        if isinstance(key, str):
            require(key not in seen, f"{label} key is duplicated: {key}", errors)
            seen.add(key)
    return seen


def product_maps(products_data: dict) -> tuple[dict[str, dict], dict[str, str]]:
    products = products_data.get("products", [])
    by_key = {item.get("key"): item for item in products if isinstance(item.get("key"), str)}
    kind_by_key = {key: str(item.get("kind")) for key, item in by_key.items()}
    return by_key, kind_by_key


def validate_artifact_sets(
    artifact_sets_data: dict,
    profile_keys: set[str],
    kind_by_product: dict[str, str],
    errors: list[str],
) -> set[str]:
    require(artifact_sets_data.get("schema_version") == 1, "artifact_sets.toml schema_version must be 1.", errors)
    artifact_sets = artifact_sets_data.get("artifact_sets", [])
    require(isinstance(artifact_sets, list) and artifact_sets, "artifact_sets.toml must define artifact_sets.", errors)
    artifact_set_keys = validate_unique_keys(artifact_sets, "artifact set", errors)

    for item in artifact_sets:
        key = item.get("key")
        profile = item.get("artifact_profile")
        extension = item.get("extension")
        expected_products = item.get("expected_products", [])
        expected_count = item.get("expected_count")
        require(profile in profile_keys, f"artifact set {key} references unknown artifact profile {profile!r}.", errors)
        require(isinstance(item.get("build_lane"), str) and item.get("build_lane"), f"artifact set {key} must define build_lane.", errors)
        require(isinstance(item.get("root"), str) and item.get("root"), f"artifact set {key} must define root.", errors)
        require("{product}" in str(item.get("path_template", "")), f"artifact set {key} path_template must contain {{product}}.", errors)
        require(extension in {".scr", ".exe"}, f"artifact set {key} must use .scr or .exe extension.", errors)
        require(isinstance(expected_products, list) and expected_products, f"artifact set {key} must define expected_products.", errors)
        require(expected_count == len(expected_products), f"artifact set {key} expected_count must match expected_products length.", errors)
        require(len(set(expected_products)) == len(expected_products), f"artifact set {key} must not duplicate expected products.", errors)

        for product in expected_products:
            require(product in kind_by_product, f"artifact set {key} references unknown product {product!r}.", errors)
            if extension == ".scr":
                require(kind_by_product.get(product) == "saver", f"artifact set {key} .scr product {product} must be a saver.", errors)
            if extension == ".exe":
                require(kind_by_product.get(product) == "app", f"artifact set {key} .exe product {product} must be an app.", errors)

    return artifact_set_keys


def validate_proof_profiles(
    proof_profiles_data: dict,
    artifact_set_keys: set[str],
    product_keys: set[str],
    errors: list[str],
) -> None:
    require(proof_profiles_data.get("schema_version") == 1, "proof_profiles.toml schema_version must be 1.", errors)
    proof_profiles = proof_profiles_data.get("proof_profiles", [])
    require(isinstance(proof_profiles, list) and proof_profiles, "proof_profiles.toml must define proof_profiles.", errors)
    validate_unique_keys(proof_profiles, "proof profile", errors)

    statuses = {"implemented", "planned", "retired"}
    classes = {"exact", "tolerant", "perceptual", "observational"}
    lifecycle_ops = {"create", "resize", "step", "render", "destroy"}

    for item in proof_profiles:
        key = item.get("key")
        status = item.get("status")
        require(status in statuses, f"proof profile {key} has unsupported status {status!r}.", errors)
        require(item.get("product") in product_keys, f"proof profile {key} references an unknown product.", errors)
        require(isinstance(item.get("width"), int) and item.get("width") > 0, f"proof profile {key} must define positive width.", errors)
        require(isinstance(item.get("height"), int) and item.get("height") > 0, f"proof profile {key} must define positive height.", errors)
        require(isinstance(item.get("seed"), int), f"proof profile {key} must define seed.", errors)
        require(isinstance(item.get("delta_ms"), int) and item.get("delta_ms") > 0, f"proof profile {key} must define positive delta_ms.", errors)
        require(item.get("comparison_class") in classes, f"proof profile {key} has unsupported comparison_class.", errors)
        lifecycle = set(item.get("lifecycle", []))
        require({"create", "step", "render", "destroy"} <= lifecycle, f"proof profile {key} must require core lifecycle operations.", errors)
        require(lifecycle <= lifecycle_ops, f"proof profile {key} has unknown lifecycle operations.", errors)
        for artifact_set in item.get("artifact_sets", []):
            require(artifact_set in artifact_set_keys, f"proof profile {key} references unknown artifact set {artifact_set!r}.", errors)
        if status == "implemented":
            baseline_capture = ROOT / str(item.get("baseline_capture", ""))
            baseline_proof = ROOT / str(item.get("baseline_proof", ""))
            require(baseline_capture.is_file(), f"implemented proof profile {key} baseline_capture must exist.", errors)
            require(baseline_proof.is_file(), f"implemented proof profile {key} baseline_proof must exist.", errors)
        if key == "ricochet.reference.v1":
            require(item.get("preset") == "classic_clean", "ricochet.reference.v1 must use admitted preset classic_clean.", errors)
            require(item.get("capture_frames") == [0, 4, 8, 32], "ricochet.reference.v1 must freeze frames 0, 4, 8, and 32.", errors)
            require(item.get("comparison_class") == "exact", "ricochet.reference.v1 must remain exact.", errors)


def main() -> int:
    errors: list[str] = []
    products_data = load_toml(PRODUCTS_PATH)
    artifact_profiles_data = load_toml(PROFILES_PATH)
    artifact_sets_data = load_toml(ARTIFACT_SETS_PATH)
    proof_profiles_data = load_toml(PROOF_PROFILES_PATH)
    products_by_key, kind_by_product = product_maps(products_data)
    profile_keys = {item.get("key") for item in artifact_profiles_data.get("artifact_profiles", [])}

    require(ARTIFACT_MANIFEST_TOOL.exists(), "Missing tools/artifactmanifest/artifactmanifest.py.", errors)
    require(PE_AUDIT_TOOL.exists(), "Missing tools/scripts/audit_pe_artifacts.py.", errors)
    pe_audit_text = PE_AUDIT_TOOL.read_text(encoding="utf-8") if PE_AUDIT_TOOL.exists() else ""
    require("--artifact-manifest" in pe_audit_text, "PE audit tool must accept exact artifact manifests.", errors)
    require("--artifact-profile" in pe_audit_text, "PE audit tool must accept catalog artifact profiles.", errors)
    require("artifact_manifest" in pe_audit_text, "PE audit JSON must report artifact manifest context.", errors)
    require("artifact_profile" in pe_audit_text, "PE audit JSON must report artifact profile context.", errors)
    artifact_set_keys = validate_artifact_sets(artifact_sets_data, profile_keys, kind_by_product, errors)
    validate_proof_profiles(proof_profiles_data, artifact_set_keys, set(products_by_key), errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Artifact set and proof profile checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
