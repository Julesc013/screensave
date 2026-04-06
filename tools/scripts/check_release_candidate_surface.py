"""Validate the final C14 release-candidate surface plus the C15 doctrine handoff."""

from __future__ import annotations

import configparser
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


def read_ini(path: pathlib.Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    parser.optionxform = str
    with path.open("r", encoding="utf-8") as handle:
        parser.read_file(handle)
    return parser


def check_manifest_versions(errors: list[str]) -> None:
    manifest_paths = sorted((ROOT / "products").glob("**/manifest.ini"))
    for path in manifest_paths:
        if path.parts[-3:-1] == ("savers", "_template"):
            continue
        text = read_text(path)
        require("version=0.15.0" in text, f"{path.relative_to(ROOT)} must carry version=0.15.0.", errors)


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_NOTES:
        require(path.exists(), f"Missing release-candidate artifact: {path.relative_to(ROOT)}", errors)

    bundle_manifest = read_ini(ROOT / "packaging" / "portable" / "bundle_manifest.ini")
    installer_manifest = read_ini(ROOT / "packaging" / "installer" / "installer_manifest.ini")
    version_header = read_text(ROOT / "platform" / "include" / "screensave" / "version.h")
    changelog = read_text(ROOT / "CHANGELOG.md")
    release_notes = read_text(ROOT / "packaging" / "release_notes" / "release-candidate-notes.md")
    readme = read_text(ROOT / "README.md")
    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")

    require(bundle_manifest.get("bundle", "staging_name") == "screensave-portable-c14-rc", "Portable manifest must stage to screensave-portable-c14-rc.", errors)
    require(bundle_manifest.get("bundle", "zip_name") == "screensave-portable-c14-rc.zip", "Portable manifest must name the C14 RC zip.", errors)
    require(bundle_manifest.get("docs", "bundle_matrix") == "validation/notes/c14-portable-bundle-matrix.md", "Portable manifest must point at the C14 portable matrix.", errors)
    require(bundle_manifest.get("docs", "core_doctrine") == "docs/roadmap/core-zip-doctrine.md", "Portable manifest must point at the Core doctrine doc.", errors)
    require(bundle_manifest.has_section("saver.anthology"), "Portable manifest must include anthology as a canonical saver section.", errors)

    require(installer_manifest.get("bundle", "staging_name") == "screensave-installer-c14-rc", "Installer manifest must stage to screensave-installer-c14-rc.", errors)
    require(installer_manifest.get("bundle", "zip_name") == "screensave-installer-c14-rc.zip", "Installer manifest must name the C14 RC zip.", errors)
    require(installer_manifest.get("portable_source", "staging_root") == "out/portable/screensave-portable-c14-rc", "Installer manifest must point at the C14 portable staging root.", errors)
    require(installer_manifest.get("docs", "installer_matrix") == "validation/notes/c14-installer-matrix.md", "Installer manifest must point at the C14 installer matrix.", errors)
    require(installer_manifest.get("doctrine", "built_on") == "core", "Installer manifest must declare that Installer is built on Core.", errors)

    require('SCREENSAVE_VERSION_TEXT "0.15.0"' in version_header, "version.h must keep the 0.15.0 release text.", errors)
    require('SCREENSAVE_VERSION_SERIES "C14 Final Rerelease Hardening Release Candidate"' in version_header, "version.h must carry the C14 release-candidate series label.", errors)

    require("# ScreenSave Release Candidate Notes" in release_notes, "release-candidate-notes.md must use the expected title.", errors)
    require("`c14-release-candidate`" in release_notes, "release-candidate-notes.md must record the canonical tag name.", errors)
    require("## C14 - 2026-03-30" in changelog, "CHANGELOG.md must record the C14 release-candidate entry.", errors)
    require("The active continuation line now extends through `C15` release doctrine and channel split. `C16` Core release refresh and baseline freeze is next." in readme, "README.md must record the C15/C16 status.", errors)
    require("`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, and `C15` release doctrine and channel split are complete. `C16` Core release refresh and baseline freeze is next." in prompt_program, "prompt-program.md must point to C15 and C16.", errors)

    check_manifest_versions(errors)

    portable_root = ROOT / "out" / "portable" / "screensave-portable-c14-rc"
    portable_zip = ROOT / "out" / "portable" / "screensave-portable-c14-rc.zip"
    installer_root = ROOT / "out" / "installer" / "screensave-installer-c14-rc"
    installer_zip = ROOT / "out" / "installer" / "screensave-installer-c14-rc.zip"

    require(portable_root.is_dir(), "Missing staged C14 portable directory.", errors)
    require(portable_zip.is_file(), "Missing staged C14 portable zip.", errors)
    require(installer_root.is_dir(), "Missing staged C14 installer directory.", errors)
    require(installer_zip.is_file(), "Missing staged C14 installer zip.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Release-candidate surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
