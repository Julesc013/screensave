"""Basic markdown and roadmap checks for the current repository stage."""

from __future__ import annotations

import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
LINK_PATTERN = re.compile(r"\[[^\]]+\]\(([^)]+)\)")

REQUIRED_PHRASES = {
    ROOT / "README.md": [
        "Series 01",
        "no real runtime implementation yet",
        ".github/",
    ],
    ROOT / "CONTRIBUTING.md": [
        "specs/",
        "prompt program",
        "check_repo_structure.py",
    ],
    ROOT / "docs" / "architecture" / "repo-structure.md": [
        ".github/",
        "community-health",
    ],
    ROOT / "docs" / "roadmap" / "prompt-program.md": [
        "S01",
        "governance",
        "Series 00 through 17",
    ],
    ROOT / "docs" / "roadmap" / "series-map.md": [
        "S01",
        "governance and repository hygiene",
    ],
}


def iter_markdown_files() -> list[pathlib.Path]:
    paths = []
    for path in ROOT.rglob("*.md"):
        if ".git" in path.parts:
            continue
        paths.append(path)
    return sorted(paths)


def check_links(path: pathlib.Path, errors: list[str]) -> None:
    content = path.read_text(encoding="utf-8")
    for target in LINK_PATTERN.findall(content):
        if target.startswith(("http://", "https://", "mailto:", "#")):
            continue
        clean_target = target.split("#", 1)[0]
        if not clean_target:
            continue
        resolved = (path.parent / clean_target).resolve()
        if not resolved.exists():
            errors.append(f"Broken local markdown link in {path.relative_to(ROOT)}: {target}")


def main() -> int:
    errors = []

    for path in iter_markdown_files():
        check_links(path, errors)

    for path, phrases in REQUIRED_PHRASES.items():
        content = path.read_text(encoding="utf-8")
        lowered = content.lower()
        for phrase in phrases:
            if phrase.lower() not in lowered:
                errors.append(f"{path.relative_to(ROOT)} is missing expected phrase: {phrase!r}")

    prompt_program = (ROOT / "docs" / "roadmap" / "prompt-program.md").read_text(encoding="utf-8")
    for index in range(18):
        series = f"S{index:02d}"
        if series not in prompt_program:
            errors.append(f"docs/roadmap/prompt-program.md is missing {series}.")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Documentation checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
