"""Audit whether Plasma v2 is promotion-ready as a visual instrument."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
CONTRACT = ROOT / "contracts" / "plasma_instrument_architecture_v1.md"
STABLE_CONTRACT = ROOT / "contracts" / "plasma_stable_promotion_v1.md"
PAW_I = ROOT / "docs" / "roadmap" / "paw-i-plasma-v2-stable-promotion.md"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion"
REPORT_JSON = REPORT_DIR / "instrument-architecture-audit.json"
REPORT_MD = REPORT_DIR / "instrument-architecture-audit.md"
V2_ISLAND = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
PLASMA_SRC = ROOT / "products" / "savers" / "plasma" / "src"

SUBCHECKS = [
    ("direct-spec", ["tools/scripts/check_plasma_v2_direct_spec.py"]),
    ("legacy-core-boundaries", ["tools/scripts/check_plasma_core_boundaries.py"]),
    ("direct-control-influence", ["tools/scripts/check_plasma_v2_influence.py"]),
    ("material-treatment", ["tools/scripts/check_plasma_v2_materials.py"]),
    ("packc-data-only", ["tools/scripts/check_packc.py"]),
    ("visualintent-contract", ["tools/scripts/check_visual_intent_contract.py"]),
    ("aide-boundary", ["tools/scripts/check_aide_evidence_bridge.py"]),
]

REQUIRED_GATE_IDS = [
    "plasma_v2_spec_contract_passes",
    "plasma_v2_plan_contract_passes",
    "plasma_v2_runtime_contract_passes",
    "legacy_preset_authority_removed",
    "direct_controls_influence_passes",
    "field_pipeline_boundaries_pass",
    "material_mapping_distinctness_passes",
    "treatment_boundaries_pass",
    "software_reference_is_canonical",
    "gl11_is_not_hidden_minimum",
    "packc_data_only_passes",
    "visualintent_candidates_reduce_to_plasma_spec",
    "aide_not_runtime_or_truth",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def run(args: list[str], timeout_seconds: int = 300) -> dict[str, Any]:
    command = [sys.executable, *args]
    result = subprocess.run(
        command,
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=timeout_seconds,
        check=False,
    )
    return {
        "command": command,
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "status": "pass" if result.returncode == 0 else "fail",
        "timeout_seconds": timeout_seconds,
    }


def add_check(checks: list[dict[str, Any]], check_id: str, ok: bool, summary: str, **data: Any) -> None:
    checks.append(
        {
            "id": check_id,
            "status": "pass" if ok else "fail",
            "summary": summary,
            "data": data,
        }
    )


def gate(gates: list[dict[str, Any]], gate_id: str, status: str, summary: str, **data: Any) -> None:
    gates.append(
        {
            "id": gate_id,
            "status": status,
            "summary": summary,
            "data": data,
        }
    )


def text_contains(path: pathlib.Path, needles: list[str]) -> tuple[bool, list[str]]:
    if not path.exists():
        return False, [repo_path(path)]
    text = path.read_text(encoding="utf-8")
    missing = [needle for needle in needles if needle not in text]
    return not missing, missing


def subcheck_status(subchecks: dict[str, dict[str, Any]], name: str) -> bool:
    return subchecks.get(name, {}).get("returncode") == 0


def build_report() -> dict[str, Any]:
    checks: list[dict[str, Any]] = []
    gates: list[dict[str, Any]] = []
    state = load_toml(STATE)
    plasma = state.get("plasma_v2", {})

    for path in (CONTRACT, STABLE_CONTRACT, PAW_I):
        add_check(checks, f"path:{repo_path(path)}", path.exists(), "Required audit authority path exists.")

    contract_ok, contract_missing = text_contains(
        CONTRACT,
        [
            "Status: active PAW-I instrument-architecture audit contract.",
            "Plasma is not a preset picker. Plasma is a visual instrument.",
            "direct controls first",
            "plasma_v2_spec",
            "plasma_v2_plan",
            "plasma_v2_runtime",
            "software_reference_is_canonical",
            "gl11_is_not_hidden_minimum",
            "aide_not_runtime_or_truth",
        ],
    )
    add_check(
        checks,
        "instrument-contract-text",
        contract_ok,
        "Instrument contract records the product thesis, pipeline, and blocking gates.",
        missing=contract_missing,
    )

    stable_ok, stable_missing = text_contains(
        STABLE_CONTRACT,
        [
            "instrument architecture audit passed",
            "validator pass != instrument architecture acceptance",
            "hidden preset authority",
            "hidden GL11 minimum",
        ],
    )
    add_check(
        checks,
        "stable-contract-fold-in",
        stable_ok,
        "Stable-promotion contract requires the instrument audit.",
        missing=stable_missing,
    )

    pawi_ok, pawi_missing = text_contains(
        PAW_I,
        [
            "Instrument Architecture Gates",
            "Plasma is not a preset picker. Plasma is a visual instrument.",
            "instrument-architecture-audit.json",
        ],
    )
    add_check(
        checks,
        "paw-i-fold-in",
        pawi_ok,
        "PAW-I roadmap records the instrument audit as a promotion gate.",
        missing=pawi_missing,
    )

    subchecks: dict[str, dict[str, Any]] = {}
    for check_id, command in SUBCHECKS:
        result = run(command)
        subchecks[check_id] = result
        add_check(
            checks,
            check_id,
            result.get("returncode") == 0,
            "Command passed." if result.get("returncode") == 0 else "Command failed.",
            run=result,
        )

    gate(
        gates,
        "plasma_v2_spec_contract_passes",
        "pass" if subcheck_status(subchecks, "direct-spec") else "fail",
        "The direct-control Plasma v2 spec island exists and passes its C89 smoke check.",
        evidence=[
            "products/savers/plasma/src/v2/plasma_v2_types.h",
            "products/savers/plasma/src/v2/plasma_v2_spec.h",
            "products/savers/plasma/src/v2/plasma_v2_spec.c",
            "tools/scripts/check_plasma_v2_direct_spec.py",
        ],
    )

    plan_files = [V2_ISLAND / "plasma_v2_plan.h", V2_ISLAND / "plasma_v2_plan.c"]
    gate(
        gates,
        "plasma_v2_plan_contract_passes",
        "pass" if all(path.exists() for path in plan_files) else "hold",
        "The direct v2 plan contract must exist in the v2 island before stable promotion.",
        missing=[repo_path(path) for path in plan_files if not path.exists()],
    )

    runtime_files = [V2_ISLAND / "plasma_v2_runtime.h", V2_ISLAND / "plasma_v2_runtime.c"]
    gate(
        gates,
        "plasma_v2_runtime_contract_passes",
        "pass" if all(path.exists() for path in runtime_files) else "hold",
        "The direct v2 runtime contract must exist in the v2 island before stable promotion.",
        missing=[repo_path(path) for path in runtime_files if not path.exists()],
    )

    legacy_files = [
        PLASMA_SRC / "plasma_presets.c",
        PLASMA_SRC / "plasma_themes.c",
        PLASMA_SRC / "plasma_selection.c",
    ]
    gate(
        gates,
        "legacy_preset_authority_removed",
        "hold",
        "Legacy preset/theme files remain compatibility evidence; PAW-I must prove they are migration inputs, not hidden runtime authority.",
        compatibility_files=[repo_path(path) for path in legacy_files if path.exists()],
    )

    gate(
        gates,
        "direct_controls_influence_passes",
        "pass" if subcheck_status(subchecks, "direct-control-influence") else "fail",
        "Basic controls with visual claims produce deterministic output differences.",
        evidence=["tools/scripts/check_plasma_v2_influence.py"],
    )

    field_dirs = [V2_ISLAND / "field", V2_ISLAND / "output"]
    gate(
        gates,
        "field_pipeline_boundaries_pass",
        "pass" if all(path.exists() for path in field_dirs) else "hold",
        "Field and output boundaries must be explicit in the v2 island before stable promotion.",
        missing=[repo_path(path) for path in field_dirs if not path.exists()],
    )

    gate(
        gates,
        "material_mapping_distinctness_passes",
        "pass" if subcheck_status(subchecks, "material-treatment") else "fail",
        "Stable materials have distinct deterministic visual response.",
        evidence=["tools/scripts/check_plasma_v2_materials.py"],
    )

    gate(
        gates,
        "treatment_boundaries_pass",
        "pass" if (PLASMA_SRC / "plasma_treatment.h").exists() and subcheck_status(subchecks, "material-treatment") else "fail",
        "Treatment remains post-material and bounded for the stable slice.",
        evidence=[repo_path(PLASMA_SRC / "plasma_treatment.h"), "tools/scripts/check_plasma_v2_materials.py"],
    )

    gate(
        gates,
        "software_reference_is_canonical",
        "pass" if subcheck_status(subchecks, "legacy-core-boundaries") else "fail",
        "Software/reference execution remains the canonical proof-bearing path.",
        evidence=["tools/scripts/check_plasma_core_boundaries.py"],
    )

    gl_doc_ok, gl_missing = text_contains(
        ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-stable-promotion-policy.md",
        [
            "renderer floor: `gdi`",
            "first optional acceleration candidate: `gl11`",
            "hidden GL11 minimum",
        ],
    )
    gate(
        gates,
        "gl11_is_not_hidden_minimum",
        "pass" if gl_doc_ok else "fail",
        "GDI remains the floor and GL11 remains optional.",
        missing=gl_missing,
    )

    gate(
        gates,
        "packc_data_only_passes",
        "pass" if subcheck_status(subchecks, "packc-data-only") else "fail",
        "packc validates data-only Plasma packs and rejects executable or unsafe payloads.",
        evidence=["tools/scripts/check_packc.py"],
    )

    gate(
        gates,
        "visualintent_candidates_reduce_to_plasma_spec",
        "hold",
        "VisualIntent is contractually descriptive, but a Plasma v2 candidate resolver to the direct spec is not yet admitted as stable evidence.",
        evidence=["contracts/visual_intent_v1.md"],
    )

    gate(
        gates,
        "aide_not_runtime_or_truth",
        "pass" if subcheck_status(subchecks, "aide-boundary") else "fail",
        "AIDE remains an evidence and repair coordinator, not product runtime or truth authority.",
        evidence=["tools/scripts/check_aide_evidence_bridge.py"],
    )

    present_gate_ids = {item["id"] for item in gates}
    add_check(
        checks,
        "required-gate-set",
        set(REQUIRED_GATE_IDS) == present_gate_ids,
        "Instrument audit emits the required PAW-I gate set.",
        missing=sorted(set(REQUIRED_GATE_IDS) - present_gate_ids),
        extra=sorted(present_gate_ids - set(REQUIRED_GATE_IDS)),
    )

    structural_pass = all(item.get("status") == "pass" for item in checks) and all(
        item.get("status") in {"pass", "hold"} for item in gates
    )
    blocking_gates = [item["id"] for item in gates if item.get("status") == "hold"]
    failed_gates = [item["id"] for item in gates if item.get("status") == "fail"]

    stable_eligible = structural_pass and not blocking_gates and not failed_gates
    current_stable = plasma.get("stable") is True or plasma.get("release_promotion") == "accepted"
    if current_stable and not stable_eligible:
        add_check(
            checks,
            "project-state-not-overpromoted",
            False,
            "Project state must not claim stable promotion while instrument gates are blocked.",
            plasma_status=plasma.get("status"),
            release_promotion=plasma.get("release_promotion"),
        )
        structural_pass = False
        stable_eligible = False
    else:
        add_check(
            checks,
            "project-state-not-overpromoted",
            True,
            "Project state does not overclaim stable promotion.",
            plasma_status=plasma.get("status"),
            release_promotion=plasma.get("release_promotion"),
        )

    if not structural_pass or failed_gates:
        status = "fail"
        recommended_state = "release-candidate-hold"
    elif stable_eligible:
        status = "promotion-ready"
        recommended_state = "stable-promoted"
    else:
        status = "hold"
        recommended_state = "release-candidate-hold"

    return {
        "schema": "screensave.plasma-v2.instrument-architecture-audit.v1",
        "status": status,
        "candidate_id": "plasma-v2-rc1",
        "stable_eligible": stable_eligible,
        "recommended_state": recommended_state,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "decision": {
            "instrument_architecture": status,
            "blocking_gates": blocking_gates,
            "failed_gates": failed_gates,
            "opened_next": "plasma-v2-stable-repair" if not stable_eligible else "plasma-v2-publication-prep",
        },
        "claim_boundary": "Instrument-architecture audit only; it does not publish Plasma, certify compatibility, accept final artistic quality, or admit AIDE as runtime truth.",
        "checks": checks,
        "gates": gates,
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Instrument-Architecture Audit",
        "",
        f"- Status: {report.get('status')}",
        f"- Candidate: {report.get('candidate_id')}",
        f"- Stable eligible: {report.get('stable_eligible')}",
        f"- Recommended state: {report.get('recommended_state')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Blocking Gates",
        "",
    ]
    blocking = report.get("decision", {}).get("blocking_gates", [])
    if blocking:
        for gate_id in blocking:
            lines.append(f"- {gate_id}")
    else:
        lines.append("- none")
    lines.extend(["", "## Gates", ""])
    for item in report.get("gates", []):
        lines.append(f"- {item.get('status')}: {item.get('id')} - {item.get('summary')}")
    lines.extend(["", "## Structural Checks", ""])
    for check in report.get("checks", []):
        lines.append(f"- {check.get('status')}: {check.get('id')} - {check.get('summary')}")
    lines.append("")
    return "\n".join(lines)


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    REPORT_MD.write_text(report_markdown(report), encoding="utf-8")


def main() -> int:
    report = build_report()
    write_report(report)
    print(f"Plasma v2 instrument architecture audit {report.get('status')}")
    return 0 if report.get("status") in {"promotion-ready", "hold"} else 1


if __name__ == "__main__":
    raise SystemExit(main())
