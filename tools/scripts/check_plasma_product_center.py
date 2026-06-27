"""Audit Plasma v2 product-center file boundaries."""

from __future__ import annotations

import json
import pathlib
import subprocess
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PLASMA = ROOT / "products" / "savers" / "plasma"
PLASMA_SRC = PLASMA / "src"
V2 = PLASMA_SRC / "v2"
AUTHORITY_DOC = PLASMA / "docs" / "plasma-v2-legacy-authority.md"
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

COMPATIBILITY_MIGRATION_INPUTS = [
    PLASMA_SRC / "plasma_config.c",
    PLASMA_SRC / "plasma_presets.c",
    PLASMA_SRC / "plasma_themes.c",
    PLASMA_SRC / "plasma_content.c",
    PLASMA_SRC / "plasma_selection.c",
    PLASMA_SRC / "plasma_benchlab.c",
    PLASMA / "presets",
    PLASMA / "themes",
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

AUTHORITY_DOC_NEEDLES = [
    "Legacy content is migration input only",
    "plasma_v2_spec",
    "plasma_v2_plan",
    "plasma_v2_runtime",
    "not hidden runtime masters",
    "not direct render authorities",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def file_entry(path: pathlib.Path, role: str, required: bool = True) -> dict[str, Any]:
    return {
        "path": repo_path(path),
        "role": role,
        "required": required,
        "exists": path.exists(),
        "kind": "directory" if path.is_dir() else "file",
    }


def authority_doc_status() -> tuple[bool, list[str]]:
    if not AUTHORITY_DOC.exists():
        return False, [repo_path(AUTHORITY_DOC)]
    text = AUTHORITY_DOC.read_text(encoding="utf-8")
    missing = [needle for needle in AUTHORITY_DOC_NEEDLES if needle not in text]
    return not missing, missing


def build_report() -> dict[str, Any]:
    center_entries = [file_entry(path, "new_product_center") for path in NEW_PRODUCT_CENTER]
    input_entries = [
        file_entry(path, "compatibility_migration_input", required=path.name != "themes")
        for path in COMPATIBILITY_MIGRATION_INPUTS
    ]
    legacy_entries = [file_entry(path, "legacy_bypass_candidate") for path in LEGACY_BYPASS_CANDIDATES]

    doc_ok, doc_missing = authority_doc_status()
    missing_center = [entry["path"] for entry in center_entries if entry["required"] and not entry["exists"]]
    missing_inputs = [entry["path"] for entry in input_entries if entry["required"] and not entry["exists"]]
    missing_legacy = [entry["path"] for entry in legacy_entries if entry["required"] and not entry["exists"]]

    blockers: list[str] = []
    if not doc_ok:
        blockers.append("legacy_authority_classification_doc_missing")
    if missing_center:
        blockers.append("new_product_center_incomplete")
    if missing_inputs:
        blockers.append("compatibility_migration_input_inventory_incomplete")
    if missing_legacy:
        blockers.append("legacy_candidate_inventory_incomplete")

    stable_eligible = not blockers
    status = "promotion-ready" if stable_eligible else "hold"

    return {
        "schema": "screensave.plasma-v2.product-center-report.v2",
        "status": status,
        "stable_eligible": stable_eligible,
        "product_center": "plasma_v2_spec_plan_runtime",
        "legacy_presence_is_not_blocking": True,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "authority_document": {
            "path": repo_path(AUTHORITY_DOC),
            "status": "pass" if doc_ok else "blocked",
            "missing": doc_missing,
        },
        "groups": {
            "new_product_center": center_entries,
            "compatibility_migration_inputs": input_entries,
            "legacy_bypass_candidates": legacy_entries,
        },
        "blocking_reasons": blockers,
        "claim_boundary": "Product-center boundary audit only; legacy files may remain as compatibility and migration inputs while Plasma v2 spec/plan/runtime own execution truth.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Product-Center Boundary Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Stable eligible: {report.get('stable_eligible')}",
        f"- Product center: {report.get('product_center')}",
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
            state = "present" if entry.get("exists") else "missing"
            suffix = "" if entry.get("required") else " (optional)"
            lines.append(f"- {state}: {entry.get('path')}{suffix}")
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
