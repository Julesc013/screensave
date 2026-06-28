"""Validate and summarize the ScreenSave AIDE evidence index."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFAULT_INDEX = ROOT / ".aide" / "evidence" / "index.toml"

REQUIRED_ENTRY_IDS = {
    "nocturne-v1-proof",
    "nocturne-v2-proof",
    "ricochet-v1-proof",
    "ricochet-v2-proof",
    "plasma-v2-proof",
    "plasma-v2-proof-bundle",
    "plasma-v2-materials",
    "plasma-v2-packc",
    "plasma-v2-evidence-packet",
    "plasma-v2-proof-matrix",
    "plasma-v2-visualintent-candidates",
    "plasma-v2-review-ledger",
    "plasma-v2-review-evidence-packet",
    "plasma-v2-packc-v1-candidate",
    "plasma-v2-acceleration-candidate",
    "plasma-v2-performance-envelope",
    "plasma-v2-stable-candidate-review",
    "plasma-v2-stable-candidate-evidence-packet",
    "portable-v2-equivalence-proof",
    "windows-current-artifact-manifests",
    "windows-current-pe-audit-json",
    "proof-bundle-v1-portable-v2",
    "aide-evidence-packets",
    "plasma-v2-publication-prep",
    "plasma-v2-publication-ready-handoff",
}

FORBIDDEN_CLAIM_WORDS = {
    "compatibility certified",
    "compatibility certification pass",
    "artistic acceptance pass",
    "release promoted",
    "gate c accepted",
}


def repo_path(path: pathlib.Path) -> str:
    try:
        return str(path.resolve().relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path)


def load_index(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def validate_index(index: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    entries = index.get("entries", [])
    entry_ids = {str(entry.get("id", "")) for entry in entries if isinstance(entry, dict)}

    if index.get("schema_version") != 1:
        errors.append("evidence index schema_version must be 1")
    if index.get("status") != "active":
        errors.append("evidence index status must be active")
    if index.get("network_calls") is not False:
        errors.append("evidence index must record network_calls false")
    if index.get("provider_or_model_calls") is not False:
        errors.append("evidence index must record provider_or_model_calls false")
    if index.get("runtime_dependency_allowed") is not False:
        errors.append("evidence index must record runtime_dependency_allowed false")
    if REQUIRED_ENTRY_IDS - entry_ids:
        errors.append("evidence index missing entries: " + ", ".join(sorted(REQUIRED_ENTRY_IDS - entry_ids)))

    for entry in entries:
        if not isinstance(entry, dict):
            errors.append("evidence index entries must be tables")
            continue
        entry_id = str(entry.get("id", ""))
        for key in ["kind", "status", "primary_ref", "generated_ref", "validator", "claim_boundary"]:
            if not str(entry.get(key, "")).strip():
                errors.append(f"{entry_id} missing {key}")
        for ref_key in ["primary_ref", "generated_ref"]:
            ref = str(entry.get(ref_key, ""))
            normalized = ref.replace("\\", "/")
            if ref_key == "generated_ref" and not normalized.startswith("out/"):
                errors.append(f"{entry_id} generated_ref must stay under out/: {ref}")
            if ".." in pathlib.PurePosixPath(normalized).parts:
                errors.append(f"{entry_id} ref must not traverse parents: {ref}")
        boundary = str(entry.get("claim_boundary", "")).lower()
        for forbidden in FORBIDDEN_CLAIM_WORDS:
            if forbidden in boundary:
                errors.append(f"{entry_id} claim boundary contains forbidden overclaim: {forbidden}")
        validator = str(entry.get("validator", ""))
        if "tools/" not in validator and "tools\\" not in validator:
            errors.append(f"{entry_id} validator must be a repo-local tool command")

    return errors


def summary_payload(index_path: pathlib.Path, index: dict[str, Any], errors: list[str]) -> dict[str, Any]:
    entries = index.get("entries", [])
    return {
        "schema": "screensave-aide-evidence-index-summary-v0",
        "status": "pass" if not errors else "fail",
        "index_ref": repo_path(index_path),
        "entry_count": len(entries) if isinstance(entries, list) else 0,
        "entry_ids": [entry.get("id") for entry in entries if isinstance(entry, dict)],
        "truth_boundary": index.get("truth_boundary", ""),
        "network_calls": index.get("network_calls"),
        "provider_or_model_calls": index.get("provider_or_model_calls"),
        "runtime_dependency_allowed": index.get("runtime_dependency_allowed"),
        "errors": errors,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--index", default=str(DEFAULT_INDEX))
    parser.add_argument("--output", default="")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    index_path = pathlib.Path(args.index)
    if not index_path.is_absolute():
        index_path = ROOT / index_path
    index = load_index(index_path)
    errors = validate_index(index)
    payload = summary_payload(index_path, index, errors)

    if args.output:
        output = pathlib.Path(args.output)
        if not output.is_absolute():
            output = ROOT / output
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    else:
        print(json.dumps(payload, indent=2, sort_keys=True))

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
    return 1 if args.check and errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
