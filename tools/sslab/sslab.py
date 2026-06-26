"""ScreenSave Lab Runner v0 for deterministic proof-kernel captures."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import pathlib
import shutil
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass
from typing import Any


THIS_DIR = pathlib.Path(__file__).resolve().parent
if str(THIS_DIR) not in sys.path:
    sys.path.insert(0, str(THIS_DIR))

from build_support import build_runner, runner_implementation_paths


ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFAULT_OUTPUT = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne"
DEFAULT_COMPARISON = ROOT / "out" / "proof" / "sslab-compare" / "comparison.json"
PROOF_REGISTRY = ROOT / "catalog" / "generated" / "proof_registry.json"

LEGACY_PROFILE_ALIASES = {
    "nocturne": "nocturne.reference.v0",
    "ricochet": "ricochet.reference.v1",
}


def display_path(path: pathlib.Path) -> str:
    """Use repo-relative proof paths when possible and absolute temp paths otherwise."""
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


def sha256_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_proof_registry() -> dict[str, Any]:
    return json.loads(PROOF_REGISTRY.read_text(encoding="utf-8"))


def find_proof_profile(profile_key: str) -> dict[str, Any]:
    registry = load_proof_registry()
    for profile in registry.get("proof_profiles", []):
        if profile.get("key") == profile_key:
            return profile
    raise ValueError(f"unknown proof profile: {profile_key}")


@dataclass
class PpmImage:
    width: int
    height: int
    pixels: bytes

    def rgb_sha256(self) -> str:
        return hashlib.sha256(self.pixels).hexdigest()

    def rgba_sha256(self) -> str:
        expanded = bytearray()
        for index in range(0, len(self.pixels), 3):
            expanded.extend(self.pixels[index : index + 3])
            expanded.append(255)
        return hashlib.sha256(expanded).hexdigest()


def read_ppm(path: pathlib.Path) -> PpmImage:
    tokens = path.read_text(encoding="ascii").split()
    if len(tokens) < 4 or tokens[0] != "P3":
        raise ValueError(f"{path} is not an ASCII P3 PPM file")
    width = int(tokens[1])
    height = int(tokens[2])
    max_value = int(tokens[3])
    if width <= 0 or height <= 0:
        raise ValueError(f"{path} has invalid dimensions")
    if max_value != 255:
        raise ValueError(f"{path} must use max value 255")
    values = [int(token) for token in tokens[4:]]
    expected_values = width * height * 3
    if len(values) != expected_values:
        raise ValueError(f"{path} has {len(values)} values; expected {expected_values}")
    for value in values:
        if value < 0 or value > 255:
            raise ValueError(f"{path} contains out-of-range color value {value}")
    return PpmImage(width=width, height=height, pixels=bytes(values))


def resolve_output_dir(path_text: str) -> pathlib.Path:
    output_dir = pathlib.Path(path_text)
    if not output_dir.is_absolute():
        output_dir = ROOT / output_dir
    output_dir.mkdir(parents=True, exist_ok=True)
    return output_dir


def write_json(path: pathlib.Path, payload: dict[str, Any]) -> dict[str, Any]:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return payload


def max_capture_frame(profile: dict[str, Any]) -> int:
    capture_frames = [int(frame) for frame in profile.get("capture_frames", [])]
    if not capture_frames:
        raise ValueError(f"proof profile does not define capture frames: {profile.get('key', '')}")
    return max(capture_frames)


def resolve_legacy_profile(args: argparse.Namespace, *, allow_ricochet: bool = True) -> dict[str, Any]:
    product = str(getattr(args, "product", "nocturne"))
    if product == "ricochet" and not allow_ricochet:
        raise ValueError("legacy render compatibility only supports the Nocturne reference profile")
    profile_key = LEGACY_PROFILE_ALIASES.get(product)
    if not profile_key:
        raise ValueError(f"legacy compatibility alias is unavailable for product: {product}")
    profile = find_proof_profile(profile_key)
    expected_pairs: list[tuple[str, object]] = [
        ("preset", profile.get("preset")),
        ("width", int(profile.get("width", 0))),
        ("height", int(profile.get("height", 0))),
        ("seed", int(profile.get("seed", 0))),
        ("delta_ms", int(profile.get("delta_ms", 0))),
    ]
    if hasattr(args, "frames"):
        expected_pairs.append(("frames", max_capture_frame(profile)))
    if hasattr(args, "resize_width"):
        expected_pairs.append(("resize_width", int(profile.get("resize_width", profile.get("width", 0)))))
    if hasattr(args, "resize_height"):
        expected_pairs.append(("resize_height", int(profile.get("resize_height", profile.get("height", 0)))))
    if hasattr(args, "create_destroy_cycles"):
        expected_pairs.append(("create_destroy_cycles", int(profile.get("create_destroy_cycles", 1))))
    for field_name, expected in expected_pairs:
        actual = getattr(args, field_name, expected)
        if actual is None:
            continue
        if actual != expected:
            raise ValueError(
                f"legacy compatibility alias requires {field_name}={expected} for {profile.get('key', product)}; "
                f"use proof --profile {profile.get('key', product)} for the named proof profile path"
            )
    return profile


def run_runner_proof(
    runner_exe: pathlib.Path,
    profile_key: str,
    output_root: pathlib.Path,
    output_json: pathlib.Path,
    execution_path: str = "v1",
) -> dict[str, Any]:
    output_root.mkdir(parents=True, exist_ok=True)
    output_json.parent.mkdir(parents=True, exist_ok=True)
    runner = subprocess.run(
        [
            str(runner_exe),
            "proof",
            "--profile",
            profile_key,
            "--path",
            execution_path,
            "--output-root",
            str(output_root),
            "--output",
            str(output_json),
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if runner.returncode != 0:
        raise RuntimeError(f"generic sslab runner failed for {profile_key}: {runner.stderr.strip()}")
    return json.loads(output_json.read_text(encoding="utf-8"))


def runner_capture_records(result: dict[str, Any]) -> list[dict[str, Any]]:
    captures: list[dict[str, Any]] = []
    for item in result.get("captures", []):
        captures.append(
            {
                "frame": int(item.get("frame", 0)),
                "status": str(item.get("status", "")),
                "raw_rgba_path": str(item.get("raw_rgba_path", "")),
                "review_ppm_path": str(item.get("review_ppm_path", "")),
                "raw_rgba_sha256": str(item.get("rgba_sha256", "")),
            }
        )
    return captures


def runner_capture_for_frame(result: dict[str, Any], frame_index: int) -> dict[str, Any]:
    captures = runner_capture_records(result)
    for capture in captures:
        if int(capture.get("frame", -1)) == int(frame_index):
            return capture
    if captures:
        return captures[-1]
    raise ValueError("generic sslab runner did not emit any captures")


def copy_path(source_text: str, target_path: pathlib.Path) -> pathlib.Path:
    source = pathlib.Path(source_text)
    if not source.is_absolute():
        source = ROOT / source
    target_path.parent.mkdir(parents=True, exist_ok=True)
    if source.resolve() != target_path.resolve():
        shutil.copyfile(source, target_path)
    return target_path


def render_proof_payload(profile: dict[str, Any], capture_path: pathlib.Path, capture_hash: str) -> dict[str, Any]:
    return {
        "proof_schema": "proof-bundle-v0",
        "proof_kernel": "proof-kernel-v0",
        "status": "informational",
        "claim_boundary": "deterministic canary capture; not compatibility certification",
        "source": {
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "runtime": {
            "runner": "tools/sslab/runner/sslab_runner.c",
            "runner_mode": "generic-libsslab-proof",
            "product": profile.get("product"),
            "preset": profile.get("preset"),
            "theme": "gray_black",
            "width": int(profile.get("width", 0)),
            "height": int(profile.get("height", 0)),
            "seed": int(profile.get("seed", 0)),
            "frames": max_capture_frame(profile),
            "delta_ms": int(profile.get("delta_ms", 0)),
            "surface": "rgba8-top-left-srgb",
            "renderer": "soft-reference-v0",
        },
        "implementation": [
            {
                "path": display_path(path),
                "sha256": sha256_file(path),
            }
            for path in runner_implementation_paths()
        ],
        "capture": {
            "path": display_path(capture_path),
            "sha256": capture_hash,
            "format": "ppm-p3-rgb-from-rgba8",
            "canonical_hash_source": "rgba8-bytes",
        },
        "limits": [
            "Nocturne canary only",
            "Generic sslab_runner drives the shared libsslab proof path",
            "No operating-system compatibility claim is certified by this proof",
        ],
    }


def write_nocturne_render_artifacts(
    profile: dict[str, Any],
    output_dir: pathlib.Path,
    runner_result: dict[str, Any],
) -> dict[str, Any]:
    capture = runner_capture_for_frame(runner_result, max_capture_frame(profile))
    capture_path = copy_path(str(capture.get("review_ppm_path", "")), output_dir / "capture.ppm")
    capture_hash = read_ppm(capture_path).rgba_sha256()
    (output_dir / "capture.sha256").write_text(capture_hash + "\n", encoding="ascii")
    proof = render_proof_payload(profile, capture_path, capture_hash)
    write_json(output_dir / "proof.json", proof)
    return proof


def lifecycle_payload(
    profile: dict[str, Any],
    output_dir: pathlib.Path,
    runner_result: dict[str, Any],
) -> dict[str, Any]:
    lifecycle = dict(runner_result.get("lifecycle", {}))
    capture = runner_capture_for_frame(runner_result, max_capture_frame(profile))
    if profile.get("product") == "nocturne":
        capture_path = copy_path(str(capture.get("review_ppm_path", "")), output_dir / "lifecycle-capture.ppm")
        schema = "screensave-nocturne-canary-lifecycle-v0"
    else:
        capture_path = copy_path(
            str(capture.get("review_ppm_path", "")),
            output_dir / f"frame-{max_capture_frame(profile):04d}.ppm",
        )
        schema = "screensave-product-lifecycle-v0"
    return write_json(
        output_dir / "lifecycle.json",
        {
            "lifecycle_schema": schema,
            "status": lifecycle.get("status"),
            "create_session": bool(lifecycle.get("create_session", 0)),
            "resize_session": bool(lifecycle.get("resize_session", 0)),
            "step_count": int(lifecycle.get("step_count", 0)),
            "render_session": bool(lifecycle.get("render_session", 0)),
            "destroy_session": bool(lifecycle.get("destroy_session", 0)),
            "create_destroy_cycles": int(lifecycle.get("create_destroy_cycles", 0)),
            "checksum": int(lifecycle.get("checksum", 0)),
            "path": display_path(output_dir / "lifecycle.json"),
            "capture_path": display_path(capture_path),
            "runner_mode": "generic-libsslab-proof",
            "runner_stdout": "",
        },
    )


def render_nocturne(args: argparse.Namespace) -> dict[str, Any]:
    profile = resolve_legacy_profile(args, allow_ricochet=False)
    output_dir = resolve_output_dir(str(args.output_dir))
    with tempfile.TemporaryDirectory() as temp_root:
        runner_exe = build_runner(pathlib.Path(temp_root))
        runner_result = run_runner_proof(
            runner_exe,
            str(profile.get("key", "")),
            output_dir / "runner",
            output_dir / "runner-proof.json",
        )
    return write_nocturne_render_artifacts(profile, output_dir, runner_result)


def lifecycle_product(args: argparse.Namespace) -> dict[str, Any]:
    profile = resolve_legacy_profile(args)
    output_dir = resolve_output_dir(str(args.output_dir))
    with tempfile.TemporaryDirectory() as temp_root:
        runner_exe = build_runner(pathlib.Path(temp_root))
        runner_result = run_runner_proof(
            runner_exe,
            str(profile.get("key", "")),
            output_dir / "runner",
            output_dir / "runner-proof.json",
        )
    return lifecycle_payload(profile, output_dir, runner_result)


def percentile(values: list[float], percent: float) -> float:
    if not values:
        return 0.0
    ordered = sorted(values)
    index = int(math.ceil((percent / 100.0) * len(ordered))) - 1
    if index < 0:
        index = 0
    if index >= len(ordered):
        index = len(ordered) - 1
    return ordered[index]


def profile_product(args: argparse.Namespace) -> dict[str, Any]:
    profile = resolve_legacy_profile(args)
    output_dir = resolve_output_dir(str(args.output_dir))

    measured_samples: list[float] = []
    soak_hashes: list[str] = []
    if int(args.iterations) <= 0:
        raise ValueError("profile iterations must be positive")
    if int(args.short_soak_cycles) <= 0:
        raise ValueError("short soak cycles must be positive")
    run_count = int(args.iterations) + int(args.short_soak_cycles)
    frames = max_capture_frame(profile)

    with tempfile.TemporaryDirectory() as temp_root:
        runner_exe = build_runner(pathlib.Path(temp_root))
        for run_index in range(run_count):
            run_dir = output_dir / f"run-{run_index:02d}"
            start = time.perf_counter()
            runner_result = run_runner_proof(
                runner_exe,
                str(profile.get("key", "")),
                run_dir,
                run_dir / "runner-proof.json",
            )
            elapsed_ms = (time.perf_counter() - start) * 1000.0
            if run_index < int(args.iterations):
                measured_samples.append(elapsed_ms / float(frames))
            capture = runner_capture_for_frame(runner_result, frames)
            soak_hashes.append(str(capture.get("raw_rgba_sha256", "")))

    short_soak_hashes = soak_hashes[-int(args.short_soak_cycles) :] if int(args.short_soak_cycles) > 0 else []
    short_soak_status = "pass"
    if short_soak_hashes and len(set(short_soak_hashes)) != 1:
        short_soak_status = "fail"

    receipt = {
        "profile_schema": "sslab-profile-v0",
        "status": "informational" if short_soak_status == "pass" else "fail",
        "product": profile.get("product"),
        "preset": profile.get("preset"),
        "runner_mode": "generic-libsslab-profile",
        "width": int(profile.get("width", 0)),
        "height": int(profile.get("height", 0)),
        "seed": int(profile.get("seed", 0)),
        "delta_ms": int(profile.get("delta_ms", 0)),
        "measured_frames": frames,
        "measurement_iterations": int(args.iterations),
        "short_soak_cycles": int(args.short_soak_cycles),
        "short_soak_status": short_soak_status,
        "short_soak_hashes": short_soak_hashes,
        "frame_time_ms_p50": percentile(measured_samples, 50.0),
        "frame_time_ms_p95": percentile(measured_samples, 95.0),
        "frame_time_ms_p99": percentile(measured_samples, 99.0),
        "claim_boundary": "informational process-level profile and bounded short-soak receipt; not a performance qualification gate",
    }
    write_json(output_dir / "profile.json", receipt)
    return receipt


def proof_ricochet_from_profile(
    runner_exe: pathlib.Path,
    profile: dict[str, Any],
    output_dir: pathlib.Path,
    execution_path: str,
) -> dict[str, Any]:
    first = run_runner_proof(
        runner_exe,
        str(profile.get("key", "")),
        output_dir / "run-a",
        output_dir / "run-a" / "runner-proof.json",
        execution_path,
    )
    second = run_runner_proof(
        runner_exe,
        str(profile.get("key", "")),
        output_dir / "run-b",
        output_dir / "run-b" / "runner-proof.json",
        execution_path,
    )
    lifecycle = lifecycle_payload(profile, output_dir / "lifecycle", first)
    captures = runner_capture_records(first)
    repeat_captures = runner_capture_records(second)
    first_hashes = [capture.get("raw_rgba_sha256") for capture in captures]
    second_hashes = [capture.get("raw_rgba_sha256") for capture in repeat_captures]
    comparison_status = "pass" if first_hashes == second_hashes else "fail"
    status = "pass" if comparison_status == "pass" and lifecycle.get("status") == "pass" else "fail"
    receipt = {
        "profile_proof_schema": "sslab-profile-proof-v0",
        "status": status,
        "profile": profile.get("key"),
        "product": profile.get("product"),
        "preset": profile.get("preset"),
        "execution_path": execution_path,
        "profile_source": display_path(PROOF_REGISTRY),
        "capture_frames": [int(frame) for frame in profile.get("capture_frames", [])],
        "captures": captures,
        "repeat_captures": repeat_captures,
        "comparison_status": comparison_status,
        "comparison_class": "exact",
        "lifecycle_ref": display_path(output_dir / "lifecycle" / "lifecycle.json"),
        "lifecycle_status": lifecycle.get("status"),
        "lifecycle_create_destroy_cycles": lifecycle.get("create_destroy_cycles"),
        "runner_mode": "generic-libsslab-proof",
        "runner_stdout": "",
        "claim_boundary": "Ricochet multi-frame deterministic and lifecycle proof only; performance, compatibility, release promotion, and artistic acceptance are separate axes.",
    }
    write_json(output_dir / "profile-proof.json", receipt)
    return receipt


def proof_nocturne_from_profile(
    runner_exe: pathlib.Path,
    profile: dict[str, Any],
    output_dir: pathlib.Path,
    execution_path: str,
) -> dict[str, Any]:
    runner_result = run_runner_proof(
        runner_exe,
        str(profile.get("key", "")),
        output_dir / "runner",
        output_dir / "runner-proof.json",
        execution_path,
    )
    render = write_nocturne_render_artifacts(profile, output_dir, runner_result)
    baseline_capture = str(profile.get("baseline_capture", ""))
    comparison: dict[str, Any] = {}
    if baseline_capture:
        comparison_args = argparse.Namespace(
            actual=str(output_dir / "capture.ppm"),
            expected=baseline_capture,
            comparison_class=str(profile.get("comparison_class", "exact")),
            tolerance=0,
            mean_tolerance=0.0,
            output_json=str(output_dir / "comparison.json"),
        )
        comparison = compare_captures(comparison_args)
    lifecycle = lifecycle_payload(profile, output_dir / "lifecycle", runner_result)
    status = "pass"
    if comparison and comparison.get("status") != "pass":
        status = "fail"
    if lifecycle.get("status") != "pass":
        status = "fail"
    receipt = {
        "profile_proof_schema": "sslab-profile-proof-v0",
        "status": status,
        "profile": profile.get("key"),
        "product": profile.get("product"),
        "preset": profile.get("preset"),
        "execution_path": execution_path,
        "profile_source": display_path(PROOF_REGISTRY),
        "capture_frames": [int(frame) for frame in profile.get("capture_frames", [])],
        "render_ref": render.get("capture", {}).get("path"),
        "render_sha256": render.get("capture", {}).get("sha256"),
        "proof_ref": display_path(output_dir / "proof.json"),
        "comparison_ref": comparison.get("path", display_path(output_dir / "comparison.json")) if comparison else "",
        "comparison_status": comparison.get("status", "not-run") if comparison else "not-run",
        "lifecycle_ref": display_path(output_dir / "lifecycle" / "lifecycle.json"),
        "lifecycle_status": lifecycle.get("status"),
        "claim_boundary": "profile-driven mechanical proof only; not compatibility certification or artistic acceptance",
    }
    write_json(output_dir / "profile-proof.json", receipt)
    return receipt


def proof_from_profile(args: argparse.Namespace) -> dict[str, Any]:
    profile = find_proof_profile(str(args.profile))
    output_dir = resolve_output_dir(str(args.output_dir))
    product = str(profile.get("product", ""))
    execution_path = str(getattr(args, "path", "v1"))
    if execution_path not in ("v1", "v2"):
        raise ValueError("proof --path must be v1 or v2")
    if product == "ricochet":
        with tempfile.TemporaryDirectory() as temp_root:
            runner_exe = build_runner(pathlib.Path(temp_root))
            return proof_ricochet_from_profile(runner_exe, profile, output_dir, execution_path)
    if product != "nocturne":
        return {
            "profile_proof_schema": "sslab-profile-proof-v0",
            "status": "blocked",
            "profile": profile.get("key"),
            "product": product,
            "reason": "Only Nocturne and Ricochet are implemented on the profile-driven path before portable v2.",
        }
    with tempfile.TemporaryDirectory() as temp_root:
        runner_exe = build_runner(pathlib.Path(temp_root))
        return proof_nocturne_from_profile(runner_exe, profile, output_dir, execution_path)


def compare_pixels(actual: PpmImage, expected: PpmImage) -> dict[str, Any]:
    if actual.width != expected.width or actual.height != expected.height:
        return {
            "dimension_match": False,
            "pixel_count": 0,
            "changed_channels": 0,
            "changed_pixels": 0,
            "max_abs_delta": None,
            "mean_abs_delta": None,
        }

    total_delta = 0
    max_delta = 0
    changed_channels = 0
    changed_pixels = 0
    pixel_count = actual.width * actual.height
    for index in range(0, len(actual.pixels), 3):
        pixel_changed = False
        for channel in range(3):
            delta = abs(actual.pixels[index + channel] - expected.pixels[index + channel])
            total_delta += delta
            if delta:
                changed_channels += 1
                pixel_changed = True
            if delta > max_delta:
                max_delta = delta
        if pixel_changed:
            changed_pixels += 1

    return {
        "dimension_match": True,
        "pixel_count": pixel_count,
        "changed_channels": changed_channels,
        "changed_pixels": changed_pixels,
        "max_abs_delta": max_delta,
        "mean_abs_delta": total_delta / float(pixel_count * 3),
    }


def comparison_status(metrics: dict[str, Any], comparison_class: str, tolerance: int, mean_tolerance: float) -> str:
    if not metrics["dimension_match"]:
        return "fail"
    if comparison_class == "observational":
        return "informational"
    if comparison_class == "exact":
        return "pass" if metrics["changed_channels"] == 0 else "fail"
    if comparison_class == "tolerant":
        return "pass" if metrics["max_abs_delta"] <= tolerance else "fail"
    if comparison_class == "perceptual":
        return "pass" if metrics["mean_abs_delta"] <= mean_tolerance else "fail"
    return "fail"


def compare_captures(args: argparse.Namespace) -> dict[str, Any]:
    actual_path = pathlib.Path(args.actual)
    expected_path = pathlib.Path(args.expected)
    output_json = pathlib.Path(args.output_json) if args.output_json else None
    if not actual_path.is_absolute():
        actual_path = ROOT / actual_path
    if not expected_path.is_absolute():
        expected_path = ROOT / expected_path
    if output_json is not None and not output_json.is_absolute():
        output_json = ROOT / output_json

    actual = read_ppm(actual_path)
    expected = read_ppm(expected_path)
    metrics = compare_pixels(actual, expected)
    status = comparison_status(metrics, args.comparison_class, args.tolerance, args.mean_tolerance)
    result = {
        "comparison_schema": "sslab-comparison-v0",
        "status": status,
        "class": args.comparison_class,
        "claim_boundary": "pixel comparison aid; not artistic acceptance",
        "actual": {
            "path": display_path(actual_path),
            "sha256": actual.rgb_sha256(),
            "rgba_sha256": actual.rgba_sha256(),
            "width": actual.width,
            "height": actual.height,
        },
        "expected": {
            "path": display_path(expected_path),
            "sha256": expected.rgb_sha256(),
            "rgba_sha256": expected.rgba_sha256(),
            "width": expected.width,
            "height": expected.height,
        },
        "thresholds": {
            "tolerance": args.tolerance,
            "mean_tolerance": args.mean_tolerance,
        },
        "metrics": metrics,
        "limits": [
            "Exact and tolerant comparisons are mechanical.",
            "Perceptual comparison currently uses mean absolute channel delta only.",
            "Observational comparisons record metrics without promotion authority.",
        ],
    }
    if output_json is not None:
        write_json(output_json, result)
        result["path"] = display_path(output_json)
    return result


def add_render_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("render", help="Render the named Nocturne proof profile through the generic sslab runner.")
    parser.add_argument("--product", default="nocturne", choices=["nocturne"])
    parser.add_argument("--preset", default="observatory_night")
    parser.add_argument("--width", type=int, default=96)
    parser.add_argument("--height", type=int, default=54)
    parser.add_argument("--seed", type=int, default=1536)
    parser.add_argument("--frames", type=int, default=8)
    parser.add_argument("--delta-ms", type=int, default=100)
    parser.add_argument("--output-dir", default=str(DEFAULT_OUTPUT.relative_to(ROOT)))
    parser.set_defaults(func=render_nocturne)


def add_compare_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("compare", help="Compare two proof-kernel PPM captures.")
    parser.add_argument("--actual", required=True)
    parser.add_argument("--expected", required=True)
    parser.add_argument(
        "--class",
        dest="comparison_class",
        default="exact",
        choices=["exact", "tolerant", "perceptual", "observational"],
    )
    parser.add_argument("--tolerance", type=int, default=0)
    parser.add_argument("--mean-tolerance", type=float, default=0.0)
    parser.add_argument("--output-json", default=str(DEFAULT_COMPARISON.relative_to(ROOT)))
    parser.set_defaults(func=compare_captures)


def add_lifecycle_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("lifecycle", help="Run a named proof-profile lifecycle compatibility alias.")
    parser.add_argument("--product", default="nocturne", choices=["nocturne", "ricochet"])
    parser.add_argument("--preset", default="observatory_night")
    parser.add_argument("--width", type=int, default=96)
    parser.add_argument("--height", type=int, default=54)
    parser.add_argument("--resize-width", type=int, default=80)
    parser.add_argument("--resize-height", type=int, default=45)
    parser.add_argument("--seed", type=int, default=1536)
    parser.add_argument("--frames", type=int, default=8)
    parser.add_argument("--delta-ms", type=int, default=100)
    parser.add_argument("--create-destroy-cycles", type=int, default=None)
    parser.add_argument("--output-dir", default=str((ROOT / "out" / "proof" / "sslab-lifecycle").relative_to(ROOT)))
    parser.set_defaults(func=lifecycle_product)


def add_profile_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("profile", help="Run a generic-runner profile compatibility alias.")
    parser.add_argument("--product", default="nocturne", choices=["nocturne", "ricochet"])
    parser.add_argument("--preset", default="observatory_night")
    parser.add_argument("--width", type=int, default=96)
    parser.add_argument("--height", type=int, default=54)
    parser.add_argument("--seed", type=int, default=1536)
    parser.add_argument("--frames", type=int, default=8)
    parser.add_argument("--delta-ms", type=int, default=100)
    parser.add_argument("--iterations", type=int, default=5)
    parser.add_argument("--short-soak-cycles", type=int, default=3)
    parser.add_argument("--output-dir", default=str((ROOT / "out" / "proof" / "sslab-profile-run").relative_to(ROOT)))
    parser.set_defaults(func=profile_product)


def add_proof_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("proof", help="Run a generated proof-profile orchestration.")
    parser.add_argument("--profile", default="nocturne.reference.v0")
    parser.add_argument("--path", choices=("v1", "v2"), default="v1")
    parser.add_argument("--output-dir", default=str((ROOT / "out" / "proof" / "sslab-profile").relative_to(ROOT)))
    parser.set_defaults(func=proof_from_profile)


def print_result(result: dict[str, Any]) -> None:
    if result.get("profile_proof_schema") == "sslab-profile-proof-v0":
        if result.get("captures"):
            capture_text = ",".join(
                f"{capture['frame']}:{str(capture.get('raw_rgba_sha256', ''))[:12]}" for capture in result.get("captures", [])
            )
        else:
            capture_text = str(result.get("render_sha256", ""))
        print(
            "profile-proof "
            f"{result['status']} "
            f"{result['profile']} "
            f"path={result.get('execution_path', 'v1')} "
            f"capture={capture_text} "
            f"comparison={result.get('comparison_status', '')} "
            f"lifecycle={result.get('lifecycle_status', '')}"
        )
        return
    if "capture" in result:
        print(f"{result['runtime']['product']} {result['capture']['sha256']} {result['capture']['path']}")
        return
    if "comparison_schema" in result:
        metrics = result["metrics"]
        print(
            "comparison "
            f"{result['status']} "
            f"{result['class']} "
            f"changed_pixels={metrics['changed_pixels']} "
            f"max_delta={metrics['max_abs_delta']}"
        )
        return
    if result.get("lifecycle_schema") in ("screensave-nocturne-canary-lifecycle-v0", "screensave-product-lifecycle-v0"):
        print(
            "lifecycle "
            f"{result['status']} "
            f"resize={result['resize_session']} "
            f"steps={result['step_count']} "
            f"cycles={result.get('create_destroy_cycles', 1)} "
            f"checksum={result['checksum']}"
        )
        return
    if result.get("profile_schema") == "sslab-profile-v0":
        print(
            "profile "
            f"{result['status']} "
            f"{result['product']} "
            f"p50={result['frame_time_ms_p50']:.3f}ms "
            f"p95={result['frame_time_ms_p95']:.3f}ms "
            f"p99={result['frame_time_ms_p99']:.3f}ms "
            f"short_soak={result['short_soak_status']}"
        )
        return
    print(json.dumps(result, sort_keys=True))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    add_render_parser(subparsers)
    add_compare_parser(subparsers)
    add_lifecycle_parser(subparsers)
    add_profile_parser(subparsers)
    add_proof_parser(subparsers)
    args = parser.parse_args()

    result = args.func(args)
    print_result(result)
    if result.get("status") == "fail":
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
