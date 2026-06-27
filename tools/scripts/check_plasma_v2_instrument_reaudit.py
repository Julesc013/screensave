"""Record the PAW-I-R1 Plasma v2 instrument re-audit outcome."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
CAPABILITIES = ROOT / "tools" / "project_adapter" / "capability_bindings.json"
AUDIT_JSON = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "instrument-architecture-audit.json"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "instrument-audit"
REPORT_JSON = REPORT_DIR / "re-audit-report.json"
REPORT_MD = REPORT_DIR / "re-audit-report.md"

PASS_REQUIRED_GATES = {
    "plasma_v2_spec_contract_passes",
    "plasma_v2_plan_contract_passes",
    "plasma_v2_runtime_contract_passes",
    "field_pipeline_boundaries_pass",
    "direct_controls_influence_passes",
    "material_mapping_distinctness_passes",
    "treatment_boundaries_pass",
    "software_reference_is_canonical",
    "gl11_is_not_hidden_minimum",
    "workbench_inspection_passes",
    "packc_data_only_passes",
    "aide_not_runtime_or_truth",
}

CONTAINMENT_GATES = {
    "legacy_preset_authority_removed",
    "visualintent_candidates_reduce_to_plasma_spec",
}

FORBIDDEN_CAPABILITIES = {
    "screensave.command",
    "screensave.run",
    "screensave.exec",
    "screensave.release.publish",
    "screensave.promote.stable.unchecked",
    "screensave.agent.apply",
}


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def git_text(args: list[str]) -> str:
    try:
        return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return "unknown"


def run_instrument_audit() -> dict[str, Any]:
    result = subprocess.run(
        [sys.executable, "tools/scripts/check_plasma_instrument_architecture.py"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=300,
        check=False,
    )
    return {
        "command": [sys.executable, "tools/scripts/check_plasma_instrument_architecture.py"],
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "status": "pass" if result.returncode == 0 else "fail",
    }


def add_check(checks: list[dict[str, Any]], check_id: str, status: str, summary: str, **data: Any) -> None:
    checks.append(
        {
            "id": check_id,
            "status": status,
            "summary": summary,
            "data": data,
        }
    )


def gate_map(audit: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {str(item.get("id")): item for item in audit.get("gates", [])}


def capability_names() -> set[str]:
    bindings = load_json(CAPABILITIES)
    return {str(item.get("name")) for item in bindings.get("capabilities", [])}


def build_report() -> dict[str, Any]:
    audit_run = run_instrument_audit()
    checks: list[dict[str, Any]] = []
    add_check(
        checks,
        "instrument-audit-rerun",
        "pass" if audit_run["returncode"] == 0 else "fail",
        "Authoritative Plasma v2 instrument audit reran for PAW-I-R1.",
        run=audit_run,
    )

    audit = load_json(AUDIT_JSON) if AUDIT_JSON.exists() else {}
    gates = gate_map(audit)
    state = load_toml(STATE)
    plasma = state.get("plasma_v2", {})

    audit_status = str(audit.get("status", "missing"))
    failed_gates = list(audit.get("decision", {}).get("failed_gates", []))
    blocking_gates = list(audit.get("decision", {}).get("blocking_gates", []))
    add_check(
        checks,
        "instrument-audit-outcome",
        "pass" if audit_status in {"promotion-ready", "hold"} and not failed_gates else "fail",
        "Instrument audit is either promotion-ready or an explicit non-promotion hold.",
        audit_status=audit_status,
        stable_eligible=audit.get("stable_eligible"),
        blocking_gates=blocking_gates,
        failed_gates=failed_gates,
    )

    for gate_id in sorted(PASS_REQUIRED_GATES):
        gate = gates.get(gate_id, {})
        add_check(
            checks,
            f"gate:{gate_id}",
            "pass" if gate.get("status") == "pass" else "fail",
            "Required instrument gate passes.",
            gate_status=gate.get("status"),
            gate_summary=gate.get("summary"),
        )

    for gate_id in sorted(CONTAINMENT_GATES):
        gate = gates.get(gate_id, {})
        gate_status = str(gate.get("status", "missing"))
        status = "pass" if gate_status == "pass" else "hold" if gate_status == "hold" else "fail"
        add_check(
            checks,
            f"gate:{gate_id}",
            status,
            "Gate is either admitted for stable promotion or explicitly blocking the hold.",
            gate_status=gate_status,
            gate_summary=gate.get("summary"),
            data=gate.get("data", {}),
        )

    present_forbidden = sorted(name for name in capability_names() if name in FORBIDDEN_CAPABILITIES)
    add_check(
        checks,
        "no-generic-command-capability",
        "pass" if not present_forbidden else "fail",
        "Project adapter exposes no generic command, publication, unchecked promotion, or agent-apply capability.",
        present=present_forbidden,
    )

    state_holds = (
        plasma.get("status") == "release-candidate-hold"
        and plasma.get("stable") is False
        and plasma.get("release_promotion") == "blocked"
    )
    add_check(
        checks,
        "no-stable-promotion-yet",
        "pass" if state_holds else "fail",
        "Project state remains a release-candidate hold until the stable-promotion gate admits promotion.",
        plasma_status=plasma.get("status"),
        stable=plasma.get("stable"),
        release_promotion=plasma.get("release_promotion"),
        opened_next=plasma.get("opened_next"),
    )

    failed_checks = [item["id"] for item in checks if item["status"] == "fail"]
    held_checks = [item["id"] for item in checks if item["status"] == "hold"]
    if failed_checks:
        status = "fail"
        recommended_state = "release-candidate-hold"
    elif held_checks or audit.get("stable_eligible") is not True:
        status = "hold"
        recommended_state = "release-candidate-hold"
    else:
        status = "promotion-ready"
        recommended_state = "stable-promoted"

    return {
        "schema": "screensave.plasma-v2.instrument-reaudit.v1",
        "status": status,
        "candidate_id": "plasma-v2-rc1",
        "recommended_state": recommended_state,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "decision": {
            "instrument_architecture": audit_status,
            "stable_eligible": audit.get("stable_eligible"),
            "blocking_gates": blocking_gates,
            "failed_gates": failed_gates,
            "held_checks": held_checks,
            "failed_checks": failed_checks,
            "opened_next": "plasma-v2-publication-prep" if status == "promotion-ready" else "plasma-v2-instrument-repair",
        },
        "claim_boundary": "PAW-I-R1 instrument re-audit only; it does not publish, certify compatibility, accept artistic quality, or promote stable release.",
        "checks": checks,
        "instrument_audit_ref": "validation/captures/plasma-v2/stable-promotion/instrument-architecture-audit.json",
    }


def report_markdown(report: dict[str, Any]) -> str:
    decision = report.get("decision", {})
    lines = [
        "# Plasma v2 Instrument Re-Audit",
        "",
        f"- Status: {report.get('status')}",
        f"- Candidate: {report.get('candidate_id')}",
        f"- Recommended state: {report.get('recommended_state')}",
        f"- Instrument architecture: {decision.get('instrument_architecture')}",
        f"- Stable eligible: {decision.get('stable_eligible')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Blocking Gates",
        "",
    ]
    blocking = decision.get("blocking_gates", [])
    if blocking:
        for gate_id in blocking:
            lines.append(f"- {gate_id}")
    else:
        lines.append("- none")
    lines.extend(["", "## Checks", ""])
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
    print(f"Plasma v2 instrument re-audit {report.get('status')}")
    return 0 if report.get("status") in {"promotion-ready", "hold"} else 1


if __name__ == "__main__":
    raise SystemExit(main())
