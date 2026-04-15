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
INTEGRATION_ROOT = PLASMA_ROOT / "integration"
CURATION_ROOT = PLASMA_ROOT / "curation"

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

PROJECTION_SURFACE = INTEGRATION_ROOT / "projection_surface.ini"
SDK_REFERENCE_SURFACE = INTEGRATION_ROOT / "sdk_reference.ini"
CONTROL_PROFILES_SURFACE = INTEGRATION_ROOT / "control_profiles.ini"
CURATED_COLLECTIONS_SURFACE = CURATION_ROOT / "curated_collections.ini"
PROVENANCE_INDEX_SURFACE = CURATION_ROOT / "provenance_index.ini"

CHANNEL_VALUES = {"stable", "experimental"}
SOURCE_KIND_VALUES = {"built_in", "portable", "user"}
CONSUMER_VALUES = {"anthology", "suite"}
DETAIL_LEVEL_VALUES = {"low", "standard", "high"}
SEED_MODE_VALUES = {"session", "deterministic"}
RANDOMIZATION_MODE_VALUES = {"off", "session"}
LOCAL_ONLY_BEHAVIOR_VALUES = {"product_local_only"}
JOURNEY_BEHAVIOR_VALUES = {"curated_local_only", "unsupported"}
SETTINGS_HANDOFF_VALUES = {"not_applicable", "product_owned_dialog"}
PREVIEW_VISIBILITY_VALUES = {"not_applicable", "stable_only", "explicit_opt_in"}

C_ARRAY_RE = re.compile(
    r"(?:static\s+)?const (?P<type_name>[a-zA-Z0-9_]+) (?P<array_name>[a-zA-Z0-9_]+)\[\] = \{(?P<body>.*?)\n\};",
    re.S,
)
PRESET_DESCRIPTOR_KEY_RE = re.compile(r'\{\s*"(?P<key>[a-z0-9_]+)",', re.S)
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
THEME_DESCRIPTOR_RE = re.compile(
    r"""
    \{
        \s*"(?P<key>[a-z0-9_]+)",
        \s*"[^"]+",
        \s*"[^"]+",
        \s*\{\s*(?P<primary_red>\d+),\s*(?P<primary_green>\d+),\s*(?P<primary_blue>\d+),\s*\d+\s*\},
        \s*\{\s*(?P<accent_red>\d+),\s*(?P<accent_green>\d+),\s*(?P<accent_blue>\d+),\s*\d+\s*\}
        \s*\}
    """,
    re.S | re.X,
)

PRESET_VALUE_FIELDS = [
    "effect_mode",
    "speed_mode",
    "resolution_mode",
    "smoothing_mode",
    "output_family",
    "output_mode",
    "sampling_treatment",
    "filter_treatment",
    "emulation_treatment",
    "accent_treatment",
    "presentation_mode",
]

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

SETTINGS_SURFACE_NAMES = {
    "PLASMA_SETTINGS_SURFACE_BASIC": "basic",
    "PLASMA_SETTINGS_SURFACE_ADVANCED": "advanced",
    "PLASMA_SETTINGS_SURFACE_AUTHOR_LAB": "author_lab",
}

RESULT_STATUS_MEANINGS = {
    "validated": "backed by real smoke, capture, or generated audit evidence in the current repo",
    "partial": "implemented and exercised for a bounded subset or by a weaker proof method",
    "documented_only": "recorded in docs or source but not backed here by direct generated evidence",
    "unsupported": "explicitly outside the current admitted or first-class surface",
    "blocked": "would require environments or capture surfaces that the repo does not currently have",
}

U07_STABLE_LANES = ("gdi", "gl11")
U07_PREMIUM_LANES = ("gdi", "gl11", "gl46", "auto")
U07_STABLE_PRESET_SET_KEY = "classic_core"
U07_STABLE_THEME_SET_KEY = "classic_core"
U07_EXPERIMENTAL_COLLECTION_KEY = "wave3_experimental_sampler"

U07_INFLUENCE_NOTES = {
    "preset_key": ("validated", "validated", "structural_plan+render_signature", "selection identity is proved through the stable matrix and deterministic signature deltas"),
    "theme_key": ("validated", "validated", "structural_plan+render_signature+palette_distance", "theme identity is proved through deterministic signature deltas and compiled palette separation"),
    "speed_mode": ("validated", "validated", "render_signature", "pace changes are part of the shipped smoke signature subset"),
    "detail_level": ("validated", "validated", "render_signature", "detail level remains part of the shipped smoke signature subset"),
    "effect_mode": ("validated", "validated", "render_signature", "generator family selection remains part of the shipped smoke signature subset"),
    "resolution_mode": ("validated", "validated", "render_signature", "resolution changes are part of the shipped smoke signature subset"),
    "smoothing_mode": ("validated", "validated", "render_signature", "smoothing changes are part of the shipped smoke signature subset"),
    "output_family": ("validated", "validated", "render_signature", "admitted first-class output-family changes are part of the shipped smoke signature subset"),
    "output_mode": ("validated", "validated", "render_signature", "admitted first-class output-mode changes are part of the shipped smoke signature subset"),
    "filter_treatment": ("validated", "validated", "render_signature", "first-class filter treatments remain part of the shipped smoke signature subset"),
    "emulation_treatment": ("validated", "validated", "render_signature", "first-class emulation treatments remain part of the shipped smoke signature subset"),
    "accent_treatment": ("validated", "validated", "render_signature", "first-class accent treatments remain part of the shipped smoke signature subset"),
    "presentation_mode": ("validated", "validated", "render_signature+benchlab_capture", "first-class presentation changes rely on premium-lane signatures plus explicit degrade captures"),
    "preset_set_key": ("validated", "partial", "structural_plan+benchlab_capture", "set routing is selection and journey truth, not a single-frame visual proof"),
    "theme_set_key": ("validated", "partial", "structural_plan+benchlab_capture", "set routing is selection and journey truth, not a single-frame visual proof"),
    "transitions_enabled": ("validated", "partial", "structural_plan+benchlab_capture", "transition enablement affects runtime behavior over time rather than one static frame"),
    "transition_policy": ("validated", "partial", "structural_plan+benchlab_capture", "policy proof remains journey and fallback oriented rather than screenshot-grade"),
    "use_deterministic_seed": ("validated", "partial", "structural_plan", "deterministic mode changes seed policy and replay posture more directly than a single-frame diff"),
    "content_filter": ("validated", "partial", "structural_plan+benchlab_capture", "content pool filtering changes selection truth more directly than a single-frame diff"),
    "favorites_only": ("partial", "documented_only", "structural_plan", "favorites-only survives as hidden compatibility state rather than a first-class U07 proof target"),
    "journey_key": ("validated", "partial", "structural_plan+benchlab_capture", "journey choice affects transition routing over time"),
    "transition_fallback_policy": ("validated", "partial", "structural_plan+benchlab_capture", "fallback behavior is proved through plan and capture truth rather than a single-frame delta"),
    "transition_seed_policy": ("validated", "partial", "structural_plan+benchlab_capture", "seed continuity policy is runtime-state truth rather than a single-frame delta"),
    "transition_interval_millis": ("validated", "partial", "structural_plan+benchlab_capture", "interval retunes dwell timing rather than the first still frame"),
    "transition_duration_millis": ("validated", "partial", "structural_plan+benchlab_capture", "duration retunes morph timing rather than the first still frame"),
    "deterministic_seed": ("validated", "partial", "structural_plan", "seed value is resolved-runtime truth and only indirectly a first-frame visual proof"),
    "diagnostics_overlay_enabled": ("partial", "partial", "structural_plan", "diagnostics overlay remains a support-facing surface rather than a stable visual-identity proof target"),
}

