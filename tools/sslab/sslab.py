"""ScreenSave Lab Runner v0 for deterministic proof-kernel captures."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import subprocess
import sys
import tempfile
from dataclasses import dataclass


ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFAULT_OUTPUT = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne"
DEFAULT_COMPARISON = ROOT / "out" / "proof" / "sslab-compare" / "comparison.json"
RUNNER = ROOT / "tools" / "sslab" / "nocturne_canary_runner.c"
SURFACE = ROOT / "platform" / "src" / "surface" / "rgba8" / "surface_rgba8.c"
SOFT_RENDERER = ROOT / "platform" / "src" / "render" / "soft" / "soft_renderer.c"
NOCTURNE_SIM = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_sim.c"
NOCTURNE_RENDER = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_render.c"
NOCTURNE_THEMES = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_themes.c"
NOCTURNE_PRESETS = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_presets.c"
RUNNER_SOURCES = [RUNNER, NOCTURNE_SIM, NOCTURNE_RENDER, NOCTURNE_THEMES, NOCTURNE_PRESETS, SURFACE, SOFT_RENDERER]


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


def compile_nocturne_runner(output_exe: pathlib.Path) -> None:
    subprocess.check_call(
        [
            "gcc",
            "-std=c89",
            "-Wall",
            "-Wextra",
            "-I",
            str(ROOT / "platform" / "include"),
            "-I",
            str(ROOT / "products" / "savers" / "nocturne" / "src"),
            *(str(path) for path in RUNNER_SOURCES),
            "-o",
            str(output_exe),
        ],
        cwd=ROOT,
    )


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


def render_nocturne(args: argparse.Namespace) -> dict:
    output_dir = pathlib.Path(args.output_dir)
    if not output_dir.is_absolute():
        output_dir = ROOT / output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    capture_path = output_dir / "capture.ppm"
    proof_path = output_dir / "proof.json"
    hash_path = output_dir / "capture.sha256"

    with tempfile.TemporaryDirectory() as temp_root:
        runner_exe = pathlib.Path(temp_root) / "nocturne_canary_runner.exe"
        compile_nocturne_runner(runner_exe)
        runner = subprocess.run(
            [
                str(runner_exe),
                "--width",
                str(args.width),
                "--height",
                str(args.height),
                "--seed",
                str(args.seed),
                "--frames",
                str(args.frames),
                "--delta-ms",
                str(args.delta_ms),
                "--preset",
                args.preset,
                "--output",
                str(capture_path),
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        if runner.returncode != 0:
            raise RuntimeError(f"compiled Nocturne runner failed: {runner.stderr.strip()}")

    capture = read_ppm(capture_path)
    capture_hash = capture.rgba_sha256()
    hash_path.write_text(capture_hash + "\n", encoding="ascii")

    proof = {
        "proof_schema": "proof-bundle-v0",
        "proof_kernel": "proof-kernel-v0",
        "status": "informational",
        "claim_boundary": "deterministic canary capture; not compatibility certification",
        "source": {
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "runtime": {
            "runner": "tools/sslab/nocturne_canary_runner.c",
            "runner_mode": "compiled-product-session",
            "runner_stdout": runner.stdout.strip(),
            "product": "nocturne",
            "preset": args.preset,
            "theme": "gray_black",
            "width": args.width,
            "height": args.height,
            "seed": args.seed,
            "frames": args.frames,
            "delta_ms": args.delta_ms,
            "surface": "rgba8-top-left-srgb",
            "renderer": "soft-reference-v0",
        },
        "implementation": [
            {
                "path": display_path(path),
                "sha256": sha256_file(path),
            }
            for path in RUNNER_SOURCES
        ],
        "capture": {
            "path": display_path(capture_path),
            "sha256": capture_hash,
            "format": "ppm-p3-rgb-from-rgba8",
            "canonical_hash_source": "rgba8-bytes",
        },
        "limits": [
            "Nocturne canary only",
            "Compiled runner drives the real Nocturne product session and render functions",
            "No operating-system compatibility claim is certified by this proof",
        ],
    }
    proof_path.write_text(json.dumps(proof, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return proof


def lifecycle_nocturne(args: argparse.Namespace) -> dict:
    output_dir = pathlib.Path(args.output_dir)
    if not output_dir.is_absolute():
        output_dir = ROOT / output_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    capture_path = output_dir / "lifecycle-capture.ppm"
    lifecycle_path = output_dir / "lifecycle.json"

    with tempfile.TemporaryDirectory() as temp_root:
        runner_exe = pathlib.Path(temp_root) / "nocturne_canary_runner.exe"
        compile_nocturne_runner(runner_exe)
        runner = subprocess.run(
            [
                str(runner_exe),
                "--width",
                str(args.width),
                "--height",
                str(args.height),
                "--seed",
                str(args.seed),
                "--frames",
                str(args.frames),
                "--delta-ms",
                str(args.delta_ms),
                "--preset",
                args.preset,
                "--exercise-resize",
                "--resize-width",
                str(args.resize_width),
                "--resize-height",
                str(args.resize_height),
                "--output",
                str(capture_path),
                "--lifecycle-output",
                str(lifecycle_path),
            ],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        if runner.returncode != 0:
            raise RuntimeError(f"compiled Nocturne lifecycle runner failed: {runner.stderr.strip()}")

    lifecycle = json.loads(lifecycle_path.read_text(encoding="utf-8"))
    lifecycle["path"] = display_path(lifecycle_path)
    lifecycle["capture_path"] = display_path(capture_path)
    lifecycle["runner_mode"] = "compiled-product-session"
    lifecycle["runner_stdout"] = runner.stdout.strip()
    return lifecycle


def compare_pixels(actual: PpmImage, expected: PpmImage) -> dict:
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


def comparison_status(metrics: dict, comparison_class: str, tolerance: int, mean_tolerance: float) -> str:
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


def compare_captures(args: argparse.Namespace) -> dict:
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
        output_json.parent.mkdir(parents=True, exist_ok=True)
        output_json.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    return result


def add_render_parser(subparsers: argparse._SubParsersAction[argparse.ArgumentParser]) -> None:
    parser = subparsers.add_parser("render", help="Render a deterministic proof-kernel canary capture.")
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
    parser = subparsers.add_parser("lifecycle", help="Run a Nocturne create/resize/step/render/destroy lifecycle proof.")
    parser.add_argument("--product", default="nocturne", choices=["nocturne"])
    parser.add_argument("--preset", default="observatory_night")
    parser.add_argument("--width", type=int, default=96)
    parser.add_argument("--height", type=int, default=54)
    parser.add_argument("--resize-width", type=int, default=80)
    parser.add_argument("--resize-height", type=int, default=45)
    parser.add_argument("--seed", type=int, default=1536)
    parser.add_argument("--frames", type=int, default=8)
    parser.add_argument("--delta-ms", type=int, default=100)
    parser.add_argument("--output-dir", default=str((ROOT / "out" / "proof" / "sslab-lifecycle").relative_to(ROOT)))
    parser.set_defaults(func=lifecycle_nocturne)


def print_result(result: dict) -> None:
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
    if result.get("lifecycle_schema") == "screensave-nocturne-canary-lifecycle-v0":
        print(
            "lifecycle "
            f"{result['status']} "
            f"resize={result['resize_session']} "
            f"steps={result['step_count']} "
            f"checksum={result['checksum']}"
        )
        return
    print(json.dumps(result, sort_keys=True))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    add_render_parser(subparsers)
    add_compare_parser(subparsers)
    add_lifecycle_parser(subparsers)
    args = parser.parse_args()

    result = args.func(args)
    print_result(result)
    if result.get("status") == "fail":
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
