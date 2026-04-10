#!/usr/bin/env python
"""Bounded Plasma Lab shell for PX40.

This tool remains CLI-first and report-first.
It validates the authored Plasma substrate, reports authored and compiled
coverage, compares authored objects, audits key and migration posture, inspects
bounded degrade behavior, and compares deterministic text captures where that
surface is honestly supportable.
"""

from __future__ import annotations

import argparse
import configparser
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


REPO_ROOT = Path(__file__).resolve().parents[2]
PLASMA_ROOT = REPO_ROOT / "products" / "savers" / "plasma"
PACK_ROOT = PLASMA_ROOT / "packs" / "lava_remix"
CAPTURE_ROOT = REPO_ROOT / "validation" / "captures"

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

CHANNEL_VALUES = {"stable", "experimental"}
SOURCE_KIND_VALUES = {"built_in", "portable", "user"}

C_ARRAY_RE = re.compile(
    r"static const (?P<type_name>[a-zA-Z0-9_]+) (?P<array_name>[a-zA-Z0-9_]+)\[\] = \{(?P<body>.*?)\n\};",
    re.S,
)
PRESET_ENTRY_RE = re.compile(
    r"""
    \{
        \s*"(?P<key>[a-z0-9_]+)",
        \s*&g_plasma_presets\[\d+\],
        \s*PLASMA_CONTENT_SOURCE_(?P<source>[A-Z_]+),
        \s*PLASMA_CONTENT_CHANNEL_(?P<channel>[A-Z_]+),
        \s*PLASMA_PRESET_MORPH_CLASS_(?P<morph>[A-Z_]+),
        \s*PLASMA_TRANSITION_BRIDGE_CLASS_(?P<bridge>[A-Z_]+),
        \s*(?P<advanced>[01]),
        \s*(?P<modern>[01]),
        \s*(?P<premium>[01]),
        \s*(?P<owner>NULL|"[a-z0-9_]+")
        \s*\}
    """,
    re.S | re.X,
)
THEME_ENTRY_RE = re.compile(
    r"""
    \{
        \s*"(?P<key>[a-z0-9_]+)",
        \s*&g_plasma_themes\[\d+\],
        \s*PLASMA_CONTENT_SOURCE_(?P<source>[A-Z_]+),
        \s*PLASMA_CONTENT_CHANNEL_(?P<channel>[A-Z_]+),
        \s*PLASMA_THEME_MORPH_CLASS_(?P<morph>[A-Z_]+),
        \s*(?P<owner>NULL|"[a-z0-9_]+")
        \s*\}
    """,
    re.S | re.X,
)
ALIAS_RE = re.compile(
    r'lstrcmpiA\(key,\s*"(?P<alias>[a-z0-9_]+)"\)\s*==\s*0\)\s*\{\s*return\s*"(?P<canonical>[a-z0-9_]+)";',
    re.S,
)

