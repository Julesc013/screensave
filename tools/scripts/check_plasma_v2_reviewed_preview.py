"""Validate the Plasma v2 reviewed-preview gate."""

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
REVIEW_DIR = ROOT / "validation" / "captures" / "plasma-v2" / "review"
REPORT_JSON = ROOT / "validation" / "captures" / "plasma-v2" / "reviewed-preview" / "gate-report.json"
REPORT_MD = ROOT / "validation" / "captures" / "plasma-v2" / "reviewed-preview" / "gate-report.md"

SUBCHECKS = [
    ["tools/scripts/check_packc.py"],
    ["tools/scripts/check_visual_intent_resolver.py"],
    ["tools/scripts/check_plasma_v2_matrix.py"],
    ["tools/scripts/check_plasma_v2_materials.py"],
    ["tools/scripts/check_plasma_v2_influence.py"],
    ["tools/scripts/check_workbench_shell.py"],
    ["tools/scripts/check_plasma_aide_review.py"],
]
DECISION_CLASSES = {"accepted-for-preview", "request-changes", "rejected", "defer-to-labs"}
NON_DECISIONS = {
    "stable_release",
    "compatibility_certification",
    "core_promotion",
    "all_platform_support",
}


def load_toml(path: pathlib.Path) -> dict[str, Any]:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def load_json(path: pathlib.Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def run_subcheck(command: list[str], errors: list[str]) -> None:
    result = subprocess.run(
        [sys.executable, *command],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=240,
    )
    require(result.returncode == 0, f"{' '.join(command)} failed: {result.stderr or result.stdout}", errors)


def review_decisions() -> list[dict[str, Any]]:
    decisions: list[dict[str, Any]] = []
    for path in sorted(REVIEW_DIR.glob("reviewer-decision*.toml")):
        payload = load_toml(path)
        if payload.get("template") is True:
            continue
        payload["_path"] = str(path.relative_to(ROOT)).replace("\\", "/")
        decisions.append(payload)
    return decisions


def validate_review_decisions(errors: list[str]) -> None:
    decisions = review_decisions()
    require(decisions, "At least one non-template visual review decision must exist.", errors)
    for decision in decisions:
        require(decision.get("schema_version") == 1, f"{decision.get('_path')} schema_version must be 1.", errors)
        require(decision.get("decision_class") in DECISION_CLASSES, f"{decision.get('_path')} decision class is invalid.", errors)
        require(NON_DECISIONS <= set(decision.get("explicit_non_decisions", [])), f"{decision.get('_path')} must list release/certification non-decisions.", errors)
        require("not stable release" in decision.get("claim_boundary", ""), f"{decision.get('_path')} must preserve stable-release boundary.", errors)
        for ref in decision.get("reviewed_capture_refs", []):
            require((ROOT / str(ref)).exists(), f"{decision.get('_path')} ref does not exist: {ref}", errors)


def validate_capabilities(errors: list[str]) -> None:
    bindings = load_json(CAPABILITY_BINDINGS)
    names = {item.get("name") for item in bindings.get("capabilities", [])}
    forbidden = {"screensave.command", "screensave.run", "screensave.exec"}
    require(not names & forbidden, "Project adapter must not expose generic command/run/exec capabilities.", errors)


def main() -> int:
    errors: list[str] = []
    state = load_toml(STATE)
    plasma = state.get("plasma_v2", {})

    require(state.get("portable_v2", {}).get("status") == "accepted", "portable_v2.status must be accepted.", errors)
    require(plasma.get("status") in {"reference-preview", "reviewed-preview"}, "plasma_v2.status must be reference-preview or reviewed-preview.", errors)
    require(plasma.get("stable") is False, "plasma_v2.stable must remain false.", errors)
    require(str(plasma.get("release_promotion", "blocked")) == "blocked", "Plasma release promotion must remain blocked.", errors)
    require("compatibility certification" in str(plasma.get("claim_boundary", "")).lower(), "Plasma claim boundary must block compatibility certification.", errors)
    require(REPORT_JSON.exists(), "Reviewed-preview gate report JSON must exist.", errors)
    require(REPORT_MD.exists(), "Reviewed-preview gate report markdown must exist.", errors)
    if REPORT_JSON.exists():
        report = load_json(REPORT_JSON)
        require(report.get("status") == "pass", "Reviewed-preview gate report status must pass.", errors)
        require("not stable release" in report.get("claim_boundary", ""), "Gate report must preserve stable-release boundary.", errors)

    validate_review_decisions(errors)
    validate_capabilities(errors)

    for command in SUBCHECKS:
        run_subcheck(command, errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 reviewed-preview gate passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
