"""Prove direct Plasma v2 Basic-control influence for the instrument audit."""

from __future__ import annotations

import hashlib
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
REPORT_JSON = REPORT_DIR / "control-influence.json"
REPORT_MD = REPORT_DIR / "control-influence.md"
OUT = ROOT / "out" / "checks" / "plasma-v2-direct-influence"

BASIC_CONTROLS = [
    ("Field", "field_family_fire"),
    ("Scale", "scale_900"),
    ("Complexity", "complexity_900"),
    ("Motion", "motion_pulse"),
    ("Speed", "speed_850"),
    ("Warp", "warp_800"),
    ("Feedback", "feedback_800"),
    ("Material", "material_aurora_cool"),
    ("Contrast", "contrast_900"),
    ("Brightness", "brightness_350"),
    ("Softness", "softness_900"),
    ("Treatment", "treatment_phosphor"),
    ("Seed", "seed_777"),
]

DIRECT_SOURCES = [
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

SMOKE_SOURCE = r'''
#include <stdio.h>

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

static ss_u32 token_equals(const char *left, const char *right)
{
    if (left == 0 || right == 0) {
        return SS_V2_FALSE;
    }
    while (*left != '\0' && *right != '\0') {
        if (*left != *right) {
            return SS_V2_FALSE;
        }
        ++left;
        ++right;
    }
    return *left == *right ? SS_V2_TRUE : SS_V2_FALSE;
}

static ss_u32 apply_variant(plasma_v2_spec *spec, const char *variant)
{
    if (token_equals(variant, "baseline") == SS_V2_TRUE) {
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "field_family_fire") == SS_V2_TRUE) {
        spec->field_family = PLASMA_V2_FIELD_FIRE;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "scale_900") == SS_V2_TRUE) {
        spec->scale = (ss_u32)900U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "complexity_900") == SS_V2_TRUE) {
        spec->complexity = (ss_u32)900U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "motion_pulse") == SS_V2_TRUE) {
        spec->motion_kind = PLASMA_V2_MOTION_PULSE;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "speed_850") == SS_V2_TRUE) {
        spec->speed = (ss_u32)850U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "warp_800") == SS_V2_TRUE) {
        spec->warp = (ss_u32)800U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "feedback_800") == SS_V2_TRUE) {
        spec->feedback = (ss_u32)800U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "material_aurora_cool") == SS_V2_TRUE) {
        return plasma_v2_spec_set_material_key(spec, "aurora_cool");
    }
    if (token_equals(variant, "contrast_900") == SS_V2_TRUE) {
        spec->contrast = (ss_u32)900U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "brightness_350") == SS_V2_TRUE) {
        spec->brightness = (ss_u32)350U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "softness_900") == SS_V2_TRUE) {
        spec->softness = (ss_u32)900U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "treatment_phosphor") == SS_V2_TRUE) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_PHOSPHOR;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "seed_777") == SS_V2_TRUE) {
        spec->seed = (ss_u32)777U;
        return SS_V2_STATUS_OK;
    }
    return SS_V2_STATUS_UNSUPPORTED;
}

static ss_u32 render_checksum(const char *variant, ss_u32 *checksum_out)
{
    plasma_v2_spec spec;
    plasma_v2_plan_request request;
    plasma_v2_plan plan;
    plasma_v2_runtime_buffers buffers;
    plasma_v2_runtime runtime;
    ss_u32 index;
    ss_u32 checksum;

    plasma_v2_spec_set_defaults(&spec);
    if (apply_variant(&spec, variant) != SS_V2_STATUS_OK) {
        return 1U;
    }
    plasma_v2_spec_clamp(&spec);
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
        return 2U;
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
        return 3U;
    }
    if (plasma_v2_runtime_advance(&runtime, (ss_u32)16U) != SS_V2_STATUS_OK) {
        return 4U;
    }
    if (plasma_v2_field_step(&runtime) != SS_V2_STATUS_OK) {
        return 5U;
    }
    if (plasma_v2_output_transform_field(&runtime) != SS_V2_STATUS_OK) {
        return 6U;
    }
    if (plasma_v2_material_map(&runtime) != SS_V2_STATUS_OK) {
        return 7U;
    }
    if (plasma_v2_treatment_apply(&runtime) != SS_V2_STATUS_OK) {
        return 8U;
    }
    if (plasma_v2_present_flat(&runtime) != SS_V2_STATUS_OK) {
        return 9U;
    }

    checksum = 0U;
    for (index = 0U; index < BYTE_CAPACITY; ++index) {
        checksum = (checksum * 33U) ^ (ss_u32)present_buffer[index];
    }
    *checksum_out = checksum;
    return checksum == 0U ? 10U : 0U;
}

int main(int argc, char **argv)
{
    ss_u32 checksum;
    ss_u32 status;

    if (argc != 2) {
        return 1;
    }
    status = render_checksum(argv[1], &checksum);
    if (status != 0U) {
        return (int)status;
    }
    printf("%u\n", checksum);
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


def compile_tool(errors: list[str]) -> pathlib.Path | None:
    cc = compiler()
    if cc is None:
        errors.append("No C compiler found for Plasma v2 direct influence compile.")
        return None
    for source in DIRECT_SOURCES:
        require(source.exists(), f"Missing source for direct influence capture: {repo_path(source)}", errors)
    if errors:
        return None

    OUT.mkdir(parents=True, exist_ok=True)
    smoke = OUT / "plasma_v2_direct_influence.c"
    exe = OUT / "plasma_v2_direct_influence.exe"
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
    command.extend(str(source) for source in DIRECT_SOURCES)
    command.extend(["-o", str(exe)])
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append("Plasma v2 direct influence compile failed: " + (result.stdout + result.stderr).strip())
        return None
    return exe


def checksum_for(tool: pathlib.Path, variant: str, errors: list[str]) -> str:
    result = subprocess.run([str(tool), variant], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(f"Direct influence capture failed for {variant}: returncode {result.returncode}")
        return ""
    text = result.stdout.strip()
    if not text:
        errors.append(f"Direct influence capture produced no checksum for {variant}.")
        return ""
    return text


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def build_report(checksums: dict[str, str], errors: list[str]) -> dict[str, Any]:
    baseline = checksums.get("baseline", "")
    controls = []
    for control, variant in BASIC_CONTROLS:
        variant_checksum = checksums.get(variant, "")
        status = "render-visible" if variant_checksum and variant_checksum != baseline else "fail"
        controls.append(
            {
                "control": control,
                "classification": status,
                "variant": variant,
                "baseline_checksum": baseline,
                "variant_checksum": variant_checksum,
                "proof_hash": hashlib.sha256(f"{control}:{baseline}:{variant_checksum}".encode("utf-8")).hexdigest()
                if variant_checksum
                else "",
            }
        )
        if status != "render-visible":
            errors.append(f"Basic control {control} did not change direct v2 render output.")
    return {
        "schema": "screensave.plasma-v2.instrument-control-influence.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "surface": {"width": 32, "height": 24, "format": "rgba8"},
        "baseline_variant": "baseline",
        "baseline_checksum": baseline,
        "allowed_classifications": [
            "plan-visible",
            "render-visible",
            "intentionally structural",
            "unsupported and hidden",
        ],
        "controls": controls,
        "validation_errors": errors,
        "claim_boundary": "Direct Basic-control influence for the PAW-I-R1 instrument audit only; this does not prove final artistic acceptance or stable promotion.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Direct-Control Influence",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Controls",
        "",
    ]
    for item in report.get("controls", []):
        lines.append(f"- {item.get('classification')}: {item.get('control')} ({item.get('variant')})")
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
    checksums: dict[str, str] = {}
    tool = compile_tool(errors)
    if tool is not None:
        checksums["baseline"] = checksum_for(tool, "baseline", errors)
        for _control, variant in BASIC_CONTROLS:
            checksums[variant] = checksum_for(tool, variant, errors)
    report = build_report(checksums, errors)
    write_report(report)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 direct-control influence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
