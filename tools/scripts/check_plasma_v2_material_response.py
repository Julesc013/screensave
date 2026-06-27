"""Validate direct Plasma v2 material response and treatment distinctness."""

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
REPORT_JSON = REPORT_DIR / "material-response.json"
REPORT_MD = REPORT_DIR / "material-response.md"
OUT = ROOT / "out" / "checks" / "plasma-v2-material-response"

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

VARIANTS = [
    "material_plasma_lava",
    "material_aurora_cool",
    "material_oceanic_blue",
    "material_museum_phosphor",
    "material_quiet_darkroom",
    "contrast_low",
    "contrast_high",
    "brightness_low",
    "brightness_high",
    "softness_low",
    "softness_high",
    "treatment_none",
    "treatment_soft",
    "treatment_phosphor",
]

MATERIAL_VARIANTS = [
    "material_plasma_lava",
    "material_aurora_cool",
    "material_oceanic_blue",
    "material_museum_phosphor",
    "material_quiet_darkroom",
]

SMOKE_SOURCE = r'''
#include <stdio.h>

#include "plasma_v2_runtime.h"
#include "field/plasma_v2_field.h"
#include "output/plasma_v2_output.h"
#include "output/plasma_v2_material.h"
#include "output/plasma_v2_treatment.h"
#include "output/plasma_v2_present.h"

#define FIELD_CAPACITY ((ss_u32)(40U * 24U))
#define BYTE_CAPACITY ((ss_u32)(40U * 24U * 4U))

static ss_u32 field_a[40U * 24U];
static ss_u32 field_b[40U * 24U];
static ss_u32 field_history[40U * 24U];
static ss_u8 material_buffer[40U * 24U * 4U];
static ss_u8 treatment_buffer[40U * 24U * 4U];
static ss_u8 present_buffer[40U * 24U * 4U];

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
    spec->field_family = PLASMA_V2_FIELD_INTERFERENCE;
    spec->warp = (ss_u32)320U;
    spec->feedback = (ss_u32)160U;
    if (token_equals(variant, "material_plasma_lava") == SS_V2_TRUE) {
        return plasma_v2_spec_set_material_key(spec, "plasma_lava");
    }
    if (token_equals(variant, "material_aurora_cool") == SS_V2_TRUE) {
        return plasma_v2_spec_set_material_key(spec, "aurora_cool");
    }
    if (token_equals(variant, "material_oceanic_blue") == SS_V2_TRUE) {
        return plasma_v2_spec_set_material_key(spec, "oceanic_blue");
    }
    if (token_equals(variant, "material_museum_phosphor") == SS_V2_TRUE) {
        return plasma_v2_spec_set_material_key(spec, "museum_phosphor");
    }
    if (token_equals(variant, "material_quiet_darkroom") == SS_V2_TRUE) {
        spec->brightness = (ss_u32)320U;
        return plasma_v2_spec_set_material_key(spec, "quiet_darkroom");
    }
    if (token_equals(variant, "contrast_low") == SS_V2_TRUE) {
        spec->contrast = (ss_u32)250U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "contrast_high") == SS_V2_TRUE) {
        spec->contrast = (ss_u32)950U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "brightness_low") == SS_V2_TRUE) {
        spec->brightness = (ss_u32)260U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "brightness_high") == SS_V2_TRUE) {
        spec->brightness = (ss_u32)760U;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "softness_low") == SS_V2_TRUE) {
        spec->softness = (ss_u32)0U;
        spec->treatment_kind = PLASMA_V2_TREATMENT_SOFT;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "softness_high") == SS_V2_TRUE) {
        spec->softness = (ss_u32)900U;
        spec->treatment_kind = PLASMA_V2_TREATMENT_SOFT;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "treatment_none") == SS_V2_TRUE) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_NONE;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "treatment_soft") == SS_V2_TRUE) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_SOFT;
        return SS_V2_STATUS_OK;
    }
    if (token_equals(variant, "treatment_phosphor") == SS_V2_TRUE) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_PHOSPHOR;
        return SS_V2_STATUS_OK;
    }
    return SS_V2_STATUS_UNSUPPORTED;
}

static ss_u32 luma(ss_u8 red, ss_u8 green, ss_u8 blue)
{
    return (((ss_u32)red * 299U) + ((ss_u32)green * 587U) + ((ss_u32)blue * 114U)) / 1000U;
}

static ss_u32 render_stats(const char *variant, ss_u32 *checksum_out, ss_u32 *min_out, ss_u32 *max_out, ss_u32 *mean_milli_out)
{
    plasma_v2_spec spec;
    plasma_v2_plan_request request;
    plasma_v2_plan plan;
    plasma_v2_runtime_buffers buffers;
    plasma_v2_runtime runtime;
    ss_u32 index;
    ss_u32 checksum;
    ss_u32 value;
    ss_u32 minimum;
    ss_u32 maximum;
    ss_u32 sum;
    ss_u32 pixel_count;
    ss_u32 offset;

    plasma_v2_spec_set_defaults(&spec);
    if (apply_variant(&spec, variant) != SS_V2_STATUS_OK) {
        return 1U;
    }
    plasma_v2_spec_clamp(&spec);
    request.struct_size = (ss_u32)sizeof(request);
    request.requested_spec = &spec;
    request.drawable_size.width = (ss_u32)40U;
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
    minimum = 255U;
    maximum = 0U;
    sum = 0U;
    pixel_count = FIELD_CAPACITY;
    for (index = 0U; index < pixel_count; ++index) {
        offset = index * 4U;
        value = luma(runtime.present_buffer[offset + 0U], runtime.present_buffer[offset + 1U], runtime.present_buffer[offset + 2U]);
        if (value < minimum) {
            minimum = value;
        }
        if (value > maximum) {
            maximum = value;
        }
        sum += value;
        checksum = (checksum * 33U) ^ (ss_u32)runtime.present_buffer[offset + 0U];
        checksum = (checksum * 33U) ^ (ss_u32)runtime.present_buffer[offset + 1U];
        checksum = (checksum * 33U) ^ (ss_u32)runtime.present_buffer[offset + 2U];
    }
    *checksum_out = checksum;
    *min_out = minimum;
    *max_out = maximum;
    *mean_milli_out = (sum * 1000U) / pixel_count;
    return checksum == 0U ? 10U : 0U;
}

int main(int argc, char **argv)
{
    ss_u32 checksum;
    ss_u32 minimum;
    ss_u32 maximum;
    ss_u32 mean_milli;
    ss_u32 status;

    if (argc != 2) {
        return 1;
    }
    status = render_stats(argv[1], &checksum, &minimum, &maximum, &mean_milli);
    if (status != 0U) {
        return (int)status;
    }
    printf("%u %u %u %u\n", checksum, minimum, maximum, mean_milli);
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
        errors.append("No C compiler found for Plasma v2 material response compile.")
        return None
    for source in DIRECT_SOURCES:
        require(source.exists(), f"Missing source for material response capture: {repo_path(source)}", errors)
    if errors:
        return None

    OUT.mkdir(parents=True, exist_ok=True)
    smoke = OUT / "plasma_v2_material_response.c"
    exe = OUT / "plasma_v2_material_response.exe"
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
        errors.append("Plasma v2 material response compile failed: " + (result.stdout + result.stderr).strip())
        return None
    return exe


def stats_for(tool: pathlib.Path, variant: str, errors: list[str]) -> dict[str, Any]:
    result = subprocess.run([str(tool), variant], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(f"Material response capture failed for {variant}: returncode {result.returncode}")
        return {}
    parts = result.stdout.strip().split()
    if len(parts) != 4:
        errors.append(f"Material response capture produced malformed output for {variant}.")
        return {}
    checksum, minimum, maximum, mean_milli = [int(part) for part in parts]
    return {
        "variant": variant,
        "checksum": str(checksum),
        "min_luminance": minimum,
        "max_luminance": maximum,
        "mean_luminance": round(mean_milli / 1000.0, 3),
        "luminance_range": maximum - minimum,
    }


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def validate(records: dict[str, dict[str, Any]], errors: list[str]) -> None:
    material_hashes = {records[name].get("checksum") for name in MATERIAL_VARIANTS if name in records}
    require(len(material_hashes) == len(MATERIAL_VARIANTS), "Every direct v2 material must produce a distinct checksum.", errors)
    require(records["contrast_high"]["luminance_range"] > records["contrast_low"]["luminance_range"], "High contrast must increase response range.", errors)
    require(records["brightness_high"]["mean_luminance"] > records["brightness_low"]["mean_luminance"], "Brightness must raise output level.", errors)
    require(records["brightness_low"]["mean_luminance"] >= 5.0, "Low brightness must remain visible.", errors)
    require(records["brightness_high"]["mean_luminance"] <= 220.0, "High brightness must remain bounded.", errors)
    require(records["softness_high"]["checksum"] != records["softness_low"]["checksum"], "Softness must change post-material output.", errors)
    require(records["treatment_none"]["checksum"] != records["treatment_soft"]["checksum"], "Soft treatment must differ from none.", errors)
    require(records["treatment_phosphor"]["checksum"] != records["treatment_soft"]["checksum"], "Phosphor treatment must differ from soft.", errors)
    require(records["material_quiet_darkroom"]["mean_luminance"] <= 90.0, "quiet_darkroom must remain dark-room comfortable.", errors)


def build_report(records: dict[str, dict[str, Any]], errors: list[str]) -> dict[str, Any]:
    if not errors:
        validate(records, errors)
    return {
        "schema": "screensave.plasma-v2.material-response.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "records": [records[name] for name in VARIANTS if name in records],
        "requirements": [
            "material changes field-to-color response",
            "contrast changes response steepness",
            "brightness changes output level within bounds",
            "softness and treatment change post-material output",
            "dark-room profile remains comfortable",
            "materials are distinct enough to review",
        ],
        "validation_errors": errors,
        "claim_boundary": "Direct material-response proof only; this does not prove final artistic acceptance, publication readiness, or stable promotion.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Material Response",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Records",
        "",
    ]
    for record in report.get("records", []):
        lines.append(
            f"- {record.get('variant')}: mean={record.get('mean_luminance')} range={record.get('luminance_range')}"
        )
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
    records: dict[str, dict[str, Any]] = {}
    tool = compile_tool(errors)
    if tool is not None:
        for variant in VARIANTS:
            records[variant] = stats_for(tool, variant, errors)
    report = build_report(records, errors)
    write_report(report)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 material response checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
