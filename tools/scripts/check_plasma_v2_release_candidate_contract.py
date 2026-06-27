"""Validate the Plasma v2 release-candidate contract."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "plasma_release_candidate_v1.md"
PLASMA_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-release-candidate-policy.md"

AXES = [
    "reference proof",
    "accelerated comparison",
    "performance envelope",
    "packc v1-candidate",
    "package stage",
    "Manager preview",
    "Workbench release-readiness",
    "support/known-limits wording",
    "visual review",
    "final artistic release-candidate decision",
    "native artifact evidence",
    "AIDE evidence",
    "release-candidate state transition",
]

EXCLUSIONS = [
    "stable release",
    "public release publication",
    "compatibility certification broadening",
    "public SDK stability",
    "all-saver migration",
    "automatic AIDE promotion",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [CONTRACT, PLASMA_DOC]:
        require(path.exists(), f"Missing release-candidate path: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        require("Status: active release-candidate contract." in text, "contract status missing.", errors)
        require("release-readiness-reviewed" in text, "contract must define input state.", errors)
        require("release-candidate" in text, "contract must define release-candidate outcome.", errors)
        require("request-changes" in text, "contract must allow request-changes outcome.", errors)
        require("defer-to-labs" in text, "contract must allow defer-to-labs outcome.", errors)
        require("plasma-v2-rc1" in text, "contract must name the first candidate identifier.", errors)
        for axis in AXES:
            require(axis in text, f"contract missing gate axis {axis}.", errors)
        for exclusion in EXCLUSIONS:
            require(exclusion in text, f"contract missing exclusion {exclusion}.", errors)
        for boundary in [
            "PE audit with zero violations is a binary fact",
            "not OS certification",
            "Only PAW-I may decide",
            "stable = false",
            'release_promotion = "blocked"',
        ]:
            require(boundary in text, f"contract missing boundary {boundary}.", errors)

    if PLASMA_DOC.exists():
        doc = PLASMA_DOC.read_text(encoding="utf-8")
        for needle in [
            "Status: release-candidate policy, not stable release.",
            "plasma-v2-rc1",
            "Package staging is not publication",
            "Artifact audit is not compatibility certification",
            "GL11 candidate evidence is not evidence for every accelerated path",
            "Manager preview is not install mutation",
            "Workbench release-readiness is not release publication",
            "stable = false",
            "release promotion remains blocked",
        ]:
            require(needle in doc, f"Plasma release-candidate doc missing {needle!r}", errors)
        require("AIDE" not in doc, "Product release-candidate doc must not introduce AIDE references.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 release-candidate contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
