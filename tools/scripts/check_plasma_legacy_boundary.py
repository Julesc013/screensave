"""Audit that legacy Plasma content is migration input, not v2 runtime truth."""

from __future__ import annotations

import json
import pathlib
import subprocess
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
PLASMA = ROOT / "products" / "savers" / "plasma"
PLASMA_SRC = PLASMA / "src"
V2 = PLASMA_SRC / "v2"
CONSTITUTION = PLASMA / "docs" / "plasma-v2-instrument-constitution.md"
AUTHORITY_DOC = PLASMA / "docs" / "plasma-v2-legacy-authority.md"
WORKBENCH_SOURCE = ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_workbench_shell.c"
PACKC_SCHEMA = ROOT / "tools" / "packc" / "schemas" / "plasma_spec_v2.schema.json"
PROOF_PROFILES = ROOT / "catalog" / "proof_profiles.toml"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
BOUNDARY_JSON = REPORT_DIR / "legacy-boundary-report.json"
BOUNDARY_MD = REPORT_DIR / "legacy-boundary-report.md"
AUTHORITY_JSON = REPORT_DIR / "legacy-authority-report.json"
AUTHORITY_MD = REPORT_DIR / "legacy-authority-report.md"

LEGACY_INPUTS = [
    PLASMA_SRC / "plasma_config.c",
    PLASMA_SRC / "plasma_presets.c",
    PLASMA_SRC / "plasma_themes.c",
    PLASMA_SRC / "plasma_content.c",
    PLASMA_SRC / "plasma_selection.c",
    PLASMA_SRC / "plasma_benchlab.c",
    PLASMA / "presets",
    PLASMA / "themes",
]