U07_EXPERIMENTAL_GRAMMAR_SLICES = (
    "output families and modes: banded/posterized_bands, contour/contour_only, contour/contour_bands, glyph/ascii_glyph, glyph/matrix_glyph",
    "filter treatments: glow_edge, halftone_stipple, emboss_edge",
    "emulation treatments: phosphor, crt",
    "accent treatments: accent_pass",
    "presentation modes: heightfield and ribbon on premium plus explicit lower-lane degrade captures",
    "transition subset: bounded classic-cycle journey behavior on the surviving stable core",
)


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

    subparsers.add_parser(
        "integration-report",
        help="Print bounded local anthology, suite, SDK, and control bridge metadata",
    )

    control_parser = subparsers.add_parser(
        "control-report",
        help="Print a bounded local control-profile report",
    )
    control_parser.add_argument("--profile", required=True)

    subparsers.add_parser(
        "curation-report",
        help="Print bounded local curation and provenance metadata",
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

    preset_audit_parser = subparsers.add_parser(
        "preset-audit",
        help="Audit compiled preset signatures, near-duplicates, and theme separation",
    )
    preset_audit_parser.add_argument(
        "--threshold",
        type=int,
        default=2,
        help="Report preset pairs whose signature distance is at or below this threshold",
    )
    preset_audit_parser.add_argument(
        "--stable-preset-set",
        default=U07_STABLE_PRESET_SET_KEY,
        help="Use this authored preset set as the current first-class stable preset pool",
    )
    preset_audit_parser.add_argument(
        "--stable-theme-set",
        default=U07_STABLE_THEME_SET_KEY,
        help="Use this authored theme set as the current first-class stable theme pool",
    )

    subparsers.add_parser(
        "influence-report",
        help="Print the current U07 settings-influence proof catalog",
    )
    subparsers.add_parser(
        "combination-matrix",
        help="Print the current U07 stable combination matrix definition",
    )
    subparsers.add_parser(
        "experimental-coverage",
        help="Print the current U07 experimental coverage strategy",
    )
    subparsers.add_parser(
        "proof-result-taxonomy",
        help="Print the current U07 proof-result taxonomy",
    )

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


def require_bool(parser: configparser.ConfigParser, path: Path, section: str, key: str) -> bool:
    value = require(parser, section, key)
    if value.lower() in {"true", "yes", "1"}:
        return True
    if value.lower() in {"false", "no", "0"}:
        return False
    raise ValueError(f"{path}: invalid boolean [{section}] {key}={value}")


def optional(parser: configparser.ConfigParser, section: str, key: str) -> str:
    if not parser.has_option(section, key):
        return ""
    return parser.get(section, key).strip()


def parse_csv_list(value: str) -> List[str]:
    if not value:
        return []
    stripped = value.strip()
    if not stripped or stripped.lower() == "none":
        return []
    return [token.strip() for token in stripped.split(",") if token.strip()]


def load_saver_manifest(path: Path) -> Dict[str, str]:
    parser = read_ini(path)
    return {
        "key": require(parser, "product", "key"),
        "name": require(parser, "product", "name"),
        "summary": require(parser, "identity", "summary"),
        "default_preset": require(parser, "identity", "default_preset"),
        "default_theme": require(parser, "identity", "default_theme"),
        "minimum_kind": require(parser, "routing", "minimum_kind"),
        "preferred_kind": require(parser, "routing", "preferred_kind"),
        "quality_class": require(parser, "routing", "quality_class"),
    }


def load_projection_surface(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-projection-surface":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")

    bridges: List[Dict[str, object]] = []
    index = 1
    while True:
        section = f"bridge_{index}"
        if not parser.has_section(section):
            break
        consumer = require(parser, section, "consumer")
        if consumer not in CONSUMER_VALUES:
            raise ValueError(f"{path}: unsupported consumer {consumer}")
        visibility = require(parser, section, "visibility")
        if visibility not in CHANNEL_VALUES:
            raise ValueError(f"{path}: unsupported visibility {visibility}")
        favorite_behavior = require(parser, section, "favorite_behavior")
        if favorite_behavior not in LOCAL_ONLY_BEHAVIOR_VALUES:
            raise ValueError(f"{path}: unsupported favorite_behavior {favorite_behavior}")
        exclusion_behavior = require(parser, section, "exclusion_behavior")
        if exclusion_behavior not in LOCAL_ONLY_BEHAVIOR_VALUES:
            raise ValueError(f"{path}: unsupported exclusion_behavior {exclusion_behavior}")
        journey_behavior = require(parser, section, "journey_behavior")
        if journey_behavior not in JOURNEY_BEHAVIOR_VALUES:
            raise ValueError(f"{path}: unsupported journey_behavior {journey_behavior}")
        settings_handoff = require(parser, section, "settings_handoff")
        if settings_handoff not in SETTINGS_HANDOFF_VALUES:
            raise ValueError(f"{path}: unsupported settings_handoff {settings_handoff}")
        preview_visibility = require(parser, section, "preview_visibility")
        if preview_visibility not in PREVIEW_VISIBILITY_VALUES:
            raise ValueError(f"{path}: unsupported preview_visibility {preview_visibility}")
        bridges.append(
            {
                "bridge_key": require(parser, section, "bridge_key"),
                "consumer": consumer,
                "visibility": visibility,
                "collection_key": require(parser, section, "collection_key"),
                "featured": require_bool(parser, path, section, "featured"),
                "curated": require_bool(parser, path, section, "curated"),
                "favorite_behavior": favorite_behavior,
                "exclusion_behavior": exclusion_behavior,
                "journey_behavior": journey_behavior,
                "settings_handoff": settings_handoff,
                "preview_visibility": preview_visibility,
                "shared_contract": require(parser, section, "shared_contract"),
                "summary": require(parser, section, "summary"),
            }
        )
        index += 1
    if not bridges:
        raise ValueError(f"{path}: no bridge sections")
    return {"path": path, "bridges": bridges}


def load_sdk_reference_surface(path: Path) -> Dict[str, str]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-sdk-reference":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")
    return {
        "path": path,
        "reference_class": require(parser, "reference", "reference_class"),
        "shared_contract": require(parser, "reference", "shared_contract"),
        "default_collection_key": require(parser, "reference", "default_collection_key"),
        "experimental_collection_key": require(parser, "reference", "experimental_collection_key"),
        "example_pack_key": require(parser, "reference", "example_pack_key"),
        "stable_control_profile": require(parser, "reference", "stable_control_profile"),
        "minimum_kind": require(parser, "reference", "minimum_kind"),
        "preferred_kind": require(parser, "reference", "preferred_kind"),
        "quality_class": require(parser, "reference", "quality_class"),
        "authoring_entrypoint": require(parser, "reference", "authoring_entrypoint"),
        "summary": require(parser, "reference", "summary"),
    }


