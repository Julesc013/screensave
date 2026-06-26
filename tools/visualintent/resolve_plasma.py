"""Resolve bounded VisualIntent v1 inputs into Plasma v2 pack candidates."""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PACKC = ROOT / "tools" / "packc" / "packc.py"
OUT_ROOT = ROOT / "out"
HEX_COLOR_RE = re.compile(r"^#[0-9a-fA-F]{6}$")

MATERIALS = ["plasma_lava", "aurora_cool", "oceanic_blue", "museum_phosphor", "quiet_darkroom"]
OUTPUT_STYLES = ["continuous", "banded", "contour"]
TREATMENTS = ["none", "restrained_dither", "restrained_crt"]


class IntentError(ValueError):
    pass


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def read_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def write_json(path: pathlib.Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise IntentError(message)


def as_float(data: dict[str, Any], section: str, key: str) -> float:
    value = data.get(section, {}).get(key)
    require(isinstance(value, float), f"{section}.{key} must be a float.")
    require(0.0 <= value <= 1.0, f"{section}.{key} must be normalized to [0.0, 1.0].")
    return float(value)


def validate_intent(data: dict[str, Any]) -> None:
    require(data.get("visual_intent_version") == 1, "visual_intent_version must be 1.")
    required_sections = {"appearance", "motion", "composition", "constraints"}
    observed_sections = {key for key, value in data.items() if isinstance(value, dict)}
    require(observed_sections == required_sections, "VisualIntent must contain exactly appearance, motion, composition, and constraints.")
    require(data.get("constraints", {}).get("deterministic") is True, "constraints.deterministic must be true.")
    require(data.get("constraints", {}).get("avoid_flashing") is True, "constraints.avoid_flashing must be true.")
    for section, keys in {
        "appearance": {"brightness", "contrast", "density", "palette", "texture"},
        "motion": {"character", "speed", "continuity", "loop_seconds"},
        "composition": {"symmetry", "depth", "focus"},
        "constraints": {"avoid_flashing", "deterministic", "maximum_brightness"},
    }.items():
        require(set(data.get(section, {})) <= keys, f"{section} contains unknown keys.")
    for section, key in (
        ("appearance", "brightness"),
        ("appearance", "contrast"),
        ("appearance", "density"),
        ("motion", "speed"),
        ("motion", "continuity"),
        ("constraints", "maximum_brightness"),
    ):
        as_float(data, section, key)
    loop_seconds = data.get("motion", {}).get("loop_seconds")
    require(isinstance(loop_seconds, int) and loop_seconds > 0, "motion.loop_seconds must be a positive integer.")
    palette = data.get("appearance", {}).get("palette")
    require(isinstance(palette, list) and palette, "appearance.palette must be a non-empty list.")
    for value in palette:
        require(isinstance(value, str) and HEX_COLOR_RE.match(value) is not None, f"Invalid palette value: {value!r}.")


def percent(value: float) -> int:
    result = int(round(value * 100.0))
    if result < 0:
        return 0
    if result > 100:
        return 100
    return result


def palette_warmth(palette: list[str]) -> tuple[int, int, int]:
    total_r = 0
    total_g = 0
    total_b = 0
    for item in palette:
        total_r += int(item[1:3], 16)
        total_g += int(item[3:5], 16)
        total_b += int(item[5:7], 16)
    count = max(1, len(palette))
    return total_r // count, total_g // count, total_b // count


def primary_material(data: dict[str, Any]) -> str:
    palette = data.get("appearance", {}).get("palette", [])
    r, g, b = palette_warmth(palette)
    brightness = as_float(data, "appearance", "brightness")
    density = as_float(data, "appearance", "density")
    if brightness <= 0.22:
        return "quiet_darkroom"
    if b > r and b >= g:
        return "oceanic_blue"
    if g > r and b > 80:
        return "aurora_cool"
    if density <= 0.35:
        return "museum_phosphor"
    return "plasma_lava"


def candidate_specs(data: dict[str, Any]) -> list[dict[str, Any]]:
    brightness = min(as_float(data, "appearance", "brightness"), as_float(data, "constraints", "maximum_brightness"))
    contrast = as_float(data, "appearance", "contrast")
    density = as_float(data, "appearance", "density")
    speed = as_float(data, "motion", "speed")
    continuity = as_float(data, "motion", "continuity")
    loop_seconds = int(data.get("motion", {}).get("loop_seconds", 90))
    symmetry = str(data.get("composition", {}).get("symmetry", "low"))
    texture = str(data.get("appearance", {}).get("texture", "soft-grain"))
    material = primary_material(data)
    material_index = MATERIALS.index(material)
    alternate_material = MATERIALS[(material_index + 1) % len(MATERIALS)]
    safety_material = "quiet_darkroom" if brightness < 0.35 else "museum_phosphor"
    field = "radial_warped" if symmetry in {"medium", "high"} else "classic_interference"
    alt_field = "classic_interference" if field == "radial_warped" else "radial_warped"
    base = {
        "schema_id": "screensave.plasma.spec.v2",
        "schema_version": 2,
        "scale": max(15, percent(0.45 + density * 0.45)),
        "complexity": max(10, percent(density)),
        "motion_speed": min(100, max(4, percent(speed))),
        "warp_amount": min(100, max(0, percent((1.0 - continuity) * 0.55 + speed * 0.25))),
        "feedback_amount": min(100, max(0, percent(max(0.0, continuity - 0.55) * 0.28))),
        "brightness": percent(brightness),
        "contrast": percent(contrast),
        "quality_intent": "safe",
        "seed_policy": "fixed" if loop_seconds >= 60 else "session",
        "presentation": "flat",
    }
    return [
        {
            **base,
            "candidate_id": "candidate-01-primary",
            "field_family": field,
            "output_style": "continuous",
            "material": material,
            "treatment": "restrained_dither" if texture in {"soft-grain", "grain"} else "none",
            "resolution_note": "closest material and motion fit",
        },
        {
            **base,
            "candidate_id": "candidate-02-structured",
            "field_family": alt_field,
            "scale": min(100, base["scale"] + 8),
            "complexity": min(100, base["complexity"] + 12),
            "warp_amount": min(100, base["warp_amount"] + 10),
            "output_style": "banded",
            "material": alternate_material,
            "treatment": "none",
            "resolution_note": "structured alternate with bounded warp",
        },
        {
            **base,
            "candidate_id": "candidate-03-comfort",
            "field_family": "classic_interference",
            "motion_speed": max(4, min(base["motion_speed"], 28)),
            "feedback_amount": min(base["feedback_amount"], 10),
            "output_style": "contour",
            "material": safety_material,
            "brightness": min(base["brightness"], 42),
            "treatment": "restrained_crt",
            "resolution_note": "dark-room comfort variant",
        },
    ]


def pack_toml(pack_id: str, name: str, spec: dict[str, Any]) -> str:
    keys = [
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
    ]
    lines = [
        "schema_version = 1",
        f'pack_id = "{pack_id}"',
        'kind = "screensave.plasma.v2"',
        'product = "plasma"',
        f'name = "{name}"',
        'license = "ScreenSave preview candidate"',
        'provenance = "VisualIntent deterministic resolver"',
        'proof_profile = "plasma.v2.reference.preview"',
        "",
        "[plasma_spec_v2]",
    ]
    for key in keys:
        value = spec[key]
        if isinstance(value, str):
            lines.append(f'{key} = "{value}"')
        else:
            lines.append(f"{key} = {value}")
    return "\n".join(lines) + "\n"


def run_packc(source: pathlib.Path, out_dir: pathlib.Path) -> dict[str, Any]:
    result = subprocess.run(
        [sys.executable, str(PACKC), "compile", str(source), "--out", str(out_dir)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if result.returncode != 0:
        raise IntentError(f"packc failed for {source.name}: {result.stderr.strip()}")
    return json.loads(result.stdout)


def resolve(input_path: pathlib.Path, output_dir: pathlib.Path) -> dict[str, Any]:
    intent = read_toml(input_path)
    validate_intent(intent)
    output_dir = output_dir.resolve()
    out_root = OUT_ROOT.resolve()
    if not (output_dir == out_root or output_dir.is_relative_to(out_root)):
        raise IntentError("resolver --out must stay under out/.")
    output_dir.mkdir(parents=True, exist_ok=True)
    candidates_dir = output_dir / "candidates"
    candidates_dir.mkdir(parents=True, exist_ok=True)
    candidates = []
    for index, spec in enumerate(candidate_specs(intent), start=1):
        candidate_id = spec["candidate_id"]
        candidate_dir = candidates_dir / candidate_id
        pack_id = f"screensave.plasma.visualintent.{candidate_id}"
        name = f"Plasma VisualIntent {index}"
        candidate_toml = candidate_dir / "pack.toml"
        candidate_dir.mkdir(parents=True, exist_ok=True)
        candidate_toml.write_text(pack_toml(pack_id, name, spec), encoding="utf-8")
        compile_receipt = run_packc(candidate_toml, candidate_dir / "compiled")
        proof_receipt = {
            "candidate_id": candidate_id,
            "proof_profile": "plasma.v2.reference.preview",
            "proof_status": "profile-reference-emitted",
            "claim_boundary": "Resolver emits pack candidates and proof-profile references only; proof execution and review remain separate gates.",
        }
        write_json(candidate_dir / "proof-receipt.json", proof_receipt)
        public_spec = dict(spec)
        public_spec.pop("candidate_id", None)
        public_spec.pop("resolution_note", None)
        candidates.append(
            {
                "candidate_id": candidate_id,
                "pack_toml": repo_path(candidate_toml),
                "compiled_manifest": compile_receipt["manifest"],
                "hash_manifest": compile_receipt["hash_manifest"],
                "proof_receipt": repo_path(candidate_dir / "proof-receipt.json"),
                "resolution_note": spec["resolution_note"],
                "spec": public_spec,
            }
        )
    summary = {
        "schema_version": "screensave.visualintent.plasma.resolution.v1",
        "source": repo_path(input_path),
        "candidate_count": 3,
        "candidate_policy": "bounded deterministic local resolver",
        "model_calls": False,
        "network_calls": False,
        "source_code_generation": False,
        "automatic_promotion": False,
        "claim_boundary": "VisualIntent candidates are preview inputs only; they are not stable release, artistic acceptance, compatibility certification, or source generation.",
        "candidates": candidates,
    }
    write_json(output_dir / "visualintent-candidates.json", summary)
    return summary


def command_resolve(args: argparse.Namespace) -> int:
    try:
        summary = resolve(pathlib.Path(args.input), pathlib.Path(args.out))
    except (OSError, IntentError, tomllib.TOMLDecodeError, json.JSONDecodeError) as exc:
        print(f"visualintent resolve fail: {exc}", file=sys.stderr)
        return 1
    print(json.dumps({"status": "pass", "candidates": summary["candidate_count"], "output": repo_path(pathlib.Path(args.out).resolve() / "visualintent-candidates.json")}, indent=2, sort_keys=True))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    resolve_cmd = subparsers.add_parser("resolve")
    resolve_cmd.add_argument("input")
    resolve_cmd.add_argument("--out", required=True)
    resolve_cmd.set_defaults(func=command_resolve)
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