CAPTURE_METADATA_KEYS = [
    "requested renderer",
    "policy target",
    "active renderer",
    "selected band",
    "degraded path",
    "selection path",
    "fallback cause",
    "renderer backend",
    "renderer status",
]
CAPTURE_BENCHLAB_KEYS = [
    "requested lane",
    "resolved lane",
    "degraded from",
    "degraded to",
    "preset key",
    "theme key",
    "preset set key",
    "theme set key",
    "journey key",
    "profile class",
    "quality class",
    "preset source",
    "preset channel",
    "theme source",
    "theme channel",
    "generator family",
    "output family",
    "output mode",
    "sampling treatment",
    "filter treatment",
    "emulation treatment",
    "accent treatment",
    "presentation mode",
    "transition requested",
    "transition enabled",
    "transition policy",
    "transition type",
    "transition fallback",
    "settings surface",
    "content filter",
    "favorites requested",
    "favorites applied",
    "benchlab forcing active",
    "clamp summary",
    "requested/resolved/degraded",
]


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Bounded Plasma Lab shell")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("validate", help="Validate the authored Plasma substrate")
    subparsers.add_parser("authoring-report", help="Print a bounded authoring inventory report")

    compare_parser = subparsers.add_parser("compare", help="Compare authored sets or journeys")
    compare_parser.add_argument("--kind", choices=["preset-set", "theme-set", "journey"], required=True)
    compare_parser.add_argument("--left", required=True)
    compare_parser.add_argument("--right", required=True)

    compat_parser = subparsers.add_parser(
        "compat-report",
        help="Print a bounded compatibility report for an authored entity or pack",
    )
    compat_parser.add_argument(
        "--kind",
        choices=["preset-set", "theme-set", "journey", "pack"],
        required=True,
    )
    compat_parser.add_argument("--target", required=True)

    subparsers.add_parser(
        "migration-report",
        help="Print a bounded migration and key-audit report for the current repo surface",
    )

    degrade_parser = subparsers.add_parser(
        "degrade-report",
        help="Print a bounded pack or capture degrade report",
    )
    degrade_group = degrade_parser.add_mutually_exclusive_group(required=True)
    degrade_group.add_argument("--pack")
    degrade_group.add_argument("--capture")

    capture_diff_parser = subparsers.add_parser(
        "capture-diff",
        help="Compare two deterministic BenchLab text captures semantically",
    )
    capture_diff_parser.add_argument("--left", required=True)
    capture_diff_parser.add_argument("--right", required=True)

    return parser


def read_ini(path: Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    parser.optionxform = str
    with path.open("r", encoding="utf-8") as handle:
        parser.read_file(handle)
    return parser


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


def normalize_profile_scope(path: Path, value: str) -> str:
    if value not in CHANNEL_VALUES:
        raise ValueError(f"{path}: unsupported profile_scope {value}")
    return value


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
    profile_scope = normalize_profile_scope(path, require(parser, "preset_set", "profile_scope"))

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
    profile_scope = normalize_profile_scope(path, require(parser, "theme_set", "profile_scope"))

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
        "profile_scope": normalize_profile_scope(path, require(parser, "journey", "profile_scope")),
        "journey_intent": require(parser, "journey", "journey_intent"),
        "steps": steps,
    }


