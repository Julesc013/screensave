"""Validate legacy Plasma preset/theme migration into direct v2 specs."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "migration-report.json"
REPORT_MD = REPORT_DIR / "migration-report.md"
OUT = ROOT / "out" / "checks" / "plasma-v2-migration"

REQUIRED_FILES = [
    SRC / "plasma_v2_types.h",
    SRC / "plasma_v2_spec.h",
    SRC / "plasma_v2_spec.c",
    SRC / "plasma_v2_migration.h",
    SRC / "plasma_v2_migration.c",
    SRC / "plasma_v2_config.h",
    SRC / "plasma_v2_config.c",
    SRC / "plasma_v2_plan.h",
    SRC / "plasma_v2_plan.c",
    SRC / "plasma_v2_runtime.h",
    SRC / "plasma_v2_runtime.c",
]

REQUIRED_RULES = [
    ("effect_mode", "field_family"),
    ("speed_mode", "speed"),
    ("resolution_mode", "field-size / internal-resolution policy"),
    ("smoothing_mode", "softness or treatment"),
    ("preset_key", "starter plasma_v2_spec"),
    ("theme_key", "material key"),
    ("output_mode", "output kind"),
    ("presentation_mode", "presentation kind"),
    ("deterministic_seed", "same seed field"),
]

REQUIRED_TARGET_TOKENS = [
    "field_family",
    "speed",
    "scale",
    "complexity",
    "softness",
    "starter_plasma_v2_spec",
    "material_key",
    "output_kind",
    "presentation_flat_for_stable",
    "seed",
]

FIXTURES = [
    {
        "legacy_preset_key": "plasma_lava",
        "legacy_theme_key": "plasma_lava",
        "resulting_v2_field_family": "classic",
        "material_key": "plasma_lava",
        "speed": 420,
        "scale": 420,
        "complexity": 520,
        "warp": 180,
        "feedback": 120,
        "treatment": "soft",
        "output_kind": "continuous",
        "presentation": "flat",
        "seed_policy": "fixed",
    },
    {
        "legacy_preset_key": "aurora_plasma",
        "legacy_theme_key": "aurora_cool",
        "resulting_v2_field_family": "aurora",
        "material_key": "aurora_cool",
        "speed": 700,
        "scale": 560,
        "complexity": 600,
        "warp": 320,
        "feedback": 120,
        "treatment": "soft",
        "output_kind": "continuous",
        "presentation": "flat",
        "seed_policy": "fixed",
    },
    {
        "legacy_preset_key": "ocean_interference",
        "legacy_theme_key": "oceanic_blue",
        "resulting_v2_field_family": "interference",
        "material_key": "oceanic_blue",
        "speed": 700,
        "scale": 360,
        "complexity": 700,
        "warp": 280,
        "feedback": 120,
        "treatment": "none",
        "output_kind": "continuous",
        "presentation": "flat",
        "seed_policy": "fixed",
    },
    {
        "legacy_preset_key": "museum_phosphor",
        "legacy_theme_key": "museum_phosphor",
        "resulting_v2_field_family": "interference",
        "material_key": "museum_phosphor",
        "speed": 220,
        "scale": 420,
        "complexity": 520,
        "warp": 260,
        "feedback": 120,
        "treatment": "phosphor",
        "output_kind": "continuous",
        "presentation": "flat",
        "seed_policy": "fixed",
    },
    {
        "legacy_preset_key": "quiet_darkroom",
        "legacy_theme_key": "quiet_darkroom",
        "resulting_v2_field_family": "fire",
        "material_key": "quiet_darkroom",
        "speed": 180,
        "scale": 680,
        "complexity": 360,
        "warp": 140,
        "feedback": 180,
        "treatment": "none",
        "output_kind": "continuous",
        "presentation": "flat",
        "seed_policy": "fixed",
    },
]

SMOKE_SOURCE = r'''
#include "plasma_v2_config.h"
#include "plasma_v2_plan.h"
#include "plasma_v2_runtime.h"

#define FIELD_CAPACITY ((ss_u32)(16U * 12U))
#define BYTE_CAPACITY ((ss_u32)(16U * 12U * 4U))

static ss_u32 field_a[16U * 12U];
static ss_u32 field_b[16U * 12U];
static ss_u32 field_history[16U * 12U];
static ss_u8 material_buffer[16U * 12U * 4U];
static ss_u8 treatment_buffer[16U * 12U * 4U];
static ss_u8 present_buffer[16U * 12U * 4U];

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

static int check_fixture(const plasma_v2_migration_fixture *fixture)
{
    plasma_v2_legacy_config_view legacy;
    plasma_v2_config config;
    plasma_v2_plan_request request;
    plasma_v2_plan plan;
    plasma_v2_runtime_buffers buffers;
    plasma_v2_runtime runtime;

    legacy.struct_size = (ss_u32)sizeof(legacy);
    legacy.effect_mode = fixture->effect_mode;
    legacy.speed_mode = fixture->speed_mode;
    legacy.resolution_mode = fixture->resolution_mode;
    legacy.smoothing_mode = fixture->smoothing_mode;
    legacy.preset_key = fixture->legacy_preset_key;
    legacy.theme_key = fixture->legacy_theme_key;
    legacy.seed = fixture->seed;
    legacy.deterministic = fixture->deterministic;

    if (plasma_v2_config_from_legacy(&legacy, &config) != SS_V2_STATUS_OK) {
        return 10;
    }
    if (plasma_v2_config_is_valid(&config) != SS_V2_TRUE) {
        return 11;
    }
    if (token_equals(plasma_v2_field_family_token(config.spec.field_family), fixture->expected_field_family) != SS_V2_TRUE) {
        return 12;
    }
    if (token_equals(config.spec.material_key, fixture->expected_material_key) != SS_V2_TRUE) {
        return 13;
    }
    if (config.spec.speed != fixture->expected_speed) {
        return 14;
    }
    if (config.spec.scale != fixture->expected_scale) {
        return 15;
    }
    if (config.spec.complexity != fixture->expected_complexity) {
        return 16;
    }
    if (config.spec.warp != fixture->expected_warp) {
        return 17;
    }
    if (config.spec.feedback != fixture->expected_feedback) {
        return 18;
    }
    if (token_equals(plasma_v2_treatment_kind_token(config.spec.treatment_kind), fixture->expected_treatment) != SS_V2_TRUE) {
        return 19;
    }
    if (token_equals(plasma_v2_output_kind_token(config.spec.output_kind), fixture->expected_output_kind) != SS_V2_TRUE) {
        return 20;
    }
    if (config.spec.seed != fixture->seed || config.spec.deterministic != SS_V2_TRUE) {
        return 21;
    }

    request.struct_size = (ss_u32)sizeof(request);
    request.requested_spec = &config.spec;
    request.drawable_size.width = (ss_u32)16U;
    request.drawable_size.height = (ss_u32)12U;
    request.requested_renderer = PLASMA_V2_RENDERER_GDI;
    request.capability_flags = PLASMA_V2_CAP_GDI | PLASMA_V2_CAP_FEEDBACK_BUFFER;
    request.base_seed = fixture->seed;
    request.stream_seed = (ss_u32)17U;
    request.allow_experimental = SS_V2_FALSE;

    if (plasma_v2_plan_compile(&request, &plan) != SS_V2_STATUS_OK) {
        return 22;
    }
    if (plasma_v2_plan_is_valid(&plan) != SS_V2_TRUE) {
        return 23;
    }
    if (plan.resolved_spec.seed != fixture->seed) {
        return 24;
    }
    if (token_equals(plasma_v2_field_family_token(plan.resolved_spec.field_family), fixture->expected_field_family) != SS_V2_TRUE) {
        return 25;
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
        return 26;
    }
    if (plasma_v2_runtime_is_valid(&runtime) != SS_V2_TRUE) {
        return 27;
    }
    return 0;
}

int main(void)
{
    const plasma_v2_migration_fixture *fixtures;
    ss_u32 count;
    ss_u32 index;
    int result;

    fixtures = plasma_v2_migration_fixtures(&count);
    if (fixtures == 0 || count != PLASMA_V2_MIGRATION_FIXTURE_COUNT || count != (ss_u32)5U) {
        return 1;
    }
    for (index = 0U; index < count; ++index) {
        result = check_fixture(&fixtures[index]);
        if (result != 0) {
            return result;
        }
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


def scan_sources(errors: list[str]) -> bool:
    for path in REQUIRED_FILES:
        require(path.exists(), f"Missing Plasma v2 migration input: {repo_path(path)}", errors)
    if errors:
        return False
    text = "\n".join(path.read_text(encoding="utf-8") for path in REQUIRED_FILES)
    for legacy_key, target in REQUIRED_RULES:
        require(legacy_key in text, f"Migration source missing legacy key {legacy_key!r}", errors)
    for token in REQUIRED_TARGET_TOKENS:
        require(token in text, f"Migration source missing target token {token!r}", errors)
    for fixture in FIXTURES:
        require(fixture["legacy_preset_key"] in text, f"Migration fixtures missing {fixture['legacy_preset_key']!r}", errors)
        require(fixture["legacy_theme_key"] in text, f"Migration fixtures missing {fixture['legacy_theme_key']!r}", errors)
    require("plasma_v2_spec_from_legacy_config_view" in text, "Migration must lower through the direct v2 spec constructor.", errors)
    require("plasma_v2_plan_compile" in text, "Migration proof must mention plan compilation.", errors)
    require("plasma_v2_runtime_bind" in text, "Migration proof must mention runtime binding.", errors)
    return not errors


def hidden_preset_authority_detected() -> bool:
    plan_runtime = "\n".join(
        (SRC / name).read_text(encoding="utf-8")
        for name in ["plasma_v2_plan.c", "plasma_v2_runtime.c", "plasma_v2_runtime.h"]
    )
    return "preset_key" in plan_runtime or "theme_key" in plan_runtime


def compile_smoke(errors: list[str]) -> None:
    cc = compiler()
    if cc is None:
        errors.append("No C compiler found for Plasma v2 migration C89 smoke compile.")
        return
    OUT.mkdir(parents=True, exist_ok=True)
    smoke = OUT / "plasma_v2_migration_smoke.c"
    exe = OUT / "plasma_v2_migration_smoke.exe"
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
        str(SRC / "plasma_v2_spec.c"),
        str(SRC / "plasma_v2_migration.c"),
        str(SRC / "plasma_v2_config.c"),
        str(SRC / "plasma_v2_plan.c"),
        str(SRC / "plasma_v2_runtime.c"),
        "-o",
        str(exe),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append("Plasma v2 migration C89 compile failed: " + (result.stdout + result.stderr).strip())
        return
    run = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run.returncode != 0:
        errors.append("Plasma v2 migration smoke failed with returncode " + str(run.returncode))


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def build_report(errors: list[str], hidden_authority: bool) -> dict[str, Any]:
    status = "pass" if not errors and not hidden_authority else "fail"
    return {
        "schema": "screensave.plasma-v2.migration-report.v2",
        "status": status,
        "fixtures_checked": len(FIXTURES),
        "all_legacy_entries_reduce_to_v2_spec": status == "pass",
        "hidden_preset_authority_detected": hidden_authority,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "migration_rules": [
            {"legacy": legacy_key, "target": target}
            for legacy_key, target in REQUIRED_RULES
        ],
        "fixtures": FIXTURES,
        "validation_errors": errors,
        "claim_boundary": "Migration proof only; not stable promotion.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Legacy Migration Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Fixtures checked: {report.get('fixtures_checked')}",
        f"- All legacy entries reduce to v2 spec: {report.get('all_legacy_entries_reduce_to_v2_spec')}",
        f"- Hidden preset authority detected: {report.get('hidden_preset_authority_detected')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Fixtures",
        "",
    ]
    for item in report.get("fixtures", []):
        lines.append(
            "- {preset} / {theme} -> field={field}, material={material}, speed={speed}, scale={scale}, treatment={treatment}".format(
                preset=item.get("legacy_preset_key"),
                theme=item.get("legacy_theme_key"),
                field=item.get("resulting_v2_field_family"),
                material=item.get("material_key"),
                speed=item.get("speed"),
                scale=item.get("scale"),
                treatment=item.get("treatment"),
            )
        )
    lines.extend(["", "## Rules", ""])
    for item in report.get("migration_rules", []):
        lines.append(f"- {item.get('legacy')} -> {item.get('target')}")
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
    source_ok = scan_sources(errors)
    hidden_authority = hidden_preset_authority_detected() if source_ok else False
    if hidden_authority:
        errors.append("Plasma v2 plan/runtime still read legacy preset_key or theme_key after migration.")
    if not errors:
        compile_smoke(errors)
    report = build_report(errors, hidden_authority)
    write_report(report)
    if report.get("status") != "pass":
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 migration checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
