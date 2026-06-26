"""Validate the Plasma v2 preview package stage."""

from __future__ import annotations

import hashlib
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STAGE_DIR = ROOT / "packaging" / "windows" / "plasma-v2-preview"
MANIFEST = STAGE_DIR / "manifest.toml"
README = STAGE_DIR / "README.md"
KNOWN_LIMITS = STAGE_DIR / "known-limits.md"
CHECKSUMS = STAGE_DIR / "checksums.json"
STAGE_TOOL = ROOT / "tools" / "release" / "stage_plasma_v2.py"
SUMMARY = ROOT / "out" / "release" / "plasma-v2-preview" / "stage-summary.json"


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in [MANIFEST, README, KNOWN_LIMITS, CHECKSUMS, STAGE_TOOL]:
        require(path.exists(), f"Missing package stage path: {path.relative_to(ROOT)}", errors)

    if MANIFEST.exists():
        manifest = load_toml(MANIFEST)
        require(manifest.get("status") == "staged-preview", "package stage status must be staged-preview.", errors)
        require(manifest.get("published") is False, "package stage must not be published.", errors)
        require(manifest.get("stable") is False, "package stage must not mark stable true.", errors)
        require(manifest.get("release_promotion") == "blocked", "package stage release promotion must remain blocked.", errors)
        require(manifest.get("compatibility_certification") == "not-claimed", "package stage must not certify compatibility.", errors)
        require(manifest.get("support_class") == "buildable", "package stage support class must be buildable.", errors)
        require("out/" in manifest.get("artifact_refs", {}).get("plasma_scr_current", ""), "Plasma .scr must be an output ref, not a committed binary.", errors)
        for section, key in [
            ("pack_refs", "example_pack"),
            ("proof_refs", "proof_bundle"),
            ("proof_refs", "stable_candidate_gate"),
            ("evidence_refs", "aide_evidence_index"),
            ("known_limits", "ref"),
            ("checksums", "ref"),
        ]:
            ref = manifest.get(section, {}).get(key, "")
            require(bool(ref) and (ROOT / ref).exists(), f"manifest ref missing or absent: {section}.{key}={ref}", errors)
        require("not publication" in manifest.get("claim_boundary", ""), "manifest must preserve publication boundary.", errors)

    if CHECKSUMS.exists():
        checksums = json.loads(CHECKSUMS.read_text(encoding="utf-8"))
        for ref, expected in checksums.get("sha256", {}).items():
            path = ROOT / ref
            require(path.exists(), f"checksum ref missing: {ref}", errors)
            if path.exists():
                require(sha256_file(path) == expected, f"checksum mismatch for {ref}", errors)
        require("certify compatibility" in checksums.get("claim_boundary", ""), "checksums must preserve certification boundary.", errors)

    for path in [README, KNOWN_LIMITS]:
        if path.exists():
            text = path.read_text(encoding="utf-8")
            require("not" in text.lower() and "stable" in text.lower(), f"{path.relative_to(ROOT)} must block stable claims.", errors)
            require("compatibility certification" in text, f"{path.relative_to(ROOT)} must block compatibility certification.", errors)

    if not errors:
        result = subprocess.run(
            [sys.executable, str(STAGE_TOOL), "summary"],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        require(result.returncode == 0, f"stage_plasma_v2.py summary failed: {result.stderr}", errors)
        require(SUMMARY.exists(), "stage_plasma_v2.py must write stage summary.", errors)
        if SUMMARY.exists():
            summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
            require(summary.get("status") == "pass", "stage summary status must pass.", errors)
            require(summary.get("published") is False, "stage summary must keep published false.", errors)
            require(summary.get("stable") is False, "stage summary must keep stable false.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 package stage checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
