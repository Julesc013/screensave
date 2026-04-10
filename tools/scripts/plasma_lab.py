#!/usr/bin/env python
"""Minimal Plasma Lab shell for PX20.

This tool is intentionally small and file-first. It validates the new authored
Plasma substrate, compares authored objects, and prints bounded degrade reports.
It does not edit files or depend on runtime binaries.
"""

from __future__ import annotations

import argparse
import configparser
import re
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Sequence, Tuple


REPO_ROOT = Path(__file__).resolve().parents[2]
PLASMA_ROOT = REPO_ROOT / "products" / "savers" / "plasma"
PACK_ROOT = PLASMA_ROOT / "packs" / "lava_remix"

EXPECTED_PRESET_SETS = {
    "classic_core": PLASMA_ROOT / "preset_sets" / "classic_core.presetset.ini",
    "dark_room_classics": PLASMA_ROOT / "preset_sets" / "dark_room_classics.presetset.ini",
    "fire_classics": PLASMA_ROOT / "preset_sets" / "fire_classics.presetset.ini",
    "plasma_classics": PLASMA_ROOT / "preset_sets" / "plasma_classics.presetset.ini",
    "interference_classics": PLASMA_ROOT / "preset_sets" / "interference_classics.presetset.ini",
    "warm_bridge_classics": PLASMA_ROOT / "preset_sets" / "warm_bridge_classics.presetset.ini",
    "cool_bridge_classics": PLASMA_ROOT / "preset_sets" / "cool_bridge_classics.presetset.ini",
}

EXPECTED_THEME_SETS = {
    "classic_core": PLASMA_ROOT / "theme_sets" / "classic_core.themeset.ini",
    "dark_room_classics": PLASMA_ROOT / "theme_sets" / "dark_room_classics.themeset.ini",
    "warm_classics": PLASMA_ROOT / "theme_sets" / "warm_classics.themeset.ini",
    "cool_classics": PLASMA_ROOT / "theme_sets" / "cool_classics.themeset.ini",
}

EXPECTED_JOURNEYS = {
    "classic_cycle": PLASMA_ROOT / "journeys" / "classic_cycle.journey.ini",
    "dark_room_cycle": PLASMA_ROOT / "journeys" / "dark_room_cycle.journey.ini",
    "warm_bridge_cycle": PLASMA_ROOT / "journeys" / "warm_bridge_cycle.journey.ini",
    "cool_bridge_cycle": PLASMA_ROOT / "journeys" / "cool_bridge_cycle.journey.ini",
}

CONTENT_KEY_RE = re.compile(r'^\s*"([a-z0-9_]+)",\s*$')
CHANNEL_VALUES = {"stable", "experimental"}
SOURCE_KIND_VALUES = {"built_in", "portable", "user"}


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Minimal Plasma Lab shell")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("validate", help="Validate the PX20 authored substrate")

    compare_parser = subparsers.add_parser("compare", help="Compare authored sets or journeys")
    compare_parser.add_argument("--kind", choices=["preset-set", "theme-set", "journey"], required=True)
    compare_parser.add_argument("--left", required=True)
    compare_parser.add_argument("--right", required=True)

    degrade_parser = subparsers.add_parser("degrade-report", help="Print a bounded pack degrade report")
    degrade_parser.add_argument("--pack", required=True)

    return parser


