"""Validate the Plasma v2 direct-control instrument spec island."""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
TEST = ROOT / "products" / "savers" / "plasma" / "tests" / "plasma_v2_spec_smoke.c"
OUT = ROOT / "out" / "checks" / "plasma-v2-direct-spec"

REQUIRED_FILES = [
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-constitution.md",
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-migration.md",
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-acceptance.md",
    SRC / "plasma_v2_types.h",
    SRC / "plasma_v2_spec.h",
    SRC / "plasma_v2_spec.c",
    TEST,
]

REQUIRED_TEXT = {
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-constitution.md": [
        "Plasma is not a preset picker.",
        "software/reference path is the visual authority",
        "not publish a release",
    ],
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-migration.md": [
        "Legacy inputs seed a v2 specification once.",
        "plasma_v2_legacy_config_view",
        "plasma_lava",
    ],
    ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-acceptance.md": [
        "Decorative sliders are not acceptable.",
        "Proof pass, visual acceptance, release promotion",
    ],
    SRC / "plasma_v2_types.h": [
        "screensave.plasma.v2.instrument-spec",
        "PLASMA_V2_CONTROL_MAX",
        "typedef struct plasma_v2_spec_tag",
        "typedef struct plasma_v2_legacy_config_view_tag",
    ],
    SRC / "plasma_v2_spec.h": [
        "plasma_v2_spec_from_legacy_config_view",
        "plasma_v2_field_family_from_token",
        "plasma_v2_spec_apply_control",
    ],
    SRC / "plasma_v2_spec.c": [
        "plasma_v2_apply_legacy_preset",
        "ocean_interference",
        "museum_phosphor",
        "PLASMA_V2_CONTROL_MAX",
    ],
}

FORBIDDEN_CODE_TEXT = [
    "windows.h",
    "HWND",
    "HDC",
    "HINSTANCE",
    "screensave_saver_environment",
    "screensave_saver_session",
    "screensave/renderer",
    "renderer_api",
    "malloc",
    "free(",
    "//",
    "_Static_assert",
    "stdbool.h",
    "stdint.h",
    "unsigned long",
    " long ",
    "DWORD",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def find_compiler() -> str | None:
    for candidate in ("gcc", "clang", "cc"):
        compiler = shutil.which(candidate)
        if compiler:
            return compiler
    return None


def scan_files(errors: list[str]) -> None:
    for path in REQUIRED_FILES:
        require(path.exists(), f"Missing required Plasma v2 direct spec file: {repo_path(path)}", errors)

    if errors:
        return

    for path, needles in REQUIRED_TEXT.items():
        text = path.read_text(encoding="utf-8")
        for needle in needles:
            require(needle in text, f"{repo_path(path)} missing {needle!r}", errors)

    for path in (SRC / "plasma_v2_types.h", SRC / "plasma_v2_spec.h", SRC / "plasma_v2_spec.c"):
        text = path.read_text(encoding="utf-8")
        for token in FORBIDDEN_CODE_TEXT:
            require(token not in text, f"{repo_path(path)} contains forbidden direct-spec token {token!r}", errors)


def compile_smoke(errors: list[str]) -> None:
    compiler = find_compiler()
    if compiler is None:
        errors.append("No C compiler found for Plasma v2 direct spec C89 smoke compile.")
        return

    OUT.mkdir(parents=True, exist_ok=True)
    exe = OUT / "plasma_v2_spec_smoke.exe"
    command = [
        compiler,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-I",
        str(SRC),
        str(TEST),
        str(SRC / "plasma_v2_spec.c"),
        "-o",
        str(exe),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(
            "Plasma v2 direct spec C89 compile failed: "
            + (result.stdout.strip() + " " + result.stderr.strip()).strip()
        )
        return

    run = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run.returncode != 0:
        errors.append(
            "Plasma v2 direct spec smoke failed: "
            + (run.stdout.strip() + " " + run.stderr.strip()).strip()
            + f" returncode={run.returncode}"
        )


def main() -> int:
    errors: list[str] = []
    scan_files(errors)
    if not errors:
        compile_smoke(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 direct spec checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