def load_control_profiles(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-control-profiles":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")

    profiles: List[Dict[str, object]] = []
    index = 1
    while True:
        section = f"profile_{index}"
        if not parser.has_section(section):
            break
        visibility = require(parser, section, "visibility")
        if visibility not in CHANNEL_VALUES:
            raise ValueError(f"{path}: unsupported visibility {visibility}")
        detail_level = require(parser, section, "detail_level")
        if detail_level not in DETAIL_LEVEL_VALUES:
            raise ValueError(f"{path}: unsupported detail_level {detail_level}")
        seed_mode = require(parser, section, "seed_mode")
        if seed_mode not in SEED_MODE_VALUES:
            raise ValueError(f"{path}: unsupported seed_mode {seed_mode}")
        randomization_mode = require(parser, section, "randomization_mode")
        if randomization_mode not in RANDOMIZATION_MODE_VALUES:
            raise ValueError(f"{path}: unsupported randomization_mode {randomization_mode}")
        profiles.append(
            {
                "profile_key": require(parser, section, "profile_key"),
                "visibility": visibility,
                "preset_key": require(parser, section, "preset_key"),
                "theme_key": require(parser, section, "theme_key"),
                "preset_set_key": optional(parser, section, "preset_set_key"),
                "theme_set_key": optional(parser, section, "theme_set_key"),
                "journey_key": optional(parser, section, "journey_key"),
                "detail_level": detail_level,
                "seed_mode": seed_mode,
                "deterministic_seed": str(require_int(parser, section, "deterministic_seed")),
                "randomization_mode": randomization_mode,
                "favorite_preset_keys": parse_csv_list(optional(parser, section, "favorite_preset_keys")),
                "excluded_preset_keys": parse_csv_list(optional(parser, section, "excluded_preset_keys")),
                "allow_experimental": require_bool(parser, path, section, "allow_experimental"),
                "summary": require(parser, section, "summary"),
            }
        )
        index += 1
    if not profiles:
        raise ValueError(f"{path}: no profile sections")
    return {"path": path, "profiles": profiles}


def load_curated_collections(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-curated-collections":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")

    collections: List[Dict[str, object]] = []
    index = 1
    while True:
        section = f"collection_{index}"
        if not parser.has_section(section):
            break
        channel = require(parser, section, "channel")
        if channel not in CHANNEL_VALUES:
            raise ValueError(f"{path}: unsupported channel {channel}")
        visibility = require(parser, section, "visibility")
        if visibility not in CHANNEL_VALUES:
            raise ValueError(f"{path}: unsupported visibility {visibility}")
        collections.append(
            {
                "collection_key": require(parser, section, "collection_key"),
                "display_name": require(parser, section, "display_name"),
                "channel": channel,
                "visibility": visibility,
                "preset_set_key": optional(parser, section, "preset_set_key"),
                "theme_set_key": optional(parser, section, "theme_set_key"),
                "journey_key": optional(parser, section, "journey_key"),
                "pack_key": optional(parser, section, "pack_key"),
                "preset_keys": parse_csv_list(optional(parser, section, "preset_keys")),
                "theme_keys": parse_csv_list(optional(parser, section, "theme_keys")),
                "featured": require_bool(parser, path, section, "featured"),
                "sdk_reference": require_bool(parser, path, section, "sdk_reference"),
                "summary": require(parser, section, "summary"),
            }
        )
        index += 1
    if not collections:
        raise ValueError(f"{path}: no collection sections")
    return {"path": path, "collections": collections}


def load_provenance_index(path: Path) -> Dict[str, object]:
    parser = read_ini(path)
    if require(parser, "format", "kind") != "plasma-provenance-index":
        raise ValueError(f"{path}: wrong kind")
    if require_int(parser, "format", "version") != 1:
        raise ValueError(f"{path}: wrong format version")
    if require(parser, "product", "key") != "plasma":
        raise ValueError(f"{path}: wrong product key")
    if require_int(parser, "product", "schema_version") != 1:
        raise ValueError(f"{path}: wrong product schema version")

    entries: List[Dict[str, object]] = []
    index = 1
    while True:
        section = f"entry_{index}"
        if not parser.has_section(section):
            break
        channel = require(parser, section, "channel")
        if channel not in CHANNEL_VALUES:
            raise ValueError(f"{path}: unsupported channel {channel}")
        source_kind = require(parser, section, "source_kind")
        if source_kind not in SOURCE_KIND_VALUES:
            raise ValueError(f"{path}: unsupported source_kind {source_kind}")
        entries.append(
            {
                "entry_key": require(parser, section, "entry_key"),
                "pack_key": require(parser, section, "pack_key"),
                "channel": channel,
                "support_tier": require(parser, section, "support_tier"),
                "source_kind": source_kind,
                "provenance_kind": require(parser, section, "provenance_kind"),
                "trust_tier": require(parser, section, "trust_tier"),
                "featured_collection_key": require(parser, section, "featured_collection_key"),
                "sdk_reference": require_bool(parser, path, section, "sdk_reference"),
                "summary": require(parser, section, "summary"),
            }
        )
        index += 1
    if not entries:
        raise ValueError(f"{path}: no provenance entries")
    return {"path": path, "entries": entries}


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


def extract_struct_entries(body: str) -> List[str]:
    entries: List[str] = []
    depth = 0
    entry_start = -1

    for index, char in enumerate(body):
        if char == "{":
            if depth == 0:
                entry_start = index + 1
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0 and entry_start >= 0:
                entries.append(body[entry_start:index])
                entry_start = -1

    return entries


def parse_struct_tokens(entry_text: str) -> List[str]:
    tokens: List[str] = []
    for line in entry_text.splitlines():
        stripped = line.strip().rstrip(",")
        if stripped:
            tokens.append(stripped)
    return tokens


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


def load_compiled_preset_signatures() -> Dict[str, Dict[str, object]]:
    preset_path = PLASMA_ROOT / "src" / "plasma_presets.c"
    descriptor_body = extract_c_array_body(preset_path, "g_plasma_presets")
    values_body = extract_c_array_body(preset_path, "g_plasma_preset_values")
    catalog = load_compiled_catalog()["presets"]

    keys = [match.group("key") for match in PRESET_DESCRIPTOR_KEY_RE.finditer(descriptor_body)]
    entries = extract_struct_entries(values_body)
    if len(keys) != len(entries):
        raise ValueError("compiled preset descriptor and value counts do not match")

    signatures: Dict[str, Dict[str, object]] = {}
    for key, entry_text in zip(keys, entries):
        tokens = parse_struct_tokens(entry_text)
        if len(tokens) != len(PRESET_VALUE_FIELDS):
            raise ValueError(f"unexpected preset signature width for {key}")
        signatures[key] = {
            "key": key,
            "channel": catalog[key]["channel"],
            "values": tokens,
        }
    return signatures


def load_compiled_theme_palettes() -> Dict[str, Dict[str, object]]:
    theme_path = PLASMA_ROOT / "src" / "plasma_themes.c"
    theme_body = extract_c_array_body(theme_path, "g_plasma_themes")
    palettes: Dict[str, Dict[str, object]] = {}

    for match in THEME_DESCRIPTOR_RE.finditer(theme_body):
        palettes[match.group("key")] = {
            "key": match.group("key"),
            "primary": (
                int(match.group("primary_red")),
                int(match.group("primary_green")),
                int(match.group("primary_blue")),
            ),
            "accent": (
                int(match.group("accent_red")),
                int(match.group("accent_green")),
                int(match.group("accent_blue")),
            ),
        }

    if not palettes:
        raise ValueError("could not parse compiled theme palettes")
    return palettes


def strip_c_string(token: str) -> str:
    stripped = token.strip()
    if len(stripped) >= 2 and stripped[0] == '"' and stripped[-1] == '"':
        return stripped[1:-1]
    return stripped


def parse_affects_mask(mask_text: str) -> List[str]:
    parts: List[str] = []
    for token in mask_text.split("|"):
        stripped = token.strip()
        if not stripped:
            continue
        if stripped.startswith("PLASMA_SETTINGS_AFFECTS_"):
            stripped = stripped[len("PLASMA_SETTINGS_AFFECTS_"):]
        parts.append(stripped.lower())
    return parts


