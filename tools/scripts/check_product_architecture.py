"""Validate the ScreenSave product architecture contract and catalog fit."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "product_architecture_v1.md"
CATALOG = ROOT / "catalog" / "products.toml"
PROJECT_STATE = ROOT / "PROJECT_STATE.toml"
VERSION = ROOT / "VERSION.toml"

REQUIRED_CONTRACT_TEXT = [
    "Status: active ScreenSave product architecture contract.",
    "Portable meaning. Native delivery. Deterministic proof. Optional automation.",
    "Core is primary. Anthology remains a saver.",
    "VisualIntent is descriptive authoring intent only.",
    "Share mechanics.",
    "Preserve meaning.",
    "ScreenSave must not adopt one universal visual graph.",
    "at least two structurally different products",
    "Nocturne",
    "Ricochet or equivalent",
    "Plasma",
    "AIDE is optional external development infrastructure.",
    "screensave status",
    "generic `screensave run anything`",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def product_by_key(catalog: dict) -> dict[str, dict]:
    return {str(item.get("key")): item for item in catalog.get("products", [])}


def main() -> int:
    errors: list[str] = []

    for path in (CONTRACT, CATALOG, PROJECT_STATE, VERSION):
        require(path.exists(), f"Missing product architecture input: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        for needle in REQUIRED_CONTRACT_TEXT:
            require(needle in text, f"Product architecture contract missing expected text: {needle!r}", errors)

    if not errors:
        catalog = load_toml(CATALOG)
        products = product_by_key(catalog)
        state = load_toml(PROJECT_STATE)
        version = load_toml(VERSION)

        saver_defaults = catalog.get("defaults", {}).get("saver", {})
        require(
            saver_defaults.get("semantic_contract") == "product-local-ir",
            "Catalog saver defaults must preserve product-local IR semantics.",
            errors,
        )
        require(
            saver_defaults.get("reference_path_required") is True,
            "Catalog saver defaults must require a reference path.",
            errors,
        )

        for key in ("nocturne", "ricochet", "plasma", "anthology", "suite", "benchlab", "sdk"):
            require(key in products, f"Catalog must include product architecture member: {key}", errors)

        nocturne = products.get("nocturne", {})
        ricochet = products.get("ricochet", {})
        plasma = products.get("plasma", {})
        anthology = products.get("anthology", {})
        suite = products.get("suite", {})
        benchlab = products.get("benchlab", {})
        sdk = products.get("sdk", {})

        require(nocturne.get("kind") == "saver", "Nocturne must remain a saver canary.", errors)
        require(ricochet.get("kind") == "saver", "Ricochet must remain a saver canary candidate.", errors)
        require(plasma.get("kind") == "saver", "Plasma must remain a saver canary target.", errors)
        require(plasma.get("ship_posture") == "GO_WITH_CAVEATS", "Plasma posture must remain explicit.", errors)
        require(anthology.get("kind") == "saver", "Anthology must remain a saver.", errors)
        require(anthology.get("meta_saver") is True, "Anthology must remain marked as the meta-saver.", errors)
        require(suite.get("role") == "optional-manager-companion", "Suite/Manager must remain optional companion.", errors)
        require(suite.get("release_channel") == "manager", "Suite/Manager must stay on manager release channel.", errors)
        require(
            benchlab.get("role") == "diagnostics-and-proof-harness",
            "BenchLab/Workbench must remain diagnostics/proof harness lineage.",
            errors,
        )
        require(benchlab.get("release_channel") == "workbench", "BenchLab/Workbench must stay on workbench channel.", errors)
        require(sdk.get("kind") == "sdk", "SDK product must remain kind=sdk.", errors)
        require(sdk.get("maturity") == "source-only", "SDK must remain source-only.", errors)

        forbidden_product_keys = {"aide", "codex", "omnigent"}
        require(
            not forbidden_product_keys & set(products),
            "AIDE/Codex/Omnigent must not become ScreenSave products.",
            errors,
        )

        authority = state.get("authority", {})
        proof_kernel = state.get("proof_kernel", {})
        doctrine = state.get("doctrine", {})
        require(
            authority.get("product_architecture") == "contracts/product_architecture_v1.md",
            "PROJECT_STATE authority must point to product_architecture_v1.",
            errors,
        )
        require(proof_kernel.get("canary_product") == "nocturne", "Proof Kernel v0 canary must remain Nocturne.", errors)
        require(
            "many product-owned semantic cores" in str(doctrine.get("semantic_rule", "")),
            "Doctrine must preserve many product-owned semantic cores.",
            errors,
        )
        require(
            version.get("schemas", {}).get("product_architecture") == 1,
            "VERSION.toml schemas.product_architecture must be 1.",
            errors,
        )
        require(
            version.get("contracts", {}).get("product_architecture") == "contracts/product_architecture_v1.md",
            "VERSION.toml contracts.product_architecture must point to product_architecture_v1.",
            errors,
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Product architecture checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

