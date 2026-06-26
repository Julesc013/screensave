"""Validate the Plasma v2 acceleration admission policy."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "plasma_acceleration_v1.md"
PLASMA_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-acceleration-policy.md"
GL11_HEADER = ROOT / "products" / "savers" / "plasma" / "src" / "plasma_v2_gl11.h"
GL11_SOURCE = ROOT / "products" / "savers" / "plasma" / "src" / "plasma_v2_gl11.c"
REALIZATION_HEADER = ROOT / "products" / "savers" / "plasma" / "src" / "plasma_v2_realization.h"
REALIZATION_SOURCE = ROOT / "products" / "savers" / "plasma" / "src" / "plasma_v2_realization.c"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [CONTRACT, PLASMA_DOC, GL11_HEADER, GL11_SOURCE, REALIZATION_HEADER, REALIZATION_SOURCE]:
        require(path.exists(), f"Missing Plasma acceleration policy path: {path.relative_to(ROOT)}", errors)

    if not errors:
        contract = CONTRACT.read_text(encoding="utf-8")
        for needle in [
            "software reference is canonical",
            "Acceleration is optional",
            "degrade honestly",
            "does not widen compatibility",
            "Visual mismatch must be classified",
            "Performance benefit must be measured",
            "Human review remains separate from deterministic proof",
            "Stable release promotion remains blocked",
            "plasma_v2_realization_gl11_candidate",
            "`exact`",
            "`tolerant-pixel`",
            "`observational`",
            "`review-only`",
        ]:
            require(needle in contract, f"plasma_acceleration_v1.md missing {needle!r}", errors)

        doc = PLASMA_DOC.read_text(encoding="utf-8")
        for needle in [
            "software reference path as the canonical product meaning",
            "GL11",
            "fall back to the software reference path",
            "does not stabilize GL21",
            "not stable release",
            "compatibility certification",
        ]:
            require(needle in doc, f"plasma-v2-acceleration-policy.md missing {needle!r}", errors)
        require("product truth outside ScreenSave review and proof records" in doc, "Plasma acceleration doc must preserve product truth boundary.", errors)
        require("AIDE" not in doc, "Plasma product acceleration doc must not reference AIDE directly.", errors)

        gl11 = GL11_SOURCE.read_text(encoding="utf-8")
        require("plasma_v2_gl11_candidate_render" in gl11, "GL11 candidate source must expose render entry.", errors)
        require("plasma_v2_core_render" in gl11, "GL11 candidate must fall back through the software reference core.", errors)
        require("windows.h" not in gl11.lower(), "GL11 candidate must not include Win32 law.", errors)
        require("opengl" not in gl11.lower(), "GL11 candidate proof path must not bind directly to OpenGL.", errors)
        require("release promotion remains blocked" in gl11, "GL11 candidate must preserve release boundary.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma acceleration policy checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