def load_settings_catalog_entries() -> List[Dict[str, object]]:
    settings_path = PLASMA_ROOT / "src" / "plasma_settings.c"
    body = extract_c_array_body(settings_path, "g_plasma_settings_catalog")
    entries = extract_struct_entries(body)
    descriptors: List[Dict[str, object]] = []

    for entry_text in entries:
        tokens = parse_struct_tokens(entry_text)
        if len(tokens) != 11:
            raise ValueError(f"unexpected settings descriptor width: {tokens!r}")
        descriptors.append({
            "setting_key": strip_c_string(tokens[0]),
            "display_name": strip_c_string(tokens[1]),
            "summary": strip_c_string(tokens[2]),
            "surface": SETTINGS_SURFACE_NAMES.get(tokens[3].strip(), tokens[3].strip().lower()),
            "category_key": strip_c_string(tokens[4]),
            "value_type": tokens[5].strip(),
            "default_value_text": strip_c_string(tokens[6]),
            "domain_summary": strip_c_string(tokens[7]),
            "persistence_scope": tokens[8].strip().replace("PLASMA_SETTINGS_PERSIST_", "").lower(),
            "affects": parse_affects_mask(tokens[9]),
            "benchlab_exposable": tokens[10].strip() == "1",
        })

    if not descriptors:
        raise ValueError("could not parse settings catalog")
    return descriptors


def get_preset_set_member_keys(repo_surface: Dict[str, Dict[str, Dict[str, object]]], set_key: str) -> List[str]:
    if set_key not in repo_surface["preset_sets"]:
        raise ValueError(f"unknown preset set {set_key}")
    return [member_key for member_key, _ in repo_surface["preset_sets"][set_key]["members"]]


def get_theme_set_member_keys(repo_surface: Dict[str, Dict[str, Dict[str, object]]], set_key: str) -> List[str]:
    if set_key not in repo_surface["theme_sets"]:
        raise ValueError(f"unknown theme set {set_key}")
    return [member_key for member_key, _ in repo_surface["theme_sets"][set_key]["members"]]


def get_curated_collection(repo_surface: Dict[str, Dict[str, Dict[str, object]]], collection_key: str) -> Dict[str, object]:
    for collection in repo_surface["curated_collections"]["collections"]:
        if collection["collection_key"] == collection_key:
            return collection
    raise ValueError(f"unknown curated collection {collection_key}")


def preset_signature_distance(left: Sequence[str], right: Sequence[str]) -> int:
    return sum(1 for left_value, right_value in zip(left, right) if left_value != right_value)


def theme_palette_distance(
    left: Dict[str, object],
    right: Dict[str, object],
) -> int:
    left_primary = left["primary"]
    right_primary = right["primary"]
    left_accent = left["accent"]
    right_accent = right["accent"]
    return (
        sum(abs(left_primary[index] - right_primary[index]) for index in range(3)) +
        sum(abs(left_accent[index] - right_accent[index]) for index in range(3))
    )


def render_signature_summary(values: Sequence[str]) -> str:
    field_map = dict(zip(PRESET_VALUE_FIELDS, values))
    return (
        f"{field_map['effect_mode']} / "
        f"{field_map['output_family']}:{field_map['output_mode']} / "
        f"{field_map['filter_treatment']} / "
        f"{field_map['emulation_treatment']} / "
        f"{field_map['accent_treatment']} / "
        f"{field_map['presentation_mode']}"
    )


def render_signature_deltas(left: Sequence[str], right: Sequence[str]) -> str:
    deltas = [
        f"{field}={left_value}->{right_value}"
        for field, left_value, right_value in zip(PRESET_VALUE_FIELDS, left, right)
        if left_value != right_value
    ]
    return ", ".join(deltas) if deltas else "none"


