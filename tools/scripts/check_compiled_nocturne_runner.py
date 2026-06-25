"""Build and validate the compiled Proof Kernel v0 Nocturne canary runner."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
RUNNER = ROOT / "tools" / "sslab" / "nocturne_canary_runner.c"
SSLAB = ROOT / "tools" / "sslab" / "sslab.py"
EXPECTED_CAPTURE = ROOT / "validation" / "captures" / "proof-kernel-v0" / "nocturne" / "capture.ppm"
SURFACE = ROOT / "platform" / "src" / "surface" / "rgba8" / "surface_rgba8.c"
SOFT_RENDERER = ROOT / "platform" / "src" / "render" / "soft" / "soft_renderer.c"
NOCTURNE_SIM = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_sim.c"
NOCTURNE_RENDER = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_render.c"
NOCTURNE_THEMES = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_themes.c"
NOCTURNE_PRESETS = ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_presets.c"


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def compile_runner(output_exe: pathlib.Path) -> None:
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
            str(RUNNER),
            str(NOCTURNE_SIM),
            str(NOCTURNE_RENDER),
            str(NOCTURNE_THEMES),
            str(NOCTURNE_PRESETS),
            str(SURFACE),
            str(SOFT_RENDERER),
            "-o",
            str(output_exe),
        ],
        cwd=ROOT,
    )


def main() -> int:
    errors: list[str] = []

    for path in (RUNNER, SSLAB, EXPECTED_CAPTURE, SURFACE, SOFT_RENDERER, NOCTURNE_SIM, NOCTURNE_RENDER, NOCTURNE_THEMES, NOCTURNE_PRESETS):
        require(path.exists(), f"Missing compiled Nocturne runner input: {path.relative_to(ROOT)}", errors)

    if RUNNER.exists():
        text = RUNNER.read_text(encoding="utf-8")
        require("compiled-nocturne" in text, "Compiled Nocturne runner must identify its proof output.", errors)
        require("product-session" in text, "Compiled Nocturne runner must identify the product-session proof path.", errors)
        require("nocturne_create_session" in text, "Compiled Nocturne runner must call the product session constructor.", errors)
        require("nocturne_step_session" in text, "Compiled Nocturne runner must call the product step function.", errors)
        require("nocturne_render_session" in text, "Compiled Nocturne runner must call the product render function.", errors)
        require("screensave/private/soft_renderer.h" in text, "Compiled Nocturne runner must use the private soft renderer.", errors)
        require("windows.h" not in text, "Compiled Nocturne runner must not include windows.h directly.", errors)
        require("runner_rng_state" not in text, "Compiled Nocturne runner must not carry a mirror RNG implementation.", errors)
        require("runner_step_session" not in text, "Compiled Nocturne runner must not carry a mirror session step implementation.", errors)
        require("runner_render_session" not in text, "Compiled Nocturne runner must not carry a mirror render implementation.", errors)

    require(shutil.which("gcc") is not None, "gcc must be available to validate the compiled Nocturne runner.", errors)

    if not errors:
        with tempfile.TemporaryDirectory() as temp_root:
            temp_path = pathlib.Path(temp_root)
            output_exe = temp_path / "nocturne_canary_runner.exe"
            actual_capture = temp_path / "capture.ppm"
            comparison_json = temp_path / "comparison.json"

            compile_runner(output_exe)
            subprocess.check_call(
                [
                    str(output_exe),
                    "--width",
                    "96",
                    "--height",
                    "54",
                    "--seed",
                    "1536",
                    "--frames",
                    "8",
                    "--delta-ms",
                    "100",
                    "--output",
                    str(actual_capture),
                ],
                cwd=ROOT,
            )
            subprocess.check_call(
                [
                    sys.executable,
                    str(SSLAB),
                    "compare",
                    "--actual",
                    str(actual_capture),
                    "--expected",
                    str(EXPECTED_CAPTURE),
                    "--class",
                    "exact",
                    "--output-json",
                    str(comparison_json),
                ],
                cwd=ROOT,
                stdout=subprocess.DEVNULL,
            )
            comparison = json.loads(comparison_json.read_text(encoding="utf-8"))
            require(comparison.get("status") == "pass", "Compiled Nocturne runner must match committed canary capture exactly.", errors)
            require(
                comparison.get("metrics", {}).get("changed_pixels") == 0,
                "Compiled Nocturne runner exact comparison must have zero changed pixels.",
                errors,
            )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Compiled Nocturne runner checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
