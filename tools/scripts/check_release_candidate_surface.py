"""Validate the preserved C14 release-candidate notes plus the later doctrine handoff."""

from __future__ import annotations

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]

REQUIRED_NOTES = (
    ROOT / "packaging" / "release_notes" / "release-candidate-notes.md",
    ROOT / "validation" / "notes" / "c14-portable-bundle-matrix.md",
    ROOT / "validation" / "notes" / "c14-installer-matrix.md",
    ROOT / "validation" / "notes" / "c14-release-readiness-matrix.md",
    ROOT / "validation" / "notes" / "c14-known-issues.md",
    ROOT / "validation" / "notes" / "c14-config-integrity.md",
    ROOT / "docs" / "roadmap" / "release-channels.md",
    ROOT / "docs" / "roadmap" / "core-zip-doctrine.md",
    ROOT / "validation" / "notes" / "c15-channel-matrix.md",
    ROOT / "packaging" / "channel_manifest.ini",
)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_NOTES:
        require(path.exists(), f"Missing release-candidate artifact: {path.relative_to(ROOT)}", errors)

    changelog = read_text(ROOT / "CHANGELOG.md")
    release_notes = read_text(ROOT / "packaging" / "release_notes" / "release-candidate-notes.md")
    readme = read_text(ROOT / "README.md")
    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")

    require("# ScreenSave Release Candidate Notes" in release_notes, "release-candidate-notes.md must use the expected title.", errors)
    require("`c14-release-candidate`" in release_notes, "release-candidate-notes.md must record the canonical tag name.", errors)
    require("## C14 - 2026-03-30" in changelog, "CHANGELOG.md must record the C14 release-candidate entry.", errors)
    require("Post-`SS` work now proceeds in the bounded `SX` substrate series." in readme, "README.md must record the post-C16 handoff.", errors)
    require("`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in prompt_program, "prompt-program.md must record the completed C16 freeze.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Release-candidate surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