def read_ini(path: Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    parser.optionxform = str
    with path.open("r", encoding="utf-8") as handle:
        parser.read_file(handle)
    return parser


def read_content_keys(source_path: Path) -> List[str]:
    keys: List[str] = []
    for line in source_path.read_text(encoding="utf-8").splitlines():
        match = CONTENT_KEY_RE.match(line)
        if match:
            keys.append(match.group(1))
    return keys


def require(parser: configparser.ConfigParser, section: str, key: str) -> str:
    if not parser.has_option(section, key):
        raise ValueError(f"missing [{section}] {key}")
    value = parser.get(section, key).strip()
    if not value:
        raise ValueError(f"empty [{section}] {key}")
    return value


def require_int(parser: configparser.ConfigParser, section: str, key: str) -> int:
    value = require(parser, section, key)
    try:
        parsed = int(value)
    except ValueError as exc:
        raise ValueError(f"invalid integer [{section}] {key}={value}") from exc
    return parsed


def load_preset_set(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-preset-set":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")
    profile_scope = require(parser, "preset_set", "profile_scope")
    if profile_scope not in CHANNEL_VALUES:
        raise ValueError(f"{path}: unsupported profile_scope {profile_scope}")

    members: List[Tuple[str, int]] = []
    index = 1
    while True:
        section = f"member_{index}"
        if not parser.has_section(section):
            break
        key = require(parser, section, "preset_key")
        weight = require_int(parser, section, "weight")
        if weight <= 0:
            raise ValueError(f"{path}: non-positive weight in {section}")
        members.append((key, weight))
        index += 1
    if not members:
        raise ValueError(f"{path}: no members")

    return {
        "path": path,
        "set_key": require(parser, "preset_set", "set_key"),
        "display_name": require(parser, "preset_set", "display_name"),
        "summary": require(parser, "preset_set", "summary"),
        "profile_scope": profile_scope,
        "members": members,
    }


def load_theme_set(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-theme-set":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")
    profile_scope = require(parser, "theme_set", "profile_scope")
    if profile_scope not in CHANNEL_VALUES:
        raise ValueError(f"{path}: unsupported profile_scope {profile_scope}")

    members: List[Tuple[str, int]] = []
    index = 1
    while True:
        section = f"member_{index}"
        if not parser.has_section(section):
            break
        key = require(parser, section, "theme_key")
        weight = require_int(parser, section, "weight")
        if weight <= 0:
            raise ValueError(f"{path}: non-positive weight in {section}")
        members.append((key, weight))
        index += 1
    if not members:
        raise ValueError(f"{path}: no members")

    return {
        "path": path,
        "set_key": require(parser, "theme_set", "set_key"),
        "display_name": require(parser, "theme_set", "display_name"),
        "summary": require(parser, "theme_set", "summary"),
        "profile_scope": profile_scope,
        "members": members,
    }


def load_journey(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-journey":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")

    steps: List[Tuple[str, str, str, int]] = []
    index = 1
    while True:
        section = f"step_{index}"
        if not parser.has_section(section):
            break
        preset_set_key = require(parser, section, "preset_set_key")
        theme_set_key = require(parser, section, "theme_set_key")
        policy = require(parser, section, "policy")
        dwell_millis = require_int(parser, section, "dwell_millis")
        if dwell_millis <= 0:
            raise ValueError(f"{path}: non-positive dwell in {section}")
        steps.append((preset_set_key, theme_set_key, policy, dwell_millis))
        index += 1
    if not steps:
        raise ValueError(f"{path}: no steps")

    return {
        "path": path,
        "journey_key": require(parser, "journey", "journey_key"),
        "display_name": require(parser, "journey", "display_name"),
        "summary": require(parser, "journey", "summary"),
        "profile_scope": require(parser, "journey", "profile_scope"),
        "journey_intent": require(parser, "journey", "journey_intent"),
        "steps": steps,
    }


def load_pack_manifest(path: Path) -> Dict[str, str]:
    parser = read_ini(path)
    return {
        "format": require(parser, "pack", "format"),
        "version": str(require_int(parser, "pack", "version")),
        "schema_version": str(require_int(parser, "pack", "schema_version")),
        "pack_key": require(parser, "pack", "pack_key"),
        "product_key": require(parser, "pack", "product_key"),
        "display_name": require(parser, "pack", "display_name"),
        "minimum_kind": parser.get("routing", "minimum_kind", fallback=""),
        "preferred_kind": parser.get("routing", "preferred_kind", fallback=""),
        "quality_class": parser.get("routing", "quality_class", fallback=""),
        "degraded_behavior": parser.get("routing", "degraded_behavior", fallback=""),
    }


def load_pack_provenance(path: Path) -> Dict[str, str]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-pack-provenance":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")

    channel = require(parser, "pack_provenance", "channel")
    if channel not in CHANNEL_VALUES:
        raise ValueError(f"{path}: unsupported channel {channel}")
    source_kind = require(parser, "pack_provenance", "source_kind")
    if source_kind not in SOURCE_KIND_VALUES:
        raise ValueError(f"{path}: unsupported source_kind {source_kind}")

    return {
        "pack_key": require(parser, "pack_provenance", "pack_key"),
        "content_version": str(require_int(parser, "pack_provenance", "content_version")),
        "channel": channel,
        "support_tier": require(parser, "pack_provenance", "support_tier"),
        "source_kind": source_kind,
        "author": require(parser, "pack_provenance", "author"),
        "provenance_kind": require(parser, "pack_provenance", "provenance_kind"),
        "summary": require(parser, "pack_provenance", "summary"),
        "product_key": require(parser, "compatibility", "product_key"),
        "pack_schema_version": str(require_int(parser, "compatibility", "pack_schema_version")),
        "minimum_kind": require(parser, "compatibility", "minimum_kind"),
        "preferred_kind": require(parser, "compatibility", "preferred_kind"),
        "quality_class": require(parser, "compatibility", "quality_class"),
        "migration_policy": require(parser, "compatibility", "migration_policy"),
        "preserves_classic_identity": require(parser, "compatibility", "preserves_classic_identity"),
        "compatibility_notes": require(parser, "compatibility", "compatibility_notes"),
    }


