"""Compile bounded, data-only ScreenSave packs."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import shutil
import sys
import tempfile
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT_ROOT = ROOT / "out"
MAX_STRING_LENGTH = 96
MAX_EXPANDED_BYTES = 65536
MAX_OUTPUT_FILES = 4

PACK_SCHEMA = "screensave.pack.v1"
PACK_MANIFEST_SCHEMA = "screensave.pack_manifest.v1"
PACK_KIND = "screensave.plasma.v2"
PLASMA_SCHEMA_ID = "screensave.plasma.spec.v2"
PLASMA_SCHEMA_VERSION = 2
ADMITTED_PROOF_PROFILES = {
    "plasma.v2.reference.preview",
    "plasma.v2.visualintent.preview",
}
PACK_STATUS = "v1-candidate"

FIELD_FAMILY = {
    "classic_interference": 0,
    "radial_warped": 1,
}
OUTPUT_STYLE = {
    "continuous": 0,
    "banded": 1,
    "contour": 2,
}
MATERIAL = {
    "plasma_lava": 0,
    "aurora_cool": 1,
    "oceanic_blue": 2,
    "museum_phosphor": 3,
    "quiet_darkroom": 4,
}
TREATMENT = {
    "none": 0,
    "restrained_dither": 1,
    "restrained_crt": 2,
}
QUALITY = {
    "safe": 0,
}
SEED_POLICY = {
    "session": 0,
    "fixed": 1,
    "randomize": 2,
}
PRESENTATION = {
    "flat": 0,
}

TOP_LEVEL_KEYS = {
    "schema_version",
    "pack_id",
    "kind",
    "product",
    "name",
    "author",
    "license",
    "provenance",
    "proof_profile",
    "compatibility",
    "plasma_spec_v2",
}
COMPATIBILITY_KEYS = {
    "minimum_schema_version",
    "maximum_schema_version",
    "pack_status",
}
SPEC_KEYS = {
    "schema_id",
    "schema_version",
    "field_family",
    "scale",
    "complexity",
    "motion_speed",
    "warp_amount",
    "feedback_amount",
    "output_style",
    "material",
    "brightness",
    "contrast",
    "treatment",
    "quality_intent",
    "seed_policy",
    "presentation",
}
FORBIDDEN_KEY_PARTS = [
    "script",
    "dll",
    "macro",
    "command",
    "exec",
    "plugin",
]
SAFE_ID = re.compile(r"^[A-Za-z0-9_.-]+$")
PATHLIKE = re.compile(r"(^[A-Za-z]:[\\/])|(^[\\/])|(\.\.)|([\\/])")


class PackError(ValueError):
    pass


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def read_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def canonical_bytes(value: Any) -> bytes:
    return (json.dumps(value, indent=2, sort_keys=True) + "\n").encode("utf-8")


def sha256_bytes(value: bytes) -> str:
    return hashlib.sha256(value).hexdigest()


def ensure_safe_string(label: str, value: Any) -> str:
    if not isinstance(value, str):
        raise PackError(f"{label} must be a string.")
    if len(value) == 0:
        raise PackError(f"{label} must not be empty.")
    if len(value) > MAX_STRING_LENGTH:
        raise PackError(f"{label} exceeds {MAX_STRING_LENGTH} characters.")
    lowered = value.lower()
    if PATHLIKE.search(value):
        raise PackError(f"{label} must not contain paths or parent traversal.")
    for token in [".dll", ".exe", ".bat", ".cmd", ".ps1", "<script"]:
        if token in lowered:
            raise PackError(f"{label} must not contain executable-looking text.")
    for token in ["http://", "https://", "ftp://", "file://"]:
        if token in lowered:
            raise PackError(f"{label} must not contain network or file URI references.")
    return value


def ensure_safe_id(label: str, value: Any) -> str:
    text = ensure_safe_string(label, value)
    if SAFE_ID.match(text) is None:
        raise PackError(f"{label} must use only letters, digits, underscore, dash, or dot.")
    return text


def check_forbidden_keys(value: Any, prefix: str = "") -> None:
    if isinstance(value, dict):
        for key, child in value.items():
            lowered = str(key).lower()
            for part in FORBIDDEN_KEY_PARTS:
                if part in lowered:
                    raise PackError(f"{prefix}{key} is forbidden in data-only packs.")
            check_forbidden_keys(child, f"{prefix}{key}.")
    elif isinstance(value, list):
        for index, child in enumerate(value):
            check_forbidden_keys(child, f"{prefix}{index}.")
    elif isinstance(value, str):
        ensure_safe_string(prefix.rstrip(".") or "string", value)


def require_keys(label: str, observed: set[str], expected: set[str]) -> None:
    missing = expected - observed
    extra = observed - expected
    if missing:
        raise PackError(f"{label} missing required keys: {', '.join(sorted(missing))}.")
    if extra:
        raise PackError(f"{label} contains unknown keys: {', '.join(sorted(extra))}.")


def require_percent(label: str, value: Any) -> int:
    if not isinstance(value, int):
        raise PackError(f"{label} must be an integer.")
    if value < 0 or value > 100:
        raise PackError(f"{label} must be in the 0..100 range.")
    return int(value)


def resolve_token(label: str, value: Any, table: dict[str, int]) -> dict[str, Any]:
    token = ensure_safe_string(label, value)
    if token not in table:
        raise PackError(f"{label} has unsupported token {token!r}.")
    return {"token": token, "value": table[token]}


def validate_and_resolve(data: dict[str, Any], source: pathlib.Path) -> dict[str, Any]:
    check_forbidden_keys(data)
    require_keys("pack", set(data), TOP_LEVEL_KEYS)
    if data.get("schema_version") != 1:
        raise PackError("pack schema_version must be 1.")
    pack_id = ensure_safe_id("pack_id", data.get("pack_id"))
    kind = ensure_safe_string("kind", data.get("kind"))
    if kind != PACK_KIND:
        raise PackError(f"kind must be {PACK_KIND}.")
    product = ensure_safe_string("product", data.get("product"))
    if product != "plasma":
        raise PackError("product must be plasma.")
    proof_profile = ensure_safe_string("proof_profile", data.get("proof_profile"))
    if proof_profile not in ADMITTED_PROOF_PROFILES:
        raise PackError(f"proof_profile must be one of {', '.join(sorted(ADMITTED_PROOF_PROFILES))}.")

    compatibility = data.get("compatibility")
    if not isinstance(compatibility, dict):
        raise PackError("compatibility must be a table.")
    require_keys("compatibility", set(compatibility), COMPATIBILITY_KEYS)
    if compatibility.get("minimum_schema_version") != PLASMA_SCHEMA_VERSION:
        raise PackError(f"compatibility.minimum_schema_version must be {PLASMA_SCHEMA_VERSION}.")
    if compatibility.get("maximum_schema_version") != PLASMA_SCHEMA_VERSION:
        raise PackError(f"compatibility.maximum_schema_version must be {PLASMA_SCHEMA_VERSION}.")
    pack_status = ensure_safe_string("compatibility.pack_status", compatibility.get("pack_status"))
    if pack_status != PACK_STATUS:
        raise PackError(f"compatibility.pack_status must be {PACK_STATUS}.")

    spec = data.get("plasma_spec_v2")
    if not isinstance(spec, dict):
        raise PackError("plasma_spec_v2 must be a table.")
    require_keys("plasma_spec_v2", set(spec), SPEC_KEYS)
    if spec.get("schema_id") != PLASMA_SCHEMA_ID:
        raise PackError(f"plasma_spec_v2.schema_id must be {PLASMA_SCHEMA_ID}.")
    if spec.get("schema_version") != PLASMA_SCHEMA_VERSION:
        raise PackError(f"plasma_spec_v2.schema_version must be {PLASMA_SCHEMA_VERSION}.")

    resolved_spec = {
        "schema_id": PLASMA_SCHEMA_ID,
        "schema_version": PLASMA_SCHEMA_VERSION,
        "field_family": resolve_token("field_family", spec.get("field_family"), FIELD_FAMILY),
        "scale": require_percent("scale", spec.get("scale")),
        "complexity": require_percent("complexity", spec.get("complexity")),
        "motion_speed": require_percent("motion_speed", spec.get("motion_speed")),
        "warp_amount": require_percent("warp_amount", spec.get("warp_amount")),
        "feedback_amount": require_percent("feedback_amount", spec.get("feedback_amount")),
        "output_style": resolve_token("output_style", spec.get("output_style"), OUTPUT_STYLE),
        "material": resolve_token("material", spec.get("material"), MATERIAL),
        "brightness": require_percent("brightness", spec.get("brightness")),
        "contrast": require_percent("contrast", spec.get("contrast")),
        "treatment": resolve_token("treatment", spec.get("treatment"), TREATMENT),
        "quality_intent": resolve_token("quality_intent", spec.get("quality_intent"), QUALITY),
        "seed_policy": resolve_token("seed_policy", spec.get("seed_policy"), SEED_POLICY),
        "presentation": resolve_token("presentation", spec.get("presentation"), PRESENTATION),
    }
    canonical_pack = {
        "pack_schema": PACK_SCHEMA,
        "pack_id": pack_id,
        "kind": kind,
        "product": product,
        "name": ensure_safe_string("name", data.get("name")),
        "author": ensure_safe_string("author", data.get("author")),
        "license": ensure_safe_string("license", data.get("license")),
        "provenance": ensure_safe_string("provenance", data.get("provenance")),
        "source_ref": repo_path(source),
        "pack_status": pack_status,
        "compatibility": {
            "minimum_schema_version": PLASMA_SCHEMA_VERSION,
            "maximum_schema_version": PLASMA_SCHEMA_VERSION,
        },
        "proof_profile": proof_profile,
        "plasma_spec_v2": resolved_spec,
        "claim_boundary": "Data-only Plasma v2 pack v1 candidate; not executable code, artistic acceptance, compatibility certification, or release promotion.",
    }
    if len(canonical_bytes(canonical_pack)) > MAX_EXPANDED_BYTES:
        raise PackError(f"expanded pack exceeds {MAX_EXPANDED_BYTES} bytes.")
    return canonical_pack


def ensure_output_dir(path: pathlib.Path) -> pathlib.Path:
    resolved = path.resolve()
    out_resolved = OUT_ROOT.resolve()
    if not (resolved == out_resolved or resolved.is_relative_to(out_resolved)):
        raise PackError("compile --out must stay under out/.")
    return resolved


def write_json(path: pathlib.Path, value: Any) -> None:
    path.write_bytes(canonical_bytes(value))


def compile_pack(source: pathlib.Path, output_dir: pathlib.Path) -> dict[str, Any]:
    data = read_toml(source)
    canonical_pack = validate_and_resolve(data, source)
    output_dir = ensure_output_dir(output_dir)
    output_parent = output_dir.parent
    output_parent.mkdir(parents=True, exist_ok=True)
    temp_dir = pathlib.Path(tempfile.mkdtemp(prefix=output_dir.name + ".tmp-", dir=output_parent))
    try:
        spec_path = temp_dir / "plasma-spec-v2.json"
        proof_path = temp_dir / "proof-profile-ref.json"
        manifest_path = temp_dir / "manifest.json"
        hash_path = temp_dir / "hash-manifest.json"
        content_hash = sha256_bytes(canonical_bytes(canonical_pack))
        proof_ref = {
            "proof_profile": canonical_pack["proof_profile"],
            "pack_status": canonical_pack["pack_status"],
            "claim_boundary": "Proof profile reference only; pack compilation does not run proof or promote release.",
        }
        manifest = {
            "pack_manifest_schema": PACK_MANIFEST_SCHEMA,
            "pack_schema": PACK_SCHEMA,
            "pack_status": canonical_pack["pack_status"],
            "pack_id": canonical_pack["pack_id"],
            "kind": canonical_pack["kind"],
            "product": canonical_pack["product"],
            "author": canonical_pack["author"],
            "license": canonical_pack["license"],
            "provenance": canonical_pack["provenance"],
            "compatibility_range": canonical_pack["compatibility"],
            "proof_profile": canonical_pack["proof_profile"],
            "file_count": MAX_OUTPUT_FILES,
            "data_only": True,
            "content_sha256": content_hash,
            "source_ref": canonical_pack["source_ref"],
            "claim_boundary": canonical_pack["claim_boundary"],
        }
        write_json(spec_path, canonical_pack)
        write_json(proof_path, proof_ref)
        write_json(manifest_path, manifest)
        files = [manifest_path, spec_path, proof_path]
        hashes = []
        total_bytes = 0
        for path in files:
            payload = path.read_bytes()
            total_bytes += len(payload)
            hashes.append({"path": path.name, "sha256": sha256_bytes(payload), "bytes": len(payload)})
        hash_manifest = {
            "pack_schema": PACK_SCHEMA,
            "pack_id": canonical_pack["pack_id"],
            "files": hashes,
            "total_bytes": total_bytes,
            "max_expanded_bytes": MAX_EXPANDED_BYTES,
        }
        write_json(hash_path, hash_manifest)
        total_bytes += len(hash_path.read_bytes())
        if total_bytes > MAX_EXPANDED_BYTES:
            raise PackError(f"compiled pack exceeds {MAX_EXPANDED_BYTES} bytes.")
        if len(list(temp_dir.iterdir())) > MAX_OUTPUT_FILES:
            raise PackError(f"compiled pack exceeds {MAX_OUTPUT_FILES} output files.")
        if output_dir.exists():
            if not output_dir.is_dir():
                raise PackError("compile --out must not replace a file.")
            shutil.rmtree(output_dir)
        temp_dir.replace(output_dir)
        temp_dir = output_dir
        return {
            "status": "pass",
            "output_dir": repo_path(output_dir),
            "manifest": repo_path(output_dir / "manifest.json"),
            "hash_manifest": repo_path(output_dir / "hash-manifest.json"),
            "proof_profile_ref": repo_path(output_dir / "proof-profile-ref.json"),
        }
    finally:
        if temp_dir.exists() and temp_dir != output_dir:
            shutil.rmtree(temp_dir)


def command_validate(args: argparse.Namespace) -> int:
    try:
        source = pathlib.Path(args.input).resolve()
        canonical_pack = validate_and_resolve(read_toml(source), source)
    except (OSError, PackError, tomllib.TOMLDecodeError) as exc:
        print(f"packc validate fail: {exc}", file=sys.stderr)
        return 1
    print(f"packc validate pass {canonical_pack['pack_id']}")
    return 0


def command_compile(args: argparse.Namespace) -> int:
    try:
        source = pathlib.Path(args.input).resolve()
        receipt = compile_pack(source, pathlib.Path(args.out))
    except (OSError, PackError, tomllib.TOMLDecodeError) as exc:
        print(f"packc compile fail: {exc}", file=sys.stderr)
        return 1
    print(json.dumps(receipt, indent=2, sort_keys=True))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    validate = subparsers.add_parser("validate")
    validate.add_argument("input")
    validate.set_defaults(func=command_validate)
    compile_cmd = subparsers.add_parser("compile")
    compile_cmd.add_argument("input")
    compile_cmd.add_argument("--out", required=True)
    compile_cmd.set_defaults(func=command_compile)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