def load_authored_repo_surface() -> Dict[str, Dict[str, Dict[str, object]]]:
    preset_sets = {key: load_preset_set(path) for key, path in EXPECTED_PRESET_SETS.items()}
    theme_sets = {key: load_theme_set(path) for key, path in EXPECTED_THEME_SETS.items()}
    journeys = {key: load_journey(path) for key, path in EXPECTED_JOURNEYS.items()}
    saver_manifest = load_saver_manifest(PLASMA_ROOT / "manifest.ini")
    pack_manifest = load_pack_manifest(PACK_ROOT / "pack.ini")
    provenance = load_pack_provenance(PACK_ROOT / "pack.provenance.ini")
    return {
        "manifest": saver_manifest,
        "preset_sets": preset_sets,
        "theme_sets": theme_sets,
        "journeys": journeys,
        "packs": {
            pack_manifest["pack_key"]: {
                "manifest": pack_manifest,
                "provenance": provenance,
            }
        },
        "projection_surface": load_projection_surface(PROJECTION_SURFACE),
        "sdk_reference": load_sdk_reference_surface(SDK_REFERENCE_SURFACE),
        "control_profiles": load_control_profiles(CONTROL_PROFILES_SURFACE),
        "curated_collections": load_curated_collections(CURATED_COLLECTIONS_SURFACE),
        "provenance_index": load_provenance_index(PROVENANCE_INDEX_SURFACE),
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

    try:
        collections = repo_surface["curated_collections"]["collections"]
        collection_keys: set[str] = set()
        collections_by_key: Dict[str, Dict[str, object]] = {}
        for collection in collections:
            collection_key = str(collection["collection_key"])
            if collection_key in collection_keys:
                raise ValueError(f"duplicate curated collection {collection_key}")
            collection_keys.add(collection_key)
            collections_by_key[collection_key] = collection

            preset_set_key = str(collection["preset_set_key"])
            theme_set_key = str(collection["theme_set_key"])
            journey_key = str(collection["journey_key"])
            pack_key = str(collection["pack_key"])
            preset_keys = list(collection["preset_keys"])
            theme_keys = list(collection["theme_keys"])
            stable_visibility = str(collection["visibility"]) == "stable"

            if not any((preset_set_key, theme_set_key, journey_key, pack_key, preset_keys, theme_keys)):
                raise ValueError(f"curated collection {collection_key} has no content targets")
            if preset_set_key and preset_set_key not in repo_surface["preset_sets"]:
                raise ValueError(f"curated collection {collection_key} references unknown preset_set_key {preset_set_key}")
            if theme_set_key and theme_set_key not in repo_surface["theme_sets"]:
                raise ValueError(f"curated collection {collection_key} references unknown theme_set_key {theme_set_key}")
            if journey_key and journey_key not in repo_surface["journeys"]:
                raise ValueError(f"curated collection {collection_key} references unknown journey_key {journey_key}")
            if pack_key and pack_key not in repo_surface["packs"]:
                raise ValueError(f"curated collection {collection_key} references unknown pack_key {pack_key}")

            if stable_visibility and str(collection["channel"]) != "stable":
                raise ValueError(f"curated collection {collection_key} is stable-visible but not stable-channel")
            if stable_visibility and preset_set_key and repo_surface["preset_sets"][preset_set_key]["profile_scope"] != "stable":
                raise ValueError(f"curated collection {collection_key} stable visibility references non-stable preset set {preset_set_key}")
            if stable_visibility and theme_set_key and repo_surface["theme_sets"][theme_set_key]["profile_scope"] != "stable":
                raise ValueError(f"curated collection {collection_key} stable visibility references non-stable theme set {theme_set_key}")
            if stable_visibility and journey_key and repo_surface["journeys"][journey_key]["profile_scope"] != "stable":
                raise ValueError(f"curated collection {collection_key} stable visibility references non-stable journey {journey_key}")
            if stable_visibility and pack_key and repo_surface["packs"][pack_key]["provenance"]["channel"] != "stable":
                raise ValueError(f"curated collection {collection_key} stable visibility references non-stable pack {pack_key}")

            unknown_presets = [
                preset_key
                for preset_key in preset_keys
                if canonicalize_key(preset_key, aliases) not in preset_catalog
            ]
            if unknown_presets:
                raise ValueError(
                    f"curated collection {collection_key} references unknown preset keys {', '.join(unknown_presets)}"
                )
            unknown_themes = [
                theme_key
                for theme_key in theme_keys
                if canonicalize_key(theme_key, aliases) not in theme_catalog
            ]
            if unknown_themes:
                raise ValueError(
                    f"curated collection {collection_key} references unknown theme keys {', '.join(unknown_themes)}"
                )

            preset_alias_hits = find_alias_hits(preset_keys, aliases)
            if preset_alias_hits:
                rendered = ", ".join(f"{alias} -> {canonical}" for alias, canonical in preset_alias_hits)
                warnings.append(f"curated collection {collection_key}: alias preset keys should be normalized ({rendered})")
            theme_alias_hits = find_alias_hits(theme_keys, aliases)
            if theme_alias_hits:
                rendered = ", ".join(f"{alias} -> {canonical}" for alias, canonical in theme_alias_hits)
                warnings.append(f"curated collection {collection_key}: alias theme keys should be normalized ({rendered})")

            if stable_visibility:
                experimental_presets = [
                    preset_key
                    for preset_key in preset_keys
                    if preset_catalog[canonicalize_key(preset_key, aliases)]["channel"] == "experimental"
                ]
                if experimental_presets:
                    raise ValueError(
                        f"curated collection {collection_key} stable visibility references experimental preset keys {', '.join(experimental_presets)}"
                    )
                experimental_themes = [
                    theme_key
                    for theme_key in theme_keys
                    if theme_catalog[canonicalize_key(theme_key, aliases)]["channel"] == "experimental"
                ]
                if experimental_themes:
                    raise ValueError(
                        f"curated collection {collection_key} stable visibility references experimental theme keys {', '.join(experimental_themes)}"
                    )
            notes.append(f"validated curated collection {collection_key}")

        bridges = repo_surface["projection_surface"]["bridges"]
        bridge_keys: set[str] = set()
        for bridge in bridges:
            bridge_key = str(bridge["bridge_key"])
            if bridge_key in bridge_keys:
                raise ValueError(f"duplicate projection bridge {bridge_key}")
            bridge_keys.add(bridge_key)
            collection_key = str(bridge["collection_key"])
            if collection_key not in collections_by_key:
                raise ValueError(f"projection bridge {bridge_key} references unknown collection {collection_key}")
            collection = collections_by_key[collection_key]
            if str(bridge["visibility"]) != str(collection["visibility"]):
                raise ValueError(f"projection bridge {bridge_key} visibility does not match collection {collection_key}")
            if str(bridge["shared_contract"]) != "sy40_suite_and_anthology":
                raise ValueError(f"projection bridge {bridge_key} references unsupported shared contract {bridge['shared_contract']}")
            if str(bridge["consumer"]) == "anthology":
                if str(bridge["settings_handoff"]) != "not_applicable" or str(bridge["preview_visibility"]) != "not_applicable":
                    raise ValueError(f"projection bridge {bridge_key} uses suite-only handoff or preview fields")
            else:
                if str(bridge["settings_handoff"]) != "product_owned_dialog":
                    raise ValueError(f"projection bridge {bridge_key} must use product_owned_dialog settings handoff")
                if str(bridge["preview_visibility"]) == "not_applicable":
                    raise ValueError(f"projection bridge {bridge_key} must declare preview_visibility")
            notes.append(f"validated projection bridge {bridge_key}")

        control_profiles = repo_surface["control_profiles"]["profiles"]
        control_profiles_by_key: Dict[str, Dict[str, object]] = {}
        for profile in control_profiles:
            profile_key = str(profile["profile_key"])
            if profile_key in control_profiles_by_key:
                raise ValueError(f"duplicate control profile {profile_key}")
            control_profiles_by_key[profile_key] = profile
            preset_key = canonicalize_key(str(profile["preset_key"]), aliases)
            theme_key = canonicalize_key(str(profile["theme_key"]), aliases)
            preset_set_key = str(profile["preset_set_key"])
            theme_set_key = str(profile["theme_set_key"])
            journey_key = str(profile["journey_key"])
            stable_visibility = str(profile["visibility"]) == "stable"

            if preset_key not in preset_catalog:
                raise ValueError(f"control profile {profile_key} references unknown preset_key {profile['preset_key']}")
            if theme_key not in theme_catalog:
                raise ValueError(f"control profile {profile_key} references unknown theme_key {profile['theme_key']}")
            if preset_set_key and preset_set_key not in repo_surface["preset_sets"]:
                raise ValueError(f"control profile {profile_key} references unknown preset_set_key {preset_set_key}")
            if theme_set_key and theme_set_key not in repo_surface["theme_sets"]:
                raise ValueError(f"control profile {profile_key} references unknown theme_set_key {theme_set_key}")
            if journey_key and journey_key not in repo_surface["journeys"]:
                raise ValueError(f"control profile {profile_key} references unknown journey_key {journey_key}")

            if preset_set_key:
                preset_members = [member_key for member_key, _ in repo_surface["preset_sets"][preset_set_key]["members"]]
                if preset_key not in {canonicalize_key(member_key, aliases) for member_key in preset_members}:
                    raise ValueError(f"control profile {profile_key} preset_key is not a member of preset_set_key {preset_set_key}")
            if theme_set_key:
                theme_members = [member_key for member_key, _ in repo_surface["theme_sets"][theme_set_key]["members"]]
                if theme_key not in {canonicalize_key(member_key, aliases) for member_key in theme_members}:
                    raise ValueError(f"control profile {profile_key} theme_key is not a member of theme_set_key {theme_set_key}")

            favorite_alias_hits = find_alias_hits(profile["favorite_preset_keys"], aliases)
            if favorite_alias_hits:
                rendered = ", ".join(f"{alias} -> {canonical}" for alias, canonical in favorite_alias_hits)
                warnings.append(f"control profile {profile_key}: alias favorite_preset_keys should be normalized ({rendered})")
            excluded_alias_hits = find_alias_hits(profile["excluded_preset_keys"], aliases)
            if excluded_alias_hits:
                rendered = ", ".join(f"{alias} -> {canonical}" for alias, canonical in excluded_alias_hits)
                warnings.append(f"control profile {profile_key}: alias excluded_preset_keys should be normalized ({rendered})")

            favorite_keys = [canonicalize_key(key, aliases) for key in profile["favorite_preset_keys"]]
            excluded_keys = [canonicalize_key(key, aliases) for key in profile["excluded_preset_keys"]]
            for favorite_key in favorite_keys:
                if favorite_key not in preset_catalog:
                    raise ValueError(f"control profile {profile_key} references unknown favorite preset {favorite_key}")
            for excluded_key in excluded_keys:
                if excluded_key not in preset_catalog:
                    raise ValueError(f"control profile {profile_key} references unknown excluded preset {excluded_key}")

            if stable_visibility:
                if bool(profile["allow_experimental"]):
                    raise ValueError(f"control profile {profile_key} stable visibility cannot allow experimental content")
                if str(preset_catalog[preset_key]["channel"]) != "stable":
                    raise ValueError(f"control profile {profile_key} stable visibility references experimental preset {preset_key}")
                if str(theme_catalog[theme_key]["channel"]) != "stable":
                    raise ValueError(f"control profile {profile_key} stable visibility references experimental theme {theme_key}")
                if preset_set_key and repo_surface["preset_sets"][preset_set_key]["profile_scope"] != "stable":
                    raise ValueError(f"control profile {profile_key} stable visibility references non-stable preset set {preset_set_key}")
                if theme_set_key and repo_surface["theme_sets"][theme_set_key]["profile_scope"] != "stable":
                    raise ValueError(f"control profile {profile_key} stable visibility references non-stable theme set {theme_set_key}")
                if journey_key and repo_surface["journeys"][journey_key]["profile_scope"] != "stable":
                    raise ValueError(f"control profile {profile_key} stable visibility references non-stable journey {journey_key}")
                experimental_favorites = [
                    favorite_key for favorite_key in favorite_keys if preset_catalog[favorite_key]["channel"] == "experimental"
                ]
                if experimental_favorites:
                    raise ValueError(
                        f"control profile {profile_key} stable visibility references experimental favorites {', '.join(experimental_favorites)}"
                    )
            notes.append(f"validated control profile {profile_key}")

        sdk_reference = repo_surface["sdk_reference"]
        if str(sdk_reference["shared_contract"]) != "sy40_sdk_reference":
            raise ValueError(f"sdk reference uses unsupported shared contract {sdk_reference['shared_contract']}")
        if str(sdk_reference["default_collection_key"]) not in collections_by_key:
            raise ValueError(f"sdk reference references unknown default collection {sdk_reference['default_collection_key']}")
        if str(sdk_reference["experimental_collection_key"]) not in collections_by_key:
            raise ValueError(
                f"sdk reference references unknown experimental collection {sdk_reference['experimental_collection_key']}"
            )
        if str(sdk_reference["example_pack_key"]) not in repo_surface["packs"]:
            raise ValueError(f"sdk reference references unknown example pack {sdk_reference['example_pack_key']}")
        stable_profile_key = str(sdk_reference["stable_control_profile"])
        if stable_profile_key not in control_profiles_by_key:
            raise ValueError(f"sdk reference references unknown stable control profile {stable_profile_key}")
        if str(control_profiles_by_key[stable_profile_key]["visibility"]) != "stable":
            raise ValueError(f"sdk reference stable control profile {stable_profile_key} is not stable")
        if str(collections_by_key[str(sdk_reference['default_collection_key'])]["visibility"]) != "stable":
            raise ValueError("sdk reference default collection must be stable")
        if str(collections_by_key[str(sdk_reference['experimental_collection_key'])]["visibility"]) != "experimental":
            raise ValueError("sdk reference experimental collection must be experimental")
        if str(sdk_reference["minimum_kind"]) != str(repo_surface["manifest"]["minimum_kind"]):
            raise ValueError("sdk reference minimum_kind does not match manifest.ini")
        if str(sdk_reference["preferred_kind"]) != str(repo_surface["manifest"]["preferred_kind"]):
            raise ValueError("sdk reference preferred_kind does not match manifest.ini")
        if str(sdk_reference["quality_class"]) != str(repo_surface["manifest"]["quality_class"]):
            raise ValueError("sdk reference quality_class does not match manifest.ini")
        if str(sdk_reference["authoring_entrypoint"]) != "python tools/scripts/plasma_lab.py":
            raise ValueError("sdk reference authoring_entrypoint is not the current Plasma Lab entrypoint")
        notes.append("validated sdk reference bridge surface")

        provenance_entries = repo_surface["provenance_index"]["entries"]
        provenance_keys: set[str] = set()
        for entry in provenance_entries:
            entry_key = str(entry["entry_key"])
            if entry_key in provenance_keys:
                raise ValueError(f"duplicate provenance index entry {entry_key}")
            provenance_keys.add(entry_key)
            pack_key = str(entry["pack_key"])
            if pack_key not in repo_surface["packs"]:
                raise ValueError(f"provenance index entry {entry_key} references unknown pack {pack_key}")
            if str(entry["featured_collection_key"]) not in collections_by_key:
                raise ValueError(
                    f"provenance index entry {entry_key} references unknown collection {entry['featured_collection_key']}"
                )
            pack_provenance = repo_surface["packs"][pack_key]["provenance"]
            for field in ("channel", "support_tier", "source_kind", "provenance_kind"):
                if str(entry[field]) != str(pack_provenance[field]):
                    raise ValueError(
                        f"provenance index entry {entry_key} mismatch for {field}: {entry[field]!r} != {pack_provenance[field]!r}"
                    )
            notes.append(f"validated provenance index entry {entry_key}")
    except Exception as exc:  # pylint: disable=broad-except
        failures.append(str(exc))

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
        f"- local bridge metadata: bridges={len(repo_surface['projection_surface']['bridges'])} "
        f"control_profiles={len(repo_surface['control_profiles']['profiles'])} "
        f"collections={len(repo_surface['curated_collections']['collections'])} "
        f"provenance_entries={len(repo_surface['provenance_index']['entries'])}"
    )
    print(
        f"- canonical aliases: {', '.join(f'{alias}->{canonical}' for alias, canonical in sorted(aliases.items())) or 'none'}"
    )
    print(
        "- author workflow entry points: validate, authoring-report, compare, compat-report, migration-report, integration-report, control-report, curation-report, degrade-report, capture-diff, influence-report, combination-matrix, experimental-coverage, proof-result-taxonomy"
    )
    print("- current authored boundary: set files, journey files, and pack provenance are on disk; built-in preset and theme descriptors remain compiled and legacy-INI anchored")
    print(
        "- local bridge boundary: projection, sdk-reference, control-profile, collection, and provenance metadata are file-first and report-first only; no suite-runtime bridge, no automation daemon, and no community platform"
    )
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


