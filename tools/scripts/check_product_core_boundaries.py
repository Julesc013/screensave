"""Validate portable product-core dependency boundaries."""

from __future__ import annotations

import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]

CORE_FILES = [
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_core.h",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_core.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_core.h",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_core.c",
]

ALLOWED_INCLUDES = {
    "nocturne_core.h",
    "ricochet_core.h",
    "screensave/v2.h",
    "stdlib.h",
    "string.h",
}

FORBIDDEN_PATTERNS = [
    (re.compile(r"windows\.h", re.I), "Win32 header"),
    (re.compile(r"screensave/saver_api\.h"), "v1 saver API"),
    (re.compile(r"screensave/saver\.h"), "v1 saver header"),
    (re.compile(r"\bscreensave_renderer\b"), "renderer API"),
    (re.compile(r"\bscreensave_saver_environment\b"), "v1 saver environment"),
    (re.compile(r"\bscreensave_saver_session\b"), "v1 saver session"),
    (re.compile(r"\bHWND\b"), "Win32 HWND"),
    (re.compile(r"\bHDC\b"), "Win32 HDC"),
    (re.compile(r"\bHINSTANCE\b"), "Win32 HINSTANCE"),
    (re.compile(r"\bReg[A-Za-z0-9_]*\b"), "Win32 registry API"),
    (re.compile(r"\blstr[A-Za-z0-9_]*\b"), "Win32 string API"),
    (re.compile(r"\bDWORD\b"), "Win32 DWORD"),
    (re.compile(r"\bunsigned\s+long\b"), "public or proof-observable unsigned long"),
    (re.compile(r"\blong\b"), "public or proof-observable long"),
]

INCLUDE_RE = re.compile(r"^\s*#\s*include\s+[<\"]([^>\"]+)[>\"]")


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def check_file(path: pathlib.Path, errors: list[str]) -> None:
    if not path.exists():
        errors.append(f"Missing product core file: {repo_path(path)}")
        return

    text = path.read_text(encoding="utf-8")
    lines = text.splitlines()
    for line_number, line in enumerate(lines, start=1):
        include_match = INCLUDE_RE.match(line)
        if include_match:
            include_name = include_match.group(1)
            if include_name not in ALLOWED_INCLUDES:
                errors.append(
                    f"{repo_path(path)}:{line_number}: product core includes disallowed dependency {include_name!r}"
                )
        for pattern, label in FORBIDDEN_PATTERNS:
            if pattern.search(line):
                errors.append(f"{repo_path(path)}:{line_number}: product core contains forbidden {label}")


def main() -> int:
    errors: list[str] = []

    for path in CORE_FILES:
        check_file(path, errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Product core boundary checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
