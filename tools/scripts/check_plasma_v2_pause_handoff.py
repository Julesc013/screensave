"""Validate the Plasma v2 publication-ready pause handoff."""

from __future__ import annotations

import json
import pathlib
import subprocess
import sys
import tomllib
from typing import Any


ROOT = pathlib.Path(__file__).resolve().parents[2]
STATE = ROOT / "PROJECT_STATE.toml"
RELEASE_MANIFEST = ROOT / "releases" / "plasma-v2-stable" / "release-manifest.toml"
ARTIFACT_MANIFEST = ROOT / "releases" / "plasma-v2-stable" / "artifact-manifest.toml"
CLOSEOUT_SCAN = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "closeout-scan.json"
DRY_RUN = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "publication-dry-run.json"
GATE_REPORT = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "gate-report.json"
STABLE_GATE = ROOT / "validation" / "captures" / "plasma-v2" / "stable-promotion" / "gate-report.json"
REPAIR_QUEUE = ROOT / ".aide" / "repairs" / "index.toml"
REPORT_JSON = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "pause-handoff-report.json"
REPORT_MD = ROOT / "validation" / "captures" / "plasma-v2" / "publication-prep" / "pause-handoff-report.md"

FORBIDDEN_PUBLICATION_PATHS = [
    ROOT / "releases" / "plasma-v2-stable" / "publication-receipt.json",
    ROOT / "releases" / "plasma-v2-stable" / "github-release.json",
    ROOT / "releases" / "plasma-v2-stable" / "uploaded-assets.json",
]

SUBCHECKS = [
    ("publication-prep-gate", [sys.executable, "tools/scripts/check_plasma_v2_publication_prep.py"], 600),
    ("stable-promotion-gate", [sys.executable, "tools/scripts/check_plasma_v2_stable_promotion.py"], 900),
    ("aide-evidence-index", [sys.executable, "tools/scripts/check_aide_evidence_index.py"], 180),
    ("repair-queue", [sys.executable, "tools/scripts/check_repair_queue.py"], 180),
    ("project-adapter-t2", [sys.executable, "tools/project_adapter/screensave_project.py", "validate", "--tier", "T2"], 900),
    ("project-adapter-t3", [sys.executable, "tools/project_adapter/screensave_project.py", "validate", "--tier", "T3"], 900),
]


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def git_text(args: list[str]) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    return result.stdout.strip()


def run(command: list[str], timeout_seconds: int) -> dict[str, Any]:
    try:
        result = subprocess.run(
            command,
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout_seconds,
            check=False,
        )
    except subprocess.TimeoutExpired as exc:
        return {
            "command": " ".join(command),
            "returncode": 124,
            "stdout_tail": (exc.stdout or "")[-4000:] if isinstance(exc.stdout, str) else "",
            "stderr_tail": (exc.stderr or "")[-4000:] if isinstance(exc.stderr, str) else "",
            "timeout_seconds": timeout_seconds,
            "timed_out": True,
        }
    return {
        "command": " ".join(command),
        "returncode": result.returncode,
        "stdout_tail": result.stdout.strip()[-4000:],
        "stderr_tail": result.stderr.strip()[-4000:],
        "timeout_seconds": timeout_seconds,
        "timed_out": False,
    }


def add_check(checks: list[dict[str, Any]], check_id: str, ok: bool, message: str, **details: Any) -> None:
    payload: dict[str, Any] = {
        "id": check_id,
        "status": "pass" if ok else "fail",
        "message": message,
    }
    payload.update(details)
    checks.append(payload)


def check_project_state(checks: list[dict[str, Any]]) -> None:
    state = load_toml(STATE) if STATE.exists() else {}
    authority = state.get("authority", {})
    development = state.get("development", {})
    plasma = state.get("plasma_v2", {})
    ok = (
        authority.get("active_program") == "plasma-v2-publication"
        and development.get("active_program") == "plasma-v2-publication"
        and authority.get("release_candidate") == "plasma-v2-rc1"
        and plasma.get("status") == "publication-ready"
        and plasma.get("stable") is True
        and plasma.get("release_promotion") == "accepted"
        and plasma.get("publication_prep") == "ready"
        and plasma.get("publication") == "not-published"
        and plasma.get("opened_next") == "plasma-v2-publication"
    )
    add_check(
        checks,
        "publication-ready-project-state",
        ok,
        "Project state records publication-ready Plasma v2 without publication.",
        active_program=authority.get("active_program"),
        plasma_status=plasma.get("status"),
        stable=plasma.get("stable"),
        release_promotion=plasma.get("release_promotion"),
        publication_prep=plasma.get("publication_prep"),
        publication=plasma.get("publication"),
        opened_next=plasma.get("opened_next"),
    )


