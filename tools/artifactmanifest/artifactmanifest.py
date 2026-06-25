"""Generate exact ScreenSave artifact-set manifests from catalog authority."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
ARTIFACT_SETS = ROOT / "catalog" / "artifact_sets.toml"


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def repo_path(path: pathlib.Path) -> str:
    resolved = path.resolve()
    try:
        return str(resolved.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(resolved)


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def source_payload() -> dict[str, Any]:
    return {
        "commit": git_text(["rev-parse", "HEAD"]),
        "branch": git_text(["branch", "--show-current"]),
        "dirty": bool(git_text(["status", "--short"])),
    }


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def artifact_fact(path: pathlib.Path, expected: bool) -> dict[str, Any]:
    exists = path.exists() and path.is_file()
    fact: dict[str, Any] = {
        "path": repo_path(path),
        "expected": expected,
        "exists": exists,
    }
    if exists:
        fact["bytes"] = path.stat().st_size
        fact["sha256"] = sha256_file(path)
    return fact


def find_artifact_set(key: str) -> dict[str, Any]:
    data = load_toml(ARTIFACT_SETS)
    for item in data.get("artifact_sets", []):
        if item.get("key") == key:
            return item
    raise KeyError(key)


def expected_path(root: pathlib.Path, path_template: str, product: str) -> pathlib.Path:
    return root / path_template.replace("{product}", product)


def observed_paths(root: pathlib.Path, extension: str) -> list[pathlib.Path]:
    if not root.exists():
        return []
    pattern = f"*{extension}" if extension else "*"
    return sorted(path for path in root.rglob(pattern) if path.is_file())


def generate_manifest(artifact_set: dict[str, Any]) -> dict[str, Any]:
    root = (ROOT / str(artifact_set["root"])).resolve()
    extension = str(artifact_set.get("extension", ""))
    path_template = str(artifact_set["path_template"])
    products = [str(product) for product in artifact_set.get("expected_products", [])]
    expected = [expected_path(root, path_template, product).resolve() for product in products]
    observed = observed_paths(root, extension)

    expected_set = {path.resolve() for path in expected}
    observed_set = {path.resolve() for path in observed}
    missing = sorted(expected_set - observed_set)
    unexpected = sorted(observed_set - expected_set)
    expected_count = int(artifact_set.get("expected_count", len(expected)))

    status = "pass"
    if len(expected) != expected_count or missing or unexpected or len(observed_set & expected_set) != expected_count:
        status = "fail"

    return {
        "manifest_schema": "screensave-artifact-set-manifest-v0",
        "status": status,
        "source": source_payload(),
        "artifact_set": {
            "key": artifact_set.get("key"),
            "artifact_profile": artifact_set.get("artifact_profile"),
            "build_lane": artifact_set.get("build_lane"),
            "root": repo_path(root),
            "path_template": path_template,
            "machine": artifact_set.get("machine"),
            "pe_format": artifact_set.get("pe_format"),
            "extension": extension,
            "expected_count": expected_count,
            "claim_boundary": artifact_set.get("claim_boundary"),
        },
        "expected_artifacts": [artifact_fact(path, True) for path in expected],
        "observed_artifacts": [artifact_fact(path, path.resolve() in expected_set) for path in observed],
        "missing_artifacts": [repo_path(path) for path in missing],
        "unexpected_artifacts": [repo_path(path) for path in unexpected],
        "counts": {
            "expected": expected_count,
            "declared_products": len(products),
            "observed_expected": len(observed_set & expected_set),
            "observed_total": len(observed),
            "missing": len(missing),
            "unexpected": len(unexpected),
        },
        "limits": [
            "This manifest records file identity and exact artifact-set membership only.",
            "PE architecture, subsystem, and imports require a separate PE audit.",
            "Runtime compatibility and artistic acceptance are not implied.",
        ],
    }


def command_generate(args: argparse.Namespace) -> int:
    try:
        artifact_set = find_artifact_set(args.artifact_set)
    except KeyError:
        print(f"unknown artifact set: {args.artifact_set}", file=sys.stderr)
        return 2

    manifest = generate_manifest(artifact_set)
    output = pathlib.Path(args.output)
    if not output.is_absolute():
        output = ROOT / output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"{manifest['status']} {args.artifact_set} {repo_path(output)}")
    return 0 if manifest["status"] == "pass" else 1


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    generate = subparsers.add_parser("generate", help="Generate an artifact-set manifest.")
    generate.add_argument("--artifact-set", required=True)
    generate.add_argument("--output", required=True)
    generate.set_defaults(func=command_generate)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