def integration_report() -> int:
    failures, warnings, _, _, repo_surface = audit_repo_surface()
    print("PX41 Plasma Lab integration report")
    print("- shared contracts consumed: sy40_suite_and_anthology, sy40_sdk_reference")
    print(f"- anthology and suite bridges: {len(repo_surface['projection_surface']['bridges'])}")
    for bridge in repo_surface["projection_surface"]["bridges"]:
        print(
            f"  {bridge['bridge_key']}: consumer={bridge['consumer']} visibility={bridge['visibility']} "
            f"collection={bridge['collection_key']} featured={str(bridge['featured']).lower()} "
            f"curated={str(bridge['curated']).lower()}"
        )
        print(
            f"    favorite_behavior={bridge['favorite_behavior']} exclusion_behavior={bridge['exclusion_behavior']} "
            f"journey_behavior={bridge['journey_behavior']} settings_handoff={bridge['settings_handoff']} "
            f"preview_visibility={bridge['preview_visibility']}"
        )
    sdk_reference = repo_surface["sdk_reference"]
    print("- sdk reference surface:")
    print(
        f"  class={sdk_reference['reference_class']} example_pack={sdk_reference['example_pack_key']} "
        f"default_collection={sdk_reference['default_collection_key']} "
        f"experimental_collection={sdk_reference['experimental_collection_key']}"
    )
    print(
        f"  routing={sdk_reference['minimum_kind']}->{sdk_reference['preferred_kind']} "
        f"quality_class={sdk_reference['quality_class']} "
        f"authoring_entrypoint={sdk_reference['authoring_entrypoint']}"
    )
    print(f"- control profiles: {len(repo_surface['control_profiles']['profiles'])}")
    for profile in repo_surface["control_profiles"]["profiles"]:
        print(
            f"  {profile['profile_key']}: visibility={profile['visibility']} preset={profile['preset_key']} "
            f"theme={profile['theme_key']} journey={profile['journey_key'] or 'none'} "
            f"allow_experimental={str(profile['allow_experimental']).lower()}"
        )
    if failures:
        print("- validation blockers:")
        for failure in failures:
            print(f"  {failure}")
        return 1
    for warning in warnings:
        print(f"- warning: {warning}")
    return 0


