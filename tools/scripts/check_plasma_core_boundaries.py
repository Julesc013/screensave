"""Validate Plasma v2 portable core boundaries and smoke execution."""

from __future__ import annotations

import pathlib
import re
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "products" / "savers" / "plasma" / "src"
V2_SRC_ROOT = ROOT / "platform" / "src" / "v2"
SMOKE_ROOT = ROOT / "out" / "checks" / "plasma-core-v2"

CORE_FILES = [
    SRC_ROOT / "plasma_v2_core.h",
    SRC_ROOT / "plasma_v2_core.c",
]

ADAPTER_FILES = [
    SRC_ROOT / "plasma_v2_adapter.h",
    SRC_ROOT / "plasma_v2_adapter.c",
]

REALIZATION_FILES = [
    SRC_ROOT / "plasma_v2_realization.h",
    SRC_ROOT / "plasma_v2_realization.c",
    SRC_ROOT / "plasma_v2_gl11.h",
    SRC_ROOT / "plasma_v2_gl11.c",
]

REQUIRED_SOURCES = [
    SRC_ROOT / "plasma_spec_v2.c",
    SRC_ROOT / "plasma_migration_v2.c",
    SRC_ROOT / "plasma_v2_core.c",
    SRC_ROOT / "plasma_v2_realization.c",
    SRC_ROOT / "plasma_v2_gl11.c",
    SRC_ROOT / "plasma_v2_adapter.c",
    V2_SRC_ROOT / "base_validate.c",
    V2_SRC_ROOT / "config_view.c",
    V2_SRC_ROOT / "surface_view.c",
    V2_SRC_ROOT / "draw_target.c",
    V2_SRC_ROOT / "session_helpers.c",
]

ALLOWED_CORE_INCLUDES = {
    "plasma_v2_core.h",
    "plasma_spec_v2.h",
    "screensave/v2.h",
    "stdlib.h",
    "string.h",
}

