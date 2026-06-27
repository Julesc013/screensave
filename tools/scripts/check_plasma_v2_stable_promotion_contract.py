"""Validate the Plasma v2 stable-promotion contract."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "plasma_stable_promotion_v1.md"
INSTRUMENT_CONTRACT = ROOT / "contracts" / "plasma_instrument_architecture_v1.md"
PLASMA_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-stable-promotion-policy.md"

AXES = [
    "release-candidate contract passed",
    "instrument architecture audit passed",
    "final proof bundle passed",
    "final visual/artistic acceptance passed",
    "native artifact package passed",
    "support/compatibility wording passed",
    "known limits accepted",
    "security/provenance/SBOM/checksum review passed",
    "Manager/Workbench inspection passed",
    "AIDE evidence and repair queue clean",
    "release publication still explicitly gated",
]

EXCLUSIONS = [
    "all-saver migration",
    "public SDK freeze",
    "broad Windows certification beyond evidence",
    "macOS/Linux support",
    "runtime executable plugin ecosystem",
    "automatic AIDE publication",
    "automatic AIDE promotion",
    "hidden preset authority",
    "hidden GL11 minimum",
    "arbitrary graph runtime",
]

OUTCOMES = [
    "stable-promoted",
    "request-changes",
    "defer-to-labs",
    "release-candidate-hold",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [CONTRACT, INSTRUMENT_CONTRACT, PLASMA_DOC]:
        require(path.exists(), f"Missing stable-promotion path: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        require("Status: active stable-promotion contract." in text, "contract status missing.", errors)
        require('stable = true' in text, "contract must explicitly permit stable true only through the gate.", errors)
        require('release_promotion = "accepted"' in text, "contract must explicitly permit accepted promotion only through the gate.", errors)
        require("release publication remains a separate" in text, "contract must keep publication separate.", errors)
        require(
            "Plasma is not a preset picker. Plasma is a visual instrument." in text,
            "contract must embed the Plasma instrument thesis.",
            errors,
        )
        for outcome in OUTCOMES:
            require(outcome in text, f"contract missing outcome {outcome}.", errors)
        for axis in AXES:
            require(axis in text, f"contract missing gate axis {axis}.", errors)
        for exclusion in EXCLUSIONS:
            require(exclusion in text, f"contract missing exclusion {exclusion}.", errors)
        for boundary in [
            "validator pass != instrument architecture acceptance",
            "proof pass != artistic acceptance",
            "artistic acceptance != compatibility certification",
            "compatibility evidence != broad support promise",
            "stable promotion != public publication",
            "AIDE EvidencePacket != ScreenSave proof truth",
        ]:
            require(boundary in text, f"contract missing claim boundary {boundary}.", errors)

    if INSTRUMENT_CONTRACT.exists():
        instrument_text = INSTRUMENT_CONTRACT.read_text(encoding="utf-8")
        for needle in [
            "Status: active PAW-I instrument-architecture audit contract.",
            "Plasma is not a preset picker. Plasma is a visual instrument.",
            "direct controls first",
            "plasma_v2_spec",
            "plasma_v2_plan",
            "plasma_v2_runtime",
            "software_reference_is_canonical",
            "gl11_is_not_hidden_minimum",
            "aide_not_runtime_or_truth",
        ]:
            require(needle in instrument_text, f"Instrument contract missing {needle!r}", errors)

    if PLASMA_DOC.exists():
        doc = PLASMA_DOC.read_text(encoding="utf-8")
        for needle in [
            "Status: stable-promotion policy, not publication.",
            "Plasma v2 may become stable only after",
            "instrument architecture",
            "direct controls, not preset selection",
            "profile: `plasma.v2.reference.preview`",
            "renderer floor: `gdi`",
            "first optional acceleration candidate: `gl11`",
            "Publication remains separately gated.",
        ]:
            require(needle in doc, f"Plasma stable-promotion doc missing {needle!r}", errors)
        for excluded in [
            "Premium",
            "glyph",
            "ribbon",
            "heightfield",
            "arbitrary graph runtime",
            "hidden GL11 minimum",
            "public SDK stability",
            "executable plug-ins",
            "all-saver migration",
        ]:
            require(excluded in doc, f"Plasma stable-promotion doc missing exclusion {excluded!r}", errors)
        require("AIDE" not in doc, "Product stable-promotion doc must not introduce AIDE references.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 stable-promotion contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
