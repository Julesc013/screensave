"""Validate the minimal BenchLab/Workbench shell binding."""

from __future__ import annotations

import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
SHELL_SOURCE = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.c"
SHELL_HEADER = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.h"
SMOKE_SOURCE = ROOT / "products" / "apps" / "benchlab" / "tests" / "workbench_shell_smoke.c"
PROOF_REGISTRY_C = ROOT / "catalog" / "generated" / "proof_registry.c"
SSLAB_HEADER = ROOT / "tools" / "sslab" / "include" / "screensave" / "sslab.h"
SSLAB_INCLUDE = ROOT / "tools" / "sslab" / "include"
SSLAB_SRC = ROOT / "tools" / "sslab" / "src"
PLATFORM_INCLUDE = ROOT / "platform" / "include"
NOCTURNE_SRC = ROOT / "products" / "savers" / "nocturne" / "src"
RICOCHET_SRC = ROOT / "products" / "savers" / "ricochet" / "src"

LIBSSLAB_SOURCES = [
    SSLAB_SRC / "abi.c",
    SSLAB_SRC / "context.c",
    SSLAB_SRC / "product.c",
    SSLAB_SRC / "session.c",
    SSLAB_SRC / "operations.c",
    SSLAB_SRC / "capture.c",
    SSLAB_SRC / "renderer_rgba8.c",
    SSLAB_SRC / "diag_stub.c",
    SSLAB_SRC / "products" / "nocturne_adapter.c",
    SSLAB_SRC / "products" / "ricochet_adapter.c",
    NOCTURNE_SRC / "nocturne_sim.c",
    NOCTURNE_SRC / "nocturne_render.c",
    NOCTURNE_SRC / "nocturne_themes.c",
    NOCTURNE_SRC / "nocturne_presets.c",
    RICOCHET_SRC / "ricochet_sim.c",
    RICOCHET_SRC / "ricochet_render.c",
    RICOCHET_SRC / "ricochet_themes.c",
    RICOCHET_SRC / "ricochet_presets.c",
    ROOT / "platform" / "src" / "core" / "config" / "config.c",
    ROOT / "platform" / "src" / "core" / "visual" / "visual_buffer.c",
    ROOT / "platform" / "src" / "surface" / "rgba8" / "surface_rgba8.c",
    ROOT / "platform" / "src" / "render" / "soft" / "soft_renderer.c",
]

INCLUDE_ARGS = [
    "-I",
    str(PLATFORM_INCLUDE),
    "-I",
    str(SSLAB_INCLUDE),
    "-I",
    str(SSLAB_SRC),
    "-I",
    str(NOCTURNE_SRC),
    "-I",
    str(RICOCHET_SRC),
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    for path in (SHELL_SOURCE, SHELL_HEADER, SMOKE_SOURCE, PROOF_REGISTRY_C, SSLAB_HEADER, *LIBSSLAB_SOURCES):
        require(path.exists(), f"Missing Workbench shell path: {path.relative_to(ROOT)}", errors)

    if SHELL_SOURCE.exists():
        text = SHELL_SOURCE.read_text(encoding="utf-8")
        for needle in [
            "catalog",
            "run",
            "inspect",
            "compare",
            "SSLAB_ABI_VERSION",
            "screensave_generated_find_proof_profile",
            "benchlab_workbench_shell_run_profile_once",
            "nocturne.reference.v0",
            "ricochet.reference.v1",
        ]:
            require(needle in text, f"benchlab_workbench_shell.c is missing expected text: {needle!r}", errors)

    if not errors:
        with tempfile.TemporaryDirectory() as temp_root:
            temp_path = pathlib.Path(temp_root)
            object_paths: list[pathlib.Path] = []
            for source in LIBSSLAB_SOURCES:
                object_path = temp_path / f"{source.stem}-{len(object_paths)}.o"
                subprocess.check_call(
                    [
                        "gcc",
                        "-std=c89",
                        "-Wall",
                        "-Wextra",
                        "-pedantic",
                        *INCLUDE_ARGS,
                        "-c",
                        str(source),
                        "-o",
                        str(object_path),
                    ],
                    cwd=ROOT,
                )
                object_paths.append(object_path)

            lib_path = temp_path / "libsslab.a"
            subprocess.check_call(
                [
                    "ar",
                    "rcs",
                    str(lib_path),
                    *(str(path) for path in object_paths),
                ],
                cwd=ROOT,
            )

            exe_path = pathlib.Path(temp_root) / "workbench_shell_smoke.exe"
            subprocess.check_call(
                [
                    "gcc",
                    "-std=c89",
                    "-Wall",
                    "-Wextra",
                    "-pedantic",
                    *INCLUDE_ARGS,
                    str(SHELL_SOURCE),
                    str(PROOF_REGISTRY_C),
                    str(SMOKE_SOURCE),
                    str(lib_path),
                    "-o",
                    str(exe_path),
                ],
                cwd=ROOT,
            )
            subprocess.check_call([str(exe_path)], cwd=ROOT)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Workbench shell checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
