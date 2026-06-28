"""Resolve bounded VisualIntent v1 inputs into Plasma v2 spec candidates."""

from __future__ import annotations

import argparse
import hashlib
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

VISUALINTENT_SCHEMA = "screensave.visual_intent.v1"
RESOLUTION_SCHEMA = "screensave.visualintent.plasma.resolution.v1"
PLASMA_SPEC_SCHEMA = "screensave.plasma.spec.v2"
DEFAULT_CANDIDATE_COUNT = 3
MAX_CANDIDATE_COUNT = 5

HEX_COLOR_RE = re.compile(r"^#[0-9a-fA-F]{6}$")
SAFE_TOKEN_RE = re.compile(r"^[A-Za-z0-9_.-]+$")
PATHLIKE_RE = re.compile(r"(^[A-Za-z]:[\\/])|(^[\\/])|(\.\.)|([\\/])")
FORBIDDEN_TEXT = [
    ".dll",
    ".exe",
    ".bat",
    ".cmd",
    ".ps1",
    "<script",
    "http://",
    "https://",
    "ftp://",
    "file://",
]

ALLOWED_TOP_LEVEL = {
    "visual_intent_version",
    "target_product",
    "candidate_count",
    "mood",
    "seed",
    "appearance",
    "motion",
    "constraints",
}
ALLOWED_APPEARANCE = {
    "brightness",
    "contrast",
    "density",
    "palette",
    "palette_hint",
    "texture",
    "warmth",
    "softness",
    "complexity",
}
ALLOWED_MOTION = {
    "motion_character",
    "motion_speed",
    "character",
    "speed",
    "continuity",
    "loop_seconds",
}
ALLOWED_CONSTRAINTS = {
    "avoid_flashing",
    "deterministic",
    "maximum_brightness",
}

MATERIALS = [
    "plasma_lava",
    "aurora_cool",
    "oceanic_blue",
    "museum_phosphor",
    "quiet_darkroom",
]
FIELD_FAMILIES = ["classic_interference", "radial_warped"]
OUTPUT_STYLES = ["continuous", "banded", "contour"]
TREATMENTS = ["none", "restrained_dither", "restrained_crt"]


class IntentError(ValueError):
    pass


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def read_input(path: pathlib.Path) -> dict[str, Any]:
    if path.suffix.lower() == ".json":
        return json.loads(path.read_text(encoding="utf-8"))
    with path.open("rb") as handle:
        return tomllib.load(handle)


