"""Validate ScreenSave product catalog and artifact-profile authority."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
PRODUCTS_PATH = ROOT / "catalog" / "products.toml"
PROFILES_PATH = ROOT / "catalog" / "artifact_profiles.toml"

PUBLIC_RENDERERS = {"gdi", "gl11", "gl21", "gl33", "gl46"}
REQUIRED_EVIDENCE_STATUSES = {
    "certified",
    "binary-audited",
    "buildable",
    "targeted",
    "experimental",
    "unsupported",
    "retired",
}


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def validate_unique_keys(items: list[dict], label: str, errors: list[str]) -> None:
    seen: set[str] = set()
    for item in items:
        key = item.get("key")
        if not isinstance(key, str) or not key:
            errors.append(f"{label} entry must have a non-empty key.")
            continue
        if key in seen:
            errors.append(f"{label} key is duplicated: {key}")
        seen.add(key)


def validate_profiles(profiles_data: dict, errors: list[str]) -> set[str]:
    require(profiles_data.get("schema_version") == 1, "artifact_profiles.toml schema_version must be 1.", errors)

    statuses = set(profiles_data.get("evidence_statuses", {}).get("allowed", []))
    require(REQUIRED_EVIDENCE_STATUSES <= statuses, "artifact_profiles.toml must define every required evidence status.", errors)
    require(
        profiles_data.get("evidence_statuses", {}).get("public_promise") == "certified",
        "artifact_profiles.toml must reserve ordinary public promises for certified evidence.",
        errors,
    )

    profiles = profiles_data.get("artifact_profiles", [])
    require(isinstance(profiles, list) and profiles, "artifact_profiles.toml must define artifact_profiles.", errors)
    validate_unique_keys(profiles, "artifact profile", errors)

    profile_keys: set[str] = set()
    for profile in profiles:
        key = profile.get("key")
        if isinstance(key, str):
            profile_keys.add(key)

        product_classes = profile.get("product_classes")
        require(isinstance(product_classes, list) and product_classes, f"profile {key} must define product_classes.", errors)
        require(profile.get("evidence_status") in statuses, f"profile {key} uses an unknown evidence_status.", errors)
        require(profile.get("public_claim") in statuses, f"profile {key} uses an unknown public_claim.", errors)
        require(isinstance(profile.get("host"), str) and profile.get("host"), f"profile {key} must define host.", errors)
        require(isinstance(profile.get("machine"), str) and profile.get("machine"), f"profile {key} must define machine.", errors)

        mandatory = set(profile.get("mandatory_renderers", []))
        optional = set(profile.get("optional_renderers", []))
        require(mandatory <= PUBLIC_RENDERERS, f"profile {key} has unknown mandatory renderer aliases.", errors)
        require(optional <= PUBLIC_RENDERERS, f"profile {key} has unknown optional renderer aliases.", errors)
        if product_classes and "saver" in product_classes:
            require("gdi" in mandatory, f"saver profile {key} must keep gdi mandatory.", errors)
            require(profile.get("artifact_extension") == ".scr", f"saver profile {key} must emit .scr artifacts.", errors)

    return profile_keys


def default_profiles_for_kind(products_data: dict, kind: str) -> list[str]:
    defaults = products_data.get("defaults", {})
    section = defaults.get(kind, {})
    if isinstance(section, dict):
        values = section.get("artifact_profiles", [])
        if isinstance(values, list):
            return values
    return []


def validate_products(products_data: dict, profile_keys: set[str], errors: list[str]) -> None:
    require(products_data.get("schema_version") == 1, "products.toml schema_version must be 1.", errors)

    products = products_data.get("products", [])
    require(isinstance(products, list) and products, "products.toml must define products.", errors)
    validate_unique_keys(products, "product", errors)

    saver_count = 0
    for product in products:
        key = product.get("key")
        kind = product.get("kind")
        require(kind in {"saver", "app", "sdk"}, f"product {key} has unsupported kind {kind!r}.", errors)
        require(isinstance(product.get("name"), str) and product.get("name"), f"product {key} must define name.", errors)

        effective_profiles = product.get("artifact_profiles")
        if effective_profiles is None:
            effective_profiles = default_profiles_for_kind(products_data, str(kind))
        require(isinstance(effective_profiles, list) and effective_profiles, f"product {key} must resolve artifact_profiles.", errors)
        for profile_key in effective_profiles:
            require(profile_key in profile_keys, f"product {key} references unknown artifact profile {profile_key!r}.", errors)

        if kind == "saver":
            saver_count += 1
            require("windows_preservation_x86_ansi" in effective_profiles, f"saver {key} must target the preservation x86 ANSI profile.", errors)

    require(saver_count >= 19, "products.toml must retain the current standalone saver family.", errors)


def main() -> int:
    errors: list[str] = []
    products_data = load_toml(PRODUCTS_PATH)
    profiles_data = load_toml(PROFILES_PATH)

    profile_keys = validate_profiles(profiles_data, errors)
    validate_products(products_data, profile_keys, errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Catalog and artifact profile checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
