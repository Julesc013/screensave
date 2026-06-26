"""Validate ScreenSave commit metadata discipline."""

from __future__ import annotations

import pathlib
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
TEMPLATE = ROOT / ".aide" / "git" / "screensave-commit-template.md"
POLICY = ROOT / ".aide" / "policies" / "screensave-commit-discipline.yaml"
REQUIRED_SECTIONS = ["Summary:", "Why:", "Scope:", "Validation:", "Evidence:", "Boundary:"]
REQUIRED_BOUNDARIES = [
    "No release promotion.",
    "No compatibility certification.",
    "No AIDE product-runtime dependency.",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def git_text(args: list[str]) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT, text=True, stderr=subprocess.DEVNULL)


def validate_text(label: str, text: str, errors: list[str]) -> None:
    for section in REQUIRED_SECTIONS:
        require(section in text, f"{label} missing required section {section}", errors)
    for boundary in REQUIRED_BOUNDARIES:
        require(boundary in text, f"{label} missing boundary line {boundary}", errors)
    require("PASS:" in text, f"{label} must record PASS validation lines.", errors)
    require("allowed:" in text and "forbidden:" in text, f"{label} must record allowed and forbidden scope.", errors)


def main() -> int:
    errors: list[str] = []
    for path in [TEMPLATE, POLICY]:
        require(path.exists(), f"Missing commit discipline path: {path.relative_to(ROOT)}", errors)
    if TEMPLATE.exists():
        validate_text("commit template", TEMPLATE.read_text(encoding="utf-8"), errors)
    if POLICY.exists():
        policy = POLICY.read_text(encoding="utf-8")
        for needle in [
            "one commit per bounded WorkUnit",
            "one commit per repair",
            "one commit per generated-catalog refresh",
            "one commit per authority transition",
            "Summary",
            "Validation",
            "No release promotion.",
            "No compatibility certification.",
            "No AIDE product-runtime dependency.",
        ]:
            require(needle in policy, f"commit discipline policy missing {needle!r}", errors)
        require("source mutation" in policy and "release promotion" in policy, "policy must preserve mutation and release boundaries.", errors)

    try:
        head_body = git_text(["log", "-1", "--format=%B"])
    except Exception as exc:
        errors.append(f"unable to read HEAD commit message: {exc}")
    else:
        validate_text("HEAD commit message", head_body, errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print("Commit metadata checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
