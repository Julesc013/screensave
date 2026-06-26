"""Validate the Plasma v2 release-readiness gate."""

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
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "release-readiness"
REPORT_JSON = REPORT_DIR / "gate-report.json"
REPORT_MD = REPORT_DIR / "gate-report.md"

SUBCHECKS = [
    ["tools/scripts/check_plasma_v2_stable_candidate.py"],
    ["tools/scripts/check_plasma_v2_package_stage.py"],
    ["tools/scripts/check_manager_pack_preview.py"],
    ["tools/scripts/check_workbench_shell.py"],
    ["tools/scripts/check_packc.py"],
    ["tools/scripts/check_plasma_v2_performance.py"],
    ["tools/scripts/check_plasma_v2_artistic_decision.py"],
    ["tools/scripts/check_agentic_policy.py"],
    ["tools/scripts/check_repair_queue.py"],
    ["tools/scripts/check_project_adapter.py"],
]

REQUIRED_PATHS = {
    "release_readiness_contract": "contracts/plasma_release_readiness_v1.md",
    "package_stage": "packaging/windows/plasma-v2-preview/manifest.toml",
    "package_known_limits": "packaging/windows/plasma-v2-preview/known-limits.md",
    "manager_preview_source": "products/apps/suite/src/manager_pack_preview.c",
    "workbench_shell": "products/apps/benchlab/src/benchlab_workbench_shell.c",
    "pack_example": "products/savers/plasma/content/v2/examples/plasma_lava_v2.toml",
    "performance_envelope": "validation/captures/plasma-v2/performance/envelope.json",
    "artistic_decision": "validation/captures/plasma-v2/final-artistic-decision/decision.release-candidate.toml",
    "agentic_policy": ".aide/agentic/policy.toml",
    "repair_queue": ".aide/repairs/index.toml",
    "capability_bindings": "tools/project_adapter/capability_bindings.json",
}

REQUIRED_CAPABILITIES = {
    "screensave.plasma.v2.stable-candidate.check",
    "screensave.plasma.v2.package-stage.check",
    "screensave.plasma.v2.manager-preview.check",
    "screensave.plasma.v2.release-readiness.check",
    "screensave.aide.workunits.check",
    "screensave.aide.repairs.check",
    "screensave.aide.agentic.check",
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
        timeout=720,
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


def validate_capabilities(results: list[dict[str, Any]]) -> None:
    bindings = load_json(CAPABILITY_BINDINGS)
    names = {item.get("name") for item in bindings.get("capabilities", [])}
    add_result(
        results,
        "fixed-release-readiness-capabilities",
        REQUIRED_CAPABILITIES <= names,
        "Fixed release-readiness capabilities must be exposed.",
        missing=sorted(REQUIRED_CAPABILITIES - names),
    )
    add_result(
        results,
        "no-generic-or-release-capabilities",
        not bool(names & FORBIDDEN_CAPABILITIES),
        "Generic command, release, promotion, and agent-apply capabilities must remain absent.",
        forbidden_hits=sorted(names & FORBIDDEN_CAPABILITIES),
    )


def scan_overclaims(results: list[dict[str, Any]]) -> None:
    paths = [
        ROOT / "PROJECT_STATE.toml",
        ROOT / "VERSION.toml",
        ROOT / "README.md",
        ROOT / "docs" / "roadmap" / "plasma-v2-release-readiness.md",
        ROOT / "products" / "savers" / "plasma" / "docs" / "plasma-v2-release-readiness.md",
        ROOT / "validation" / "captures" / "plasma-v2" / "final-artistic-decision" / "decision.release-candidate.toml",
    ]
    forbidden = [
        "stable = true",
        "release_promotion = \"accepted\"",
        "release_promotion = \"promoted\"",
        "compatibility_certification = \"certified\"",
        "compatibility certified",
        "final_stable_artistic_acceptance = true",
        "automatic_release = true",
    ]
    hits: list[str] = []
    for path in paths:
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="ignore").lower()
        for token in forbidden:
            if token in text:
                hits.append(f"{repo_path(path)} contains {token}")
    add_result(results, "no-release-overclaims", not hits, "No stable release, publication, certification, or final acceptance overclaim may exist.", hits=hits)


