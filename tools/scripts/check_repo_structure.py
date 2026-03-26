"""Repository governance and structure checks for the current series."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]

REQUIRED_PATHS = [
    ROOT / "CONTRIBUTING.md",
    ROOT / ".github" / "ISSUE_TEMPLATE" / "config.yml",
    ROOT / ".github" / "ISSUE_TEMPLATE" / "bug_report.yml",
    ROOT / ".github" / "ISSUE_TEMPLATE" / "compatibility_report.yml",
    ROOT / ".github" / "ISSUE_TEMPLATE" / "saver_idea.yml",
    ROOT / ".github" / "ISSUE_TEMPLATE" / "feature_request.yml",
    ROOT / ".github" / "PULL_REQUEST_TEMPLATE.md",
    ROOT / ".github" / "CODEOWNERS",
    ROOT / ".github" / "workflows" / "ci.yml",
    ROOT / ".github" / "workflows" / "docs.yml",
    ROOT / ".github" / "workflows" / "release.yml",
    ROOT / "tools" / "scripts" / "check_repo_structure.py",
    ROOT / "tools" / "scripts" / "check_codex_config.py",
    ROOT / "tools" / "scripts" / "check_docs_basics.py",
    ROOT / "tools" / "scripts" / "check_release_scaffold.py",
]

FORBIDDEN_PATHS = [
    ROOT / "src",
]

TEXT_CHECKS = {
    ROOT / ".github" / "ISSUE_TEMPLATE" / "config.yml": [
        "blank_issues_enabled: false",
        "Repository Guide",
        "Compatibility Contract",
        "Contribution Guide",
    ],
    ROOT / ".github" / "ISSUE_TEMPLATE" / "bug_report.yml": [
        "issue_kind",
        "affected_area",
        "product_or_path",
        "expected_behavior",
        "actual_behavior",
        "reproduction_steps",
        "environment",
    ],
    ROOT / ".github" / "ISSUE_TEMPLATE" / "compatibility_report.yml": [
        "os_version",
        "architecture",
        "renderer_mode",
        "result_classification",
        "reproducibility",
        "evidence",
    ],
    ROOT / ".github" / "ISSUE_TEMPLATE" / "saver_idea.yml": [
        "idea_type",
        "category",
        "lineup_fit",
        "shared_systems",
        "compatibility_concerns",
    ],
    ROOT / ".github" / "ISSUE_TEMPLATE" / "feature_request.yml": [
        "requested_capability",
        "affected_area",
        "change_scope",
        "motivation",
        "constraints",
    ],
    ROOT / ".github" / "PULL_REQUEST_TEMPLATE.md": [
        "## Summary",
        "## Specs And Docs Consulted",
        "## Files Changed",
        "## Verification Performed",
        "## Risks And Follow-Up",
        "## Contract Impact",
    ],
    ROOT / ".github" / "CODEOWNERS": [
        "* @Julesc013",
        "/.github/ @Julesc013",
        "/specs/ @Julesc013",
        "/platform/ @Julesc013",
        "/products/ @Julesc013",
        "/packaging/ @Julesc013",
    ],
    ROOT / ".github" / "workflows" / "ci.yml": [
        "push:",
        "pull_request:",
        "actions/checkout@v4",
        "actions/setup-python@v5",
        "check_repo_structure.py",
        "check_codex_config.py",
    ],
    ROOT / ".github" / "workflows" / "docs.yml": [
        "pull_request:",
        "workflow_dispatch:",
        "actions/checkout@v4",
        "actions/setup-python@v5",
        "check_docs_basics.py",
    ],
    ROOT / ".github" / "workflows" / "release.yml": [
        "workflow_dispatch:",
        "actions/checkout@v4",
        "actions/setup-python@v5",
        "actions/upload-artifact@v4",
        "check_release_scaffold.py",
    ],
}


def main() -> int:
    errors = []

    for path in REQUIRED_PATHS:
        if not path.exists():
            errors.append(f"Missing required path: {path.relative_to(ROOT)}")

    for path in FORBIDDEN_PATHS:
        if path.exists():
            errors.append(f"Forbidden legacy path still present: {path.relative_to(ROOT)}")

    for path, needles in TEXT_CHECKS.items():
        if not path.exists():
            continue
        content = path.read_text(encoding="utf-8")
        for needle in needles:
            if needle not in content:
                errors.append(f"{path.relative_to(ROOT)} is missing expected text: {needle!r}")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Repository structure and governance checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