def check_publication_packet(checks: list[dict[str, Any]]) -> None:
    required = [RELEASE_MANIFEST, ARTIFACT_MANIFEST, CLOSEOUT_SCAN, DRY_RUN, GATE_REPORT, STABLE_GATE]
    missing = [str(path.relative_to(ROOT)) for path in required if not path.exists()]
    add_check(checks, "pause-evidence-paths", not missing, "Pause handoff evidence paths exist.", missing=missing)
    if RELEASE_MANIFEST.exists():
        manifest = load_toml(RELEASE_MANIFEST)
        publication = manifest.get("publication", {})
        ok = (
            manifest.get("stable_promotion") == "accepted"
            and manifest.get("compatibility_certification") == "not-broadened"
            and manifest.get("all_saver_migration") == "not-claimed"
            and publication.get("public_upload") is False
            and publication.get("github_release_created") is False
            and publication.get("release_page_published") is False
            and publication.get("publication_receipt") == "not-created"
        )
        add_check(checks, "release-manifest-boundary", ok, "Release manifest remains publication-prep only.")
    if ARTIFACT_MANIFEST.exists():
        artifacts = load_toml(ARTIFACT_MANIFEST).get("artifacts", [])
        published = [item.get("id") for item in artifacts if isinstance(item, dict) and item.get("published") is not False]
        add_check(checks, "artifact-manifest-unpublished", not published, "Artifact manifest has no published entries.", published=published)
    if CLOSEOUT_SCAN.exists():
        scan = load_json(CLOSEOUT_SCAN)
        findings = scan.get("closeout_findings", {})
        ok = (
            scan.get("status") == "pass"
            and scan.get("repair_queue_scan", {}).get("open_blocking_count") == 0
            and findings.get("release_publication_performed") is False
            and findings.get("compatibility_certification_broadened") is False
        )
        add_check(checks, "closeout-scan", ok, "Closeout scan records no blocking publication-prep repairs.")
    if DRY_RUN.exists():
        dry_run = load_json(DRY_RUN)
        dry_checks = dry_run.get("checks", {})
        ok = (
            dry_run.get("status") == "pass"
            and dry_run.get("publication_result") == "not-published"
            and dry_checks.get("no_public_upload_occurred") is True
            and dry_checks.get("no_release_tag_created") is True
            and dry_checks.get("no_github_release_created") is True
            and dry_checks.get("no_compatibility_certification_broadening") is True
        )
        add_check(checks, "publication-dry-run", ok, "Publication dry run verifies the packet without publishing.")


def check_no_forbidden_side_effects(checks: list[dict[str, Any]]) -> None:
    existing = [str(path.relative_to(ROOT)) for path in FORBIDDEN_PUBLICATION_PATHS if path.exists()]
    plasma_tags = git_text(["tag", "--list", "plasma-v2*"]).splitlines()
    add_check(checks, "no-publication-receipts", not existing, "No publication/upload receipt files exist.", existing=existing)
    add_check(checks, "no-plasma-release-tags", not plasma_tags, "No local Plasma v2 release tag exists.", tags=plasma_tags)


def check_repair_queue_marker(checks: list[dict[str, Any]]) -> None:
    queue = load_toml(REPAIR_QUEUE) if REPAIR_QUEUE.exists() else {}
    marker = queue.get("publication_ready_closeout", {})
    ok = (
        marker.get("status") == "no-blocking-publication-prep-repairs"
        and marker.get("work_unit") == "SS-PLV2-JX0"
        and marker.get("open_blocking_count") == 0
        and marker.get("blocking_repairs") == []
    )
    add_check(checks, "repair-queue-closeout-marker", ok, "Repair queue records no blocking publication-prep closeout repairs.", marker=marker)


def check_subcommands(checks: list[dict[str, Any]]) -> None:
    for check_id, command, timeout_seconds in SUBCHECKS:
        result = run(command, timeout_seconds)
        add_check(
            checks,
            check_id,
            result.get("returncode") == 0,
            "Command passed." if result.get("returncode") == 0 else "Command failed.",
            run=result,
        )


def build_report() -> dict[str, Any]:
    checks: list[dict[str, Any]] = []
    check_project_state(checks)
    check_publication_packet(checks)
    check_no_forbidden_side_effects(checks)
    check_repair_queue_marker(checks)
    check_subcommands(checks)
    status = "pause-ready" if all(item.get("status") == "pass" for item in checks) else "pause-hold"
    return {
        "schema": "screensave.plasma-v2.publication-ready.pause-handoff.v1",
        "status": status,
        "work_unit": "SS-PLV2-JX0",
        "release_candidate": "plasma-v2-rc1",
        "source": {
            "branch": git_text(["branch", "--show-current"]),
            "commit": git_text(["rev-parse", "HEAD"]),
            "dirty": bool(git_text(["status", "--short"])),
        },
        "checks": checks,
        "claim_boundary": "Pause handoff only; publication-ready is not publication, stable promotion is not compatibility certification broadening, AIDE evidence is not product truth, and Workbench shell evidence is not a graphical Workbench MVP.",
    }


def write_reports(report: dict[str, Any]) -> None:
    REPORT_JSON.parent.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Publication-Ready Pause Handoff Report",
        "",
        f"Status: {report['status']}",
        f"WorkUnit: {report['work_unit']}",
        f"Release candidate: {report['release_candidate']}",
        "",
        "## Checks",
        "",
    ]
    for check in report.get("checks", []):
        lines.append(f"- {check.get('id')}: {check.get('status')} - {check.get('message')}")
    lines.extend(["", "## Boundary", "", report["claim_boundary"], ""])
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    report = build_report()
    write_reports(report)
    if report.get("status") != "pause-ready":
        for check in report.get("checks", []):
            if check.get("status") != "pass":
                print(f"{check.get('id')}: {check.get('message')}", file=sys.stderr)
        print("Plasma v2 pause handoff hold", file=sys.stderr)
        return 1
    print("Plasma v2 pause handoff pause-ready")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
