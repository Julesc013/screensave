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
        "Status: planned contract, not yet a runtime implementation.",
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
    ROOT / "contracts" / "proof_bundle_v0.md": [
        "Status: initial contract",
        "source commit and dirty status",
        "artifact profile",
        "binary audit facts",
        "fallback and degradation reasons",
        "A binary audit without runtime execution may support `binary-audited`",
    ],
    ROOT / "contracts" / "surface_rgba8_v0.md": [
        "Status: private proof-kernel contract.",
        "channel order: red, green, blue, alpha",
        "row order: top-left origin",
        "row stride: `width * 4`",
        "No primitive may delegate proof output to unspecified platform rasterization.",
    ],
    ROOT / "contracts" / "proof_kernel_v0.md": [
        "Status: active implementation milestone.",
        "canonical catalog authority",
        "minimal software/reference renderer",
        "fixed-step headless canary host",
        "exact repeatability comparison",
        "Proof Kernel v1 is the target for",
        "The first canary is `nocturne`.",
        "AIDE remains an optional evidence consumer",
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
        "accept arbitrary output paths",
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
    require(version.get("abi", {}).get("portable_contract") == "planned-v2", "VERSION.toml must keep portable_contract planned-v2.", errors)
    require(version.get("proof", {}).get("bundle_schema") == "proof-bundle-v0", "VERSION.toml must point at proof-bundle-v0.", errors)
    require(version.get("schemas", {}).get("screensave_doctrine") == 1, "VERSION.toml schemas.screensave_doctrine must be 1.", errors)
    require(version.get("schemas", {}).get("proof_kernel") == 1, "VERSION.toml schemas.proof_kernel must be 1.", errors)
    require(version.get("schemas", {}).get("surface_rgba8") == 1, "VERSION.toml schemas.surface_rgba8 must be 1.", errors)
    require(version.get("schemas", {}).get("project_adapter") == 1, "VERSION.toml schemas.project_adapter must be 1.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
