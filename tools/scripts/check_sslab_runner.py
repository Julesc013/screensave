"""Build one private libsslab and validate the generic sslab runner."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
SSLAB_INCLUDE = ROOT / "tools" / "sslab" / "include"
SSLAB_SRC = ROOT / "tools" / "sslab" / "src"
PLATFORM_INCLUDE = ROOT / "platform" / "include"
NOCTURNE_SRC = ROOT / "products" / "savers" / "nocturne" / "src"
RICOCHET_SRC = ROOT / "products" / "savers" / "ricochet" / "src"
PROOF_REGISTRY_C = ROOT / "catalog" / "generated" / "proof_registry.c"
GENERIC_RUNNER = ROOT / "tools" / "sslab" / "runner" / "sslab_runner.c"
NOCTURNE_HASH = "5394a14b6622c17bfb10cd5721c08a4c92cdbddfb12f55c954ef1d5f6ef878b2"

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
    "-I",
    str(ROOT / "catalog" / "generated"),
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def build_runner(temp_path: pathlib.Path) -> pathlib.Path:
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


def run_proof(runner_path: pathlib.Path, profile: str) -> dict[str, object]:
    output = subprocess.check_output(
        [str(runner_path), "proof", "--profile", profile],
        cwd=ROOT,
        text=True,
    )
    return json.loads(output)


def main() -> int:
    errors: list[str] = []
    for path in [GENERIC_RUNNER, PROOF_REGISTRY_C, *LIBSSLAB_SOURCES]:
        require(path.exists(), f"Missing sslab runner path: {path.relative_to(ROOT)}", errors)

    if not errors:
        with tempfile.TemporaryDirectory() as temp_root:
            runner = build_runner(pathlib.Path(temp_root))
            nocturne = run_proof(runner, "nocturne.reference.v0")
            ricochet = run_proof(runner, "ricochet.reference.v1")
            nocturne_captures = nocturne.get("captures", [])
            ricochet_captures = ricochet.get("captures", [])

            require(nocturne.get("status") == "pass", "Nocturne generic runner proof must pass.", errors)
            require(nocturne.get("profile") == "nocturne.reference.v0", "Nocturne proof must record profile key.", errors)
            require(isinstance(nocturne_captures, list) and len(nocturne_captures) == 1, "Nocturne proof must emit one capture.", errors)
            if isinstance(nocturne_captures, list) and nocturne_captures:
                require(
                    nocturne_captures[0].get("rgba_sha256") == NOCTURNE_HASH,
                    "Nocturne generic runner must retain the established RGBA hash.",
                    errors,
                )

            require(ricochet.get("status") == "pass", "Ricochet generic runner proof must pass.", errors)
            require(ricochet.get("profile") == "ricochet.reference.v1", "Ricochet proof must record profile key.", errors)
            require(isinstance(ricochet_captures, list) and len(ricochet_captures) == 4, "Ricochet proof must emit four captures.", errors)
            if isinstance(ricochet_captures, list):
                for capture in ricochet_captures:
                    require(capture.get("status") == "pass", "Each Ricochet capture must pass.", errors)
                    require(len(str(capture.get("rgba_sha256", ""))) == 64, "Each Ricochet capture must emit a SHA-256.", errors)
            lifecycle = ricochet.get("lifecycle", {})
            require(isinstance(lifecycle, dict), "Ricochet proof must emit lifecycle facts.", errors)
            if isinstance(lifecycle, dict):
                require(lifecycle.get("create_destroy_cycles") == 32, "Ricochet lifecycle must run 32 create/destroy cycles.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Generic sslab runner checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
