"""Validate the Plasma v2 release-candidate gate."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
CAPABILITY_BINDINGS = ROOT / "tools" / "project_adapter" / "capability_bindings.json"
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate"
REPORT_JSON = REPORT_DIR / "gate-report.json"
REPORT_MD = REPORT_DIR / "gate-report.md"

SUBCHECKS = [
    ["tools/scripts/check_plasma_v2_release_candidate_contract.py"],
    ["tools/scripts/check_plasma_v2_package_stage.py"],
    ["tools/scripts/check_plasma_v2_release_candidate_evidence.py"],
    ["tools/scripts/check_plasma_v2_artistic_decision.py"],
    ["tools/scripts/check_support_claims.py"],
    ["tools/scripts/check_plasma_v2_release_candidate_review.py"],
    ["tools/scripts/check_agentic_policy.py"],
    ["tools/scripts/check_agentic_receipts.py"],
    ["tools/scripts/check_repair_queue.py"],
    ["tools/scripts/check_project_adapter.py"],
]

REQUIRED_PATHS = {
    "contract": "contracts/plasma_release_candidate_v1.md",
    "candidate_package": "packaging/windows/plasma-v2-release-candidate/manifest.toml",
    "candidate_checksums": "packaging/windows/plasma-v2-release-candidate/checksums.sha256",
    "proof_matrix": "validation/captures/plasma-v2/release-candidate/proof-matrix.json",
    "proof_bundle": "validation/captures/plasma-v2/release-candidate/proof-bundle-v1.json",
    "evidence_index": "validation/captures/plasma-v2/release-candidate/evidence-index.json",
    "support_claims": "validation/captures/plasma-v2/release-candidate/support-claims.json",
    "artistic_decision": "validation/captures/plasma-v2/final-artistic-decision/decision.release-candidate.toml",
    "manager_review": "validation/captures/plasma-v2/release-candidate/manager-review.json",
    "workbench_review": "validation/captures/plasma-v2/release-candidate/workbench-review.json",
    "repair_burndown": "validation/captures/plasma-v2/release-candidate/repair-burndown.json",
    "agentic_policy": ".aide/agentic/release-candidate-policy.toml",
}

REQUIRED_CAPABILITIES = {
    "screensave.plasma.v2.release-candidate.check",
    "screensave.plasma.v2.release-candidate.package",
    "screensave.plasma.v2.release-candidate.evidence",
    "screensave.plasma.v2.release-candidate.review",
}

FORBIDDEN_CAPABILITIES = {
    "screensave.command",
    "screensave.run",
    "screensave.exec",
    "screensave.release.publish",
    "screensave.promote.stable",
    "screensave.agent.apply",
}


def repo_path(path: pathlib.Path) -> str:
    return str(path.resolve().relative_to(ROOT)).replace("\\", "/")


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_subcheck(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(
        [sys.executable, *command],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=900,
    )
    return {
        "command": " ".join(command),
        "status": "pass" if result.returncode == 0 else "fail",
        "returncode": result.returncode,
        "stdout_tail": result.stdout.strip().splitlines()[-8:],
        "stderr_tail": result.stderr.strip().splitlines()[-8:],
    }


def add_result(results: list[dict[str, Any]], result_id: str, ok: bool, summary: str, **payload: Any) -> None:
    results.append(
        {
            "id": result_id,
            "status": "pass" if ok else "fail",
            "summary": summary,
            "payload": payload,
        }
    )


def validate_state(results: list[dict[str, Any]]) -> None:
    state = load_toml(STATE)
    authority = state.get("authority", {})
    plasma = state.get("plasma_v2", {})
    status = plasma.get("status")
    release_candidate = plasma.get("release_candidate", authority.get("release_candidate"))

    add_result(
        results,
        "portable-v2-accepted",
        state.get("portable_v2", {}).get("status") == "accepted",
        "Portable v2 must be accepted before Plasma release-candidate work.",
        observed=state.get("portable_v2", {}).get("status"),
    )
    add_result(
        results,
        "plasma-status",
        status in {"release-readiness-reviewed", "release-candidate", "release-candidate-hold"},
        "Plasma v2 must be at release-readiness-reviewed before transition, release-candidate after transition, or release-candidate-hold after stable-promotion review.",
        observed=status,
    )
    add_result(
        results,
        "plasma-not-stable",
        plasma.get("stable") is False and plasma.get("release_promotion") == "blocked",
        "Plasma v2 must remain not stable and release promotion must remain blocked.",
        stable=plasma.get("stable"),
        release_promotion=plasma.get("release_promotion"),
    )
    if status == "release-readiness-reviewed":
        add_result(
            results,
            "pre-transition-release-candidate",
            authority.get("release_candidate") == "none" and release_candidate in {None, "none"},
            "Before transition, the release candidate must still be none.",
            authority_release_candidate=authority.get("release_candidate"),
            plasma_release_candidate=release_candidate,
        )
        add_result(
            results,
            "pre-transition-active-program",
            authority.get("active_program") == "plasma-v2-release-candidate",
            "Before transition, the active program must be plasma-v2-release-candidate.",
            observed=authority.get("active_program"),
        )
    if status in {"release-candidate", "release-candidate-hold"}:
        expected_program = "plasma-v2-stable-repair" if status == "release-candidate-hold" else "plasma-v2-stable-promotion"
        add_result(
            results,
            "post-transition-release-candidate",
            authority.get("release_candidate") == "plasma-v2-rc1" and release_candidate == "plasma-v2-rc1",
            "After transition, rc1 must be recorded by authority and Plasma state.",
            authority_release_candidate=authority.get("release_candidate"),
            plasma_release_candidate=release_candidate,
        )
        add_result(
            results,
            "post-transition-active-program",
            authority.get("active_program") == expected_program,
            "After transition, the active program must match the current Plasma v2 decision lane.",
            observed=authority.get("active_program"),
            expected=expected_program,
        )


def validate_paths(results: list[dict[str, Any]]) -> None:
    for path_id, relative in REQUIRED_PATHS.items():
        path = ROOT / relative
        add_result(results, f"path-{path_id}", path.exists(), f"Required release-candidate input exists: {relative}")


def validate_artifacts(results: list[dict[str, Any]]) -> None:
    decision = load_toml(ROOT / REQUIRED_PATHS["artistic_decision"])
    add_result(
        results,
        "artistic-decision",
        decision.get("decision_state") == "accepted-for-release-candidate"
        and decision.get("boundaries", {}).get("stable_release") is False
        and decision.get("boundaries", {}).get("compatibility_certification") is False,
        "Artistic decision must accept release-candidate only and block stable/certification claims.",
        decision_state=decision.get("decision_state"),
    )

    support = load_json(ROOT / REQUIRED_PATHS["support_claims"])
    add_result(
        results,
        "support-claims",
        support.get("stable") is False
        and support.get("release_promotion") == "blocked"
        and support.get("compatibility_certification") == "not-claimed",
        "Support claims must remain not-stable, release-blocked, and uncertified.",
        support=support,
    )

    repair = load_json(ROOT / REQUIRED_PATHS["repair_burndown"])
    add_result(
        results,
        "repair-burndown",
        repair.get("open_blocking_count") == 0,
        "Repair queue must have no open blocking release-candidate repairs.",
        open_blocking_count=repair.get("open_blocking_count"),
    )

    package = load_toml(ROOT / REQUIRED_PATHS["candidate_package"])
    add_result(
        results,
        "package-not-published",
        package.get("published") is False
        and package.get("stable") is False
        and package.get("public_release") is False
        and package.get("release_promotion") == "blocked",
        "Release-candidate package must remain staged only.",
        package=package,
    )


def validate_capabilities(results: list[dict[str, Any]]) -> None:
    bindings = load_json(CAPABILITY_BINDINGS)
    names = {item.get("name") for item in bindings.get("capabilities", [])}
    add_result(
        results,
        "fixed-release-candidate-capabilities",
        REQUIRED_CAPABILITIES <= names,
        "Fixed release-candidate capabilities must be exposed.",
        missing=sorted(REQUIRED_CAPABILITIES - names),
    )
    add_result(
        results,
        "no-generic-or-promotion-capabilities",
        not bool(names & FORBIDDEN_CAPABILITIES),
        "Generic command, release, stable-promotion, and agent-apply capabilities must remain absent.",
        forbidden_hits=sorted(names & FORBIDDEN_CAPABILITIES),
    )


def scan_overclaims(results: list[dict[str, Any]]) -> None:
    paths = [
        ROOT / "PROJECT_STATE.toml",
        ROOT / "VERSION.toml",
        ROOT / "README.md",
        ROOT / "packaging" / "windows" / "plasma-v2-release-candidate" / "manifest.toml",
        ROOT / "validation" / "captures" / "plasma-v2" / "release-candidate" / "support-claims.json",
    ]
    forbidden = [
        "stable = true",
        "release_promotion = \"accepted\"",
        "compatibility_certification = \"certified\"",
        "public_release = true",
        "published = true",
        "stable-release-published",
        "screensave.release.publish",
        "screensave.promote.stable",
        "screensave.agent.apply",
    ]
    hits: list[str] = []
    for path in paths:
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="ignore").lower()
        for token in forbidden:
            if token in text:
                hits.append(f"{repo_path(path)} contains {token}")
    add_result(results, "no-release-overclaims", not hits, "No publication, stable, certification, or promotion overclaim may exist.", hits=hits)


def run_validators(results: list[dict[str, Any]]) -> None:
    for command in SUBCHECKS:
        run = run_subcheck(command)
        add_result(results, "validator-" + command[0].split("/")[-1].replace(".py", ""), run["status"] == "pass", "Required release-candidate validator must pass.", run=run)


def build_report() -> dict[str, Any]:
    results: list[dict[str, Any]] = []
    validate_state(results)
    validate_paths(results)
    validate_artifacts(results)
    validate_capabilities(results)
    scan_overclaims(results)
    run_validators(results)
    passed = all(result.get("status") == "pass" for result in results)
    return {
        "schema": "screensave.plasma-v2.release-candidate-gate.v1",
        "status": "pass" if passed else "fail",
        "candidate_id": "plasma-v2-rc1",
        "product": "plasma",
        "profile": "plasma.v2.reference.preview",
        "claim_boundary": "Release-candidate gate only; not stable release, release publication, compatibility certification, public SDK stability, all-saver migration, or automatic promotion.",
        "results": results,
    }


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Release-Candidate Gate",
        "",
        f"Status: {report.get('status')}",
        "",
        report.get("claim_boundary", ""),
        "",
        "| Check | Status | Summary |",
        "| --- | --- | --- |",
    ]
    for result in report.get("results", []):
        lines.append(f"| {result.get('id')} | {result.get('status')} | {result.get('summary')} |")
    REPORT_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    report = build_report()
    write_report(report)
    if report.get("status") != "pass":
        for result in report.get("results", []):
            if result.get("status") != "pass":
                print(f"{result.get('id')}: {result.get('summary')}", file=sys.stderr)
        return 1
    print(f"Plasma v2 release-candidate gate passed: {repo_path(REPORT_JSON)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
