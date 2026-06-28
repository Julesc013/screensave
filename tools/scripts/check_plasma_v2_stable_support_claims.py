"""Validate Plasma v2 stable-promotion support wording."""

from __future__ import annotations

import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PRODUCT_DOC = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-stable-support.md"
COMPAT_DOC = ROOT / "docs" / "compatibility" / "plasma-v2-stable.md"
CLAIMS = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "support-claims.json"
DECISION = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision" / "decision.stable.toml"

PRODUCT_PHRASES = [
    "Status: stable-promotion support packet, release-candidate hold.",
    "buildable",
    "targeted",
    "experimental",
    "GDI remains the universal floor",
    "GL11 candidate evidence is compared against the software reference",
    "does not make a compatibility certification claim",
    "does not turn release-candidate readiness into stable release promotion",
]

COMPAT_PHRASES = [
    "Status: stable-promotion compatibility wording for `plasma-v2-rc1`.",
    "not an operating-system certification",
    "PE audit zero violations record binary facts",
    "does not certify Windows 95, Windows 98, Windows ME, or NT4",
    "GL11 remains the first optional acceleration candidate",
    "`packc` v1-candidate remains data-only",
    "Manager preview does not imply install mutation",
    "Workbench release-readiness does not imply publication",
    "held until final human artistic acceptance is supplied",
]

FORBIDDEN_TEXT = [
    "stable = true",
    "release_promotion = \"accepted\"",
    "compatibility_certification = \"certified\"",
    "certified on Windows",
    "Windows 95 certified",
    "Windows 98 certified",
    "Windows ME certified",
    "NT4 certified",
    "macOS support",
    "Linux support",
    "GL33 stable",
    "Vulkan support",
    "public SDK stability is accepted",
]


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def final_artistic_accepted() -> bool:
    if not DECISION.exists():
        return False
    decision = load_toml(DECISION)
    return str(decision.get("decision_state", decision.get("decision", ""))) == "accepted-for-stable"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def check_text(path: pathlib.Path, phrases: list[str], errors: list[str]) -> None:
    require(path.exists(), f"Missing stable support doc: {path.relative_to(ROOT)}", errors)
    if not path.exists():
        return
    text = path.read_text(encoding="utf-8")
    lower = text.lower()
    for phrase in phrases:
        require(phrase in text, f"{path.relative_to(ROOT)} missing phrase {phrase!r}", errors)
    for forbidden in FORBIDDEN_TEXT:
        require(forbidden.lower() not in lower, f"{path.relative_to(ROOT)} contains forbidden support claim {forbidden!r}", errors)
    if path == PRODUCT_DOC:
        require("AIDE" not in text, "Product stable support doc must not introduce AIDE references.", errors)


def main() -> int:
    errors: list[str] = []
    check_text(PRODUCT_DOC, PRODUCT_PHRASES, errors)
    check_text(COMPAT_DOC, COMPAT_PHRASES, errors)

    require(CLAIMS.exists(), f"Missing stable support claims JSON: {CLAIMS.relative_to(ROOT)}", errors)
    if CLAIMS.exists():
        data = load_json(CLAIMS)
        accepted = final_artistic_accepted()
        require(data.get("schema") == "screensave.plasma-v2.stable-promotion.support-claims.v1", "stable support claims schema mismatch.", errors)
        require(data.get("candidate_id") == "plasma-v2-rc1", "stable support claims must name rc1.", errors)
        require(data.get("stable") is accepted, "stable support claims stable flag must match the final verdict.", errors)
        require(
            data.get("release_promotion") == ("accepted" if accepted else "blocked"),
            "stable support claims release promotion must match the final verdict.",
            errors,
        )
        require(data.get("compatibility_certification") == "not-claimed", "stable support claims must not claim certification.", errors)
        require("not release publication" in data.get("claim_boundary", ""), "stable support claim boundary must block publication.", errors)
        claims = data.get("claims", {})
        for key in [
            "current_windows_x86_artifact_evidence",
            "binary_audited_facts",
            "buildable_facts",
            "targeted_preservation_aspirations",
            "unsupported_lanes",
            "stable_release",
        ]:
            require(key in claims, f"stable support claims JSON missing {key}.", errors)
        if accepted:
            require("accepted" in claims.get("stable_release", ""), "stable support claims must record accepted stable scope.", errors)
        else:
            require("blocked" in claims.get("stable_release", ""), "stable support claims must keep stable release blocked.", errors)
        refs = data.get("refs", {})
        for key, ref in refs.items():
            require((ROOT / ref).exists(), f"stable support claims ref missing: {key}={ref}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 stable support claim checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
