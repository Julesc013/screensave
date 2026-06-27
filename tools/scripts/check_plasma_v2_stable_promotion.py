"""Check the Plasma v2 stable-promotion decision gate."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion"
REPORT_JSON = REPORT_DIR / "gate-report.json"
REPORT_MD = REPORT_DIR / "gate-report.md"
DECISION = ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision" / "decision.stable.toml"
REPAIR_BURN = REPORT_DIR / "repair-burndown.json"
EVIDENCE = REPORT_DIR / "proof-bundle-v1.json"
INSTRUMENT_AUDIT = REPORT_DIR / "instrument-architecture-audit.json"

SUBCHECKS = [
    ("release-candidate-gate", ["tools/scripts/check_plasma_v2_release_candidate.py"]),
    ("stable-promotion-contract", ["tools/scripts/check_plasma_v2_stable_promotion_contract.py"]),
    ("instrument-architecture-audit", ["tools/scripts/check_plasma_instrument_architecture.py"]),
    ("package-stage", ["tools/scripts/check_plasma_v2_package_stage.py"]),
    ("stable-promotion-evidence", ["tools/scripts/check_plasma_v2_stable_promotion_evidence.py"]),
    ("final-artistic-decision", ["tools/scripts/check_plasma_v2_final_artistic_acceptance.py"]),
    ("release-candidate-support", ["tools/scripts/check_support_claims.py"]),
    ("stable-support", ["tools/scripts/check_plasma_v2_stable_support_claims.py"]),
    ("release-provenance", ["tools/scripts/check_release_provenance.py"]),
    ("release-security", ["tools/scripts/check_release_security.py"]),
    ("stable-review", ["tools/scripts/check_plasma_v2_stable_review.py"]),
    ("aide-evidence-bridge", ["tools/scripts/check_aide_evidence_bridge.py"]),
    ("aide-evidence-index", ["tools/scripts/check_aide_evidence_index.py"]),
    ("repair-queue", ["tools/scripts/check_repair_queue.py"]),
    ("project-adapter", ["tools/scripts/check_project_adapter.py"]),
]


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
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


def capability_names() -> set[str]:
    bindings = load_json(ROOT / "tools" / "project_adapter" / "capability_bindings.json")
    return {str(item.get("name")) for item in bindings.get("capabilities", [])}


def build_report() -> dict[str, Any]:
    checks: list[dict[str, Any]] = []
    state = load_toml(STATE)
    plasma = state.get("plasma_v2", {})
    authority = state.get("authority", {})
    development = state.get("development", {})
    decision = load_toml(DECISION) if DECISION.exists() else {}
    repair_burn = load_json(REPAIR_BURN) if REPAIR_BURN.exists() else {}
    proof_bundle = load_json(EVIDENCE) if EVIDENCE.exists() else {}
    instrument_audit = load_json(INSTRUMENT_AUDIT) if INSTRUMENT_AUDIT.exists() else {}

    state_status = plasma.get("status")
    is_pre_transition = state_status == "release-candidate"
    is_hold_transition = state_status in {"release-candidate-hold", "request-changes", "defer-to-labs"}
    add_check(
        checks,
        "project-state-before-or-hold",
        (is_pre_transition or is_hold_transition)
        and plasma.get("stable") is False
        and plasma.get("release_promotion") == "blocked"
        and plasma.get("release_candidate") == "plasma-v2-rc1"
        and authority.get("release_candidate") == "plasma-v2-rc1",
        "Project state is a release candidate before the decision or a non-stable hold after the decision.",
        plasma_status=state_status,
        active_program=authority.get("active_program"),
        development_active_program=development.get("active_program"),
    )

    for check_id, command in SUBCHECKS:
        result = run(command)
        add_check(
            checks,
            check_id,
            result.get("returncode") == 0,
            "Command passed." if result.get("returncode") == 0 else "Command failed.",
            run=result,
        )

    names = capability_names()
    required_capabilities = {
        "screensave.plasma.v2.stable-promotion.check",
        "screensave.plasma.v2.stable-promotion.proof",
        "screensave.plasma.v2.stable-promotion.package",
        "screensave.plasma.v2.stable-promotion.support",
        "screensave.plasma.v2.stable-promotion.security",
        "screensave.plasma.v2.stable-promotion.review",
    }
    forbidden_capabilities = {
        "screensave.command",
        "screensave.run",
        "screensave.exec",
        "screensave.release.publish",
        "screensave.promote.stable.unchecked",
        "screensave.agent.apply",
    }
    add_check(
        checks,
        "fixed-stable-capabilities",
        required_capabilities <= names,
        "Project adapter exposes only fixed stable-promotion capabilities.",
        missing=sorted(required_capabilities - names),
    )
    present_forbidden = sorted(name for name in names if name in forbidden_capabilities)
    add_check(
        checks,
        "no-generic-or-publication-capability",
        not present_forbidden,
        "Project adapter does not expose generic command, publication, unchecked promotion, or agent apply capabilities.",
        present=present_forbidden,
    )

    decision_state = decision.get("decision_state")
    blocking_count = int(repair_burn.get("open_blocking_count", 999))
    accepted = decision_state == "accepted-for-stable"
    instrument_status = instrument_audit.get("status")
    instrument_ready = instrument_audit.get("stable_eligible") is True
    add_check(
        checks,
        "artistic-decision-outcome",
        accepted or decision_state in {"request-changes", "defer-to-labs", "rejected"},
        "Final artistic decision is either accepted-for-stable or a valid non-promotion outcome.",
        decision_state=decision_state,
    )
    add_check(
        checks,
        "repair-queue-outcome",
        (accepted and blocking_count == 0) or ((not accepted) and blocking_count > 0),
        "Repair queue outcome matches the artistic decision.",
        open_blocking_count=blocking_count,
        blocking_repairs=repair_burn.get("blocking_repairs", []),
    )
    add_check(
        checks,
        "instrument-architecture-outcome",
        instrument_ready or instrument_status == "hold",
        "Instrument audit is either promotion-ready or a valid non-promotion hold.",
        instrument_status=instrument_status,
        stable_eligible=instrument_audit.get("stable_eligible"),
        blocking_gates=instrument_audit.get("decision", {}).get("blocking_gates", []),
    )
    axes = proof_bundle.get("result_axes", {})
    expected_promotion_status = "ready" if accepted else "blocked"
    proof_decision_axis_ok = (
        axes.get("artistic_decision", {}).get("status") == decision_state
        and axes.get("stable_promotion", {}).get("status") == expected_promotion_status
    )
    add_check(
        checks,
        "proof-bundle-decision-axis",
        proof_decision_axis_ok,
        "Proof bundle records the stable-promotion decision axis.",
        artistic_decision=axes.get("artistic_decision", {}),
        stable_promotion=axes.get("stable_promotion", {}),
    )

    publication_dir = ROOT / "releases" / "plasma-v2-stable"
    add_check(
        checks,
        "no-publication-packet",
        not publication_dir.exists(),
        "No stable publication packet has been created during the stable-promotion decision.",
        release_dir=str(publication_dir.relative_to(ROOT)),
    )

    structural_pass = all(item.get("status") == "pass" for item in checks)
    if not structural_pass:
        decision_status = "fail"
        recommended_state = "release-candidate-hold"
    elif accepted and blocking_count == 0 and instrument_ready:
        decision_status = "promotion-ready"
        recommended_state = "stable-promoted"
    else:
        decision_status = "hold"
        recommended_state = "release-candidate-hold"

    return {
        "schema": "screensave.plasma-v2.stable-promotion.gate-report.v1",
        "status": decision_status,
        "candidate_id": "plasma-v2-rc1",
        "recommended_state": recommended_state,
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "decision": {
            "artistic_decision": decision_state,
            "instrument_architecture": instrument_status,
            "instrument_stable_eligible": instrument_ready,
            "instrument_blocking_gates": instrument_audit.get("decision", {}).get("blocking_gates", []),
            "open_blocking_repairs": blocking_count,
            "stable": accepted and blocking_count == 0 and instrument_ready,
            "release_promotion": "accepted" if accepted and blocking_count == 0 and instrument_ready else "blocked",
        },
        "claim_boundary": "Stable-promotion gate report only; it does not publish a release, certify compatibility, accept artistic quality automatically, or admit AIDE source mutation.",
        "checks": checks,
    }


def report_markdown(report: dict[str, Any]) -> str:
    lines = [
        "# Plasma v2 Stable-Promotion Gate Report",
        "",
        f"- Status: {report.get('status')}",
        f"- Candidate: {report.get('candidate_id')}",
        f"- Recommended state: {report.get('recommended_state')}",
        f"- Artistic decision: {report.get('decision', {}).get('artistic_decision')}",
        f"- Instrument architecture: {report.get('decision', {}).get('instrument_architecture')}",
        f"- Instrument stable eligible: {report.get('decision', {}).get('instrument_stable_eligible')}",
        f"- Open blocking repairs: {report.get('decision', {}).get('open_blocking_repairs')}",
        f"- Claim boundary: {report.get('claim_boundary')}",
        "",
        "## Checks",
        "",
    ]
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
    print(f"Plasma v2 stable-promotion gate {report.get('status')}")
    return 0 if report.get("status") in {"promotion-ready", "hold"} else 1


if __name__ == "__main__":
    raise SystemExit(main())