def load_pack_manifest(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    assets: List[Tuple[str, str]] = []
    for section_key, value in parser.items("files"):
        if "_" in section_key:
            asset_kind, _ = section_key.split("_", 1)
            assets.append((asset_kind, value.strip()))
    return {
        "format": require(parser, "pack", "format"),
        "version": str(require_int(parser, "pack", "version")),
        "schema_version": str(require_int(parser, "pack", "schema_version")),
        "pack_key": require(parser, "pack", "pack_key"),
        "product_key": require(parser, "pack", "product_key"),
        "display_name": require(parser, "pack", "display_name"),
        "description": require(parser, "pack", "description"),
        "minimum_kind": parser.get("routing", "minimum_kind", fallback=""),
        "preferred_kind": parser.get("routing", "preferred_kind", fallback=""),
        "quality_class": parser.get("routing", "quality_class", fallback=""),
        "degraded_behavior": parser.get("routing", "degraded_behavior", fallback=""),
        "assets": assets,
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


def extract_c_array_body(path: Path, array_name: str) -> str:
    text = path.read_text(encoding="utf-8")
    for match in C_ARRAY_RE.finditer(text):
        if match.group("array_name") == array_name:
            return match.group("body")
    raise ValueError(f"{path}: could not find {array_name}")


def load_alias_map() -> Dict[str, str]:
    source = (PLASMA_ROOT / "src" / "plasma_presets.c").read_text(encoding="utf-8")
    aliases: Dict[str, str] = {}
    for match in ALIAS_RE.finditer(source):
        aliases[match.group("alias")] = match.group("canonical")
    return aliases


def load_compiled_catalog() -> Dict[str, Dict[str, Dict[str, object]]]:
    content_path = PLASMA_ROOT / "src" / "plasma_content.c"
    preset_body = extract_c_array_body(content_path, "g_preset_entries")
    theme_body = extract_c_array_body(content_path, "g_theme_entries")

    presets: Dict[str, Dict[str, object]] = {}
    for match in PRESET_ENTRY_RE.finditer(preset_body):
        owner = match.group("owner")
        presets[match.group("key")] = {
            "key": match.group("key"),
            "source": match.group("source").lower(),
            "channel": match.group("channel").lower(),
            "morph_class": match.group("morph").lower(),
            "bridge_class": match.group("bridge").lower(),
            "advanced_capable": match.group("advanced") == "1",
            "modern_capable": match.group("modern") == "1",
            "premium_capable": match.group("premium") == "1",
            "owner_pack_key": None if owner == "NULL" else owner.strip('"'),
        }

    themes: Dict[str, Dict[str, object]] = {}
    for match in THEME_ENTRY_RE.finditer(theme_body):
        owner = match.group("owner")
        themes[match.group("key")] = {
            "key": match.group("key"),
            "source": match.group("source").lower(),
            "channel": match.group("channel").lower(),
            "morph_class": match.group("morph").lower(),
            "owner_pack_key": None if owner == "NULL" else owner.strip('"'),
        }

    if not presets or not themes:
        raise ValueError("Could not parse the compiled Plasma content catalog.")

    return {
        "presets": presets,
        "themes": themes,
        "aliases": load_alias_map(),
    }


def load_authored_repo_surface() -> Dict[str, Dict[str, Dict[str, object]]]:
    preset_sets = {key: load_preset_set(path) for key, path in EXPECTED_PRESET_SETS.items()}
    theme_sets = {key: load_theme_set(path) for key, path in EXPECTED_THEME_SETS.items()}
    journeys = {key: load_journey(path) for key, path in EXPECTED_JOURNEYS.items()}
    manifest = load_pack_manifest(PACK_ROOT / "pack.ini")
    provenance = load_pack_provenance(PACK_ROOT / "pack.provenance.ini")
    return {
        "preset_sets": preset_sets,
        "theme_sets": theme_sets,
        "journeys": journeys,
        "packs": {
            manifest["pack_key"]: {
                "manifest": manifest,
                "provenance": provenance,
            }
        },
    }


def canonicalize_key(key: str, aliases: Dict[str, str]) -> str:
    lowered = key.lower()
    return aliases.get(lowered, lowered)


def count_channels(entries: Iterable[Dict[str, object]]) -> Counter:
    counter: Counter = Counter()
    for entry in entries:
        counter[str(entry["channel"])] += 1
    return counter


def find_alias_hits(keys: Iterable[str], aliases: Dict[str, str]) -> List[Tuple[str, str]]:
    hits: List[Tuple[str, str]] = []
    for key in keys:
        canonical = aliases.get(key.lower())
        if canonical is not None:
            hits.append((key, canonical))
    return hits


def resolve_capture_path(raw_path: str) -> Path:
    path = Path(raw_path)
    if path.is_absolute():
        candidate = path
    else:
        candidate = REPO_ROOT / path
        if not candidate.exists():
            candidate = CAPTURE_ROOT / raw_path
    if not candidate.exists():
        raise ValueError(f"capture path does not exist: {raw_path}")
    return candidate


def parse_key_value_lines(lines: Sequence[str]) -> Dict[str, str]:
    values: Dict[str, str] = {}
    for raw_line in lines:
        if ":" not in raw_line:
            continue
        key, value = raw_line.split(":", 1)
        values[key.strip().lower()] = value.strip()
    return values


def load_capture_report(path: Path) -> Dict[str, str]:
    lines = path.read_text(encoding="utf-8").splitlines()
    if not lines or not lines[0].startswith("BenchLab"):
        raise ValueError(f"{path}: unsupported capture header")

    top_lines: List[str] = []
    benchlab_lines: List[str] = []
    section = "top"
    seen_benchlab = 0
    for line in lines[1:]:
        if line.strip() == "Plasma BenchLab":
            seen_benchlab += 1
            section = "benchlab" if seen_benchlab >= 2 else "skip"
            continue
        if seen_benchlab >= 2 and section == "benchlab":
            benchlab_lines.append(line)
        elif section == "top":
            top_lines.append(line)

    top = parse_key_value_lines(top_lines)
    benchlab = parse_key_value_lines(benchlab_lines)
    if "requested lane" not in benchlab or "resolved lane" not in benchlab:
        raise ValueError(f"{path}: could not parse Plasma BenchLab report surface")

    parsed: Dict[str, str] = {"path": str(path)}
    for key in CAPTURE_METADATA_KEYS:
        if key in top:
            parsed[key] = top[key]
    for key in CAPTURE_BENCHLAB_KEYS:
        if key in benchlab:
            parsed[key] = benchlab[key]
    return parsed


def audit_repo_surface() -> Tuple[List[str], List[str], List[str], Dict[str, object], Dict[str, object]]:
    catalog = load_compiled_catalog()
    repo_surface = load_authored_repo_surface()
    failures: List[str] = []
    warnings: List[str] = []
    notes: List[str] = []

    preset_catalog = catalog["presets"]
    theme_catalog = catalog["themes"]
    aliases = catalog["aliases"]

    for set_key, data in repo_surface["preset_sets"].items():
        if data["set_key"] != set_key:
            failures.append(f"{data['path']}: set_key mismatch")
            continue
        member_keys = [member_key for member_key, _ in data["members"]]
        if len(member_keys) != len(set(member_keys)):
            failures.append(f"{data['path']}: duplicate preset members")
            continue
        unknown = [
            member_key
            for member_key in member_keys
            if canonicalize_key(member_key, aliases) not in preset_catalog
        ]
        if unknown:
            failures.append(f"{data['path']}: unknown preset keys {', '.join(unknown)}")
            continue
        alias_hits = find_alias_hits(member_keys, aliases)
        if alias_hits:
            rendered = ", ".join(f"{alias} -> {canonical}" for alias, canonical in alias_hits)
            warnings.append(f"{data['path']}: alias keys should be normalized ({rendered})")
        stable_scope = str(data["profile_scope"]) == "stable"
        experimental_members = [
            member_key
            for member_key in member_keys
            if preset_catalog[canonicalize_key(member_key, aliases)]["channel"] == "experimental"
        ]
        if stable_scope and experimental_members:
            failures.append(
                f"{data['path']}: stable preset set references experimental members {', '.join(experimental_members)}"
            )
            continue
        notes.append(f"validated preset set {set_key}")

    for set_key, data in repo_surface["theme_sets"].items():
        if data["set_key"] != set_key:
            failures.append(f"{data['path']}: set_key mismatch")
            continue
        member_keys = [member_key for member_key, _ in data["members"]]
        if len(member_keys) != len(set(member_keys)):
            failures.append(f"{data['path']}: duplicate theme members")
            continue
        unknown = [
            member_key
            for member_key in member_keys
            if canonicalize_key(member_key, aliases) not in theme_catalog
        ]
        if unknown:
            failures.append(f"{data['path']}: unknown theme keys {', '.join(unknown)}")
            continue
        alias_hits = find_alias_hits(member_keys, aliases)
        if alias_hits:
            rendered = ", ".join(f"{alias} -> {canonical}" for alias, canonical in alias_hits)
            warnings.append(f"{data['path']}: alias keys should be normalized ({rendered})")
        stable_scope = str(data["profile_scope"]) == "stable"
        experimental_members = [
            member_key
            for member_key in member_keys
            if theme_catalog[canonicalize_key(member_key, aliases)]["channel"] == "experimental"
        ]
        if stable_scope and experimental_members:
            failures.append(
                f"{data['path']}: stable theme set references experimental members {', '.join(experimental_members)}"
            )
            continue
        notes.append(f"validated theme set {set_key}")

    for journey_key, data in repo_surface["journeys"].items():
        if data["journey_key"] != journey_key:
            failures.append(f"{data['path']}: journey_key mismatch")
            continue
        if data["journey_intent"] != "ordered_cycle":
            failures.append(f"{data['path']}: unsupported journey_intent {data['journey_intent']}")
            continue
        stable_scope = str(data["profile_scope"]) == "stable"
        for preset_set_key, theme_set_key, policy, _ in data["steps"]:
            if preset_set_key not in EXPECTED_PRESET_SETS:
                failures.append(f"{data['path']}: unknown preset_set_key {preset_set_key}")
                break
            if theme_set_key not in EXPECTED_THEME_SETS:
                failures.append(f"{data['path']}: unknown theme_set_key {theme_set_key}")
                break
            if policy != "preset_set":
                failures.append(f"{data['path']}: unsupported policy {policy}")
                break
            if stable_scope and repo_surface["preset_sets"][preset_set_key]["profile_scope"] != "stable":
                failures.append(
                    f"{data['path']}: stable journey references non-stable preset set {preset_set_key}"
                )
                break
            if stable_scope and repo_surface["theme_sets"][theme_set_key]["profile_scope"] != "stable":
                failures.append(
                    f"{data['path']}: stable journey references non-stable theme set {theme_set_key}"
                )
                break
        else:
            notes.append(f"validated journey {journey_key}")

    try:
        manifest = repo_surface["packs"]["lava_remix"]["manifest"]
        provenance = repo_surface["packs"]["lava_remix"]["provenance"]
        for field in ("pack_key", "product_key", "minimum_kind", "preferred_kind", "quality_class"):
            if manifest[field] != provenance[field]:
                raise ValueError(
                    f"pack provenance mismatch for {field}: {manifest[field]!r} != {provenance[field]!r}"
                )
        if provenance["preserves_classic_identity"].lower() not in {"true", "yes", "1"}:
            raise ValueError("pack provenance must preserve classic identity")
        for asset_kind, relative_path in manifest["assets"]:
            asset_path = PACK_ROOT / relative_path
            if not asset_path.exists():
                raise ValueError(f"missing pack asset {relative_path}")
            if asset_kind == "preset":
                if canonicalize_key("plasma_lava", aliases) not in preset_catalog:
                    raise ValueError("pack preset asset canonical key is unknown")
            elif asset_kind == "theme":
                if canonicalize_key("plasma_lava", aliases) not in theme_catalog:
                    raise ValueError("pack theme asset canonical key is unknown")
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

    return failures, warnings, notes, catalog, repo_surface


def validate() -> int:
    failures, warnings, notes, _, _ = audit_repo_surface()
    if failures:
        print("PX40 Plasma Lab validate: FAILED")
        for failure in failures:
            print(f"- {failure}")
        for warning in warnings:
            print(f"- warning: {warning}")
        return 1

    print("PX40 Plasma Lab validate: OK")
    for note in notes:
        print(f"- {note}")
    for warning in warnings:
        print(f"- warning: {warning}")
    return 0


def authored_scope_counts(entries: Iterable[Dict[str, object]]) -> str:
    counter: Counter = Counter()
    for entry in entries:
        counter[str(entry["profile_scope"])] += 1
    return ", ".join(f"{scope}={counter[scope]}" for scope in sorted(counter)) or "none"


def authoring_report() -> int:
    failures, warnings, _, catalog, repo_surface = audit_repo_surface()
    preset_channels = count_channels(catalog["presets"].values())
    theme_channels = count_channels(catalog["themes"].values())
    aliases = catalog["aliases"]

    print("PX40 Plasma Lab authoring report")
    print(
        f"- compiled preset catalog: total={len(catalog['presets'])} "
        f"stable={preset_channels['stable']} experimental={preset_channels['experimental']}"
    )
    print(
        f"- compiled theme catalog: total={len(catalog['themes'])} "
        f"stable={theme_channels['stable']} experimental={theme_channels['experimental']}"
    )
    print(
        f"- authored preset sets: total={len(repo_surface['preset_sets'])} "
        f"scopes={authored_scope_counts(repo_surface['preset_sets'].values())}"
    )
    print(
        f"- authored theme sets: total={len(repo_surface['theme_sets'])} "
        f"scopes={authored_scope_counts(repo_surface['theme_sets'].values())}"
    )
    print(
        f"- authored journeys: total={len(repo_surface['journeys'])} "
        f"scopes={authored_scope_counts(repo_surface['journeys'].values())}"
    )
    print(
        f"- pack provenance: total={len(repo_surface['packs'])} "
        f"channels={', '.join(sorted(pack['provenance']['channel'] for pack in repo_surface['packs'].values()))}"
    )
    print(
        f"- canonical aliases: {', '.join(f'{alias}->{canonical}' for alias, canonical in sorted(aliases.items())) or 'none'}"
    )
    print("- author workflow entry points: validate, authoring-report, compare, compat-report, migration-report, degrade-report, capture-diff")
    print("- current authored boundary: set files, journey files, and pack provenance are on disk; built-in preset and theme descriptors remain compiled and legacy-INI anchored")
    print("- lab boundary: CLI-first and report-first only; no live editor, no gallery browser, and no pixel-diff renderer harness")
    if failures:
        print("- repo audit status: failures present")
        for failure in failures:
            print(f"  {failure}")
    else:
        print("- repo audit status: clean")
    for warning in warnings:
        print(f"- warning: {warning}")
    return 0 if not failures else 1


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
    changed = [key for key in shared if left_members[key] != right_members[key]]

    print(f"{kind_label} compare: {left[kind_label]} -> {right[kind_label]}")
    print(f"- shared members: {', '.join(shared) if shared else 'none'}")
    print(f"- left only: {', '.join(left_only) if left_only else 'none'}")
    print(f"- right only: {', '.join(right_only) if right_only else 'none'}")
    print(f"- left scope: {left['profile_scope']}")
    print(f"- right scope: {right['profile_scope']}")
    if changed:
        print("- weight deltas:")
        for key in changed:
            print(f"  {key}: {left_members[key]} -> {right_members[key]}")
    else:
        print("- weight deltas: none")
    return 0


def compare_journeys(left: Dict[str, object], right: Dict[str, object]) -> int:
    print(f"journey compare: {left['journey_key']} -> {right['journey_key']}")
    print(f"- left scope: {left['profile_scope']}")
    print(f"- right scope: {right['profile_scope']}")
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


def summarize_member_channels(
    member_keys: Iterable[str],
    compiled_catalog: Dict[str, Dict[str, object]],
    aliases: Dict[str, str],
) -> Counter:
    counter: Counter = Counter()
    for member_key in member_keys:
        canonical = canonicalize_key(member_key, aliases)
        if canonical in compiled_catalog:
            counter[str(compiled_catalog[canonical]["channel"])] += 1
    return counter


def compat_report(kind: str, target: str) -> int:
    _, warnings, _, catalog, repo_surface = audit_repo_surface()
    aliases = catalog["aliases"]

    if kind == "preset-set":
        data = repo_surface["preset_sets"][target]
        member_keys = [member_key for member_key, _ in data["members"]]
        channels = summarize_member_channels(member_keys, catalog["presets"], aliases)
        premium_members = [
            member_key
            for member_key in member_keys
            if catalog["presets"][canonicalize_key(member_key, aliases)]["premium_capable"]
        ]
        print(f"Plasma compat report: preset-set {target}")
        print(f"- scope: {data['profile_scope']}")
        print(f"- members: {len(member_keys)}")
        print(f"- member channels: stable={channels['stable']} experimental={channels['experimental']}")
        print(f"- premium-capable members: {', '.join(premium_members) if premium_members else 'none'}")
        print(f"- alias hits: {', '.join(alias for alias, _ in find_alias_hits(member_keys, aliases)) or 'none'}")
        return 0

    if kind == "theme-set":
        data = repo_surface["theme_sets"][target]
        member_keys = [member_key for member_key, _ in data["members"]]
        channels = summarize_member_channels(member_keys, catalog["themes"], aliases)
        print(f"Plasma compat report: theme-set {target}")
        print(f"- scope: {data['profile_scope']}")
        print(f"- members: {len(member_keys)}")
        print(f"- member channels: stable={channels['stable']} experimental={channels['experimental']}")
        print(f"- alias hits: {', '.join(alias for alias, _ in find_alias_hits(member_keys, aliases)) or 'none'}")
        return 0

    if kind == "journey":
        data = repo_surface["journeys"][target]
        print(f"Plasma compat report: journey {target}")
        print(f"- scope: {data['profile_scope']}")
        print(f"- intent: {data['journey_intent']}")
        print(f"- steps: {len(data['steps'])}")
        for index, step in enumerate(data["steps"], 1):
            preset_set_key, theme_set_key, policy, dwell_millis = step
            preset_scope = repo_surface["preset_sets"][preset_set_key]["profile_scope"]
            theme_scope = repo_surface["theme_sets"][theme_set_key]["profile_scope"]
            print(
                f"- step {index}: preset_set={preset_set_key} ({preset_scope}) "
                f"theme_set={theme_set_key} ({theme_scope}) policy={policy} dwell={dwell_millis}"
            )
        return 0

    pack = repo_surface["packs"][target]
    manifest = pack["manifest"]
    provenance = pack["provenance"]
    print(f"Plasma compat report: pack {target}")
    print(f"- channel: {provenance['channel']}")
    print(f"- source: {provenance['source_kind']}")
    print(f"- support tier: {provenance['support_tier']}")
    print(
        f"- routing: minimum_kind={manifest['minimum_kind']} "
        f"preferred_kind={manifest['preferred_kind']} quality_class={manifest['quality_class']}"
    )
    print(f"- migration policy: {provenance['migration_policy']}")
    print(f"- preserves classic identity: {provenance['preserves_classic_identity']}")
    print(f"- assets: {', '.join(f'{asset_kind}:{path}' for asset_kind, path in manifest['assets'])}")
    for warning in warnings:
        print(f"- warning: {warning}")
    return 0


def migration_report() -> int:
    failures, warnings, _, catalog, repo_surface = audit_repo_surface()
    aliases = catalog["aliases"]
    authored_alias_hits: List[str] = []

    for preset_set in repo_surface["preset_sets"].values():
        authored_alias_hits.extend(
            f"{preset_set['path']}: {alias} -> {canonical}"
            for alias, canonical in find_alias_hits(
                [member_key for member_key, _ in preset_set["members"]],
                aliases,
            )
        )
    for theme_set in repo_surface["theme_sets"].values():
        authored_alias_hits.extend(
            f"{theme_set['path']}: {alias} -> {canonical}"
            for alias, canonical in find_alias_hits(
                [member_key for member_key, _ in theme_set["members"]],
                aliases,
            )
        )

    print("PX40 Plasma Lab migration report")
    print(
        f"- authored formats: preset_sets={len(repo_surface['preset_sets'])}@v1 "
        f"theme_sets={len(repo_surface['theme_sets'])}@v1 "
        f"journeys={len(repo_surface['journeys'])}@v1 "
        f"pack_provenance={len(repo_surface['packs'])}@v1"
    )
    print(
        f"- canonical alias map: {', '.join(f'{alias}->{canonical}' for alias, canonical in sorted(aliases.items())) or 'none'}"
    )
    print(
        f"- authored alias usage: {', '.join(authored_alias_hits) if authored_alias_hits else 'none'}"
    )
    print("- deprecated-key rewrite surface: none")
    print("- migration posture: read-only reports only; no automatic file rewrites are performed")
    print(
        "- compiled/authored boundary: built-in preset and theme descriptors remain compiled and legacy-INI anchored, so broader preset/theme migration remains later-wave work"
    )
    print(
        f"- pack migration policy: lava_remix -> {repo_surface['packs']['lava_remix']['provenance']['migration_policy']}"
    )
    if failures:
        print("- validation blockers:")
        for failure in failures:
            print(f"  {failure}")
        return 1
    for warning in warnings:
        print(f"- warning: {warning}")
    return 0


def degrade_report_for_pack(pack_key: str) -> int:
    if pack_key != "lava_remix":
        print(f"Unknown pack {pack_key}", file=sys.stderr)
        return 1

    manifest = load_pack_manifest(PACK_ROOT / "pack.ini")
    provenance = load_pack_provenance(PACK_ROOT / "pack.provenance.ini")

    print(f"Plasma degrade report for pack {pack_key}")
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


def degrade_report_for_capture(path_text: str) -> int:
    path = resolve_capture_path(path_text)
    capture = load_capture_report(path)

    print(f"Plasma degrade report for capture {path.relative_to(REPO_ROOT)}")
    print(
        f"- renderer path: requested={capture.get('requested renderer', 'unknown')} "
        f"active={capture.get('active renderer', 'unknown')} backend={capture.get('renderer backend', 'unknown')}"
    )
    print(
        f"- lane path: requested={capture.get('requested lane', 'unknown')} "
        f"resolved={capture.get('resolved lane', 'unknown')}"
    )
    print(
        f"- degraded path: from={capture.get('degraded from', 'none')} "
        f"to={capture.get('degraded to', 'none')}"
    )
    print(
        f"- visual result: output={capture.get('output family', 'unknown')} / "
        f"{capture.get('output mode', 'unknown')} presentation={capture.get('presentation mode', 'unknown')}"
    )
    print(
        f"- treatment slots: {capture.get('sampling treatment', 'unknown')} | "
        f"{capture.get('filter treatment', 'unknown')} | "
        f"{capture.get('emulation treatment', 'unknown')} | "
        f"{capture.get('accent treatment', 'unknown')}"
    )
    print(f"- content filter: {capture.get('content filter', 'unknown')}")
    print(f"- forcing active: {capture.get('benchlab forcing active', 'unknown')}")
    print(f"- clamp summary: {capture.get('clamp summary', 'unknown')}")
    print(f"- requested/resolved/degraded: {capture.get('requested/resolved/degraded', 'unknown')}")
    return 0


def capture_diff(left_path_text: str, right_path_text: str) -> int:
    left_path = resolve_capture_path(left_path_text)
    right_path = resolve_capture_path(right_path_text)
    left = load_capture_report(left_path)
    right = load_capture_report(right_path)
    compared_keys = CAPTURE_METADATA_KEYS + CAPTURE_BENCHLAB_KEYS

    changed: List[str] = []
    same: List[str] = []
    for key in compared_keys:
        left_value = left.get(key)
        right_value = right.get(key)
        if left_value is None and right_value is None:
            continue
        if left_value == right_value:
            same.append(key)
        else:
            changed.append(key)

    print(
        "Plasma capture diff: "
        f"{left_path.relative_to(REPO_ROOT)} -> {right_path.relative_to(REPO_ROOT)}"
    )
    print("- mode: semantic BenchLab text-capture comparison")
    print("- compared keys: " + ", ".join(compared_keys))
    if same:
        print("- unchanged keys: " + ", ".join(same))
    else:
        print("- unchanged keys: none")
    if changed:
        print("- changed keys:")
        for key in changed:
            print(f"  {key}: {left.get(key, 'none')} -> {right.get(key, 'none')}")
    else:
        print("- changed keys: none")
    return 0


def main(argv: Sequence[str]) -> int:
    args = build_parser().parse_args(argv)
    if args.command == "validate":
        return validate()
    if args.command == "authoring-report":
        return authoring_report()
    if args.command == "compare":
        return compare(args.kind, args.left, args.right)
    if args.command == "compat-report":
        return compat_report(args.kind, args.target)
    if args.command == "migration-report":
        return migration_report()
    if args.command == "degrade-report":
        if args.pack:
            return degrade_report_for_pack(args.pack)
        return degrade_report_for_capture(args.capture)
    return capture_diff(args.left, args.right)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
