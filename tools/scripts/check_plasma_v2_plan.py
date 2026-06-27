"""Validate the Plasma v2 direct plan compiler and degradation law."""

from __future__ import annotations

import json
import pathlib
import shutil
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
PLAN_HEADER = SRC / "plasma_v2_plan.h"
PLAN_SOURCE = SRC / "plasma_v2_plan.c"
SPEC_HEADER = SRC / "plasma_v2_spec.h"
SPEC_SOURCE = SRC / "plasma_v2_spec.c"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "plan-report.json"
REPORT_MD = REPORT_DIR / "plan-report.md"
OUT = ROOT / "out" / "checks" / "plasma-v2-plan"

REQUIRED_FIELDS = [
    "requested_spec",
    "resolved_spec",
    "drawable_size",
    "field_size",
    "output_size",
    "requested_renderer",
    "active_renderer",
    "capability_flags",
    "degradation_flags",
    "base_seed",
    "stream_seed",
    "use_fixed_point",
    "use_software_reference",
    "use_feedback_buffer",
    "allow_experimental",
    "degrade_reason",
]

REQUIRED_TOKENS = [
    "PLASMA_V2_PLAN_SCHEMA_VERSION",
    "PLASMA_V2_RENDERER_SOFTWARE",
    "PLASMA_V2_RENDERER_GDI",
    "PLASMA_V2_RENDERER_GL11",
    "PLASMA_V2_CAP_SOFTWARE_REFERENCE",
    "PLASMA_V2_CAP_GDI",
    "PLASMA_V2_CAP_GL11",
    "PLASMA_V2_DEGRADE_RENDERER",
    "PLASMA_V2_DEGRADE_FIELD_SIZE",
    "PLASMA_V2_DEGRADE_EXPERIMENTAL",
    "plasma_v2_plan_compile",
    "plasma_v2_plan_is_valid",
    "plasma_v2_plan_renderer_token",
]

FORBIDDEN_TEXT = [
    "windows.h",
    "HWND",
    "HDC",
    "HINSTANCE",
    "screensave_saver_environment",
    "screensave_saver_session",
    "screensave/renderer",
    "renderer_api",
    "malloc",
    "free(",
    "fopen",
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
#include "plasma_v2_plan.h"

int main(void)
{
    plasma_v2_spec spec;
    plasma_v2_plan_request request;
    plasma_v2_plan plan;

    plasma_v2_spec_set_defaults(&spec);
    spec.output_kind = PLASMA_V2_OUTPUT_GLYPH;
    spec.treatment_kind = PLASMA_V2_TREATMENT_CRT;
    spec.scale = (ss_u32)1200U;

    request.struct_size = (ss_u32)sizeof(request);
    request.requested_spec = &spec;
    request.drawable_size.width = (ss_u32)640U;
    request.drawable_size.height = (ss_u32)480U;
    request.requested_renderer = PLASMA_V2_RENDERER_GL11;
    request.capability_flags = PLASMA_V2_CAP_GDI | PLASMA_V2_CAP_FEEDBACK_BUFFER;
    request.base_seed = (ss_u32)4096U;
    request.stream_seed = (ss_u32)17U;
    request.allow_experimental = SS_V2_FALSE;

    if (plasma_v2_plan_compile(&request, &plan) != SS_V2_STATUS_OK) {
        return 1;
    }
    if (plasma_v2_plan_is_valid(&plan) != SS_V2_TRUE) {
        return 2;
    }
    if (plan.requested_spec.output_kind != PLASMA_V2_OUTPUT_GLYPH) {
        return 3;
    }
    if (plan.resolved_spec.output_kind != PLASMA_V2_OUTPUT_CONTINUOUS) {
        return 4;
    }
    if (plan.requested_renderer != PLASMA_V2_RENDERER_GL11) {
        return 5;
    }
    if (plan.active_renderer != PLASMA_V2_RENDERER_GDI) {
        return 6;
    }
    if ((plan.degradation_flags & PLASMA_V2_DEGRADE_RENDERER) == 0U) {
        return 7;
    }
    if ((plan.degradation_flags & PLASMA_V2_DEGRADE_FIELD_SIZE) == 0U) {
        return 8;
    }
    if ((plan.degradation_flags & PLASMA_V2_DEGRADE_EXPERIMENTAL) == 0U) {
        return 9;
    }
    if (plan.use_fixed_point != SS_V2_TRUE || plan.use_software_reference != SS_V2_TRUE) {
        return 10;
    }
    if (plan.use_feedback_buffer != SS_V2_TRUE) {
        return 11;
    }
    if (plan.base_seed != (ss_u32)4096U || plan.stream_seed != (ss_u32)17U) {
        return 12;
    }
    if (plan.resolved_spec.scale != PLASMA_V2_CONTROL_MAX) {
        return 13;
    }
    if (plasma_v2_plan_renderer_token(plan.active_renderer)[0] == '\0') {
        return 14;
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
    for path in (PLAN_HEADER, PLAN_SOURCE, SPEC_HEADER, SPEC_SOURCE):
        require(path.exists(), f"Missing Plasma v2 plan input: {repo_path(path)}", errors)
    if errors:
        return

    header = PLAN_HEADER.read_text(encoding="utf-8")
    source = PLAN_SOURCE.read_text(encoding="utf-8")
    combined = header + "\n" + source
    for field in REQUIRED_FIELDS:
        require(field in header, f"plasma_v2_plan.h missing plan field {field!r}", errors)
    for token in REQUIRED_TOKENS:
        require(token in combined, f"Plasma v2 plan missing {token!r}", errors)
    for path in (PLAN_HEADER, PLAN_SOURCE):
        text = path.read_text(encoding="utf-8")
        for forbidden in FORBIDDEN_TEXT:
            require(forbidden not in text, f"{repo_path(path)} contains forbidden plan token {forbidden!r}", errors)
    for token in ["capability_or_stable_scope_degrade", "PLASMA_V2_CAP_SOFTWARE_REFERENCE", "PLASMA_V2_RENDERER_GL11"]:
        require(token in source, f"Plasma v2 plan source missing degradation law token {token!r}", errors)


def compile_smoke(errors: list[str]) -> None:
    cc = compiler()
    if cc is None:
        errors.append("No C compiler found for Plasma v2 plan C89 smoke compile.")
        return

    OUT.mkdir(parents=True, exist_ok=True)
    smoke = OUT / "plasma_v2_plan_smoke.c"
    exe = OUT / "plasma_v2_plan_smoke.exe"
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
        str(PLAN_SOURCE),
        str(SPEC_SOURCE),
        "-o",
        str(exe),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append("Plasma v2 plan C89 compile failed: " + (result.stdout + result.stderr).strip())
        return
    run = subprocess.run([str(exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run.returncode != 0:
        errors.append("Plasma v2 plan smoke failed with returncode " + str(run.returncode))


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def build_report(errors: list[str]) -> dict[str, Any]:
    status = "pass" if not errors else "fail"
    return {
        "schema": "screensave.plasma-v2.plan-report.v1",
        "status": status,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "required_facts": REQUIRED_FIELDS,
        "validation_errors": errors,
        "claim_boundary": "Plan compiler proof only; this does not promote Plasma v2 or publish a release.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Plan Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Required Facts",
        "",
    ]
    for item in report.get("required_facts", []):
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
    print("Plasma v2 plan checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
