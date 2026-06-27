"""Audit that legacy Plasma files are contained as shims, not v2 truth."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PLASMA_SRC = ROOT / "products" / "savers" / "plasma" / "src"
CONSTITUTION = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-instrument-constitution.md"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "legacy-boundary-report.json"
REPORT_MD = REPORT_DIR / "legacy-boundary-report.md"

COMPATIBILITY_SHIMS = [
    "plasma_config.c",
    "plasma_presets.c",
    "plasma_themes.c",
    "plasma_content.c",
    "plasma_selection.c",
    "plasma_benchlab.c",
]

LEGACY_BYPASS_CANDIDATES = [
    "plasma_sim.c",
    "plasma_render.c",
    "plasma_plan.c",
    "plasma_advanced.c",
    "plasma_modern.c",
    "plasma_premium.c",
    "plasma_output.c",
    "plasma_treatment.c",
    "plasma_presentation.c",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def reads_v2(path: pathlib.Path) -> bool:
    if not path.exists():
        return False
    text = path.read_text(encoding="utf-8", errors="ignore")
    return "plasma_v2_" in text or "plasma_spec_v2" in text


def classify(name: str, expected_role: str) -> dict[str, Any]:
    path = PLASMA_SRC / name
    return {
        "path": repo_path(path),
        "expected_role": expected_role,
        "exists": path.exists(),
        "references_v2": reads_v2(path),
    }


def constitution_ok() -> tuple[bool, list[str]]:
    required = [
        "The legacy Plasma files may remain as migration evidence",
        "They must not be",
        "the default product center",
        "Presets must not be hidden runtime authority",
    ]
    if not CONSTITUTION.exists():
        return False, [repo_path(CONSTITUTION)]
    text = CONSTITUTION.read_text(encoding="utf-8")
    missing = [item for item in required if item not in text]
    return not missing, missing


def build_report() -> dict[str, Any]:
    shims = [classify(name, "compatibility_shim") for name in COMPATIBILITY_SHIMS]
    legacy = [classify(name, "legacy_bypass_candidate") for name in LEGACY_BYPASS_CANDIDATES]
    doc_ok, doc_missing = constitution_ok()

    missing_files = [item["path"] for item in [*shims, *legacy] if not item["exists"]]
    legacy_without_v2 = [item["path"] for item in legacy if item["exists"] and not item["references_v2"]]
    shim_without_v2 = [item["path"] for item in shims if item["exists"] and not item["references_v2"]]

    blockers = []
    if not doc_ok:
        blockers.append("constitution_boundary_missing")
    if missing_files:
        blockers.append("legacy_inventory_incomplete")
    if legacy_without_v2:
        blockers.append("legacy_bypass_candidates_not_wrapped_over_v2")
    if shim_without_v2:
        blockers.append("compatibility_shims_not_explicitly_wrapped_over_v2")

    structural_pass = doc_ok and not missing_files
    legacy_authority_removed = structural_pass and not legacy_without_v2 and not shim_without_v2
    status = "promotion-ready" if legacy_authority_removed else "hold" if structural_pass else "fail"

    return {
        "schema": "screensave.plasma-v2.legacy-boundary-report.v1",
        "status": status,
        "legacy_authority_removed": legacy_authority_removed,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "constitution": {
            "path": repo_path(CONSTITUTION),
            "status": "pass" if doc_ok else "fail",
            "missing": doc_missing,
        },
        "compatibility_shims": shims,
        "legacy_bypass_candidates": legacy,
        "blocking_reasons": blockers,
        "claim_boundary": "Legacy boundary audit only; hold means old Plasma files are not yet proven to be wrappers or migration-only inputs to the direct v2 instrument center.",
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Legacy Boundary Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Legacy authority removed: {report.get('legacy_authority_removed')}",
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
    lines.extend(["", "## Compatibility Shims", ""])
    for item in report.get("compatibility_shims", []):
        lines.append(f"- {item.get('path')}: references_v2={item.get('references_v2')}")
    lines.extend(["", "## Legacy Bypass Candidates", ""])
    for item in report.get("legacy_bypass_candidates", []):
        lines.append(f"- {item.get('path')}: references_v2={item.get('references_v2')}")
    lines.append("")
    return "\n".join(lines)


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    REPORT_MD.write_text(report_markdown(report), encoding="utf-8")


def main() -> int:
    report = build_report()
    write_report(report)
    print(f"Plasma v2 legacy boundary audit {report.get('status')}")
    return 0 if report.get("status") in {"promotion-ready", "hold"} else 1


if __name__ == "__main__":
    raise SystemExit(main())
