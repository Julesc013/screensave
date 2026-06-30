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
HOLD_REPORT = REPORT_DIR / "hold-report.json"
REPAIR_BURN = REPORT_DIR / "repair-burndown.json"
EVIDENCE = REPORT_DIR / "proof-bundle-v1.json"
INSTRUMENT_AUDIT = REPORT_DIR / "instrument-architecture-audit.json"
VISUAL_REJECTION = ROOT / "validation" / "captures" / "plasma-v2" / "visual-rejection" / "verdict.toml"

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
    is_promoted_transition = state_status in {"stable-promoted", "publication-ready"}
    is_visual_hold_transition = state_status == "publication-hold"
    expected_promoted_program = {
        "stable-promoted": "plasma-v2-publication-prep",
        "publication-ready": "plasma-v2-publication",
    }.get(str(state_status))
    add_check(
        checks,
        "project-state-transition",
        (
            (
                (is_pre_transition or is_hold_transition)
                and plasma.get("stable") is False
                and plasma.get("release_promotion") == "blocked"
            )
            or (
                is_promoted_transition
                and plasma.get("stable") is True
                and plasma.get("release_promotion") == "accepted"
                and authority.get("active_program") == expected_promoted_program
                and development.get("active_program") == expected_promoted_program
            )
            or (
                is_visual_hold_transition
                and plasma.get("stable") is False
                and plasma.get("release_promotion") == "withdrawn-for-visual-quality"
                and plasma.get("current_product_verdict") == "visual-rejected"
                and authority.get("active_program") == "plasma-v3-visual-core-spike"
                and development.get("active_program") == "plasma-v3-visual-core-spike"
            )
        )
        and plasma.get("release_candidate") == "plasma-v2-rc1"
        and authority.get("release_candidate") == "plasma-v2-rc1",
        "Project state is a release candidate, a valid hold, or an accepted post-stable transition.",
        plasma_status=state_status,
        active_program=authority.get("active_program"),
        development_active_program=development.get("active_program"),
    )

    active_visual_rejection = False
    if VISUAL_REJECTION.exists():
        verdict = load_toml(VISUAL_REJECTION)
        visual = verdict.get("visual_verdict", {})
        active_visual_rejection = (
            visual.get("decision") == "reject-publication"
            and visual.get("current_product_verdict") == "visual-rejected"
            and visual.get("publication") == "not-published"
        )
        add_check(
            checks,
            "visual-rejection-blocker",
            not active_visual_rejection,
            "Active real-display visual rejection blocks Plasma v2 stable promotion and publication lineage.",
            verdict=str(VISUAL_REJECTION.relative_to(ROOT)),
            decision=visual.get("decision"),
            current_product_verdict=visual.get("current_product_verdict"),
            reasons=visual.get("reasons", []),
        )
    else:
        add_check(checks, "visual-rejection-blocker", True, "No real-display visual rejection verdict is recorded.")

    if active_visual_rejection:
        add_check(
            checks,
            "stable-promotion-deep-subchecks",
            False,
            "Deep stable-promotion subchecks skipped because active real-display visual rejection already blocks promotion.",
        )
    else:
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

    if not accepted:
        hold_report = load_json(HOLD_REPORT) if HOLD_REPORT.exists() else {}
        blocker_ids = {str(item.get("id")) for item in hold_report.get("exact_blockers", []) if isinstance(item, dict)}
        add_check(
            checks,
            "hold-report-recorded",
            hold_report.get("status") == "hold"
            and hold_report.get("final_artistic_decision") == decision_state
            and hold_report.get("stable") is False
            and hold_report.get("release_promotion") == "blocked"
            and hold_report.get("repair_workunits") == ["SS-PLV2-I-REPAIR-001"]
            and "final_stable_artistic_acceptance_not_accepted" in blocker_ids
            and "missing_project_owned_accepted_for_stable_verdict" in blocker_ids,
            "Stable-promotion hold report records the exact final artistic decision blocker.",
            hold_report_ref=str(HOLD_REPORT.relative_to(ROOT)),
            blocker_ids=sorted(blocker_ids),
        )

    publication_dir = ROOT / "releases" / "plasma-v2-stable"
    publication_manifest = publication_dir / "release-manifest.toml"
    publication_payload = load_toml(publication_manifest) if publication_manifest.exists() else {}
    publication_state_ok = (
        not publication_dir.exists()
        or (
            publication_manifest.exists()
            and publication_payload.get("publication_status") == "not-published"
            and publication_payload.get("publication", {}).get("public_upload") is False
            and publication_payload.get("publication", {}).get("release_page_published") is False
            and publication_payload.get("publication", {}).get("github_release_created") is False
        )
    )
    add_check(
        checks,
        "publication-boundary",
        publication_state_ok,
        "Any stable publication packet remains local, non-published, and upload-free.",
        release_dir=str(publication_dir.relative_to(ROOT)),
        publication_status=publication_payload.get("publication_status"),
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
        "claim_boundary": "Stable-promotion gate report only; it does not publish a release, certify compatibility, accept artistic quality automatically, admit AIDE source mutation, or override a real-display human visual verdict.",
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
