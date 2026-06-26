"""Validate the public portable v2 header seam."""

from __future__ import annotations

import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
V2_ROOT = ROOT / "platform" / "include" / "screensave" / "v2"
AGGREGATE_HEADER = ROOT / "platform" / "include" / "screensave" / "v2.h"

REQUIRED_HEADERS = [
    V2_ROOT / "base.h",
    V2_ROOT / "types.h",
    V2_ROOT / "clock.h",
    V2_ROOT / "seed.h",
    V2_ROOT / "diagnostics.h",
    V2_ROOT / "config.h",
    V2_ROOT / "surface.h",
    V2_ROOT / "draw.h",
    V2_ROOT / "session.h",
    V2_ROOT / "product.h",
    AGGREGATE_HEADER,
]

FORBIDDEN_NATIVE_TERMS = [
    "windows.h",
    "HWND",
    "HDC",
    "HINSTANCE",
    "HANDLE",
    "HMODULE",
    "LPSTR",
    "LPCSTR",
    "DWORD",
    "WPARAM",
    "LPARAM",
    "LRESULT",
    "WINAPI",
    "registry",
    "AppKit",
    "Core Graphics",
    "Metal",
    "X11",
    "SDL",
]

C89_FORBIDDEN = [
    "//",
    "_Static_assert",
    "static_assert",
    "stdint.h",
    "stdbool.h",
    "inline",
    "restrict",
]

BASE_TYPE_NEEDLES = [
    "typedef unsigned char ss_u8;",
    "typedef unsigned short ss_u16;",
    "typedef unsigned int ss_u32;",
    "typedef signed int ss_i32;",
    "sizeof(ss_u8) == 1U",
    "sizeof(ss_u16) == 2U",
    "sizeof(ss_u32) == 4U",
    "sizeof(ss_i32) == 4U",
]

STRUCT_RE = re.compile(r"typedef\s+struct\s+\w+\s*\{(?P<body>.*?)\}\s*(?P<alias>\w+)\s*;", re.S)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def strip_block_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/", "", text, flags=re.S)


def normalized_struct_fields(body: str) -> list[str]:
    text = strip_block_comments(body)
    fields: list[str] = []
    for line in text.splitlines():
        stripped = line.strip()
        if stripped:
            fields.append(stripped)
    return fields


def scan_headers(errors: list[str]) -> None:
    for path in REQUIRED_HEADERS:
        require(path.exists(), f"Missing portable v2 header: {path.relative_to(ROOT)}", errors)
    if errors:
        return

    base_text = (V2_ROOT / "base.h").read_text(encoding="utf-8")
    for needle in BASE_TYPE_NEEDLES:
        require(needle in base_text, f"base.h is missing fixed-width guard text: {needle}", errors)

    for path in REQUIRED_HEADERS:
        text = path.read_text(encoding="utf-8")
        relative = path.relative_to(ROOT)
        for term in FORBIDDEN_NATIVE_TERMS:
            require(term not in text, f"{relative} contains forbidden native term: {term}", errors)
        for term in C89_FORBIDDEN:
            require(term not in text, f"{relative} contains a non-C89 header token: {term}", errors)
        if path != V2_ROOT / "base.h":
            require(
                not re.search(r"\bunsigned\s+long\b|\blong\b", text),
                f"{relative} must not use public or persisted long fields.",
                errors,
            )
            require(
                not re.search(r"\bunsigned\s+int\b|\bsigned\s+int\b|\bint\b|\bshort\b", text),
                f"{relative} must use ss_v2 fixed-width aliases instead of raw integer types.",
                errors,
            )

        for match in STRUCT_RE.finditer(text):
            alias = match.group("alias")
            fields = normalized_struct_fields(match.group("body"))
            require(len(fields) >= 2, f"{relative}:{alias} must expose at least the v2 prefix fields.", errors)
            if len(fields) >= 2:
                require(
                    fields[0] == "ss_u32 struct_size;" and fields[1] == "ss_u32 abi_version;",
                    f"{relative}:{alias} must start with ss_u32 struct_size; ss_u32 abi_version;",
                    errors,
                )

    aggregate_text = AGGREGATE_HEADER.read_text(encoding="utf-8")
    for header in REQUIRED_HEADERS:
        if header == AGGREGATE_HEADER:
            continue
        include = f'#include "screensave/v2/{header.name}"'
        require(include in aggregate_text, f"v2.h must include {header.name}.", errors)


def main() -> int:
    errors: list[str] = []
    scan_headers(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Portable v2 header checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
