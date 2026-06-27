"""Validate the direct Plasma v2 field-to-presentation pipeline stages."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
FIELD = SRC / "field"
OUTPUT = SRC / "output"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "field-pipeline-report.json"
REPORT_MD = REPORT_DIR / "field-pipeline-report.md"
OUT = ROOT / "out" / "checks" / "plasma-v2-field-pipeline"

REQUIRED_FILES = [
    SRC / "plasma_v2_types.h",
    SRC / "plasma_v2_spec.h",
    SRC / "plasma_v2_spec.c",
    SRC / "plasma_v2_plan.h",
    SRC / "plasma_v2_plan.c",
    SRC / "plasma_v2_runtime.h",
    SRC / "plasma_v2_runtime.c",
    FIELD / "plasma_v2_field.h",
    FIELD / "plasma_v2_field.c",
    FIELD / "plasma_v2_sources.h",
    FIELD / "plasma_v2_sources.c",
    FIELD / "plasma_v2_generators.h",
    FIELD / "plasma_v2_generators.c",
    FIELD / "plasma_v2_modifiers.h",
    FIELD / "plasma_v2_modifiers.c",
    FIELD / "plasma_v2_feedback.h",
    FIELD / "plasma_v2_feedback.c",
    OUTPUT / "plasma_v2_output.h",
    OUTPUT / "plasma_v2_output.c",
    OUTPUT / "plasma_v2_material.h",
    OUTPUT / "plasma_v2_material.c",
    OUTPUT / "plasma_v2_treatment.h",
    OUTPUT / "plasma_v2_treatment.c",
    OUTPUT / "plasma_v2_present.h",
    OUTPUT / "plasma_v2_present.c",
]

REQUIRED_TOKENS = [
    "plasma_v2_source_wave",
    "plasma_v2_source_radial",
    "plasma_v2_source_noise",
    "plasma_v2_generate_classic",
    "plasma_v2_generate_interference",
    "plasma_v2_generate_fire",
    "plasma_v2_apply_domain_warp",
    "plasma_v2_feedback_apply",
    "plasma_v2_output_transform_field",
    "plasma_v2_material_map",
    "plasma_v2_treatment_apply",
    "plasma_v2_present_flat",
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
    "tan(",
    "pow(",
    "sqrt(",
    "//",
    "_Static_assert",
    "stdbool.h",
    "stdint.h",
    "unsigned long",
    " long ",
    "DWORD",
]

STAGE_NAMES = [
    "providers / analytic sources",
    "field generators",
    "field modifiers",
    "output transform",
    "material mapping",
    "treatment stack",
    "presentation",
]

SMOKE_SOURCE = r'''
#include "plasma_v2_runtime.h"
#include "field/plasma_v2_field.h"
#include "output/plasma_v2_output.h"
#include "output/plasma_v2_material.h"
#include "output/plasma_v2_treatment.h"
#include "output/plasma_v2_present.h"

#define FIELD_CAPACITY ((ss_u32)(32U * 24U))
#define BYTE_CAPACITY ((ss_u32)(32U * 24U * 4U))

static ss_u32 field_a[32U * 24U];
static ss_u32 field_b[32U * 24U];
static ss_u32 field_history[32U * 24U];
static ss_u8 material_buffer[32U * 24U * 4U];
static ss_u8 treatment_buffer[32U * 24U * 4U];
static ss_u8 present_buffer[32U * 24U * 4U];

static ss_u32 render_frame(ss_u32 family, ss_u32 *checksum_out)
{
    plasma_v2_spec spec;
    plasma_v2_plan_request request;
    plasma_v2_plan plan;
    plasma_v2_runtime_buffers buffers;
    plasma_v2_runtime runtime;
    ss_u32 index;
    ss_u32 checksum;

    plasma_v2_spec_set_defaults(&spec);
    spec.field_family = family;
    spec.output_kind = PLASMA_V2_OUTPUT_CONTINUOUS;
    spec.treatment_kind = PLASMA_V2_TREATMENT_SOFT;
    spec.warp = (ss_u32)260U;
    spec.feedback = (ss_u32)180U;

    request.struct_size = (ss_u32)sizeof(request);
    request.requested_spec = &spec;
    request.drawable_size.width = (ss_u32)32U;
    request.drawable_size.height = (ss_u32)24U;
    request.requested_renderer = PLASMA_V2_RENDERER_GDI;
    request.capability_flags = PLASMA_V2_CAP_GDI | PLASMA_V2_CAP_FEEDBACK_BUFFER;
    request.base_seed = (ss_u32)4096U;
    request.stream_seed = (ss_u32)17U;
    request.allow_experimental = SS_V2_FALSE;
    if (plasma_v2_plan_compile(&request, &plan) != SS_V2_STATUS_OK) {
        return 1U;
    }

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
        return 2U;
    }
    if (plasma_v2_runtime_advance(&runtime, (ss_u32)16U) != SS_V2_STATUS_OK) {
        return 3U;
    }
    if (plasma_v2_field_step(&runtime) != SS_V2_STATUS_OK) {
        return 4U;
    }
    if (plasma_v2_output_transform_field(&runtime) != SS_V2_STATUS_OK) {
        return 5U;
    }
    if (plasma_v2_material_map(&runtime) != SS_V2_STATUS_OK) {
        return 6U;
    }
    if (plasma_v2_treatment_apply(&runtime) != SS_V2_STATUS_OK) {
        return 7U;
    }
    if (plasma_v2_present_flat(&runtime) != SS_V2_STATUS_OK) {
        return 8U;
    }

    checksum = 0U;
    for (index = 0U; index < BYTE_CAPACITY; ++index) {
        checksum = (checksum * 33U) ^ (ss_u32)present_buffer[index];
    }
    *checksum_out = checksum;
    return checksum == 0U ? 9U : 0U;
}

int main(void)
{
    ss_u32 classic;
    ss_u32 interference;
    ss_u32 fire;

    if (render_frame(PLASMA_V2_FIELD_CLASSIC, &classic) != 0U) {
        return 1;
    }
    if (render_frame(PLASMA_V2_FIELD_INTERFERENCE, &interference) != 0U) {
        return 2;
    }
    if (render_frame(PLASMA_V2_FIELD_FIRE, &fire) != 0U) {
        return 3;
    }
    if (classic == interference || classic == fire || interference == fire) {
        return 4;
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


def direct_sources() -> list[pathlib.Path]:
    return [
        SRC / "plasma_v2_spec.c",
        SRC / "plasma_v2_plan.c",
        SRC / "plasma_v2_runtime.c",
        FIELD / "plasma_v2_sources.c",
        FIELD / "plasma_v2_generators.c",
        FIELD / "plasma_v2_feedback.c",
        FIELD / "plasma_v2_modifiers.c",
        FIELD / "plasma_v2_field.c",
        OUTPUT / "plasma_v2_output.c",
        OUTPUT / "plasma_v2_material.c",
        OUTPUT / "plasma_v2_treatment.c",
        OUTPUT / "plasma_v2_present.c",
    ]


def scan_sources(errors: list[str]) -> None:
    for path in REQUIRED_FILES:
        require(path.exists(), f"Missing Plasma v2 pipeline input: {repo_path(path)}", errors)
    if errors:
        return

    combined = "\n".join(path.read_text(encoding="utf-8") for path in REQUIRED_FILES)
    for token in REQUIRED_TOKENS:
        require(token in combined, f"Plasma v2 field pipeline missing {token!r}", errors)
    for path in REQUIRED_FILES:
        text = path.read_text(encoding="utf-8")
        for forbidden in FORBIDDEN_TEXT:
            require(forbidden not in text, f"{repo_path(path)} contains forbidden field-pipeline token {forbidden!r}", errors)


def compile_smoke(errors: list[str]) -> None:
    cc = compiler()
    if cc is None:
        errors.append("No C compiler found for Plasma v2 field pipeline C89 smoke compile.")
        return

    OUT.mkdir(parents=True, exist_ok=True)
    smoke = OUT / "plasma_v2_field_pipeline_smoke.c"
    exe = OUT / "plasma_v2_field_pipeline_smoke.exe"
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
    ]
    command.extend(str(path) for path in direct_sources())
    command.extend(["-o", str(exe)])
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append("Plasma v2 field pipeline C89 compile failed: " + (result.stdout + result.stderr).strip())
        return
    run = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run.returncode != 0:
        errors.append("Plasma v2 field pipeline smoke failed with returncode " + str(run.returncode))


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def build_report(errors: list[str]) -> dict[str, Any]:
    return {
        "schema": "screensave.plasma-v2.field-pipeline-report.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "stable_scope": {
            "field_families": ["classic", "interference", "fire"],
            "sources": ["sine/wave", "radial", "deterministic noise"],
            "modifiers": ["domain warp", "feedback"],
            "output": ["continuous raster"],
            "presentation": ["flat bitmap"],
        },
        "pipeline_stages": STAGE_NAMES,
        "validation_errors": errors,
        "claim_boundary": "Field-pipeline proof only; this does not prove final artistic acceptance, legacy migration, Workbench inspection, or stable promotion.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Field Pipeline Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Pipeline Stages",
        "",
    ]
    for item in report.get("pipeline_stages", []):
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
    print("Plasma v2 field pipeline checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
