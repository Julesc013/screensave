"""Validate the Plasma v2 stable-candidate gate."""

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
REPORT_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "stable-candidate"
REPORT_JSON = REPORT_DIR / "gate-report.json"
REPORT_MD = REPORT_DIR / "gate-report.md"

SUBCHECKS = [
    ["tools/scripts/check_packc.py"],
    ["tools/scripts/check_visual_intent_resolver.py"],
    ["tools/scripts/check_plasma_v2_matrix.py"],
    ["tools/scripts/check_plasma_v2_materials.py"],
    ["tools/scripts/check_plasma_v2_influence.py"],
    ["tools/scripts/check_plasma_v2_acceleration.py"],
    ["tools/scripts/check_plasma_v2_performance.py"],
    ["tools/scripts/check_workbench_shell.py"],
    ["tools/scripts/check_plasma_v2_visual_review_round.py"],
    ["tools/scripts/check_aide_evidence_index.py"],
    ["tools/scripts/check_aide_evidence_bridge.py"],
]

REQUIRED_PATHS = {
    "pack_example": "products/savers/plasma/content/v2/examples/plasma_lava_v2.toml",
    "acceleration_matrix": "validation/captures/plasma-v2/acceleration/matrix.json",
    "performance_envelope": "validation/captures/plasma-v2/performance/envelope.json",
    "visual_review_round": "validation/captures/plasma-v2/stable-candidate-review/review-summary.json",
    "aide_stable_candidate": ".aide/evidence/plasma-v2-stable-candidate.toml",
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
        timeout=360,
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
    forbidden = {"screensave.command", "screensave.run", "screensave.exec"}
    add_result(
        results,
        "no-generic-command-capability",
        not bool(names & forbidden),
        "Project adapter must not expose generic command, run, or exec capabilities.",
        forbidden_hits=sorted(names & forbidden),
    )


def scan_overclaims(results: list[dict[str, Any]]) -> None:
    paths = [
        ROOT / "PROJECT_STATE.toml",
        ROOT / "VERSION.toml",
        ROOT / "README.md",
        ROOT / "docs" / "roadmap" / "paw-f-plasma-v2-accelerated-stable-candidate.md",
        ROOT / "validation" / "captures" / "plasma-v2" / "stable-candidate-review" / "review-summary.md",
    ]
    forbidden = [
        "stable = true",
        "release_promotion = \"promoted\"",
        "compatibility_certification = \"certified\"",
        "compatibility certified",
        "final_artistic_acceptance = \"accepted\"",
    ]
    hits: list[str] = []
    for path in paths:
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="ignore").lower()
        for token in forbidden:
            if token in text:
                hits.append(f"{repo_path(path)} contains {token}")
    add_result(results, "no-overclaim-scan", not hits, "No stable release, final acceptance, or certification overclaim may exist.", hits=hits)


def scan_aide_runtime_dependency(results: list[dict[str, Any]]) -> None:
    hits: list[str] = []
    for path in (ROOT / "products" / "savers" / "plasma" / "src").glob("*"):
        if path.suffix.lower() not in {".c", ".h"}:
            continue
        text = path.read_text(encoding="utf-8", errors="ignore")
        if "AIDE" in text:
            hits.append(repo_path(path))
    add_result(results, "no-aide-runtime-dependency", not hits, "Plasma runtime/source files must not depend on AIDE.", hits=hits)


def build_report() -> dict[str, Any]:
    results: list[dict[str, Any]] = []
    state = load_toml(STATE)
    plasma = state.get("plasma_v2", {})

    add_result(
        results,
        "portable-v2-accepted",
        state.get("portable_v2", {}).get("status") == "accepted",
        "Portable v2 must be accepted before Plasma stable-candidate work.",
        observed=state.get("portable_v2", {}).get("status"),
    )
    add_result(
        results,
        "plasma-status-input",
        plasma.get("status") in {"reviewed-preview", "stable-candidate"},
        "Plasma v2 must enter the gate from reviewed-preview and may exit as stable-candidate.",
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
        add_result(results, f"path-{path_id}", path.exists(), f"Required stable-candidate evidence exists: {relative}")

    if (ROOT / REQUIRED_PATHS["acceleration_matrix"]).exists():
        matrix = load_json(ROOT / REQUIRED_PATHS["acceleration_matrix"])
        add_result(
            results,
            "acceleration-matrix-status",
            matrix.get("status") == "pass" and matrix.get("row_count", 0) >= 10,
            "Acceleration matrix must pass and cover the candidate matrix.",
            row_count=matrix.get("row_count"),
            status=matrix.get("status"),
        )
    if (ROOT / REQUIRED_PATHS["performance_envelope"]).exists():
        envelope = load_json(ROOT / REQUIRED_PATHS["performance_envelope"])
        add_result(
            results,
            "performance-envelope-status",
            envelope.get("status") in {"pass", "pass-with-warnings"},
            "Performance envelope must be recorded with explicit warning status when needed.",
            status=envelope.get("status"),
        )
    if (ROOT / REQUIRED_PATHS["visual_review_round"]).exists():
        review = load_json(ROOT / REQUIRED_PATHS["visual_review_round"])
        add_result(
            results,
            "visual-review-round-status",
            review.get("decision_class") == "accepted-for-stable-candidate" and review.get("stable_release") is False,
            "Visual review round must accept stable-candidate only and keep stable release false.",
            decision_class=review.get("decision_class"),
            stable_release=review.get("stable_release"),
        )

    validate_capabilities(results)
    scan_overclaims(results)
    scan_aide_runtime_dependency(results)

    for command in SUBCHECKS:
        run = run_subcheck(command)
        add_result(results, "validator-" + command[0].split("/")[-1].replace(".py", ""), run["status"] == "pass", "Required validator must pass.", run=run)

    passed = all(result.get("status") == "pass" for result in results)
    return {
        "schema_version": "screensave.plasma-v2.stable-candidate-gate.v1",
        "status": "pass" if passed else "fail",
        "product": "plasma",
        "profile": "plasma.v2.reference.preview",
        "claim_boundary": "Plasma v2 stable-candidate gate only; not stable release, final artistic acceptance, compatibility certification, or automatic promotion.",
        "results": results,
    }


def write_report(report: dict[str, Any]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    lines = [
        "# Plasma v2 Stable-Candidate Gate",
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
    print(f"Plasma v2 stable-candidate gate passed: {repo_path(REPORT_JSON)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
