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
PRODUCT_CONSTITUTION = ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-instrument-constitution.md"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion"
REPORT_JSON = REPORT_DIR / "instrument-architecture-audit.json"
REPORT_MD = REPORT_DIR / "instrument-architecture-audit.md"
V2_ISLAND = ROOT / "products" / "savers" / "plasma" / "src" / "v2"
PLASMA_SRC = ROOT / "products" / "savers" / "plasma" / "src"
INSTRUMENT_AUDIT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
PRODUCT_CENTER_REPORT = INSTRUMENT_AUDIT_DIR / "product-center-report.json"
LEGACY_BOUNDARY_REPORT = INSTRUMENT_AUDIT_DIR / "legacy-boundary-report.json"
VISUALINTENT_SPEC_REDUCTION_REPORT = INSTRUMENT_AUDIT_DIR / "visualintent" / "spec-reduction-report.json"
VISUALINTENT_PROOF_SUMMARY = INSTRUMENT_AUDIT_DIR / "visualintent" / "proof-summary.json"

SUBCHECKS = [
    ("direct-spec", ["tools/scripts/check_plasma_v2_direct_spec.py"]),
    ("spec-v2-authority", ["tools/scripts/check_plasma_spec_v2.py"]),
    ("plan-contract", ["tools/scripts/check_plasma_v2_plan.py"]),
    ("runtime-contract", ["tools/scripts/check_plasma_v2_runtime.py"]),
    ("runtime-hot-loop-hazards", ["tools/scripts/check_plasma_v2_no_hot_loop_hazards.py"]),
    ("field-pipeline", ["tools/scripts/check_plasma_v2_field_pipeline.py"]),
    ("legacy-migration", ["tools/scripts/check_plasma_v2_migration.py"]),
    ("product-center-boundary", ["tools/scripts/check_plasma_product_center.py"]),
    ("legacy-boundary", ["tools/scripts/check_plasma_legacy_boundary.py"]),
    ("legacy-core-boundaries", ["tools/scripts/check_plasma_core_boundaries.py"]),
    ("direct-control-influence", ["tools/scripts/check_plasma_v2_influence.py"]),
    ("material-response", ["tools/scripts/check_plasma_v2_material_response.py"]),
    ("material-treatment", ["tools/scripts/check_plasma_v2_materials.py"]),
    ("acceleration-optionality", ["tools/scripts/check_plasma_v2_acceleration.py"]),
    ("workbench-inspection", ["tools/scripts/check_plasma_v2_workbench_inspection.py"]),
    ("packc-data-only", ["tools/scripts/check_packc.py"]),
    ("visualintent-contract", ["tools/scripts/check_visual_intent_contract.py"]),
    ("visualintent-resolver", ["tools/scripts/check_visual_intent_resolver.py"]),
    ("visualintent-proof", ["tools/scripts/check_plasma_v2_visualintent_proof.py"]),
    ("aide-boundary", ["tools/scripts/check_aide_evidence_bridge.py"]),
    ("aide-instrument-repair", ["tools/scripts/check_plasma_v2_aide_repair_evidence.py"]),
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
    "workbench_inspection_passes",
    "packc_data_only_passes",
    "visualintent_candidates_reduce_to_plasma_spec",
    "aide_not_runtime_or_truth",
]


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    if not path.exists():
        return {}
    return json.loads(path.read_text(encoding="utf-8"))


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

    for path in (CONTRACT, STABLE_CONTRACT, PAW_I, PRODUCT_CONSTITUTION):
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
            "bounded deterministic Plasma candidate",
            "explicit plasma_v2_spec",
            "candidate-level claim",
            "network/model/provider calls",
            "aide_not_runtime_or_truth",
            "products/savers/plasma/docs/plasma-v2-instrument-constitution.md",
            "plasma-v2-instrument-repair",
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

    constitution_ok, constitution_missing = text_contains(
        PRODUCT_CONSTITUTION,
        [
            "Status: active product constitution for PAW-I-R.",
            "Plasma is not a preset picker. Plasma is a visual instrument.",
            "`plasma_v2_spec` is product meaning.",
            "`plasma_v2_plan` is resolved executable truth.",
            "`plasma_v2_runtime` owns deterministic state and buffers.",
            "The software/reference path is canonical.",
            "AIDE does not own product truth",
        ],
    )
    add_check(
        checks,
        "product-constitution-text",
        constitution_ok,
        "Product-local instrument constitution records the PAW-I-R truth boundary.",
        missing=constitution_missing,
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

    product_center_report = load_json(PRODUCT_CENTER_REPORT)
    legacy_boundary_report = load_json(LEGACY_BOUNDARY_REPORT)
    visualintent_spec_report = load_json(VISUALINTENT_SPEC_REDUCTION_REPORT)
    visualintent_proof_summary = load_json(VISUALINTENT_PROOF_SUMMARY)
    add_check(
        checks,
        "product-center-report-outcome",
        product_center_report.get("status") in {"promotion-ready", "hold"},
        "Product-center boundary report is present and has a valid audit status.",
        status=product_center_report.get("status"),
        blocking_reasons=product_center_report.get("blocking_reasons", []),
    )
    add_check(
        checks,
        "legacy-boundary-report-outcome",
        legacy_boundary_report.get("status") in {"promotion-ready", "hold"},
        "Legacy boundary report is present and has a valid audit status.",
        status=legacy_boundary_report.get("status"),
        blocking_reasons=legacy_boundary_report.get("blocking_reasons", []),
    )
    add_check(
        checks,
        "visualintent-spec-reduction-report-outcome",
        visualintent_spec_report.get("status") == "pass"
        and visualintent_spec_report.get("visualintent_candidates_reduce_to_plasma_spec") is True
        and visualintent_spec_report.get("all_candidates_packc_valid") is True
        and visualintent_spec_report.get("all_candidates_have_plasma_v2_spec") is True
        and visualintent_spec_report.get("executable_payloads_detected") is False
        and visualintent_spec_report.get("network_or_model_calls") is False,
        "VisualIntent spec-reduction report proves bounded candidates reduce to Plasma v2 specs.",
        status=visualintent_spec_report.get("status"),
        fixtures_checked=visualintent_spec_report.get("fixtures_checked"),
        candidate_count_per_fixture=visualintent_spec_report.get("candidate_count_per_fixture"),
    )
    add_check(
        checks,
        "visualintent-proof-summary-outcome",
        visualintent_proof_summary.get("status") == "pass"
        and visualintent_proof_summary.get("candidate_specs_are_deterministic") is True
        and visualintent_proof_summary.get("candidate_specs_compile") is True
        and visualintent_proof_summary.get("proof_status") == "packc-and-spec-proof-only"
        and visualintent_proof_summary.get("all_candidate_capture_refs_generated") is False,
        "VisualIntent proof summary records deterministic spec/pack proof and explicit capture limits.",
        status=visualintent_proof_summary.get("status"),
        proof_status=visualintent_proof_summary.get("proof_status"),
        proof_profile=visualintent_proof_summary.get("proof_profile"),
    )

    gate(
        gates,
        "plasma_v2_spec_contract_passes",
        "pass" if subcheck_status(subchecks, "direct-spec") and subcheck_status(subchecks, "spec-v2-authority") else "fail",
        "The direct-control Plasma v2 spec island exists, carries semantic authority, and passes C89 smoke checks.",
        evidence=[
            "products/savers/plasma/src/v2/plasma_v2_types.h",
            "products/savers/plasma/src/v2/plasma_v2_spec.h",
            "products/savers/plasma/src/v2/plasma_v2_spec.c",
            "tools/scripts/check_plasma_v2_direct_spec.py",
            "tools/scripts/check_plasma_spec_v2.py",
        ],
    )

    plan_files = [V2_ISLAND / "plasma_v2_plan.h", V2_ISLAND / "plasma_v2_plan.c"]
    gate(
        gates,
        "plasma_v2_plan_contract_passes",
        "pass" if all(path.exists() for path in plan_files) and subcheck_status(subchecks, "plan-contract") else "hold",
        "The direct v2 plan contract must exist in the v2 island and pass the degradation-law checker before stable promotion.",
        missing=[repo_path(path) for path in plan_files if not path.exists()],
        evidence=["tools/scripts/check_plasma_v2_plan.py"],
    )

    runtime_files = [V2_ISLAND / "plasma_v2_runtime.h", V2_ISLAND / "plasma_v2_runtime.c"]
    gate(
        gates,
        "plasma_v2_runtime_contract_passes",
        "pass"
        if all(path.exists() for path in runtime_files)
        and subcheck_status(subchecks, "runtime-contract")
        and subcheck_status(subchecks, "runtime-hot-loop-hazards")
        else "hold",
        "The direct v2 runtime contract must exist, own deterministic buffers, and pass hot-loop hazard checks before stable promotion.",
        missing=[repo_path(path) for path in runtime_files if not path.exists()],
        evidence=[
            "tools/scripts/check_plasma_v2_runtime.py",
            "tools/scripts/check_plasma_v2_no_hot_loop_hazards.py",
        ],
    )

    legacy_ready = (
        product_center_report.get("stable_eligible") is True
        and legacy_boundary_report.get("legacy_authority_removed") is True
    )
    legacy_structural = (
        subcheck_status(subchecks, "product-center-boundary")
        and subcheck_status(subchecks, "legacy-boundary")
        and product_center_report.get("status") in {"promotion-ready", "hold"}
        and legacy_boundary_report.get("status") in {"promotion-ready", "hold"}
    )
    gate(
        gates,
        "legacy_preset_authority_removed",
        "pass" if legacy_ready else "hold" if legacy_structural else "fail",
        "Legacy preset/theme files must be migration inputs or wrappers over the direct v2 center, not hidden runtime authority.",
        migration_check="pass" if subcheck_status(subchecks, "legacy-migration") else "fail",
        migration_evidence=["tools/scripts/check_plasma_v2_migration.py"],
        product_center_status=product_center_report.get("status"),
        legacy_boundary_status=legacy_boundary_report.get("status"),
        product_center_blockers=product_center_report.get("blocking_reasons", []),
        legacy_boundary_blockers=legacy_boundary_report.get("blocking_reasons", []),
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
        "pass" if all(path.exists() for path in field_dirs) and subcheck_status(subchecks, "field-pipeline") else "hold",
        "Field and output boundaries must be explicit and pass the direct field-to-presentation pipeline checker before stable promotion.",
        missing=[repo_path(path) for path in field_dirs if not path.exists()],
        evidence=["tools/scripts/check_plasma_v2_field_pipeline.py"],
    )

    gate(
        gates,
        "material_mapping_distinctness_passes",
        "pass" if subcheck_status(subchecks, "material-response") and subcheck_status(subchecks, "material-treatment") else "fail",
        "Stable materials have distinct deterministic visual response in both the direct v2 island and the existing proof path.",
        evidence=["tools/scripts/check_plasma_v2_material_response.py", "tools/scripts/check_plasma_v2_materials.py"],
    )

    gate(
        gates,
        "treatment_boundaries_pass",
        "pass"
        if (PLASMA_SRC / "plasma_treatment.h").exists()
        and subcheck_status(subchecks, "material-response")
        and subcheck_status(subchecks, "material-treatment")
        else "fail",
        "Treatment remains post-material and bounded for the stable slice.",
        evidence=[
            repo_path(PLASMA_SRC / "plasma_treatment.h"),
            "tools/scripts/check_plasma_v2_material_response.py",
            "tools/scripts/check_plasma_v2_materials.py",
        ],
    )

    gate(
        gates,
        "software_reference_is_canonical",
        "pass" if subcheck_status(subchecks, "legacy-core-boundaries") and subcheck_status(subchecks, "acceleration-optionality") else "fail",
        "Software/reference execution remains the canonical proof-bearing path.",
        evidence=["tools/scripts/check_plasma_core_boundaries.py", "tools/scripts/check_plasma_v2_acceleration.py"],
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
        "pass" if gl_doc_ok and subcheck_status(subchecks, "acceleration-optionality") else "fail",
        "GDI remains the floor and GL11 remains optional.",
        missing=gl_missing,
        evidence=["tools/scripts/check_plasma_v2_acceleration.py"],
    )

    gate(
        gates,
        "workbench_inspection_passes",
        "pass" if subcheck_status(subchecks, "workbench-inspection") else "fail",
        "Workbench must inspect the real Plasma v2 instrument evidence without becoming runtime truth or promotion authority.",
        evidence=["tools/scripts/check_plasma_v2_workbench_inspection.py"],
    )

    gate(
        gates,
        "packc_data_only_passes",
        "pass" if subcheck_status(subchecks, "packc-data-only") else "fail",
        "packc validates data-only Plasma packs and rejects executable or unsafe payloads.",
        evidence=["tools/scripts/check_packc.py"],
    )

    visualintent_ready = (
        subcheck_status(subchecks, "visualintent-contract")
        and subcheck_status(subchecks, "visualintent-resolver")
        and subcheck_status(subchecks, "visualintent-proof")
        and subcheck_status(subchecks, "packc-data-only")
        and subcheck_status(subchecks, "workbench-inspection")
        and visualintent_spec_report.get("status") == "pass"
        and visualintent_spec_report.get("visualintent_candidates_reduce_to_plasma_spec") is True
        and visualintent_spec_report.get("all_candidates_packc_valid") is True
        and visualintent_spec_report.get("all_candidates_have_plasma_v2_spec") is True
        and visualintent_spec_report.get("executable_payloads_detected") is False
        and visualintent_spec_report.get("network_or_model_calls") is False
        and visualintent_proof_summary.get("status") == "pass"
        and visualintent_proof_summary.get("candidate_specs_are_deterministic") is True
        and visualintent_proof_summary.get("candidate_specs_compile") is True
    )
    visualintent_structural = (
        subcheck_status(subchecks, "visualintent-contract")
        and subcheck_status(subchecks, "visualintent-resolver")
        and subcheck_status(subchecks, "visualintent-proof")
        and visualintent_spec_report.get("status") in {"pass", "blocked"}
        and visualintent_proof_summary.get("status") in {"pass", "blocked"}
    )
    gate(
        gates,
        "visualintent_candidates_reduce_to_plasma_spec",
        "pass" if visualintent_ready else "hold" if visualintent_structural else "fail",
        "VisualIntent candidates reduce through a bounded local resolver into explicit Plasma v2 specs, packc data, and preview proof evidence.",
        evidence=[
            "contracts/visual_intent_v1.md",
            "tools/scripts/check_visual_intent_resolver.py",
            "tools/scripts/check_plasma_v2_visualintent_proof.py",
            "validation/captures/plasma-v2/instrument-audit/visualintent/spec-reduction-report.json",
            "validation/captures/plasma-v2/instrument-audit/visualintent/proof-summary.json",
            "validation/captures/plasma-v2/instrument-audit/workbench-inspection.json",
        ],
        claim_boundary="VisualIntent candidates are bounded data/spec review inputs only; not runtime truth, artistic acceptance, compatibility certification, or stable promotion.",
    )

    gate(
        gates,
        "aide_not_runtime_or_truth",
        "pass"
        if subcheck_status(subchecks, "aide-boundary")
        and subcheck_status(subchecks, "aide-instrument-repair")
        else "fail",
        "AIDE remains an evidence and repair coordinator, not product runtime, artistic judge, or truth authority.",
        evidence=["tools/scripts/check_aide_evidence_bridge.py", "tools/scripts/check_plasma_v2_aide_repair_evidence.py"],
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
            "opened_next": "plasma-v2-instrument-repair" if not stable_eligible else "plasma-v2-publication-prep",
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
