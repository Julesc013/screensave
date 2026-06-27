"""Audit Plasma v2 product-center file boundaries."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PLASMA_SRC = ROOT / "products" / "savers" / "plasma" / "src"
V2 = PLASMA_SRC / "v2"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "product-center-report.json"
REPORT_MD = REPORT_DIR / "product-center-report.md"

NEW_PRODUCT_CENTER = [
    V2 / "plasma_v2_spec.h",
    V2 / "plasma_v2_spec.c",
    V2 / "plasma_v2_plan.h",
    V2 / "plasma_v2_plan.c",
    V2 / "plasma_v2_runtime.h",
    V2 / "plasma_v2_runtime.c",
    V2 / "field",
    V2 / "output",
]

COMPATIBILITY_SHIMS = [
    PLASMA_SRC / "plasma_config.c",
    PLASMA_SRC / "plasma_presets.c",
    PLASMA_SRC / "plasma_themes.c",
    PLASMA_SRC / "plasma_content.c",
    PLASMA_SRC / "plasma_selection.c",
    PLASMA_SRC / "plasma_benchlab.c",
]

LEGACY_BYPASS_CANDIDATES = [
    PLASMA_SRC / "plasma_sim.c",
    PLASMA_SRC / "plasma_render.c",
    PLASMA_SRC / "plasma_plan.c",
    PLASMA_SRC / "plasma_advanced.c",
    PLASMA_SRC / "plasma_modern.c",
    PLASMA_SRC / "plasma_premium.c",
    PLASMA_SRC / "plasma_output.c",
    PLASMA_SRC / "plasma_treatment.c",
    PLASMA_SRC / "plasma_presentation.c",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def file_entry(path: pathlib.Path, role: str) -> dict[str, Any]:
    return {
        "path": repo_path(path),
        "role": role,
        "exists": path.exists(),
        "kind": "directory" if path.is_dir() else "file",
    }


def build_report() -> dict[str, Any]:
    center_entries = [file_entry(path, "new_product_center") for path in NEW_PRODUCT_CENTER]
    shim_entries = [file_entry(path, "compatibility_shim") for path in COMPATIBILITY_SHIMS]
    legacy_entries = [file_entry(path, "legacy_bypass_candidate") for path in LEGACY_BYPASS_CANDIDATES]

    missing_center = [entry["path"] for entry in center_entries if not entry["exists"]]
    missing_shims = [entry["path"] for entry in shim_entries if not entry["exists"]]
    missing_legacy = [entry["path"] for entry in legacy_entries if not entry["exists"]]

    blockers = []
    if missing_center:
        blockers.append("new_product_center_incomplete")
    if missing_shims:
        blockers.append("compatibility_shim_inventory_incomplete")
    if missing_legacy:
        blockers.append("legacy_candidate_inventory_incomplete")
    if not missing_legacy:
        blockers.append("legacy_bypass_candidates_still_present")

    structural_pass = not missing_shims and not missing_legacy
    stable_eligible = structural_pass and not missing_center and "legacy_bypass_candidates_still_present" not in blockers
    status = "promotion-ready" if stable_eligible else "hold" if structural_pass else "fail"

    return {
        "schema": "screensave.plasma-v2.product-center-report.v1",
        "status": status,
        "stable_eligible": stable_eligible,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "groups": {
            "new_product_center": center_entries,
            "compatibility_shims": shim_entries,
            "legacy_bypass_candidates": legacy_entries,
        },
        "blocking_reasons": blockers,
        "claim_boundary": "Product-center boundary audit only; hold means legacy files are not yet proven to be wrappers over the direct v2 instrument center.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Product-Center Boundary Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Stable eligible: {report.get('stable_eligible')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Blocking Reasons",
        "",
    ]
    blockers = report.get("blocking_reasons", [])
    if blockers:
        for item in blockers:
            lines.append(f"- {item}")
    else:
        lines.append("- none")
    lines.extend(["", "## File Groups", ""])
    for group_name, entries in report.get("groups", {}).items():
        lines.append(f"### {group_name}")
        for entry in entries:
            lines.append(f"- {'present' if entry.get('exists') else 'missing'}: {entry.get('path')}")
        lines.append("")
    return "\n".join(lines)


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    REPORT_MD.write_text(report_markdown(report), encoding="utf-8")


def main() -> int:
    report = build_report()
    write_report(report)
    print(f"Plasma v2 product-center boundary audit {report.get('status')}")
    return 0 if report.get("status") in {"promotion-ready", "hold"} else 1


if __name__ == "__main__":
    raise SystemExit(main())
