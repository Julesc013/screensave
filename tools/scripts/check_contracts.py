"""Validate versioned ScreenSave contract scaffolding."""

from __future__ import annotations

import pathlib
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
VERSION_PATH = ROOT / "VERSION.toml"

REQUIRED_CONTRACTS = {
    ROOT / "contracts" / "README.md": [
        "versioned ScreenSave contracts",
        "Do not treat a draft contract as implemented runtime behavior.",
    ],
    ROOT / "contracts" / "portable_semantics_v2.md": [
        "Status: accepted portable seam for the Gate C Nocturne/Ricochet canary slice.",
        "not a public SDK stability claim",
        "Portable Layer May Know",
        "Portable Layer Must Not Know",
        "Product-Local IR Rule",
        "Mandatory Reference Path",
        "Compatibility Shim Rule",
        "`HWND`",
        "`HDC`",
        "X11 windows",
    ],
    ROOT / "contracts" / "renderer_aliases_v1.md": [
        "Status: active compatibility contract.",
        "`gdi`",
        "`gl11`",
        "`gl21`",
        "`gl33`",
        "`gl46`",
        "`null`",
        "gdi` remains mandatory",
    ],
    ROOT / "contracts" / "screensave_doctrine_v1.md": [
        "Status: active product doctrine.",
        "Portable meaning. Native delivery. Deterministic proof. Optional automation.",
        "Share mechanics.",
        "Preserve meaning.",
        "AIDE governs bounded development activity around ScreenSave. ScreenSave governs the product.",
        "Each saver product owns its visual thesis",
    ],
    ROOT / "contracts" / "product_architecture_v1.md": [
        "Status: active ScreenSave product architecture contract.",
        "Core is primary. Anthology remains a saver.",
        "VisualIntent is descriptive authoring intent only.",
        "ScreenSave must not adopt one universal visual graph.",
        "at least two structurally different products",
        "AIDE is optional external development infrastructure.",
        "generic `screensave run anything`",
    ],
    ROOT / "contracts" / "plasma_instrument_architecture_v1.md": [
        "Status: active PAW-I instrument-architecture audit contract.",
        "Plasma is not a preset picker. Plasma is a visual instrument.",
        "direct controls first",
        "plasma_v2_spec",
        "plasma_v2_plan",
        "plasma_v2_runtime",
        "software_reference_is_canonical",
        "gl11_is_not_hidden_minimum",
        "aide_not_runtime_or_truth",
        "plasma-v2-instrument-repair",
        "products/savers/plasma/docs/plasma-v2-instrument-constitution.md",
    ],
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-instrument-constitution.md": [
        "Status: active product constitution for PAW-I-R.",
        "Plasma is not a preset picker. Plasma is a visual instrument.",
        "`plasma_v2_spec` is product meaning.",
        "`plasma_v2_plan` is resolved executable truth.",
        "`plasma_v2_runtime` owns deterministic state and buffers.",
        "The software/reference path is canonical.",
        "AIDE does not own product truth",
    ],
    ROOT / "contracts" / "proof_bundle_v0.md": [
        "Status: initial contract",
        "source commit and dirty status",
        "artifact profile",
        "binary audit facts",
        "fallback and degradation reasons",
        "A binary audit without runtime execution may support `binary-audited`",
        "Adapter proof receipts may embed artifact-profile PE audit facts",
    ],
    ROOT / "contracts" / "proof_bundle_v1.md": [
        "Status: active contract for normalized proof receipts.",
        "result_axes",
        "`execution`",
        "`capture`",
        "`comparison`",
        "`artifact_audit`",
        "`compatibility`",
        "`artistic_review`",
        "`release_promotion`",
        "does not certify an operating system",
    ],
    ROOT / "contracts" / "surface_rgba8_v0.md": [
        "Status: private proof-kernel contract.",
        "channel order: red, green, blue, alpha",
        "row order: top-left origin",
        "row stride: `width * 4`",
        "No primitive may delegate proof output to unspecified platform rasterization.",
    ],
    ROOT / "contracts" / "proof_kernel_v0.md": [
        "Status: complete implementation milestone.",
        "canonical catalog authority",
        "minimal software/reference renderer",
        "compiled Nocturne canary runner",
        "real Nocturne product session and render functions",
        "fixed-step headless canary host",
        "exact repeatability comparison",
        "Proof Kernel v1 is the target for",
        "The current active program is `plasma-v2-reference-slice`",
        "The first canary is `nocturne`.",
        "AIDE remains an optional evidence consumer",
    ],
    ROOT / "contracts" / "libsslab_abi_v0.md": [
        "Status: active private proof-runtime ABI.",
        "not the portable v2 semantic seam",
        "sizeof(unsigned long) == 4",
        "Opaque handles must be used",
        "Python must not:",
        "Nocturne and Ricochet run through one compiled library",
    ],
    ROOT / "tools" / "sslab" / "include" / "screensave" / "sslab.h": [
        "SCREENSAVE_SSLAB_H",
        "SSLAB_ABI_VERSION 0ul",
        "SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES 4ul",
        "typedef struct sslab_context sslab_context;",
        "typedef struct sslab_product sslab_product;",
        "typedef struct sslab_session sslab_session;",
        "sslab_check_host_abi",
        "sslab_run_lifecycle",
        "sslab_run_profile",
    ],
    ROOT / "contracts" / "project_adapter_v0.md": [
        "Status: active ScreenSave development protocol.",
        "status",
        "capabilities",
        "catalog",
        "validate",
        "render",
        "compare",
        "audit",
        "proof",
        "AIDE may consume receipts",
        "tools/project_adapter/capability_bindings.json",
        "tools/project_adapter/receipt_schemas.json",
        "tools/project_adapter/artifact_profile_audit_roots.json",
        "Admission Model",
        "blocked worker sessions",
        "generic command execution",
        "accept arbitrary output paths",
        "proof-receipt PE audit fields record binary facts only",
    ],
    ROOT / "contracts" / "visual_intent_v1.md": [
        "Status: draft authoring contract, not executable runtime behavior.",
        "not a universal graphics language",
        "product-family resolver",
        "product-specific specification",
        "must not bypass product schemas, proof",
        "generate C source",
    ],
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def load_toml(path: pathlib.Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def main() -> int:
    errors: list[str] = []

    for path, needles in REQUIRED_CONTRACTS.items():
        require(path.exists(), f"Missing contract path: {path.relative_to(ROOT)}", errors)
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8")
        for needle in needles:
            require(needle in text, f"{path.relative_to(ROOT)} is missing expected text: {needle!r}", errors)

    version = load_toml(VERSION_PATH)
    require(version.get("abi", {}).get("portable_contract") == "accepted-v2", "VERSION.toml must record portable_contract accepted-v2.", errors)
    require(version.get("proof", {}).get("bundle_schema") == "proof-bundle-v0", "VERSION.toml must point at proof-bundle-v0.", errors)
    require(
        version.get("proof", {}).get("normalized_bundle_schema") == "proof-bundle-v1",
        "VERSION.toml must point at normalized proof-bundle-v1.",
        errors,
    )
    require(version.get("schemas", {}).get("screensave_doctrine") == 1, "VERSION.toml schemas.screensave_doctrine must be 1.", errors)
    require(version.get("schemas", {}).get("product_architecture") == 1, "VERSION.toml schemas.product_architecture must be 1.", errors)
    require(version.get("schemas", {}).get("proof_kernel") == 1, "VERSION.toml schemas.proof_kernel must be 1.", errors)
    require(version.get("schemas", {}).get("proof_bundle_normalized") == 1, "VERSION.toml schemas.proof_bundle_normalized must be 1.", errors)
    require(version.get("schemas", {}).get("surface_rgba8") == 1, "VERSION.toml schemas.surface_rgba8 must be 1.", errors)
    require(version.get("schemas", {}).get("project_adapter") == 1, "VERSION.toml schemas.project_adapter must be 1.", errors)
    require(version.get("schemas", {}).get("visual_intent") == 1, "VERSION.toml schemas.visual_intent must be 1.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
