"""Validate the Plasma v2 deterministic runtime buffer contract."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
RUNTIME_HEADER = SRC / "plasma_v2_runtime.h"
RUNTIME_SOURCE = SRC / "plasma_v2_runtime.c"
PLAN_SOURCE = SRC / "plasma_v2_plan.c"
SPEC_SOURCE = SRC / "plasma_v2_spec.c"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "runtime-report.json"
REPORT_MD = REPORT_DIR / "runtime-report.md"
OUT = ROOT / "out" / "checks" / "plasma-v2-runtime"

REQUIRED_FIELDS = [
    "resolved_plan",
    "field_a",
    "field_b",
    "field_history",
    "material_buffer",
    "treatment_buffer",
    "present_buffer",
    "frame_index",
    "elapsed_millis",
    "phase0",
    "phase1",
    "phase2",
    "rng_state",
]

REQUIRED_FUNCTIONS = [
    "plasma_v2_runtime_required_field_cells",
    "plasma_v2_runtime_required_pixel_bytes",
    "plasma_v2_runtime_bind",
    "plasma_v2_runtime_reset",
    "plasma_v2_runtime_advance",
    "plasma_v2_runtime_is_valid",
]

FORBIDDEN_TEXT = [
    "windows.h",
    "HWND",
    "HDC",
    "HINSTANCE",
    "screensave_renderer",
    "renderer_api",
    "malloc",
    "calloc",
    "realloc",
    "free(",
    "fopen",
    "fread",
    "fwrite",
    "sin(",
    "cos(",
    "//",
    "_Static_assert",
    "stdbool.h",
    "stdint.h",
    "unsigned long",
    " long ",
    "DWORD",
]

SMOKE_SOURCE = r'''
#include "plasma_v2_runtime.h"

#define FIELD_CAPACITY ((ss_u32)(64U * 40U))
#define BYTE_CAPACITY ((ss_u32)(64U * 40U * 4U))

static ss_u32 field_a[64U * 40U];
static ss_u32 field_b[64U * 40U];
static ss_u32 field_history[64U * 40U];
static ss_u8 material_buffer[64U * 40U * 4U];
static ss_u8 treatment_buffer[64U * 40U * 4U];
static ss_u8 present_buffer[64U * 40U * 4U];

int main(void)
{
    plasma_v2_spec spec;
    plasma_v2_plan_request request;
    plasma_v2_plan plan;
    plasma_v2_runtime_buffers buffers;
    plasma_v2_runtime runtime;
    ss_u32 cells;
    ss_u32 bytes;
    ss_u32 initial_rng;
    ss_u32 initial_phase;

    plasma_v2_spec_set_defaults(&spec);
    request.struct_size = (ss_u32)sizeof(request);
    request.requested_spec = &spec;
    request.drawable_size.width = (ss_u32)64U;
    request.drawable_size.height = (ss_u32)40U;
    request.requested_renderer = PLASMA_V2_RENDERER_GDI;
    request.capability_flags = PLASMA_V2_CAP_GDI | PLASMA_V2_CAP_FEEDBACK_BUFFER;
    request.base_seed = (ss_u32)4096U;
    request.stream_seed = (ss_u32)17U;
    request.allow_experimental = SS_V2_FALSE;

    if (plasma_v2_plan_compile(&request, &plan) != SS_V2_STATUS_OK) {
        return 1;
    }
    if (plasma_v2_runtime_required_field_cells(&plan, &cells) != SS_V2_STATUS_OK) {
        return 2;
    }
    if (plasma_v2_runtime_required_pixel_bytes(&plan, &bytes) != SS_V2_STATUS_OK) {
        return 3;
    }
    if (cells != FIELD_CAPACITY || bytes != BYTE_CAPACITY) {
        return 4;
    }

    field_a[0] = (ss_u32)9U;
    present_buffer[0] = (ss_u8)9U;
    buffers.struct_size = (ss_u32)sizeof(buffers);
    buffers.field_a = field_a;
    buffers.field_b = field_b;
    buffers.field_history = field_history;
    buffers.material_buffer = material_buffer;
    buffers.treatment_buffer = treatment_buffer;
    buffers.present_buffer = present_buffer;
    buffers.field_cell_count = FIELD_CAPACITY;
    buffers.material_byte_count = BYTE_CAPACITY;
    buffers.treatment_byte_count = BYTE_CAPACITY;
    buffers.present_byte_count = BYTE_CAPACITY;

    if (plasma_v2_runtime_bind(&plan, &buffers, &runtime) != SS_V2_STATUS_OK) {
        return 5;
    }
    if (plasma_v2_runtime_is_valid(&runtime) != SS_V2_TRUE) {
        return 6;
    }
    if (field_a[0] != 0U || present_buffer[0] != 0U) {
        return 7;
    }
    initial_rng = runtime.rng_state;
    initial_phase = runtime.phase0;
    if (plasma_v2_runtime_advance(&runtime, (ss_u32)16U) != SS_V2_STATUS_OK) {
        return 8;
    }
    if (runtime.frame_index != (ss_u32)1U || runtime.elapsed_millis != (ss_u32)16U) {
        return 9;
    }
    if (runtime.rng_state == initial_rng || runtime.phase0 == initial_phase) {
        return 10;
    }
    if (runtime.resolved_plan.use_software_reference != SS_V2_TRUE) {
        return 11;
    }

    return 0;
}
'''


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def compiler() -> str | None:
    for candidate in ("gcc", "clang", "cc"):
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def scan_sources(errors: list[str]) -> None:
    for path in (RUNTIME_HEADER, RUNTIME_SOURCE, PLAN_SOURCE, SPEC_SOURCE):
        require(path.exists(), f"Missing Plasma v2 runtime input: {repo_path(path)}", errors)
    if errors:
        return

    header = RUNTIME_HEADER.read_text(encoding="utf-8")
    source = RUNTIME_SOURCE.read_text(encoding="utf-8")
    combined = header + "\n" + source
    for field in REQUIRED_FIELDS:
        require(field in header, f"plasma_v2_runtime.h missing runtime field {field!r}", errors)
    for function in REQUIRED_FUNCTIONS:
        require(function in combined, f"Plasma v2 runtime missing function {function!r}", errors)
    for path in (RUNTIME_HEADER, RUNTIME_SOURCE):
        text = path.read_text(encoding="utf-8")
        for forbidden in FORBIDDEN_TEXT:
            require(forbidden not in text, f"{repo_path(path)} contains forbidden runtime token {forbidden!r}", errors)


def compile_smoke(errors: list[str]) -> None:
    cc = compiler()
    if cc is None:
        errors.append("No C compiler found for Plasma v2 runtime C89 smoke compile.")
        return

    OUT.mkdir(parents=True, exist_ok=True)
    smoke = OUT / "plasma_v2_runtime_smoke.c"
    exe = OUT / "plasma_v2_runtime_smoke.exe"
    smoke.write_text(SMOKE_SOURCE.strip() + "\n", encoding="utf-8")
    command = [
        cc,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-I",
        str(SRC),
        str(smoke),
        str(RUNTIME_SOURCE),
        str(PLAN_SOURCE),
        str(SPEC_SOURCE),
        "-o",
        str(exe),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append("Plasma v2 runtime C89 compile failed: " + (result.stdout + result.stderr).strip())
        return
    run = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run.returncode != 0:
        errors.append("Plasma v2 runtime smoke failed with returncode " + str(run.returncode))


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def build_report(errors: list[str]) -> dict[str, Any]:
    return {
        "schema": "screensave.plasma-v2.runtime-report.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "required_state": REQUIRED_FIELDS,
        "validation_errors": errors,
        "claim_boundary": "Runtime buffer proof only; this does not prove field quality, renderer output, artistic acceptance, or stable promotion.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Runtime Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Required State",
        "",
    ]
    for item in report.get("required_state", []):
        lines.append(f"- {item}")
    lines.extend(["", "## Errors", ""])
    errors = report.get("validation_errors", [])
    if errors:
        for error in errors:
            lines.append(f"- {error}")
    else:
        lines.append("- none")
    lines.append("")
    return "\n".join(lines)


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    REPORT_MD.write_text(report_markdown(report), encoding="utf-8")


def main() -> int:
    errors: list[str] = []
    scan_sources(errors)
    if not errors:
        compile_smoke(errors)
    report = build_report(errors)
    write_report(report)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 runtime checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