def scan_aide_runtime_dependency(results: list[dict[str, Any]]) -> None:
    hits: list[str] = []
    for root in (ROOT / "platform", ROOT / "products"):
        for path in root.rglob("*"):
            if not path.is_file() or path.suffix.lower() in {".png", ".jpg", ".jpeg", ".gif", ".ico", ".bmp"}:
                continue
            try:
                text = path.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            if ".aide" in text or "AIDE" in text:
                hits.append(repo_path(path))
    add_result(results, "no-aide-product-runtime-dependency", not hits, "Product/runtime tree must not depend on AIDE.", hits=hits[:20], hit_count=len(hits))


def build_report() -> dict[str, Any]:
    results: list[dict[str, Any]] = []
    state = load_toml(STATE)
    plasma = state.get("plasma_v2", {})

    add_result(
        results,
        "portable-v2-accepted",
        state.get("portable_v2", {}).get("status") == "accepted",
        "Portable v2 must be accepted before release-readiness review.",
        observed=state.get("portable_v2", {}).get("status"),
    )
    add_result(
        results,
        "plasma-status-input",
        plasma.get("status") in {"stable-candidate", "release-readiness-reviewed"},
        "Plasma v2 must enter as stable-candidate and may exit as release-readiness-reviewed.",
        observed=plasma.get("status"),
    )
    add_result(
        results,
        "plasma-not-stable",
        plasma.get("stable") is False and plasma.get("release_promotion") == "blocked",
        "Plasma v2 must remain not stable and release promotion must remain blocked.",
        plasma=plasma,
    )

    for path_id, relative in REQUIRED_PATHS.items():
        path = ROOT / relative
        add_result(results, f"path-{path_id}", path.exists(), f"Required release-readiness evidence exists: {relative}")

    package_stage = ROOT / REQUIRED_PATHS["package_stage"]
    if package_stage.exists():
        text = package_stage.read_text(encoding="utf-8")
        add_result(
            results,
            "package-stage-boundary",
            "published = false" in text and "stable = false" in text and 'release_promotion = "blocked"' in text,
            "Package stage must remain unpublished, not stable, and release-blocked.",
        )

    decision_path = ROOT / REQUIRED_PATHS["artistic_decision"]
    if decision_path.exists():
        decision = load_toml(decision_path)
        boundaries = decision.get("boundaries", {})
        add_result(
            results,
            "artistic-decision-release-candidate-only",
            decision.get("decision_state") == "accepted-for-release-candidate"
            and boundaries.get("final_stable_artistic_acceptance") is False
            and boundaries.get("stable_release") is False,
            "Artistic decision may accept release-candidate preparation only.",
            decision_state=decision.get("decision_state"),
            boundaries=boundaries,
        )

    validate_capabilities(results)
    scan_overclaims(results)
    scan_aide_runtime_dependency(results)

    for command in SUBCHECKS:
        run = run_subcheck(command)
        add_result(results, "validator-" + command[0].split("/")[-1].replace(".py", ""), run["status"] == "pass", "Required release-readiness validator must pass.", run=run)

    passed = all(result.get("status") == "pass" for result in results)
    return {
        "schema_version": "screensave.plasma-v2.release-readiness-gate.v1",
        "status": "pass" if passed else "fail",
        "product": "plasma",
        "profile": "plasma.v2.reference.preview",
        "claim_boundary": "Plasma v2 release-readiness review only; not stable release, release publication, compatibility certification, public SDK stability, or automatic promotion.",
        "results": results,
    }


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Release-Readiness Gate",
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
    print(f"Plasma v2 release-readiness gate passed: {repo_path(REPORT_JSON)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
