"""Validate Plasma v2 stable-promotion security review."""

from __future__ import annotations

import json
import pathlib
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
REVIEW = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "security-review.json"
SBOM = ROOT / "packaging" / "windows" / "plasma-v2-stable-promotion" / "sbom.json"
PACK_MANIFEST = ROOT / "packaging" / "windows" / "plasma-v2-stable-promotion" / "manifest.toml"
KNOWN_LIMITS = ROOT / "packaging" / "windows" / "plasma-v2-stable-promotion" / "known-limits.md"
ROLLBACK = ROOT / "packaging" / "windows" / "plasma-v2-stable-promotion" / "rollback-notes.md"

RUNTIME_SCAN_ROOTS = [
    ROOT / "products" / "savers" / "plasma" / "src",
    ROOT / "platform" / "src",
    ROOT / "platform" / "include",
]

NETWORK_TOKENS = [
    "WinHttp",
    "WinInet",
    "URLDownload",
    "InternetOpen",
    "socket(",
    "connect(",
    "https://",
    "http://",
]


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def scan_runtime_tokens(tokens: list[str]) -> list[str]:
    hits: list[str] = []
    for root in RUNTIME_SCAN_ROOTS:
        if not root.exists():
            continue
        for path in root.rglob("*"):
            if not path.is_file() or path.suffix.lower() not in {".c", ".h"}:
                continue
            text = path.read_text(encoding="utf-8", errors="ignore")
            for token in tokens:
                if token in text:
                    hits.append(f"{path.relative_to(ROOT)}:{token}")
    return hits


def main() -> int:
    errors: list[str] = []
    for path in (REVIEW, SBOM, PACK_MANIFEST, KNOWN_LIMITS, ROLLBACK):
        require(path.exists(), f"Missing security input: {path.relative_to(ROOT)}", errors)

    if REVIEW.exists():
        review = load_json(REVIEW)
        require(review.get("schema") == "screensave.plasma-v2.stable-promotion.security-review.v1", "security review schema mismatch.", errors)
        require(review.get("candidate_id") == "plasma-v2-rc1", "security review must name rc1.", errors)
        require(review.get("status") == "pass-with-hold", "security review must preserve the stable hold.", errors)
        checks = review.get("checks", {})
        for key in (
            "no_executable_pack_payloads",
            "packc_negative_fixtures",
            "no_network_dependency_in_saver_runtime",
            "no_development_plane_runtime_dependency",
            "license_provenance_metadata",
            "known_limits_document",
            "rollback_notes",
        ):
            require(checks.get(key) == "pass", f"security check must pass: {key}", errors)
        require(checks.get("release_publication") == "not-performed", "security review must not record publication.", errors)
        require(checks.get("final_artistic_acceptance") == "request-changes", "security review must preserve artistic hold.", errors)
        require("not publication" in review.get("claim_boundary", ""), "security review must block publication.", errors)
        require("compatibility certification" in review.get("claim_boundary", ""), "security review must block compatibility certification.", errors)
        for key, ref in review.get("refs", {}).items():
            require((ROOT / ref).exists(), f"security review ref missing: {key}={ref}", errors)

    if SBOM.exists():
        sbom = load_json(SBOM)
        require(sbom.get("package_id") == "screensave.plasma-v2.stable-promotion-stage", "SBOM package id mismatch.", errors)
        require(sbom.get("compatibility_certification") == "not claimed", "SBOM must not claim compatibility certification.", errors)
        for component in sbom.get("components", []):
            require(component.get("executable_payload") is not True, f"SBOM component must not be executable pack payload: {component.get('name')}", errors)
        forbidden_payloads = set(sbom.get("forbidden_payloads", []))
        for payload in ("scripts", "DLLs", "macros", "network references", "runtime executable plugins"):
            require(payload in forbidden_payloads, f"SBOM must forbid {payload}.", errors)

    network_hits = scan_runtime_tokens(NETWORK_TOKENS)
    require(not network_hits, f"saver/platform runtime must not add network dependencies: {network_hits}", errors)
    development_hits = scan_runtime_tokens([".aide", "AIDE"])
    require(not development_hits, f"saver/platform runtime must not depend on AIDE: {development_hits}", errors)

    if KNOWN_LIMITS.exists():
        known_limits = KNOWN_LIMITS.read_text(encoding="utf-8")
        require("Compatibility certification is not claimed" in known_limits, "known limits must block compatibility certification.", errors)
    if ROLLBACK.exists():
        rollback = ROLLBACK.read_text(encoding="utf-8")
        require("no installer side effects" in rollback, "rollback notes must record no installer side effects.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 release security checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
