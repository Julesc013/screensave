"""Inspect the Plasma v2 preview package stage."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "packaging" / "windows" / "plasma-v2-preview" / "manifest.toml"
CHECKSUMS = ROOT / "packaging" / "windows" / "plasma-v2-preview" / "checksums.json"
OUT = ROOT / "out" / "release" / "plasma-v2-preview" / "stage-summary.json"


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def load_manifest() -> dict[str, Any]:
    with MANIFEST.open("rb") as handle:
        return tomllib.load(handle)


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def build_summary() -> dict[str, Any]:
    manifest = load_manifest()
    checksums = json.loads(CHECKSUMS.read_text(encoding="utf-8"))
    observed: dict[str, str] = {}
    for ref in checksums.get("sha256", {}):
        observed[ref] = sha256_file(ROOT / ref)
    return {
        "schema": "screensave.plasma-v2.package-stage-summary.v1",
        "status": "pass",
        "package_id": manifest.get("package_id"),
        "published": manifest.get("published"),
        "stable": manifest.get("stable"),
        "support_class": manifest.get("support_class"),
        "artifact_ref": manifest.get("artifact_refs", {}).get("plasma_scr_current"),
        "checksum_ref": repo_path(CHECKSUMS),
        "observed_sha256": observed,
        "claim_boundary": "Stage summary only; no publication, stable release, compatibility certification, or installer mutation.",
    }


def command_summary(args: argparse.Namespace) -> int:
    summary = build_summary()
    output = pathlib.Path(args.output) if args.output else OUT
    if not output.is_absolute():
        output = ROOT / output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(json.dumps({"status": "pass", "output": repo_path(output)}, indent=2, sort_keys=True))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    summary = subparsers.add_parser("summary")
    summary.add_argument("--output", default="")
    summary.set_defaults(func=command_summary)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
