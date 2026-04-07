"""Validate the frozen C16 release baseline surface."""

from __future__ import annotations

import configparser
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]

REQUIRED_PATHS = (
    ROOT / "packaging" / "release_notes" / "core-baseline-notes.md",
    ROOT / "packaging" / "release_notes" / "core-compatibility-notes.md",
    ROOT / "validation" / "notes" / "c16-core-inclusion-matrix.md",
    ROOT / "validation" / "notes" / "c16-companion-channel-matrix.md",
    ROOT / "validation" / "notes" / "c16-installer-matrix.md",
    ROOT / "validation" / "notes" / "c16-known-issues.md",
    ROOT / "validation" / "notes" / "c16-release-baseline.md",
    ROOT / "docs" / "roadmap" / "release-channels.md",
    ROOT / "docs" / "roadmap" / "core-zip-doctrine.md",
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

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing C16 baseline artifact: {path.relative_to(ROOT)}", errors)

    bundle_manifest = read_ini(ROOT / "packaging" / "portable" / "bundle_manifest.ini")
    installer_manifest = read_ini(ROOT / "packaging" / "installer" / "installer_manifest.ini")
    channel_manifest = read_ini(ROOT / "packaging" / "channel_manifest.ini")
    version_header = read_text(ROOT / "platform" / "include" / "screensave" / "version.h")
    changelog = read_text(ROOT / "CHANGELOG.md")
    baseline_note = read_text(ROOT / "packaging" / "release_notes" / "core-baseline-notes.md")
    release_baseline = read_text(ROOT / "validation" / "notes" / "c16-release-baseline.md")
    readme = read_text(ROOT / "README.md")
    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")

    require(bundle_manifest.get("bundle", "staging_name") == "screensave-core-c16-baseline", "Portable manifest must stage to screensave-core-c16-baseline.", errors)
    require(bundle_manifest.get("bundle", "zip_name") == "screensave-core-c16-baseline.zip", "Portable manifest must name the C16 Core zip.", errors)
    require(bundle_manifest.get("docs", "bundle_matrix") == "validation/notes/c16-core-inclusion-matrix.md", "Portable manifest must point at the C16 Core inclusion matrix.", errors)
    require(bundle_manifest.get("docs", "release_baseline") == "validation/notes/c16-release-baseline.md", "Portable manifest must point at the C16 release baseline note.", errors)
    require(bundle_manifest.has_section("saver.anthology"), "Portable manifest must include anthology as a canonical saver section.", errors)

    require(installer_manifest.get("bundle", "staging_name") == "screensave-installer-c16-baseline", "Installer manifest must stage to screensave-installer-c16-baseline.", errors)
    require(installer_manifest.get("bundle", "zip_name") == "screensave-installer-c16-baseline.zip", "Installer manifest must name the C16 installer zip.", errors)
    require(installer_manifest.get("portable_source", "staging_root") == "out/portable/screensave-core-c16-baseline", "Installer manifest must point at the C16 Core staging root.", errors)
    require(installer_manifest.get("docs", "installer_matrix") == "validation/notes/c16-installer-matrix.md", "Installer manifest must point at the C16 installer matrix.", errors)
    require(installer_manifest.get("doctrine", "built_on") == "core", "Installer manifest must declare that Installer is built on Core.", errors)

    require(channel_manifest.get("doctrine", "version") == "C16", "Channel manifest must record doctrine version C16.", errors)
    require(channel_manifest.get("doctrine", "freeze_marker") == "c16-core-baseline", "Channel manifest must record the C16 baseline tag name.", errors)

    require('SCREENSAVE_VERSION_TEXT "0.15.0"' in version_header, "version.h must keep the 0.15.0 release text.", errors)
    require('SCREENSAVE_VERSION_SERIES "C16 Core Baseline Freeze"' in version_header, "version.h must carry the C16 baseline series label.", errors)

    require("# ScreenSave Core Baseline Notes" in baseline_note, "core-baseline-notes.md must use the expected title.", errors)
    require("`c16-core-baseline`" in baseline_note, "core-baseline-notes.md must record the canonical baseline tag name.", errors)
    require("# C16 Release Baseline" in release_baseline, "c16-release-baseline.md must use the expected title.", errors)
    require("Canonical baseline tag: `c16-core-baseline`" in release_baseline, "c16-release-baseline.md must record the canonical baseline tag name.", errors)

    require("## C16 - 2026-04-06" in changelog, "CHANGELOG.md must record the C16 release baseline entry.", errors)
    require("Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16`." in readme, "README.md must record C16 as complete.", errors)
    require("Post-`C16` work now proceeds as a short `SS` release-closure series first:" in readme, "README.md must record the post-C16 handoff.", errors)
    require("`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in prompt_program, "prompt-program.md must record the completed C16 freeze.", errors)

    check_manifest_versions(errors)

    portable_root = ROOT / "out" / "portable" / "screensave-core-c16-baseline"
    portable_zip = ROOT / "out" / "portable" / "screensave-core-c16-baseline.zip"
    installer_root = ROOT / "out" / "installer" / "screensave-installer-c16-baseline"
    installer_zip = ROOT / "out" / "installer" / "screensave-installer-c16-baseline.zip"
    suite_artifact = ROOT / "out" / "msvc" / "vs2022" / "Release" / "suite" / "suite.exe"
    benchlab_artifact = ROOT / "out" / "msvc" / "vs2022" / "Release" / "benchlab" / "benchlab.exe"

    require(portable_root.is_dir(), "Missing staged C16 Core directory.", errors)
    require(portable_zip.is_file(), "Missing staged C16 Core zip.", errors)
    require(installer_root.is_dir(), "Missing staged C16 installer directory.", errors)
    require(installer_zip.is_file(), "Missing staged C16 installer zip.", errors)
    require(suite_artifact.is_file(), "Missing release-build suite companion artifact.", errors)
    require(benchlab_artifact.is_file(), "Missing release-build BenchLab companion artifact.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Release baseline surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

