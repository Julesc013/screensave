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
]

REQUIRED_RULES = [
    ("effect_mode", "field_family"),
    ("speed_mode", "speed"),
    ("resolution_mode", "scale_complexity_softness"),
    ("smoothing_mode", "softness_or_treatment"),
    ("preset_key", "starter_plasma_v2_spec"),
    ("theme_key", "material_key"),
    ("output_mode", "output_kind"),
    ("presentation_mode", "presentation_flat_for_stable"),
    ("deterministic_seed", "seed"),
]

SMOKE_SOURCE = r'''
#include "plasma_v2_config.h"

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

int main(void)
{
    plasma_v2_legacy_config_view legacy;
    plasma_v2_config config;

    legacy.struct_size = (ss_u32)sizeof(legacy);
    legacy.effect_mode = (ss_i32)2;
    legacy.speed_mode = (ss_i32)2;
    legacy.resolution_mode = (ss_i32)2;
    legacy.smoothing_mode = (ss_i32)2;
    legacy.preset_key = "ocean_interference";
    legacy.theme_key = "museum_phosphor";
    legacy.seed = (ss_u32)12345U;
    legacy.deterministic = SS_V2_TRUE;

    if (plasma_v2_config_from_legacy(&legacy, &config) != SS_V2_STATUS_OK) {
        return 1;
    }
    if (plasma_v2_config_is_valid(&config) != SS_V2_TRUE) {
        return 2;
    }
    if (config.spec.field_family != PLASMA_V2_FIELD_INTERFERENCE) {
        return 3;
    }
    if (config.spec.speed != (ss_u32)700U) {
        return 4;
    }
    if (config.spec.treatment_kind != PLASMA_V2_TREATMENT_PHOSPHOR) {
        return 5;
    }
    if (token_equals(config.spec.material_key, "museum_phosphor") != SS_V2_TRUE) {
        return 6;
    }
    if (config.spec.seed != (ss_u32)12345U) {
        return 7;
    }
    if (token_equals(plasma_v2_migration_rule_for_legacy_key("preset_key"), "starter_plasma_v2_spec") != SS_V2_TRUE) {
        return 8;
    }
    if (token_equals(plasma_v2_migration_rule_for_legacy_key("theme_key"), "material_key") != SS_V2_TRUE) {
        return 9;
    }
    if (token_equals(plasma_v2_migration_rule_for_legacy_key("presentation_mode"), "presentation_flat_for_stable") != SS_V2_TRUE) {
        return 10;
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
    for path in REQUIRED_FILES:
        require(path.exists(), f"Missing Plasma v2 migration input: {repo_path(path)}", errors)
    if errors:
        return
    text = "\n".join(path.read_text(encoding="utf-8") for path in REQUIRED_FILES)
    for legacy_key, target in REQUIRED_RULES:
        require(legacy_key in text, f"Migration source missing legacy key {legacy_key!r}", errors)
        require(target in text, f"Migration source missing target rule {target!r}", errors)
    require("plasma_v2_spec_from_legacy_config_view" in text, "Migration must lower through the direct v2 spec constructor.", errors)


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


def build_report(errors: list[str]) -> dict[str, Any]:
    return {
        "schema": "screensave.plasma-v2.migration-report.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "migration_rules": [{"legacy": key, "target": target} for key, target in REQUIRED_RULES],
        "validation_errors": errors,
        "claim_boundary": "Legacy migration proof only; this does not prove old Plasma files are wrappers, remove every legacy authority path, or promote stable.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Legacy Migration Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Rules",
        "",
    ]
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
    scan_sources(errors)
    if not errors:
        compile_smoke(errors)
    report = build_report(errors)
    write_report(report)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 migration checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