def write_json(path: pathlib.Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def write_text(path: pathlib.Path, value: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(value, encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise IntentError(message)


def check_safe_string(label: str, value: Any) -> str:
    require(isinstance(value, str), f"{label} must be a string.")
    require(value != "", f"{label} must not be empty.")
    require(len(value) <= 96, f"{label} exceeds 96 characters.")
    lowered = value.lower()
    require(PATHLIKE_RE.search(value) is None, f"{label} must not contain paths or traversal.")
    for token in FORBIDDEN_TEXT:
        require(token not in lowered, f"{label} must not contain executable, network, or file URI text.")
    return value


def check_unknown_keys(label: str, observed: set[str], allowed: set[str]) -> None:
    extra = observed - allowed
    require(not extra, f"{label} contains unknown keys: {', '.join(sorted(extra))}.")


def as_float(value: Any, label: str) -> float:
    require(isinstance(value, (float, int)) and not isinstance(value, bool), f"{label} must be numeric.")
    numeric = float(value)
    require(0.0 <= numeric <= 1.0, f"{label} must be normalized to [0.0, 1.0].")
    return numeric


def optional_float(value: Any, default: float, label: str) -> float:
    if value is None:
        return default
    return as_float(value, label)


def percent(value: float) -> int:
    result = int(round(value * 100.0))
    if result < 0:
        return 0
    if result > 100:
        return 100
    return result


def slugify(value: str, max_length: int = 28) -> str:
    lowered = value.lower()
    chars = []
    for char in lowered:
        if char.isalnum():
            chars.append(char)
        elif char in {" ", "_", "-"}:
            chars.append("-")
    slug = "-".join(part for part in "".join(chars).split("-") if part)
    if len(slug) > max_length:
        slug = slug[:max_length].rstrip("-")
    return slug or "intent"


def average_palette(palette: list[str]) -> tuple[int, int, int]:
    red = 0
    green = 0
    blue = 0
    for color in palette:
        red += int(color[1:3], 16)
        green += int(color[3:5], 16)
        blue += int(color[5:7], 16)
    count = max(1, len(palette))
    return red // count, green // count, blue // count


def normalize_intent(data: dict[str, Any]) -> dict[str, Any]:
    require(data.get("visual_intent_version") == 1, "visual_intent_version must be 1.")
    check_unknown_keys("VisualIntent", set(data), ALLOWED_TOP_LEVEL)

    appearance = data.get("appearance")
    motion = data.get("motion")
    constraints = data.get("constraints")
    require(isinstance(appearance, dict), "appearance must be a table.")
    require(isinstance(motion, dict), "motion must be a table.")
    require(isinstance(constraints, dict), "constraints must be a table.")
    check_unknown_keys("appearance", set(appearance), ALLOWED_APPEARANCE)
    check_unknown_keys("motion", set(motion), ALLOWED_MOTION)
    check_unknown_keys("constraints", set(constraints), ALLOWED_CONSTRAINTS)

    target_product = check_safe_string("target_product", data.get("target_product", "plasma"))
    require(target_product == "plasma", "target_product must be plasma.")

    candidate_count = data.get("candidate_count", DEFAULT_CANDIDATE_COUNT)
    require(isinstance(candidate_count, int) and not isinstance(candidate_count, bool), "candidate_count must be an integer.")
    require(1 <= candidate_count <= MAX_CANDIDATE_COUNT, f"candidate_count must be between 1 and {MAX_CANDIDATE_COUNT}.")

    mood = check_safe_string("mood", data.get("mood", "warm-room"))
    seed = data.get("seed", 4096)
    require(isinstance(seed, int) and not isinstance(seed, bool) and seed >= 0, "seed must be a non-negative integer.")

    brightness = as_float(appearance.get("brightness"), "appearance.brightness")
    contrast = as_float(appearance.get("contrast"), "appearance.contrast")
    density = as_float(appearance.get("density"), "appearance.density")
    palette = appearance.get("palette")
    require(isinstance(palette, list) and palette, "appearance.palette must be a non-empty list.")
    for value in palette:
        require(isinstance(value, str) and HEX_COLOR_RE.match(value) is not None, f"Invalid palette value: {value!r}.")

    palette_hint = check_safe_string("appearance.palette_hint", appearance.get("palette_hint", "neutral"))
    texture = check_safe_string("appearance.texture", appearance.get("texture", "soft-grain"))
    warmth = optional_float(appearance.get("warmth"), 0.5, "appearance.warmth")
    softness = optional_float(appearance.get("softness"), 0.5, "appearance.softness")
    complexity = optional_float(appearance.get("complexity"), density, "appearance.complexity")

    motion_character = check_safe_string(
        "motion.motion_character",
        motion.get("motion_character", motion.get("character", "slow-drifting")),
    )
    motion_speed = optional_float(motion.get("motion_speed", motion.get("speed")), 0.2, "motion.motion_speed")
    continuity = optional_float(motion.get("continuity"), 0.85, "motion.continuity")
    loop_seconds = motion.get("loop_seconds", 90)
    require(isinstance(loop_seconds, int) and loop_seconds > 0, "motion.loop_seconds must be a positive integer.")

    avoid_flashing = constraints.get("avoid_flashing")
    deterministic = constraints.get("deterministic")
    require(avoid_flashing is True, "constraints.avoid_flashing must be true.")
    require(deterministic is True, "constraints.deterministic must be true.")
    maximum_brightness = optional_float(constraints.get("maximum_brightness"), 1.0, "constraints.maximum_brightness")

    return {
        "visual_intent_version": 1,
        "input_schema": VISUALINTENT_SCHEMA,
        "target_product": target_product,
        "candidate_count": candidate_count,
        "mood": mood,
        "seed": seed,
        "brightness": min(brightness, maximum_brightness),
        "contrast": contrast,
        "density": density,
        "palette": palette,
        "palette_hint": palette_hint,
        "texture": texture,
        "warmth": warmth,
        "softness": softness,
        "complexity": complexity,
        "motion_character": motion_character,
        "motion_speed": motion_speed,
        "continuity": continuity,
        "loop_seconds": loop_seconds,
        "avoid_flashing": avoid_flashing,
        "deterministic": deterministic,
        "maximum_brightness": maximum_brightness,
    }


def stable_digest(intent: dict[str, Any]) -> str:
    payload = json.dumps(intent, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(payload).hexdigest()[:10]


def choose_material(intent: dict[str, Any]) -> str:
    palette_hint = str(intent["palette_hint"]).lower()
    mood = str(intent["mood"]).lower()
    red, green, blue = average_palette(intent["palette"])
    if intent["brightness"] <= 0.24 or "dark" in mood:
        return "quiet_darkroom"
    if "phosphor" in palette_hint or "phosphor" in mood:
        return "museum_phosphor"
    if "aurora" in palette_hint or "aurora" in mood or (green > red and blue > 80):
        return "aurora_cool"
    if "ocean" in palette_hint or "cool" in palette_hint or (blue > red and blue >= green):
        return "oceanic_blue"
    return "plasma_lava"


def base_spec(intent: dict[str, Any]) -> dict[str, Any]:
    material = choose_material(intent)
    field_family = "radial_warped" if intent["density"] >= 0.64 or "aurora" in str(intent["mood"]).lower() else "classic_interference"
    treatment = "restrained_dither" if intent["softness"] >= 0.55 else "none"
    if material == "museum_phosphor":
        treatment = "restrained_crt"
    return {
        "schema_id": PLASMA_SPEC_SCHEMA,
        "schema_version": 2,
        "field_family": field_family,
        "scale": max(15, percent(0.35 + intent["density"] * 0.45)),
        "complexity": max(10, percent((intent["density"] + intent["complexity"]) * 0.5)),
        "motion_speed": max(4, percent(intent["motion_speed"])),
        "warp_amount": percent((1.0 - intent["continuity"]) * 0.45 + intent["motion_speed"] * 0.25),
        "feedback_amount": percent(max(0.0, intent["continuity"] - 0.55) * 0.25),
        "output_style": "continuous",
        "material": material,
        "brightness": percent(intent["brightness"]),
        "contrast": percent(intent["contrast"]),
        "treatment": treatment,
        "quality_intent": "safe",
        "seed_policy": "fixed",
        "presentation": "flat",
    }


def candidate_specs(intent: dict[str, Any]) -> list[dict[str, Any]]:
    base = base_spec(intent)
    material_index = MATERIALS.index(base["material"])
    alternate_material = MATERIALS[(material_index + 1) % len(MATERIALS)]
    safety_material = "quiet_darkroom" if intent["brightness"] < 0.36 else "museum_phosphor"
    variants = [
        (
            "primary",
            {
                **base,
                "rationale": "closest bounded Plasma v2 match for mood, palette, density, and motion",
                "constraints_applied": ["avoid_flashing", "deterministic", "safe_quality", "flat_presentation"],
            },
        ),
        (
            "structured",
            {
                **base,
                "field_family": "radial_warped" if base["field_family"] == "classic_interference" else "classic_interference",
                "scale": min(100, base["scale"] + 8),
                "complexity": min(100, base["complexity"] + 10),
                "warp_amount": min(100, base["warp_amount"] + 8),
                "output_style": "banded",
                "material": alternate_material,
                "treatment": "none",
                "rationale": "structured alternate that keeps candidate order deterministic and bounded",
                "constraints_applied": ["candidate_count_bounded", "packc_data_only", "safe_quality", "flat_presentation"],
            },
        ),
        (
            "comfort",
            {
                **base,
                "field_family": "classic_interference",
                "motion_speed": min(base["motion_speed"], 28),
                "feedback_amount": min(base["feedback_amount"], 10),
                "output_style": "contour",
                "material": safety_material,
                "brightness": min(base["brightness"], 42),
                "treatment": "restrained_crt" if safety_material == "museum_phosphor" else "none",
                "rationale": "comfort/safety variant with restrained brightness and motion",
                "constraints_applied": ["avoid_flashing", "maximum_brightness", "safe_quality", "flat_presentation"],
            },
        ),
        (
            "soft",
            {
                **base,
                "scale": min(100, base["scale"] + 5),
                "complexity": max(10, base["complexity"] - 8),
                "warp_amount": max(0, base["warp_amount"] - 6),
                "treatment": "restrained_dither",
                "rationale": "softened variant that preserves descriptive intent without adding runtime features",
                "constraints_applied": ["softness", "safe_quality", "flat_presentation"],
            },
        ),
        (
            "contrast",
            {
                **base,
                "contrast": min(100, base["contrast"] + 10),
                "brightness": min(base["brightness"], 55),
                "material": "museum_phosphor" if base["material"] != "museum_phosphor" else "plasma_lava",
                "rationale": "bounded contrast variant for review comparison only",
                "constraints_applied": ["contrast", "avoid_flashing", "safe_quality", "flat_presentation"],
            },
        ),
    ]
    digest = stable_digest(intent)
    mood_slug = slugify(intent["mood"])
    candidates = []
    for index, (variant, spec) in enumerate(variants[: int(intent["candidate_count"])], start=1):
        candidate_id = f"plasma-{mood_slug}-{index:02d}-{variant}-{digest}"
        public_spec = {key: spec[key] for key in [
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
        ]}
        candidates.append(
            {
                "candidate_id": candidate_id,
                "variant": variant,
                "product": "plasma",
                "profile": "plasma.v2.visualintent.preview",
                "plasma_v2_spec": public_spec,
                "rationale": spec["rationale"],
                "constraints_applied": spec["constraints_applied"],
                "warnings": [],
                "refusals": [],
                "claim_boundary": "VisualIntent candidate is descriptive review input reduced to Plasma v2 data/spec only; not runtime truth, artistic acceptance, compatibility certification, or stable promotion.",
            }
        )
    return candidates


def pack_toml(pack_id: str, name: str, spec: dict[str, Any]) -> str:
    lines = [
        "schema_version = 1",
        f'pack_id = "{pack_id}"',
        'kind = "screensave.plasma.v2"',
        'product = "plasma"',
        f'name = "{name}"',
        'author = "ScreenSave"',
        'license = "ScreenSave sample content"',
        'provenance = "VisualIntent deterministic resolver"',
        'proof_profile = "plasma.v2.reference.preview"',
        "",
        "[compatibility]",
        "minimum_schema_version = 2",
        "maximum_schema_version = 2",
        'pack_status = "v1-candidate"',
        "",
        "[plasma_spec_v2]",
    ]
    for key in [
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
    ]:
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


def ensure_out_dir(path: pathlib.Path) -> pathlib.Path:
    resolved = path.resolve()
    out_root = OUT_ROOT.resolve()
    require(resolved == out_root or resolved.is_relative_to(out_root), "resolver --out must stay under out/.")
    return resolved


def resolve(input_path: pathlib.Path, output_dir: pathlib.Path) -> dict[str, Any]:
    input_path = input_path.resolve()
    require(input_path.exists(), f"VisualIntent input does not exist: {input_path}")
    intent = normalize_intent(read_input(input_path))
    output_dir = ensure_out_dir(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    candidates_dir = output_dir / "candidates"
    candidates_dir.mkdir(parents=True, exist_ok=True)

    candidate_records = []
    for candidate in candidate_specs(intent):
        candidate_id = candidate["candidate_id"]
        require(SAFE_TOKEN_RE.match(candidate_id) is not None, f"candidate_id is unsafe: {candidate_id}")
        candidate_dir = candidates_dir / candidate_id
        spec = candidate["plasma_v2_spec"]
        pack_id = f"screensave.plasma.visualintent.{candidate_id}"
        candidate_toml = candidate_dir / "pack.toml"
        write_text(candidate_toml, pack_toml(pack_id, f"VisualIntent {candidate_id}", spec))
        compile_receipt = run_packc(candidate_toml, candidate_dir / "compiled")
        proof_receipt = {
            "candidate_id": candidate_id,
            "proof_profile": "plasma.v2.visualintent.preview",
            "proof_status": "packc-and-spec-proof-only",
            "claim_boundary": "Resolver records proof-profile mapping only; capture generation and artistic review remain separate ScreenSave gates.",
        }
        write_json(candidate_dir / "proof-receipt.json", proof_receipt)
        spec_path = candidate_dir / "plasma-v2-spec.json"
        write_json(spec_path, spec)
        candidate_records.append(
            {
                **candidate,
                "pack_toml": repo_path(candidate_toml),
                "spec_json": repo_path(spec_path),
                "compiled_manifest": compile_receipt["manifest"],
                "hash_manifest": compile_receipt["hash_manifest"],
                "proof_receipt": repo_path(candidate_dir / "proof-receipt.json"),
            }
        )

    summary = {
        "schema_version": RESOLUTION_SCHEMA,
        "input_schema": VISUALINTENT_SCHEMA,
        "source": repo_path(input_path),
        "target_product": "plasma",
        "candidate_count": len(candidate_records),
        "candidate_count_requested": intent["candidate_count"],
        "candidate_count_default": DEFAULT_CANDIDATE_COUNT,
        "candidate_count_max": MAX_CANDIDATE_COUNT,
        "candidate_policy": "bounded deterministic local resolver",
        "model_calls": False,
        "network_calls": False,
        "source_code_generation": False,
        "runtime_mutation": False,
        "automatic_promotion": False,
        "compatibility_certification": False,
        "stable_release": False,
        "claim_boundary": "VisualIntent candidates are bounded data/spec candidates only; not artistic acceptance, compatibility certification, source generation, runtime truth, or stable promotion.",
        "normalized_intent": intent,
        "candidates": candidate_records,
    }
    write_json(output_dir / "visualintent-candidates.json", summary)
    return summary


def command_resolve(args: argparse.Namespace) -> int:
    try:
        summary = resolve(pathlib.Path(args.input), pathlib.Path(args.out))
    except (OSError, IntentError, tomllib.TOMLDecodeError, json.JSONDecodeError) as exc:
        print(f"visualintent resolve fail: {exc}", file=sys.stderr)
        return 1
    print(
        json.dumps(
            {
                "status": "pass",
                "candidates": summary["candidate_count"],
                "output": repo_path(pathlib.Path(args.out).resolve() / "visualintent-candidates.json"),
            },
            indent=2,
            sort_keys=True,
        )
    )
    return 0


def main(argv: list[str] | None = None) -> int:
    if argv is None:
        argv = sys.argv[1:]
    if argv and argv[0] == "resolve":
        parser = argparse.ArgumentParser(description=__doc__)
        parser.add_argument("command")
        parser.add_argument("input")
        parser.add_argument("--out", required=True)
        args = parser.parse_args(argv)
        return command_resolve(args)

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input")
    parser.add_argument("--out", required=True)
    args = parser.parse_args(argv)
    return command_resolve(args)


if __name__ == "__main__":
    raise SystemExit(main())