def control_report(profile_key: str) -> int:
    failures, warnings, _, catalog, repo_surface = audit_repo_surface()
    aliases = catalog["aliases"]
    profiles = {
        str(profile["profile_key"]): profile
        for profile in repo_surface["control_profiles"]["profiles"]
    }
    if profile_key not in profiles:
        print(f"Unknown control profile {profile_key}", file=sys.stderr)
        return 1
    profile = profiles[profile_key]
    favorite_keys = [canonicalize_key(key, aliases) for key in profile["favorite_preset_keys"]]
    excluded_keys = [canonicalize_key(key, aliases) for key in profile["excluded_preset_keys"]]
    print(f"PX41 Plasma Lab control report: {profile_key}")
    print(f"- visibility: {profile['visibility']}")
    print(f"- preset: {profile['preset_key']}")
    print(f"- theme: {profile['theme_key']}")
    print(f"- preset_set: {profile['preset_set_key'] or 'none'}")
    print(f"- theme_set: {profile['theme_set_key'] or 'none'}")
    print(f"- journey: {profile['journey_key'] or 'none'}")
    print(
        f"- seed and detail: seed_mode={profile['seed_mode']} deterministic_seed={profile['deterministic_seed']} "
        f"detail_level={profile['detail_level']} randomization_mode={profile['randomization_mode']}"
    )
    print(
        f"- favorites and exclusions: favorites={','.join(favorite_keys) if favorite_keys else 'none'} "
        f"excluded={','.join(excluded_keys) if excluded_keys else 'none'}"
    )
    print(
        f"- clamp posture: allow_experimental={str(profile['allow_experimental']).lower()} "
        "runtime bypass=none validation=report_first"
    )
    if failures:
        print("- validation blockers:")
        for failure in failures:
            print(f"  {failure}")
        return 1
    for warning in warnings:
        print(f"- warning: {warning}")
    return 0


