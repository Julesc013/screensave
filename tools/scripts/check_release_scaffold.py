"""Generate and validate a conservative release-scaffold report."""

from __future__ import annotations

import argparse
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
REQUIRED_PATHS = [
    ROOT / "README.md",
    ROOT / "CHANGELOG.md",
    ROOT / "CONTRIBUTING.md",
    ROOT / "specs" / "release_contract.md",
    ROOT / "packaging" / "README.md",
    ROOT / "packaging" / "release_notes" / "README.md",
    ROOT / "packaging" / "release_notes" / "release-candidate-notes.md",
    ROOT / "validation" / "README.md",
    ROOT / "validation" / "notes" / "c14-release-readiness-matrix.md",
    ROOT / ".github" / "workflows" / "release.yml",
]


def build_report(version_label: str, track: str, missing: list[pathlib.Path], changelog_has_label: bool) -> str:
    lines = [
        "# Release Scaffold Report",
        "",
        f"- Track: {track}",
        f"- Version label: {version_label or 'not provided'}",
        "",
        "## Required repository inputs",
    ]

    for path in REQUIRED_PATHS:
        status = "present" if path not in missing else "missing"
        lines.append(f"- `{path.relative_to(ROOT)}`: {status}")

    lines.extend(
        [
            "",
            "## Changelog note",
            f"- Provided version label present in changelog: {'yes' if changelog_has_label else 'no'}",
            "",
            "## Current deferrals",
            "- This workflow does not build binaries.",
            "- This workflow does not publish releases.",
            "- Portable and installer packaging are implemented, but output completeness still depends on locally available saver binaries.",
        ]
    )

    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--version-label", default="")
    parser.add_argument("--track", default="preview")
    parser.add_argument("--output", required=False)
    args = parser.parse_args()

    missing = [path for path in REQUIRED_PATHS if not path.exists()]
    changelog_text = (ROOT / "CHANGELOG.md").read_text(encoding="utf-8")
    changelog_has_label = bool(args.version_label) and args.version_label in changelog_text
    report = build_report(args.version_label, args.track, missing, changelog_has_label)

    if args.output:
        output_path = (ROOT / args.output).resolve()
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(report, encoding="utf-8")

    sys.stdout.write(report)

    if missing:
        for path in missing:
            print(f"Missing release scaffold input: {path.relative_to(ROOT)}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