V2_AUTHORITY_FILES = [
    V2 / "plasma_v2_spec.h",
    V2 / "plasma_v2_spec.c",
    V2 / "plasma_v2_plan.h",
    V2 / "plasma_v2_plan.c",
    V2 / "plasma_v2_runtime.h",
    V2 / "plasma_v2_runtime.c",
    V2 / "plasma_v2_migration.h",
    V2 / "plasma_v2_migration.c",
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

BOUNDARY_NEEDLES = [
    "Legacy content is migration input only",
    "not hidden runtime masters",
    "not uninspected plan mutators",
    "not direct render authorities",
    "plasma_v2_spec",
    "plasma_v2_plan",
    "plasma_v2_runtime",
]

MIGRATION_NEEDLES = [
    "plasma_v2_migration_from_legacy_config",
    "plasma_v2_spec_from_legacy_config_view",
    "starter_plasma_v2_spec",
    "material_key",
    "presentation_flat_for_stable",
]

PLAN_RUNTIME_NEEDLES = [
    "requested_spec",
    "resolved_spec",
    "plasma_v2_plan_compile",
    "resolved_plan",
]

WORKBENCH_LEGACY_NEEDLES = [
    "legacy_source_kind",
    "legacy_key",
    "legacy_authority",
    "migration_input_only",
    "v2_spec_plan_runtime",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def read_text(path: pathlib.Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="ignore")


def path_entry(path: pathlib.Path, role: str, required: bool = True) -> dict[str, Any]:
    return {
        "path": repo_path(path),
        "role": role,
        "required": required,
        "exists": path.exists(),
        "kind": "directory" if path.is_dir() else "file",
    }


def missing_needles(path: pathlib.Path, needles: list[str]) -> list[str]:
    text = read_text(path)
    if not text:
        return [repo_path(path)]
    return [needle for needle in needles if needle not in text]


def source_has_all(paths: list[pathlib.Path], needles: list[str]) -> tuple[bool, list[str]]:
    text = "\n".join(read_text(path) for path in paths)
    missing = [needle for needle in needles if needle not in text]
    return not missing, missing


def build_authority_report() -> dict[str, Any]:
    legacy_inputs = [
        path_entry(path, "legacy_input", required=path.name != "themes")
        for path in LEGACY_INPUTS
    ]
    v2_files = [path_entry(path, "v2_authority") for path in V2_AUTHORITY_FILES]
    shims = [path_entry(path, "compatibility_shim") for path in COMPATIBILITY_SHIMS]
    bypass = [path_entry(path, "legacy_bypass_candidate") for path in LEGACY_BYPASS_CANDIDATES]

    blockers: list[str] = []
    missing_legacy_inputs = [entry["path"] for entry in legacy_inputs if entry["required"] and not entry["exists"]]
    missing_v2 = [entry["path"] for entry in v2_files if entry["required"] and not entry["exists"]]
    missing_shims = [entry["path"] for entry in shims if entry["required"] and not entry["exists"]]
    missing_bypass = [entry["path"] for entry in bypass if entry["required"] and not entry["exists"]]
    if missing_legacy_inputs:
        blockers.append("legacy_input_inventory_incomplete")
    if missing_v2:
        blockers.append("v2_authority_files_incomplete")
    if missing_shims:
        blockers.append("compatibility_shim_inventory_incomplete")
    if missing_bypass:
        blockers.append("legacy_bypass_candidate_inventory_incomplete")

    authority_doc_missing = missing_needles(AUTHORITY_DOC, BOUNDARY_NEEDLES)
    constitution_missing = missing_needles(CONSTITUTION, ["Presets must not be hidden runtime authority"])
    migration_ok, migration_missing = source_has_all(
        [V2 / "plasma_v2_migration.c", V2 / "plasma_v2_spec.c"],
        MIGRATION_NEEDLES,
    )
    plan_runtime_ok, plan_runtime_missing = source_has_all(
        [V2 / "plasma_v2_plan.h", V2 / "plasma_v2_plan.c", V2 / "plasma_v2_runtime.h"],
        PLAN_RUNTIME_NEEDLES,
    )
    workbench_missing = missing_needles(WORKBENCH_SOURCE, WORKBENCH_LEGACY_NEEDLES)
    packc_missing = missing_needles(PACKC_SCHEMA, ["field_family", "material", "treatment"])
    proof_missing = missing_needles(PROOF_PROFILES, ["plasma.v2.reference.preview", "preset"])

    if authority_doc_missing:
        blockers.append("legacy_authority_doc_incomplete")
    if constitution_missing:
        blockers.append("instrument_constitution_legacy_boundary_incomplete")
    if not migration_ok:
        blockers.append("legacy_inputs_do_not_reduce_to_v2_spec")
    if not plan_runtime_ok:
        blockers.append("runtime_plan_authority_not_proven")
    if workbench_missing:
        blockers.append("workbench_legacy_migration_facts_missing")
    if packc_missing:
        blockers.append("packc_v2_spec_examples_not_proven")
    if proof_missing:
        blockers.append("proof_profiles_do_not_reference_v2_evidence")

    legacy_removed = not blockers
    return {
        "schema": "screensave.plasma-v2.legacy-authority-report.v1",
        "status": "pass" if legacy_removed else "blocked",
        "legacy_preset_authority_removed": legacy_removed,
        "legacy_inputs": legacy_inputs,
        "v2_authority_files": v2_files,
        "compatibility_shims": shims,
        "bypass_candidates": bypass,
        "blocked_reasons": blockers,
        "evidence": {
            "authority_document_missing": authority_doc_missing,
            "constitution_missing": constitution_missing,
            "migration_missing": migration_missing,
            "plan_runtime_missing": plan_runtime_missing,
            "workbench_missing": workbench_missing,
            "packc_missing": packc_missing,
            "proof_missing": proof_missing,
        },
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "claim_boundary": "Legacy content is migration input only; Plasma v2 spec/plan/runtime own execution truth.",
    }


def build_boundary_report(authority_report: dict[str, Any]) -> dict[str, Any]:
    legacy_removed = bool(authority_report.get("legacy_preset_authority_removed"))
    return {
        "schema": "screensave.plasma-v2.legacy-boundary-report.v2",
        "status": "promotion-ready" if legacy_removed else "hold",
        "legacy_authority_removed": legacy_removed,
        "source": authority_report.get("source", {}),
        "constitution": {
            "path": repo_path(CONSTITUTION),
            "status": "pass"
            if not authority_report.get("evidence", {}).get("constitution_missing")
            else "blocked",
            "missing": authority_report.get("evidence", {}).get("constitution_missing", []),
        },
        "authority_report": repo_path(AUTHORITY_JSON),
        "compatibility_shims": authority_report.get("compatibility_shims", []),
        "legacy_bypass_candidates": authority_report.get("bypass_candidates", []),
        "blocking_reasons": authority_report.get("blocked_reasons", []),
        "claim_boundary": authority_report.get("claim_boundary"),
    }


def report_markdown(report: dict[str, Any], title: str) -> str:
    lines = [
        f"# {title}",
        "",
        f"- Status: {report.get('status')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Blocking Reasons",
        "",
    ]
    blockers = report.get("blocked_reasons") or report.get("blocking_reasons") or []
    if blockers:
        for item in blockers:
            lines.append(f"- {item}")
    else:
        lines.append("- none")
    lines.extend(["", "## Authority Groups", ""])
    for group in ["legacy_inputs", "v2_authority_files", "compatibility_shims", "bypass_candidates"]:
        entries = report.get(group, [])
        if not entries:
            continue
        lines.append(f"### {group}")
        for entry in entries:
            state = "present" if entry.get("exists") else "missing"
            suffix = "" if entry.get("required") else " (optional)"
            lines.append(f"- {state}: {entry.get('path')}{suffix}")
        lines.append("")
    return "\n".join(lines)


def write_reports(authority_report: dict[str, Any], boundary_report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    AUTHORITY_JSON.write_text(json.dumps(authority_report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    AUTHORITY_MD.write_text(report_markdown(authority_report, "Plasma v2 Legacy Authority Report"), encoding="utf-8")
    BOUNDARY_JSON.write_text(json.dumps(boundary_report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    BOUNDARY_MD.write_text(report_markdown(boundary_report, "Plasma v2 Legacy Boundary Report"), encoding="utf-8")


def main() -> int:
    authority_report = build_authority_report()
    boundary_report = build_boundary_report(authority_report)
    write_reports(authority_report, boundary_report)
    print(f"Plasma v2 legacy boundary audit {boundary_report.get('status')}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
