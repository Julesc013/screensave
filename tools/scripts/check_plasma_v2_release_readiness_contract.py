"""Validate the Plasma v2 release-readiness contract."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "plasma_release_readiness_v1.md"
PLASMA_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-release-readiness.md"

AXES = [
    "reference proof",
    "accelerated comparison",
    "performance envelope",
    "packc v1 candidate",
    "Workbench Author/Profile/Review",
    "Manager preview",
    "artifact packaging",
    "support wording",
    "known limits",
    "visual review",
    "final artistic decision",
    "release promotion",
]
STATUSES = ["stable-candidate", "release-readiness-reviewed", "release-candidate", "stable"]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [CONTRACT, PLASMA_DOC]:
        require(path.exists(), f"Missing release-readiness path: {path.relative_to(ROOT)}", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        require("Status: active release-readiness contract." in text, "contract status missing.", errors)
        for status in STATUSES:
            require(status in text, f"contract missing status {status}.", errors)
        for axis in AXES:
            require(axis in text, f"contract missing gate axis {axis}.", errors)
        for boundary in ["stable release", "compatibility certification", "public SDK stability", "PAW-H"]:
            require(boundary in text, f"contract missing boundary {boundary}.", errors)

    if PLASMA_DOC.exists():
        doc = PLASMA_DOC.read_text(encoding="utf-8")
        for needle in [
            "Status: release-readiness contract, not release.",
            "stable candidate",
            "package staging is not publication",
            "artifact audit is not compatibility certification",
            "stable =",
            "false",
            "release promotion still blocked",
        ]:
            require(needle in doc, f"Plasma release-readiness doc missing {needle!r}", errors)
        require("AIDE" not in doc, "Product release-readiness doc must not introduce AIDE references.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 release-readiness contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
