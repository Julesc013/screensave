"""Validate the public portable v2 header seam."""

from __future__ import annotations

import pathlib
import re
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
V2_ROOT = ROOT / "platform" / "include" / "screensave" / "v2"
AGGREGATE_HEADER = ROOT / "platform" / "include" / "screensave" / "v2.h"
V2_SOURCE_ROOT = ROOT / "platform" / "src" / "v2"
SMOKE_ROOT = ROOT / "out" / "checks" / "portable-v2"

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

REQUIRED_SOURCE_FILES = [
    V2_SOURCE_ROOT / "base_validate.c",
    V2_SOURCE_ROOT / "diagnostics.c",
    V2_SOURCE_ROOT / "config_view.c",
    V2_SOURCE_ROOT / "surface_view.c",
    V2_SOURCE_ROOT / "draw_target.c",
    V2_SOURCE_ROOT / "draw_renderer_bridge.c",
    V2_SOURCE_ROOT / "session_helpers.c",
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
SMOKE_SOURCE = r'''
#include "screensave/v2.h"

static void smoke_sink(void *user_data, const ss_v2_diag_message *message)
{
    (void)user_data;
    (void)message;
}

int main(void)
{
    ss_v2_u64_parts parts;
    ss_v2_pointi point_a;
    ss_v2_pointi point_b;
    ss_v2_size size;
    ss_v2_recti rect;
    ss_v2_color_rgba8 color;
    ss_v2_clock clock;
    ss_v2_seed seed;
    ss_v2_diag_message message;
    ss_v2_diag_context diagnostics;
    ss_v2_config_view config;
    ss_v2_surface_desc surface;
    ss_v2_draw_ops draw_ops;
    ss_v2_draw_target draw_target;
    ss_v2_session_desc session_desc;
    ss_v2_resize_desc resize_desc;
    ss_v2_advance_desc advance_desc;
    ss_v2_render_desc render_desc;
    ss_v2_session_ops session_ops;
    ss_v2_product_descriptor product;
    ss_u8 pixels[16];

    parts.struct_size = (ss_u32)sizeof(parts);
    parts.abi_version = SS_V2_ABI_VERSION;
    parts.low = 0U;
    parts.high = 0U;

    point_a.struct_size = (ss_u32)sizeof(point_a);
    point_a.abi_version = SS_V2_ABI_VERSION;
    point_a.x = 0;
    point_a.y = 0;
    point_b = point_a;

    size.struct_size = (ss_u32)sizeof(size);
    size.abi_version = SS_V2_ABI_VERSION;
    size.width = 2U;
    size.height = 2U;

    rect.struct_size = (ss_u32)sizeof(rect);
    rect.abi_version = SS_V2_ABI_VERSION;
    rect.x = 0;
    rect.y = 0;
    rect.width = 1U;
    rect.height = 1U;

    color.struct_size = (ss_u32)sizeof(color);
    color.abi_version = SS_V2_ABI_VERSION;
    color.red = 0U;
    color.green = 0U;
    color.blue = 0U;
    color.alpha = 255U;

    clock.struct_size = (ss_u32)sizeof(clock);
    clock.abi_version = SS_V2_ABI_VERSION;
    clock.frame_index = parts;
    clock.elapsed_ms = parts;
    clock.delta_ms = 100U;
    clock.fixed_step_ms = 100U;

    seed.struct_size = (ss_u32)sizeof(seed);
    seed.abi_version = SS_V2_ABI_VERSION;
    seed.base_seed = 1U;
    seed.stream_seed = 2U;
    seed.deterministic = SS_V2_TRUE;

    message.struct_size = (ss_u32)sizeof(message);
    message.abi_version = SS_V2_ABI_VERSION;
    message.level = SS_V2_DIAG_LEVEL_INFO;
    message.domain = SS_V2_DIAG_DOMAIN_PRODUCT;
    message.code = 0U;
    message.origin = "smoke";
    message.text = "ok";

    diagnostics.struct_size = (ss_u32)sizeof(diagnostics);
    diagnostics.abi_version = SS_V2_ABI_VERSION;
    diagnostics.sink = smoke_sink;
    diagnostics.user_data = 0;
    diagnostics.minimum_level = SS_V2_DIAG_LEVEL_DEBUG;

    config.struct_size = (ss_u32)sizeof(config);
    config.abi_version = SS_V2_ABI_VERSION;
    config.product_schema_id = "smoke";
    config.schema_version = 1U;
    config.bytes = 0;
    config.byte_count = 0U;

    surface.struct_size = (ss_u32)sizeof(surface);
    surface.abi_version = SS_V2_ABI_VERSION;
    surface.width = 2U;
    surface.height = 2U;
    surface.stride_bytes = 8U;
    surface.format = SS_V2_SURFACE_FORMAT_RGBA8;
    surface.origin = SS_V2_SURFACE_ORIGIN_TOP_LEFT;
    surface.pixels = pixels;

    draw_ops.struct_size = (ss_u32)sizeof(draw_ops);
    draw_ops.abi_version = SS_V2_ABI_VERSION;
    draw_ops.capability_flags = 0U;
    draw_ops.clear = 0;
    draw_ops.fill_rect = 0;
    draw_ops.frame_rect = 0;
    draw_ops.line = 0;
    draw_ops.polyline = 0;

    draw_target.struct_size = (ss_u32)sizeof(draw_target);
    draw_target.abi_version = SS_V2_ABI_VERSION;
    draw_target.user_data = 0;
    draw_target.ops = &draw_ops;
    draw_target.surface = &surface;

    session_desc.struct_size = (ss_u32)sizeof(session_desc);
    session_desc.abi_version = SS_V2_ABI_VERSION;
    session_desc.mode = SS_V2_SESSION_MODE_PROOF;
    session_desc.dimensions = size;
    session_desc.seed = seed;
    session_desc.clock = clock;
    session_desc.product_config = config;
    session_desc.diagnostics = &diagnostics;

    resize_desc.struct_size = (ss_u32)sizeof(resize_desc);
    resize_desc.abi_version = SS_V2_ABI_VERSION;
    resize_desc.dimensions = size;
    resize_desc.diagnostics = &diagnostics;

    advance_desc.struct_size = (ss_u32)sizeof(advance_desc);
    advance_desc.abi_version = SS_V2_ABI_VERSION;
    advance_desc.clock = clock;
    advance_desc.diagnostics = &diagnostics;

    render_desc.struct_size = (ss_u32)sizeof(render_desc);
    render_desc.abi_version = SS_V2_ABI_VERSION;
    render_desc.clock = clock;
    render_desc.draw_target = &draw_target;
    render_desc.surface = &surface;
    render_desc.diagnostics = &diagnostics;

    session_ops.struct_size = (ss_u32)sizeof(session_ops);
    session_ops.abi_version = SS_V2_ABI_VERSION;
    session_ops.create = 0;
    session_ops.destroy = 0;
    session_ops.resize = 0;
    session_ops.advance = 0;
    session_ops.render = 0;

    product.struct_size = (ss_u32)sizeof(product);
    product.abi_version = SS_V2_ABI_VERSION;
    product.product_key = "smoke";
    product.display_name = "Smoke";
    product.product_version = "0";
    product.config_schema_id = "smoke";
    product.config_schema_version = 1U;
    product.capability_flags = 0U;
    product.session_ops = &session_ops;

    return (int)(message.code + product.config_schema_version + point_b.struct_size);
}
'''


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

    for path in REQUIRED_HEADERS + sorted(V2_ROOT.rglob("*.h")):
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


def scan_sources(errors: list[str]) -> None:
    for path in REQUIRED_SOURCE_FILES:
        require(path.exists(), f"Missing portable v2 runtime source: {path.relative_to(ROOT)}", errors)
    if errors:
        return

    for path in REQUIRED_SOURCE_FILES:
        text = path.read_text(encoding="utf-8")
        relative = path.relative_to(ROOT)
        for term in FORBIDDEN_NATIVE_TERMS:
            require(term not in text, f"{relative} contains forbidden native term: {term}", errors)
        for term in ["//", "_Static_assert", "static_assert", "stdint.h", "stdbool.h", "inline", "restrict"]:
            require(term not in text, f"{relative} contains a non-C89 source token: {term}", errors)


def c_compiler() -> str | None:
    for candidate in ["gcc", "cc", "clang"]:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def compile_with_c89(source: pathlib.Path, output: pathlib.Path, errors: list[str]) -> None:
    compiler = c_compiler()
    if compiler is None:
        errors.append("No C compiler found for portable v2 C89 smoke compile.")
        return
    output.parent.mkdir(parents=True, exist_ok=True)
    command = [
        compiler,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-c",
        str(source),
        "-o",
        str(output),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(
            f"C89 compile failed for {source.relative_to(ROOT)}: "
            f"{result.stdout.strip()} {result.stderr.strip()}".strip()
        )


def compile_smoke(errors: list[str]) -> None:
    smoke_source = SMOKE_ROOT / "portable_v2_smoke.c"
    smoke_source.parent.mkdir(parents=True, exist_ok=True)
    smoke_source.write_text(SMOKE_SOURCE.strip() + "\n", encoding="utf-8")
    compile_with_c89(smoke_source, SMOKE_ROOT / "portable_v2_smoke.o", errors)
    for source in REQUIRED_SOURCE_FILES:
        compile_with_c89(source, SMOKE_ROOT / (source.stem + ".o"), errors)


def main() -> int:
    errors: list[str] = []
    scan_headers(errors)
    scan_sources(errors)
    if not errors:
        compile_smoke(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Portable v2 checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
