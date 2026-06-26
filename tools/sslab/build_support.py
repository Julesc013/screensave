"""Shared build helpers for the generic sslab runner."""

from __future__ import annotations

import pathlib
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SSLAB_INCLUDE = ROOT / "tools" / "sslab" / "include"
SSLAB_SRC = ROOT / "tools" / "sslab" / "src"
PLATFORM_INCLUDE = ROOT / "platform" / "include"
NOCTURNE_SRC = ROOT / "products" / "savers" / "nocturne" / "src"
RICOCHET_SRC = ROOT / "products" / "savers" / "ricochet" / "src"
PROOF_REGISTRY_C = ROOT / "catalog" / "generated" / "proof_registry.c"
GENERIC_RUNNER = ROOT / "tools" / "sslab" / "runner" / "sslab_runner.c"

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
    NOCTURNE_SRC / "nocturne_core.c",
    NOCTURNE_SRC / "nocturne_v1_adapter.c",
    NOCTURNE_SRC / "nocturne_sim.c",
    NOCTURNE_SRC / "nocturne_render.c",
    NOCTURNE_SRC / "nocturne_v2_adapter.c",
    NOCTURNE_SRC / "nocturne_themes.c",
    NOCTURNE_SRC / "nocturne_presets.c",
    RICOCHET_SRC / "ricochet_core.c",
    RICOCHET_SRC / "ricochet_v1_adapter.c",
    RICOCHET_SRC / "ricochet_sim.c",
    RICOCHET_SRC / "ricochet_render.c",
    RICOCHET_SRC / "ricochet_v2_adapter.c",
    RICOCHET_SRC / "ricochet_themes.c",
    RICOCHET_SRC / "ricochet_presets.c",
    ROOT / "platform" / "src" / "v2" / "base_validate.c",
    ROOT / "platform" / "src" / "v2" / "config_view.c",
    ROOT / "platform" / "src" / "v2" / "surface_view.c",
    ROOT / "platform" / "src" / "v2" / "draw_target.c",
    ROOT / "platform" / "src" / "v2" / "session_helpers.c",
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
    "-I",
    str(ROOT / "catalog" / "generated"),
]


def runner_implementation_paths() -> list[pathlib.Path]:
    """Return the full generic runner implementation surface."""
    return [*LIBSSLAB_SOURCES, PROOF_REGISTRY_C, GENERIC_RUNNER]


def build_runner(temp_path: pathlib.Path) -> pathlib.Path:
    """Build one private libsslab archive and the generic sslab runner."""
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
    subprocess.check_call(["ar", "rcs", str(lib_path), *(str(path) for path in object_paths)], cwd=ROOT)
    runner_path = temp_path / "sslab_runner.exe"
    subprocess.check_call(
        [
            "gcc",
            "-std=c89",
            "-Wall",
            "-Wextra",
            "-pedantic",
            *INCLUDE_ARGS,
            str(GENERIC_RUNNER),
            str(PROOF_REGISTRY_C),
            str(lib_path),
            "-o",
            str(runner_path),
        ],
        cwd=ROOT,
    )
    return runner_path
