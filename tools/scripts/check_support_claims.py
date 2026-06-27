"""Validate Plasma v2 release-candidate support wording."""

from __future__ import annotations

import json
import pathlib
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PRODUCT_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-support-matrix.md"
COMPAT_DOC = ROOT / "docs" / "compatibility" / "plasma-v2-release-candidate.md"
CLAIMS = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "support-claims.json"

PRODUCT_PHRASES = [
    "Status: release-candidate support matrix, not stable release.",
    "buildable",
    "targeted",
    "experimental",
    "GDI remains the universal floor",
    "GL11 candidate evidence is compared against the software reference",
    "does not make a compatibility certification claim",
    "does not turn release-candidate readiness into stable release promotion",
]

COMPAT_PHRASES = [
    "Status: release-candidate compatibility wording.",
    "not an operating-system certification",
    "PE audit zero violations do not imply OS certification",
    "GL11 candidate evidence does not imply all accelerated paths",
    "`packc` v1-candidate does not imply an executable plugin ecosystem",
    "Manager preview does not imply install mutation",
    "Workbench release-readiness does not imply publication",
    "`stable = false` remains required",
    "Only `certified` evidence should be worded as ordinary public compatibility",
]

FORBIDDEN_TEXT = [
    "stable = true",
    "release_promotion = \"accepted\"",
    "compatibility_certification = \"certified\"",
    "certified on Windows",
    "all accelerated paths are supported",
    "plugin ecosystem is admitted",
    "publication is approved",
]


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def check_text(path: pathlib.Path, phrases: list[str], errors: list[str]) -> None:
    require(path.exists(), f"Missing support doc: {path.relative_to(ROOT)}", errors)
    if not path.exists():
        return
    text = path.read_text(encoding="utf-8")
    lower = text.lower()
    for phrase in phrases:
        require(phrase in text, f"{path.relative_to(ROOT)} missing phrase {phrase!r}", errors)
    for forbidden in FORBIDDEN_TEXT:
        require(forbidden.lower() not in lower, f"{path.relative_to(ROOT)} contains forbidden support claim {forbidden!r}", errors)
    if path.parts[-4:-1] == ("products", "savers", "plasma"):
        require("AIDE" not in text, "Product support doc must not introduce AIDE references.", errors)


def main() -> int:
    errors: list[str] = []
    check_text(PRODUCT_DOC, PRODUCT_PHRASES, errors)
    check_text(COMPAT_DOC, COMPAT_PHRASES, errors)

    require(CLAIMS.exists(), f"Missing support claims JSON: {CLAIMS.relative_to(ROOT)}", errors)
    if CLAIMS.exists():
        data = load_json(CLAIMS)
        require(data.get("schema") == "screensave.plasma-v2.release-candidate.support-claims.v1", "support claims schema mismatch.", errors)
        require(data.get("candidate_id") == "plasma-v2-rc1", "support claims must name rc1.", errors)
        require(data.get("stable") is False, "support claims must keep stable false.", errors)
        require(data.get("release_promotion") == "blocked", "support claims must keep release promotion blocked.", errors)
        require(data.get("compatibility_certification") == "not-claimed", "support claims must not claim certification.", errors)
        require("not stable release" in data.get("claim_boundary", ""), "support claim boundary must block stable release.", errors)
        claims = data.get("claims", {})
        for key in [
            "current_windows_artifact_evidence",
            "pe_audit_zero_violations",
            "gl11_candidate",
            "packc_v1_candidate",
            "manager_preview",
            "workbench_release_readiness",
            "stable_release",
        ]:
            require(key in claims, f"support claims JSON missing {key}.", errors)
        refs = data.get("refs", {})
        for key, ref in refs.items():
            require((ROOT / ref).exists(), f"support claims ref missing: {key}={ref}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 support claim checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
