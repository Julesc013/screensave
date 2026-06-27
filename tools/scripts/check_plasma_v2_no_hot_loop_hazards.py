"""Reject hot-loop hazards from the direct Plasma v2 reference island."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
V2 = ROOT / "products" / "savers" / "plasma" / "src" / "v2"

SCAN_GLOBS = [
    "plasma_v2_runtime.c",
    "field/*.c",
    "output/*.c",
]

FORBIDDEN = [
    "malloc",
    "calloc",
    "realloc",
    "free(",
    "fopen",
    "fread",
    "fwrite",
    "sin(",
    "cos(",
    "tan(",
    "pow(",
    "sqrt(",
    "windows.h",
    "screensave_renderer",
    "renderer_api",
    "HDC",
    "HWND",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def scan_files() -> list[pathlib.Path]:
    paths: list[pathlib.Path] = []
    for pattern in SCAN_GLOBS:
        paths.extend(sorted(V2.glob(pattern)))
    return paths


def main() -> int:
    errors: list[str] = []
    paths = scan_files()
    if not (V2 / "plasma_v2_runtime.c").exists():
        errors.append("Missing direct Plasma v2 runtime source for hazard scan.")

    for path in paths:
        text = path.read_text(encoding="utf-8")
        for token in FORBIDDEN:
            if token in text:
                errors.append(f"{repo_path(path)} contains hot-loop hazard token {token!r}")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 hot-loop hazard checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
