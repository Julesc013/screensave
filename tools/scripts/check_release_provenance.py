"""Validate Plasma v2 stable-promotion provenance review."""

from __future__ import annotations

import hashlib
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STABLE_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion"
REVIEW = STABLE_DIR / "provenance-review.json"
DECISION = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision" / "decision.stable.toml"
PACKAGE_DIR = ROOT / "packaging" / "windows" / "plasma-v2-stable-promotion"
MANIFEST = PACKAGE_DIR / "manifest.toml"
PROVENANCE = PACKAGE_DIR / "provenance.json"
SBOM = PACKAGE_DIR / "sbom.json"
CHECKSUMS = PACKAGE_DIR / "checksums.sha256"


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


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def parse_sha256_lines(path: pathlib.Path, errors: list[str]) -> dict[str, str]:
    values: dict[str, str] = {}
    for line_number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        stripped = line.strip()
        if not stripped:
            continue
        parts = stripped.split(None, 1)
        if len(parts) != 2:
            errors.append(f"{path.relative_to(ROOT)} line {line_number} is not '<sha256> <path>'.")
            continue
        digest, ref = parts
        values[ref.strip()] = digest
    return values


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def git_commit_exists(value: str) -> bool:
    if not value:
        return False
    result = subprocess.run(
        ["git", "cat-file", "-e", f"{value}^{{commit}}"],
        cwd=ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    return result.returncode == 0


def main() -> int:
    errors: list[str] = []
    for path in (REVIEW, MANIFEST, PROVENANCE, SBOM, CHECKSUMS):
        require(path.exists(), f"Missing provenance input: {path.relative_to(ROOT)}", errors)

    if MANIFEST.exists():
        manifest = load_toml(MANIFEST)
        require(manifest.get("status") == "staged-stable-promotion", "package manifest must be staged-stable-promotion.", errors)
        require(manifest.get("published") is False, "package manifest must keep published false.", errors)
        require(manifest.get("stable") is False, "package manifest must keep stable false before accepted state.", errors)
        require(manifest.get("compatibility_certification") == "not-claimed", "package manifest must not certify compatibility.", errors)

    if PROVENANCE.exists():
        provenance = load_json(PROVENANCE)
        source_commit = provenance.get("source_commit", "")
        require(git_commit_exists(source_commit), "package provenance source commit must exist.", errors)
        require(provenance.get("source_dirty_at_intake") is False, "package provenance must record clean source intake.", errors)
        artifact = provenance.get("artifact", {})
        artifact_path = ROOT / artifact.get("path", "")
        require(artifact_path.exists(), f"provenance artifact path missing: {artifact.get('path')}", errors)
        if artifact_path.exists():
            require(sha256_file(artifact_path) == artifact.get("sha256"), "provenance artifact sha256 mismatch.", errors)
        require(provenance.get("boundary", {}).get("published") is False, "package provenance must keep published false.", errors)
        require(provenance.get("boundary", {}).get("compatibility_certification") == "not claimed", "package provenance must not claim compatibility certification.", errors)

    if REVIEW.exists():
        review = load_json(REVIEW)
        accepted = final_artistic_accepted()
        expected_status = "pass" if accepted else "pass-with-hold"
        expected_artistic = "accepted-for-stable" if accepted else "request-changes"
        require(review.get("schema") == "screensave.plasma-v2.stable-promotion.provenance-review.v1", "provenance review schema mismatch.", errors)
        require(review.get("candidate_id") == "plasma-v2-rc1", "provenance review must name rc1.", errors)
        require(review.get("status") == expected_status, f"provenance review status must be {expected_status}.", errors)
        require(git_commit_exists(review.get("review_source_commit", "")), "review source commit must exist.", errors)
        require(git_commit_exists(review.get("package_source_commit", "")), "package source commit must exist.", errors)
        require(review.get("boundary", {}).get("release_publication") is False, "provenance review must block publication.", errors)
        require(review.get("boundary", {}).get("stable_release") is accepted, "provenance review stable release flag must match the final verdict.", errors)
        require(review.get("boundary", {}).get("compatibility_certification") == "not claimed", "provenance review must not claim certification.", errors)
        require(review.get("boundary", {}).get("final_artistic_acceptance") == expected_artistic, "provenance review must match artistic decision.", errors)
        for key, ref in review.get("refs", {}).items():
            require((ROOT / ref).exists(), f"provenance review ref missing: {key}={ref}", errors)

    if CHECKSUMS.exists():
        checksums = parse_sha256_lines(CHECKSUMS, errors)
        require(bool(checksums), "stable-promotion checksum file must list package refs.", errors)
        for ref, expected in checksums.items():
            path = ROOT / ref
            require(path.exists(), f"checksum ref missing: {ref}", errors)
            if path.exists():
                require(sha256_file(path) == expected, f"checksum mismatch for {ref}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 release provenance checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
