"""Validate Workbench inspection of Plasma v2 instrument evidence."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
SHELL_SOURCE = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.c"
SHELL_HEADER = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.h"
WORKBENCH_CHECK = ROOT / "tools" / "scripts" / "check_workbench_shell.py"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "workbench-inspection.json"
REPORT_MD = REPORT_DIR / "workbench-inspection.md"

REQUIRED_FIELDS = [
    "requested_spec",
    "resolved_plan",
    "degradation_reason",
    "field_pipeline",
    "material_response",
    "treatment_stack",
    "software_reference_path",
    "gl11_optionality",
    "capture_refs",
    "control_influence_status",
    "review_status",
    "promotion_status",
]

REQUIRED_REFS = [
    "validation/captures/plasma-v2/instrument-audit/plan-report.json",
    "validation/captures/plasma-v2/instrument-audit/field-pipeline-report.json",
    "validation/captures/plasma-v2/instrument-audit/material-response.json",
    "validation/captures/plasma-v2/instrument-audit/control-influence.json",
    "validation/captures/plasma-v2/acceleration/matrix.json",
    "release-candidate-hold",
    "not a second runtime",
    "promotion authority",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def run_workbench_check(errors: list[str]) -> None:
    result = subprocess.run(
        [sys.executable, str(WORKBENCH_CHECK)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if result.returncode != 0:
        errors.append("check_workbench_shell.py failed: " + (result.stderr or result.stdout).strip())


def build_report(errors: list[str]) -> dict[str, Any]:
    for path in (SHELL_SOURCE, SHELL_HEADER):
        if not path.exists():
            errors.append(f"Missing Workbench source: {repo_path(path)}")
    if not errors:
        text = SHELL_SOURCE.read_text(encoding="utf-8") + "\n" + SHELL_HEADER.read_text(encoding="utf-8")
        for field in REQUIRED_FIELDS:
            if field not in text:
                errors.append(f"Workbench instrument inspection missing field {field!r}")
        for ref in REQUIRED_REFS:
            if ref not in text:
                errors.append(f"Workbench instrument inspection missing evidence ref {ref!r}")

    return {
        "schema": "screensave.plasma-v2.workbench-instrument-inspection.v1",
        "status": "pass" if not errors else "fail",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "required_fields": REQUIRED_FIELDS,
        "required_refs": REQUIRED_REFS,
        "validation_errors": errors,
        "claim_boundary": "Workbench inspection evidence only; Workbench consumes ScreenSave proof and does not own runtime truth, artistic acceptance, or release promotion.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Workbench Instrument Inspection",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Required Fields",
        "",
    ]
    for field in report.get("required_fields", []):
        lines.append(f"- {field}")
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
    run_workbench_check(errors)
    report = build_report(errors)
    write_report(report)
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Plasma v2 Workbench inspection checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