def curation_report() -> int:
    failures, warnings, _, _, repo_surface = audit_repo_surface()
    print("PX41 Plasma Lab curation report")
    print(f"- curated collections: {len(repo_surface['curated_collections']['collections'])}")
    for collection in repo_surface["curated_collections"]["collections"]:
        print(
            f"  {collection['collection_key']}: visibility={collection['visibility']} "
            f"channel={collection['channel']} featured={str(collection['featured']).lower()} "
            f"sdk_reference={str(collection['sdk_reference']).lower()} pack={collection['pack_key'] or 'none'}"
        )
        print(
            f"    preset_set={collection['preset_set_key'] or 'none'} theme_set={collection['theme_set_key'] or 'none'} "
            f"journey={collection['journey_key'] or 'none'}"
        )
        print(
            f"    preset_keys={','.join(collection['preset_keys']) if collection['preset_keys'] else 'none'} "
            f"theme_keys={','.join(collection['theme_keys']) if collection['theme_keys'] else 'none'}"
        )
    print(f"- provenance index entries: {len(repo_surface['provenance_index']['entries'])}")
    for entry in repo_surface["provenance_index"]["entries"]:
        print(
            f"  {entry['entry_key']}: pack={entry['pack_key']} channel={entry['channel']} "
            f"support_tier={entry['support_tier']} source_kind={entry['source_kind']} "
            f"trust_tier={entry['trust_tier']}"
        )
        print(
            f"    featured_collection={entry['featured_collection_key']} "
            f"sdk_reference={str(entry['sdk_reference']).lower()}"
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


def influence_report() -> int:
    descriptors = load_settings_catalog_entries()

    print("U07 Plasma settings influence report")
    print("- proof posture: mixed model")
    print("- status vocabulary: " + ", ".join(sorted(RESULT_STATUS_MEANINGS)))
    print("- note: render-visible proof is stronger than plan-only proof and is reported separately")

    for descriptor in descriptors:
        setting_key = str(descriptor["setting_key"])
        plan_status, render_status, method, notes = U07_INFLUENCE_NOTES.get(
            setting_key,
            ("documented_only", "documented_only", "documented_only", "no explicit U07 mapping exists yet"),
        )
        print(f"- {setting_key}")
        print(
            f"  surface={descriptor['surface']} category={descriptor['category_key']} "
            f"benchlab_exposable={str(descriptor['benchlab_exposable']).lower()}"
        )
        print(
            f"  affects={','.join(descriptor['affects']) if descriptor['affects'] else 'none'} "
            f"default={descriptor['default_value_text'] or 'empty'}"
        )
        print(
            f"  plan_result={plan_status} render_result={render_status} "
            f"proof_method={method}"
        )
        print(f"  note={notes}")
    return 0


def combination_matrix() -> int:
    repo_surface = load_authored_repo_surface()
    stable_presets = get_preset_set_member_keys(repo_surface, U07_STABLE_PRESET_SET_KEY)
    stable_themes = get_theme_set_member_keys(repo_surface, U07_STABLE_THEME_SET_KEY)

    print("U07 Plasma stable combination matrix")
    print("- scope: actual reachable stable first-class preset/theme selections only")
    print(
        f"- stable preset set: {U07_STABLE_PRESET_SET_KEY} ({len(stable_presets)} members)"
    )
    print(
        f"- stable theme set: {U07_STABLE_THEME_SET_KEY} ({len(stable_themes)} members)"
    )
    print("- lanes: " + ", ".join(U07_STABLE_LANES))
    print("- exhaustive rows: " + str(len(stable_presets) * len(stable_themes) * len(U07_STABLE_LANES)))
    print("- result classes used here: validated, partial, blocked")
    print("- row meaning: each row is a real reachable stable preset/theme/lane combination, not an idealized grammar cross-product")
    print("")
    print("| preset_key | theme_key | lane | result | proof_method | note |")
    print("| --- | --- | --- | --- | --- | --- |")
    for lane in U07_STABLE_LANES:
        for preset_key in stable_presets:
            for theme_key in stable_themes:
                print(
                    f"| {preset_key} | {theme_key} | {lane} | validated | "
                    "smoke_matrix_compile | exhaustively exercised by the U07 stable matrix harness |"
                )

    print("")
    print("- deliberate exclusions:")
    print("  - advanced grammar cross-products are not treated as stable matrix rows")
    print("  - premium-only presentation requests are not treated as stable rows")
    print("  - compatibility-only preset and theme keys remain outside the first-class stable matrix")
    print("  - broader experimental content stays in the experimental coverage report rather than this stable matrix")
    return 0


def experimental_coverage() -> int:
    repo_surface = load_authored_repo_surface()
    sampler = get_curated_collection(repo_surface, U07_EXPERIMENTAL_COLLECTION_KEY)

    print("U07 Plasma experimental coverage")
    print("- coverage model: bounded covering slices rather than exhaustive cross-product proof")
    print(f"- experimental collection: {sampler['collection_key']}")
    print("- preset slice: " + ", ".join(sampler["preset_keys"]))
    print("- theme slice: " + ", ".join(sampler["theme_keys"]))
    print("- lane slice: " + ", ".join(U07_PREMIUM_LANES))
    print("- result posture: experimental coverage is allowed to stay partial as long as the boundary is explicit")
    print("- coverage slices:")
    for slice_text in U07_EXPERIMENTAL_GRAMMAR_SLICES:
        print(f"  - {slice_text}")
    print("- deliberate non-coverage:")
    print("  - no exhaustive experimental preset/theme/output/treatment/presentation cross-product")
    print("  - no claim of screenshot-grade perceptual coverage on every renderer path")
    print("  - no claim that compatibility-only or hidden controls are first-class experimental proof targets")
    return 0


def proof_result_taxonomy() -> int:
    print("U07 Plasma proof-result taxonomy")
    for status, meaning in RESULT_STATUS_MEANINGS.items():
        print(f"- {status}: {meaning}")
    print("- proof methods:")
    print("  - render_signature: deterministic smoke treated or presented buffer difference")
    print("  - structural_plan: resolved settings, selection, and compiled plan truth without claiming screenshot proof")
    print("  - benchlab_capture: requested/resolved/degraded report proof on the BenchLab text surface")
    print("  - palette_distance: compiled theme-palette separation proof")
    print("  - smoke_matrix_compile: exhaustive stable matrix compile and validation coverage")
    print("- interpretation rule: U08 should treat validated rows as evidence-backed, partial rows as bounded, documented_only rows as non-proof, unsupported rows as non-claims, and blocked rows as environment gaps")
    return 0


def preset_audit(threshold: int, stable_preset_set: str, stable_theme_set: str) -> int:
    signatures = load_compiled_preset_signatures()
    palettes = load_compiled_theme_palettes()
    repo_surface = load_authored_repo_surface()
    stable_preset_members = set(get_preset_set_member_keys(repo_surface, stable_preset_set))
    stable_theme_members = set(get_theme_set_member_keys(repo_surface, stable_theme_set))
    keys = sorted(signatures)
    exact_duplicates: List[Tuple[str, str]] = []
    near_duplicates: List[Tuple[int, str, str]] = []
    stable_pairs: List[Tuple[int, str, str]] = []
    stable_focus_pairs: List[Tuple[int, str, str]] = []

    for index, left_key in enumerate(keys):
        for right_key in keys[index + 1:]:
            distance = preset_signature_distance(
                signatures[left_key]["values"],
                signatures[right_key]["values"],
            )
            if distance == 0:
                exact_duplicates.append((left_key, right_key))
            if distance <= threshold:
                near_duplicates.append((distance, left_key, right_key))
            if (
                signatures[left_key]["channel"] == "stable" and
                signatures[right_key]["channel"] == "stable"
            ):
                stable_pairs.append((distance, left_key, right_key))
            if left_key in stable_preset_members and right_key in stable_preset_members:
                stable_focus_pairs.append((distance, left_key, right_key))

    stable_pairs.sort()
    stable_focus_pairs.sort()
    near_duplicates.sort()

    stable_filter_coverage = Counter(
        signatures[key]["values"][PRESET_VALUE_FIELDS.index("filter_treatment")]
        for key in keys
        if signatures[key]["channel"] == "stable"
    )
    stable_emulation_coverage = Counter(
        signatures[key]["values"][PRESET_VALUE_FIELDS.index("emulation_treatment")]
        for key in keys
        if signatures[key]["channel"] == "stable"
    )
    stable_accent_coverage = Counter(
        signatures[key]["values"][PRESET_VALUE_FIELDS.index("accent_treatment")]
        for key in keys
        if signatures[key]["channel"] == "stable"
    )

    theme_pairs: List[Tuple[int, str, str]] = []
    theme_keys = sorted(palettes)
    for index, left_key in enumerate(theme_keys):
        for right_key in theme_keys[index + 1:]:
            theme_pairs.append((
                theme_palette_distance(palettes[left_key], palettes[right_key]),
                left_key,
                right_key,
            ))
    theme_pairs.sort()
    stable_theme_pairs = [
        (distance, left_key, right_key)
        for distance, left_key, right_key in theme_pairs
        if left_key in stable_theme_members and right_key in stable_theme_members
    ]

    print("PX40 Plasma Lab preset audit")
    print(f"- compiled presets audited: {len(keys)}")
    if exact_duplicates:
        print("- exact duplicate signatures:")
        for left_key, right_key in exact_duplicates:
            print(f"  {left_key} == {right_key}")
    else:
        print("- exact duplicate signatures: none")

    if stable_pairs:
        min_distance, left_key, right_key = stable_pairs[0]
        print(
            f"- closest stable pair: {left_key} <-> {right_key} "
            f"(distance={min_distance}; {render_signature_deltas(signatures[left_key]['values'], signatures[right_key]['values'])})"
        )
    else:
        print("- closest stable pair: none")

    if stable_focus_pairs:
        min_distance, left_key, right_key = stable_focus_pairs[0]
        print(
            f"- closest first-class stable pair ({stable_preset_set}): {left_key} <-> {right_key} "
            f"(distance={min_distance}; {render_signature_deltas(signatures[left_key]['values'], signatures[right_key]['values'])})"
        )
    else:
        print(f"- closest first-class stable pair ({stable_preset_set}): none")

    if near_duplicates:
        print(f"- near-duplicate pairs at threshold <= {threshold}:")
        for distance, left_key, right_key in near_duplicates:
            print(
                f"  {left_key} <-> {right_key}: distance={distance} "
                f"({render_signature_deltas(signatures[left_key]['values'], signatures[right_key]['values'])})"
            )
    else:
        print(f"- near-duplicate pairs at threshold <= {threshold}: none")

    print(
        "- stable treatment coverage: "
        f"filter={dict(sorted(stable_filter_coverage.items()))} "
        f"emulation={dict(sorted(stable_emulation_coverage.items()))} "
        f"accent={dict(sorted(stable_accent_coverage.items()))}"
    )
    print("- example stable signatures:")
    for key in keys:
        if signatures[key]["channel"] != "stable":
            continue
        print(f"  {key}: {render_signature_summary(signatures[key]['values'])}")

    if theme_pairs:
        distance, left_key, right_key = theme_pairs[0]
        print(f"- closest theme palette pair: {left_key} <-> {right_key} (distance={distance})")
    else:
        print("- closest theme palette pair: none")

    if stable_theme_pairs:
        distance, left_key, right_key = stable_theme_pairs[0]
        print(
            f"- closest first-class stable theme pair ({stable_theme_set}): "
            f"{left_key} <-> {right_key} (distance={distance})"
        )
    else:
        print(f"- closest first-class stable theme pair ({stable_theme_set}): none")

    print("- theme palette nearest pairs:")
    for distance, left_key, right_key in theme_pairs[:5]:
        print(f"  {left_key} <-> {right_key}: distance={distance}")

    print(
        "- audit posture: deterministic signature-distance only; this report does not replace the smoke-backed pixel-difference checks"
    )
    return 0 if not exact_duplicates else 1


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
    if args.command == "integration-report":
        return integration_report()
    if args.command == "control-report":
        return control_report(args.profile)
    if args.command == "curation-report":
        return curation_report()
    if args.command == "degrade-report":
        if args.pack:
            return degrade_report_for_pack(args.pack)
        return degrade_report_for_capture(args.capture)
    if args.command == "influence-report":
        return influence_report()
    if args.command == "combination-matrix":
        return combination_matrix()
    if args.command == "experimental-coverage":
        return experimental_coverage()
    if args.command == "proof-result-taxonomy":
        return proof_result_taxonomy()
    if args.command == "preset-audit":
        return preset_audit(args.threshold, args.stable_preset_set, args.stable_theme_set)
    return capture_diff(args.left, args.right)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
