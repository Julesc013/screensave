"""Validate the Plasma v2 publication-prep contract."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "plasma_publication_prep_v1.md"
PLASMA_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-publication-policy.md"
STABLE_CONTRACT = ROOT / "contracts" / "plasma_stable_promotion_v1.md"

AXES = [
    "stable-promotion accepted",
    "release manifest",
    "artifact manifest",
    "checksums",
    "provenance",
    "SBOM or third-party notice",
    "known limits",
    "support matrix",
    "install notes",
    "rollback notes",
    "package-stage evidence",
    "Manager preview evidence",
    "Workbench release facts",
    "AIDE EvidencePacket",
    "no public upload yet",
    "no release page publication yet",
    "no compatibility certification broadening",
    "no all-saver migration claim",
]

CLAIM_BOUNDARIES = [
    "stable promotion != public publication",
    "publication prep != actual publication",
    "release manifest != uploaded release asset",
    "checksum record != external download verification",
    "package-stage evidence != compatibility certification",
    "current Windows evidence != preservation certification",
    "Manager preview evidence != install/apply mutation",
    "Workbench release facts != graphical Workbench MVP",
    "AIDE EvidencePacket != ScreenSave release authority",
]

PACKET_PATHS = [
    "releases/plasma-v2-stable/release-manifest.toml",
    "releases/plasma-v2-stable/artifact-manifest.toml",
    "releases/plasma-v2-stable/checksums.sha256",
    "releases/plasma-v2-stable/provenance.json",
    "releases/plasma-v2-stable/known-limits.md",
    "releases/plasma-v2-stable/support-matrix.md",
    "releases/plasma-v2-stable/install-notes.md",
    "releases/plasma-v2-stable/rollback-notes.md",
    "releases/plasma-v2-stable/third-party-notices.md",
    "releases/plasma-v2-stable/publication-checklist.md",
]

EXCLUSIONS = [
    "public release publication",
    "release asset upload",
    "GitHub release creation",
    "release page publication",
    "broad Windows preservation certification",
    "macOS/Linux support",
    "all Core saver migration",
    "public SDK freeze",
    "runtime executable plugin ecosystem",
    "Manager install/apply mutation",
    "graphical Workbench MVP",
    "automatic AIDE publication",
    "automatic AIDE promotion",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [CONTRACT, PLASMA_DOC, STABLE_CONTRACT]:
        require(path.exists(), f"Missing publication-prep contract path: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        require("Status: active publication-prep contract." in text, "contract status missing.", errors)
        require("Publication prep is not stable promotion." in text, "contract must separate stable promotion.", errors)
        require("Actual publication remains a separate act." in text, "contract must separate actual publication.", errors)
        require("Publication prep is also not compatibility certification." in text, "contract must separate compatibility certification.", errors)
        require('status = "publication-ready"' in text, "contract must define publication-ready state.", errors)
        require('status = "stable-promoted"' in text, "contract must preserve stable-promoted hold state.", errors)
        require('publication = "not-published"' in text, "contract must require not-published state.", errors)
        require('active_program = "plasma-v2-publication"' in text, "contract must define the next publication program.", errors)
        for axis in AXES:
            require(axis in text, f"contract missing gate axis {axis}.", errors)
        for boundary in CLAIM_BOUNDARIES:
            require(boundary in text, f"contract missing claim boundary {boundary}.", errors)
        for packet_path in PACKET_PATHS:
            require(packet_path in text, f"contract missing packet path {packet_path}.", errors)
        for exclusion in EXCLUSIONS:
            require(exclusion in text, f"contract missing exclusion {exclusion}.", errors)

    if PLASMA_DOC.exists():
        doc = PLASMA_DOC.read_text(encoding="utf-8")
        for needle in [
            "Status: publication-prep policy, not publication.",
            "stable-promoted rc1 slice",
            "publication is a separate release packet decision",
            "It must not upload or publish release assets.",
            "profile: `plasma.v2.reference.preview`",
            "software/reference path: canonical",
            "renderer floor: `gdi`",
            "first optional acceleration candidate: `gl11`",
            "data-only `packc` v1-candidate",
            "`plasma_v2_spec`, `plasma_v2_plan`, and `plasma_v2_runtime`",
            "broad Windows preservation certification is not created",
            "macOS, Linux",
            "public SDK stability",
            "all Core saver migration",
            "runtime executable plug-ins",
            "Manager install/apply mutation",
            "graphical Workbench authoring app",
            "later publication gate",
        ]:
            require(needle in doc, f"Plasma publication policy missing {needle!r}", errors)
        require("AIDE" not in doc, "Product publication policy must not introduce AIDE references.", errors)

    if STABLE_CONTRACT.exists():
        stable_text = STABLE_CONTRACT.read_text(encoding="utf-8")
        require(
            "stable promotion != public publication" in stable_text,
            "stable-promotion contract must already separate publication.",
            errors,
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 publication-prep contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