FORBIDDEN_CORE_PATTERNS = [
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
    (re.compile(r"\bAIDE\b"), "AIDE dependency"),
    (re.compile(r"\bWorkbench\b"), "Workbench dependency"),
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

INCLUDE_RE = re.compile(r"^\s*#\s*include\s+[<\"]([^>\"]+)[>\"]")

SMOKE_SOURCE = r'''
#include "plasma_v2_adapter.h"
#include "plasma_spec_v2.h"

static void fill_size(ss_v2_size *size, ss_u32 width, ss_u32 height)
{
    size->struct_size = (ss_u32)sizeof(*size);
    size->abi_version = SS_V2_ABI_VERSION;
    size->width = width;
    size->height = height;
}

static void fill_parts(ss_v2_u64_parts *parts, ss_u32 low)
{
    parts->struct_size = (ss_u32)sizeof(*parts);
    parts->abi_version = SS_V2_ABI_VERSION;
    parts->low = low;
    parts->high = 0U;
}

static void fill_clock(ss_v2_clock *clock, ss_u32 frame, ss_u32 delta_ms)
{
    clock->struct_size = (ss_u32)sizeof(*clock);
    clock->abi_version = SS_V2_ABI_VERSION;
    fill_parts(&clock->frame_index, frame);
    fill_parts(&clock->elapsed_ms, frame * delta_ms);
    clock->delta_ms = delta_ms;
    clock->fixed_step_ms = delta_ms;
}

int main(void)
{
    const ss_v2_product_descriptor *product;
    ss_v2_session *session;
    ss_v2_session_desc session_desc;
    ss_v2_advance_desc advance_desc;
    ss_v2_render_desc render_desc;
    ss_v2_surface_desc surface;
    ss_v2_config_view config;
    ss_v2_seed seed;
    ss_v2_clock clock;
    ss_v2_size size;
    plasma_spec_v2 spec;
    ss_u8 pixels[8U * 6U * 4U];
    ss_u32 index;
    ss_u32 sum;

    plasma_spec_v2_set_defaults(&spec);
    spec.struct_size = (ss_u32)sizeof(spec);
    spec.output_style = PLASMA_V2_OUTPUT_BANDED;

    product = plasma_v2_product_descriptor();
    if (ss_v2_product_descriptor_is_valid(product) != SS_V2_STATUS_OK) {
        return 1;
    }

    fill_size(&size, 8U, 6U);
    fill_clock(&clock, 0U, 100U);

    seed.struct_size = (ss_u32)sizeof(seed);
    seed.abi_version = SS_V2_ABI_VERSION;
    seed.base_seed = 4096U;
    seed.stream_seed = 17U;
    seed.deterministic = SS_V2_TRUE;

    config.struct_size = (ss_u32)sizeof(config);
    config.abi_version = SS_V2_ABI_VERSION;
    config.product_schema_id = PLASMA_SPEC_V2_SCHEMA_ID;
    config.schema_version = PLASMA_SPEC_V2_SCHEMA_VERSION;
    config.bytes = &spec;
    config.byte_count = (ss_u32)sizeof(spec);

    session_desc.struct_size = (ss_u32)sizeof(session_desc);
    session_desc.abi_version = SS_V2_ABI_VERSION;
    session_desc.mode = SS_V2_SESSION_MODE_PROOF;
    session_desc.dimensions = size;
    session_desc.seed = seed;
    session_desc.clock = clock;
    session_desc.product_config = config;
    session_desc.diagnostics = 0;

    session = 0;
    if (product->session_ops->create(&session_desc, &session) != SS_V2_STATUS_OK || session == 0) {
        return 2;
    }

    advance_desc.struct_size = (ss_u32)sizeof(advance_desc);
    advance_desc.abi_version = SS_V2_ABI_VERSION;
    fill_clock(&advance_desc.clock, 1U, 100U);
    advance_desc.diagnostics = 0;
    if (product->session_ops->advance(session, &advance_desc) != SS_V2_STATUS_OK) {
        product->session_ops->destroy(session);
        return 3;
    }

    for (index = 0U; index < (ss_u32)sizeof(pixels); ++index) {
        pixels[index] = 0U;
    }
    surface.struct_size = (ss_u32)sizeof(surface);
    surface.abi_version = SS_V2_ABI_VERSION;
    surface.width = 8U;
    surface.height = 6U;
    surface.stride_bytes = 8U * 4U;
    surface.format = SS_V2_SURFACE_FORMAT_RGBA8;
    surface.origin = SS_V2_SURFACE_ORIGIN_TOP_LEFT;
    surface.pixels = pixels;

    render_desc.struct_size = (ss_u32)sizeof(render_desc);
    render_desc.abi_version = SS_V2_ABI_VERSION;
    fill_clock(&render_desc.clock, 1U, 100U);
    render_desc.draw_target = 0;
    render_desc.surface = &surface;
    render_desc.diagnostics = 0;
    if (product->session_ops->render(session, &render_desc) != SS_V2_STATUS_OK) {
        product->session_ops->destroy(session);
        return 4;
    }

    sum = 0U;
    for (index = 0U; index < (ss_u32)sizeof(pixels); ++index) {
        sum += pixels[index];
    }
    product->session_ops->destroy(session);
    return sum > 0U ? 0 : 5;
}
'''


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def check_core_file(path: pathlib.Path, errors: list[str]) -> None:
    if not path.exists():
        errors.append(f"Missing Plasma v2 core file: {repo_path(path)}")
        return

    text = path.read_text(encoding="utf-8")
    for line_number, line in enumerate(text.splitlines(), start=1):
        include_match = INCLUDE_RE.match(line)
        if include_match:
            include_name = include_match.group(1)
            if include_name not in ALLOWED_CORE_INCLUDES:
                errors.append(
                    f"{repo_path(path)}:{line_number}: Plasma core includes disallowed dependency {include_name!r}"
                )
        for pattern, label in FORBIDDEN_CORE_PATTERNS:
            if pattern.search(line):
                errors.append(f"{repo_path(path)}:{line_number}: Plasma core contains forbidden {label}")
        for term in C89_FORBIDDEN:
            if term in line:
                errors.append(f"{repo_path(path)}:{line_number}: Plasma core contains non-C89 token {term!r}")


def scan_files(errors: list[str]) -> None:
    for path in CORE_FILES:
        check_core_file(path, errors)
    for path in ADAPTER_FILES:
        require(path.exists(), f"Missing Plasma v2 adapter file: {repo_path(path)}", errors)
    for path in REALIZATION_FILES:
        require(path.exists(), f"Missing Plasma v2 realization file: {repo_path(path)}", errors)
    if ADAPTER_FILES[1].exists():
        adapter_text = ADAPTER_FILES[1].read_text(encoding="utf-8")
        for needle in [
            "plasma_v2_product_descriptor",
            "PLASMA_SPEC_V2_SCHEMA_ID",
            "SS_V2_PRODUCT_CAP_REFERENCE_CPU",
            "SS_V2_PRODUCT_CAP_RGBA8_SURFACE",
        ]:
            require(needle in adapter_text, f"plasma_v2_adapter.c missing {needle!r}", errors)


def c_compiler() -> str | None:
    for candidate in ["gcc", "cc", "clang"]:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def compile_and_run(errors: list[str]) -> None:
    compiler = c_compiler()
    if compiler is None:
        errors.append("No C compiler found for Plasma v2 core C89 smoke compile.")
        return

    for source in REQUIRED_SOURCES:
        require(source.exists(), f"Missing source for Plasma v2 core smoke: {repo_path(source)}", errors)
    if errors:
        return

    SMOKE_ROOT.mkdir(parents=True, exist_ok=True)
    smoke_source = SMOKE_ROOT / "plasma_core_v2_smoke.c"
    output = SMOKE_ROOT / "plasma_core_v2_smoke.exe"
    smoke_source.write_text(SMOKE_SOURCE.strip() + "\n", encoding="utf-8")

    command = [
        compiler,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-I",
        str(SRC_ROOT),
        str(smoke_source),
    ]
    command.extend(str(source) for source in REQUIRED_SOURCES)
    command.extend(["-o", str(output)])
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(
            "Plasma v2 core C89 compile failed: "
            + (result.stdout.strip() + " " + result.stderr.strip()).strip()
        )
        return

    run_result = subprocess.run([str(output)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run_result.returncode != 0:
        errors.append(
            "Plasma v2 core smoke program failed: "
            + (run_result.stdout.strip() + " " + run_result.stderr.strip()).strip()
        )


def main() -> int:
    errors: list[str] = []
    scan_files(errors)
    if not errors:
        compile_and_run(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 core boundary checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
