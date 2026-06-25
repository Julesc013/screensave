"""Validate the draft VisualIntent v1 contract and example."""

from __future__ import annotations

import pathlib
import re
import sys
import tomllib


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "visual_intent_v1.md"
EXAMPLE = ROOT / "contracts" / "examples" / "visual_intent.example.toml"
HEX_COLOR_RE = re.compile(r"^#[0-9a-fA-F]{6}$")

REQUIRED_SECTIONS = {"appearance", "motion", "composition", "constraints"}
ALLOWED_KEYS = {
    "appearance": {"brightness", "contrast", "density", "palette", "texture"},
    "motion": {"character", "speed", "continuity", "loop_seconds"},
    "composition": {"symmetry", "depth", "focus"},
    "constraints": {"avoid_flashing", "deterministic", "maximum_brightness"},
}
NORMALIZED_FIELDS = {
    ("appearance", "brightness"),
    ("appearance", "contrast"),
    ("appearance", "density"),
    ("motion", "speed"),
    ("motion", "continuity"),
    ("constraints", "maximum_brightness"),
}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def validate_example(data: dict, errors: list[str]) -> None:
    require(data.get("visual_intent_version") == 1, "VisualIntent example must use version 1.", errors)

    sections = {key for key, value in data.items() if isinstance(value, dict)}
    require(sections == REQUIRED_SECTIONS, "VisualIntent example must contain exactly the admitted sections.", errors)

    for section in REQUIRED_SECTIONS:
        section_data = data.get(section, {})
        require(isinstance(section_data, dict), f"{section} must be a table.", errors)
        unknown = set(section_data) - ALLOWED_KEYS[section]
        require(not unknown, f"{section} contains unknown keys: {sorted(unknown)}", errors)

    for section, key in NORMALIZED_FIELDS:
        value = data.get(section, {}).get(key)
        require(isinstance(value, float), f"{section}.{key} must be a float.", errors)
        if isinstance(value, float):
            require(0.0 <= value <= 1.0, f"{section}.{key} must be normalized to [0.0, 1.0].", errors)

    palette = data.get("appearance", {}).get("palette")
    require(isinstance(palette, list) and palette, "appearance.palette must be a non-empty list.", errors)
    if isinstance(palette, list):
        for value in palette:
            require(isinstance(value, str) and HEX_COLOR_RE.match(value) is not None, f"Invalid palette value: {value!r}", errors)

    loop_seconds = data.get("motion", {}).get("loop_seconds")
    require(isinstance(loop_seconds, int) and loop_seconds > 0, "motion.loop_seconds must be a positive integer.", errors)
    require(data.get("constraints", {}).get("deterministic") is True, "constraints.deterministic must be true in the v1 example.", errors)
    require(data.get("constraints", {}).get("avoid_flashing") is True, "constraints.avoid_flashing must be true in the v1 example.", errors)


def main() -> int:
    errors: list[str] = []

    require(CONTRACT.exists(), "Missing VisualIntent contract.", errors)
    require(EXAMPLE.exists(), "Missing VisualIntent example.", errors)

    if CONTRACT.exists():
        text = CONTRACT.read_text(encoding="utf-8")
        for needle in (
            "Status: draft authoring contract, not executable runtime behavior.",
            "not a universal graphics language",
            "product-specific specification",
            "must not bypass product schemas, proof",
            "does not generate C source",
        ):
            require(needle in text, f"VisualIntent contract is missing expected boundary text: {needle!r}", errors)

    if EXAMPLE.exists():
        with EXAMPLE.open("rb") as handle:
            validate_example(tomllib.load(handle), errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("VisualIntent contract checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