def validate() -> int:
    preset_keys = set(read_content_keys(PLASMA_ROOT / "src" / "plasma_presets.c"))
    theme_keys = set(read_content_keys(PLASMA_ROOT / "src" / "plasma_themes.c"))
    failures: List[str] = []
    notes: List[str] = []

    for set_key, path in EXPECTED_PRESET_SETS.items():
        try:
            data = load_preset_set(path)
            if data["set_key"] != set_key:
                raise ValueError(f"{path}: set_key mismatch")
            member_keys = [member_key for member_key, _ in data["members"]]
            if len(member_keys) != len(set(member_keys)):
                raise ValueError(f"{path}: duplicate preset members")
            unknown = [member_key for member_key in member_keys if member_key not in preset_keys]
            if unknown:
                raise ValueError(f"{path}: unknown preset keys {', '.join(unknown)}")
            notes.append(f"validated preset set {set_key}")
        except Exception as exc:  # pylint: disable=broad-except
            failures.append(str(exc))

    for set_key, path in EXPECTED_THEME_SETS.items():
        try:
            data = load_theme_set(path)
            if data["set_key"] != set_key:
                raise ValueError(f"{path}: set_key mismatch")
            member_keys = [member_key for member_key, _ in data["members"]]
            if len(member_keys) != len(set(member_keys)):
                raise ValueError(f"{path}: duplicate theme members")
            unknown = [member_key for member_key in member_keys if member_key not in theme_keys]
            if unknown:
                raise ValueError(f"{path}: unknown theme keys {', '.join(unknown)}")
            notes.append(f"validated theme set {set_key}")
        except Exception as exc:  # pylint: disable=broad-except
            failures.append(str(exc))

    for journey_key, path in EXPECTED_JOURNEYS.items():
        try:
            data = load_journey(path)
            if data["journey_key"] != journey_key:
                raise ValueError(f"{path}: journey_key mismatch")
            if data["journey_intent"] != "ordered_cycle":
                raise ValueError(f"{path}: unsupported journey_intent {data['journey_intent']}")
            for preset_set_key, theme_set_key, policy, _ in data["steps"]:
                if preset_set_key not in EXPECTED_PRESET_SETS:
                    raise ValueError(f"{path}: unknown preset_set_key {preset_set_key}")
                if theme_set_key not in EXPECTED_THEME_SETS:
                    raise ValueError(f"{path}: unknown theme_set_key {theme_set_key}")
                if policy != "preset_set":
                    raise ValueError(f"{path}: unsupported policy {policy}")
            notes.append(f"validated journey {journey_key}")
        except Exception as exc:  # pylint: disable=broad-except
            failures.append(str(exc))

    try:
        manifest = load_pack_manifest(PACK_ROOT / "pack.ini")
        provenance = load_pack_provenance(PACK_ROOT / "pack.provenance.ini")
        for field in ("pack_key", "product_key", "minimum_kind", "preferred_kind", "quality_class"):
            if manifest[field] != provenance[field]:
                raise ValueError(
                    f"pack provenance mismatch for {field}: {manifest[field]!r} != {provenance[field]!r}"
                )
        if provenance["preserves_classic_identity"].lower() not in {"true", "yes", "1"}:
            raise ValueError("pack provenance must preserve classic identity")
        notes.append("validated lava_remix pack provenance")
    except Exception as exc:  # pylint: disable=broad-except
        failures.append(str(exc))

    sdk_check = subprocess.run(  # noqa: S603
        [sys.executable, str(REPO_ROOT / "tools" / "scripts" / "check_sdk_surface.py"), str(PACK_ROOT)],
        cwd=REPO_ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    if sdk_check.returncode != 0:
        failures.append(f"check_sdk_surface.py failed:\n{sdk_check.stdout.strip()}")
    else:
        notes.append("validated lava_remix against the shared SDK pack shell")

    if failures:
        print("PX20 Plasma Lab validate: FAILED")
        for failure in failures:
            print(f"- {failure}")
        return 1

    print("PX20 Plasma Lab validate: OK")
    for note in notes:
        print(f"- {note}")
    return 0


def resolve_compare_target(kind: str, target: str) -> Dict[str, object]:
    if kind == "preset-set":
        return load_preset_set(EXPECTED_PRESET_SETS[target])
    if kind == "theme-set":
        return load_theme_set(EXPECTED_THEME_SETS[target])
    return load_journey(EXPECTED_JOURNEYS[target])


def compare_sets(left: Dict[str, object], right: Dict[str, object], kind_label: str) -> int:
    left_members = dict(left["members"])
    right_members = dict(right["members"])
    shared = sorted(set(left_members) & set(right_members))
    left_only = sorted(set(left_members) - set(right_members))
    right_only = sorted(set(right_members) - set(left_members))
    changed = [
        key for key in shared if left_members[key] != right_members[key]
    ]

    print(f"{kind_label} compare: {left[kind_label]} -> {right[kind_label]}")
    print(f"- shared members: {', '.join(shared) if shared else 'none'}")
    print(f"- left only: {', '.join(left_only) if left_only else 'none'}")
    print(f"- right only: {', '.join(right_only) if right_only else 'none'}")
    if changed:
        print("- weight deltas:")
        for key in changed:
            print(f"  {key}: {left_members[key]} -> {right_members[key]}")
    else:
        print("- weight deltas: none")
    return 0


def compare_journeys(left: Dict[str, object], right: Dict[str, object]) -> int:
    print(f"journey compare: {left['journey_key']} -> {right['journey_key']}")
    max_len = max(len(left["steps"]), len(right["steps"]))
    for index in range(max_len):
        left_step = left["steps"][index] if index < len(left["steps"]) else None
        right_step = right["steps"][index] if index < len(right["steps"]) else None
        print(f"- step {index + 1}: {left_step!r} -> {right_step!r}")
    return 0


def compare(kind: str, left_target: str, right_target: str) -> int:
    left = resolve_compare_target(kind, left_target)
    right = resolve_compare_target(kind, right_target)
    if kind == "preset-set":
        return compare_sets(left, right, "set_key")
    if kind == "theme-set":
        return compare_sets(left, right, "set_key")
    return compare_journeys(left, right)


def degrade_report(pack_key: str) -> int:
    if pack_key != "lava_remix":
        print(f"Unknown pack {pack_key}", file=sys.stderr)
        return 1

    manifest = load_pack_manifest(PACK_ROOT / "pack.ini")
    provenance = load_pack_provenance(PACK_ROOT / "pack.provenance.ini")

    print(f"Plasma degrade report for {pack_key}")
    print(f"- product: {manifest['product_key']}")
    print(f"- source: {provenance['source_kind']}")
    print(f"- channel: {provenance['channel']}")
    print(f"- support tier: {provenance['support_tier']}")
    print(
        f"- routing: minimum_kind={manifest['minimum_kind']} "
        f"preferred_kind={manifest['preferred_kind']} quality_class={manifest['quality_class']}"
    )
    print(f"- degraded behavior: {manifest['degraded_behavior']}")
    print(f"- migration policy: {provenance['migration_policy']}")
    print(f"- preserves classic identity: {provenance['preserves_classic_identity']}")
    print(f"- compatibility notes: {provenance['compatibility_notes']}")
    return 0


def main(argv: Sequence[str]) -> int:
    args = build_parser().parse_args(argv)
    if args.command == "validate":
        return validate()
    if args.command == "compare":
        return compare(args.kind, args.left, args.right)
    return degrade_report(args.pack)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
