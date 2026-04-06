"""Validate the frozen C16 installer definition, staged package, and status docs."""

from __future__ import annotations

import configparser
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "packaging" / "installer" / "installer_manifest.ini"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_manifest() -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    parser.optionxform = str
    with MANIFEST_PATH.open("r", encoding="ascii") as handle:
        parser.read_file(handle)
    return parser


def saver_names(root: Path) -> list[str]:
    if not root.is_dir():
        return []
    return sorted(path.stem for path in root.glob("*.scr"))


def main() -> int:
    errors: list[str] = []

    require(MANIFEST_PATH.exists(), "Missing packaging/installer/installer_manifest.ini.", errors)
    require((ROOT / "packaging" / "installer" / "build_installer.py").exists(), "Missing packaging/installer/build_installer.py.", errors)
    require((ROOT / "packaging" / "installer" / "install_screensave.ps1").exists(), "Missing packaging/installer/install_screensave.ps1.", errors)
    require((ROOT / "packaging" / "installer" / "uninstall_screensave.ps1").exists(), "Missing packaging/installer/uninstall_screensave.ps1.", errors)
    require((ROOT / "packaging" / "installer" / "installer_common.ps1").exists(), "Missing packaging/installer/installer_common.ps1.", errors)
    require((ROOT / "packaging" / "installer" / "layout.md").exists(), "Missing packaging/installer/layout.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "installer-release-notes.md").exists(), "Missing packaging/release_notes/installer-release-notes.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "core-baseline-notes.md").exists(), "Missing packaging/release_notes/core-baseline-notes.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "core-compatibility-notes.md").exists(), "Missing packaging/release_notes/core-compatibility-notes.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-installer-matrix.md").exists(), "Missing validation/notes/c16-installer-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-core-inclusion-matrix.md").exists(), "Missing validation/notes/c16-core-inclusion-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-release-baseline.md").exists(), "Missing validation/notes/c16-release-baseline.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-known-issues.md").exists(), "Missing validation/notes/c16-known-issues.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-companion-channel-matrix.md").exists(), "Missing validation/notes/c16-companion-channel-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c14-config-integrity.md").exists(), "Missing validation/notes/c14-config-integrity.md.", errors)
    require((ROOT / "validation" / "notes" / "c05-windows-integration-matrix.md").exists(), "Missing validation/notes/c05-windows-integration-matrix.md.", errors)

    if errors:
        for message in errors:
            print(message, file=sys.stderr)
        return 1

    manifest = read_manifest()
    staging_root = ROOT / "out" / "installer" / manifest.get("bundle", "staging_name")
    zip_path = ROOT / "out" / "installer" / manifest.get("bundle", "zip_name")
    portable_root = ROOT / manifest.get("portable_source", "staging_root")

    require(staging_root.is_dir(), f"Missing staged installer directory: {staging_root.relative_to(ROOT)}", errors)
    require(zip_path.is_file(), f"Missing staged installer zip: {zip_path.relative_to(ROOT)}", errors)
    require(portable_root.is_dir(), f"Missing portable payload source: {portable_root.relative_to(ROOT)}", errors)

    staged_payload_root = staging_root / "PAYLOAD"
    require(staged_payload_root.is_dir(), "Staged installer payload is missing.", errors)
    require((staging_root / "DOCS" / "INSTALLER-STATUS.md").is_file(), "Missing staged installer status note.", errors)
    require((staging_root / "DOCS" / "INSTALL-UNINSTALL-MATRIX.md").is_file(), "Missing staged installer matrix copy.", errors)
    require((staging_root / "DOCS" / "INSTALLER-RELEASE-NOTES.md").is_file(), "Missing staged installer release-notes copy.", errors)
    require((staging_root / "DOCS" / "CORE-BASELINE.md").is_file(), "Missing staged installer core-baseline note.", errors)
    require((staging_root / "DOCS" / "RELEASE-BASELINE.md").is_file(), "Missing staged installer release-baseline note.", errors)
    require((staging_root / "DOCS" / "COMPANION-CHANNELS.md").is_file(), "Missing staged installer companion-channel note.", errors)
    require((staging_root / "DOCS" / "KNOWN-ISSUES.md").is_file(), "Missing staged installer known-issues note.", errors)
    require((staging_root / "DOCS" / "COMPATIBILITY-NOTES.md").is_file(), "Missing staged installer compatibility note.", errors)
    require((staging_root / "DOCS" / "CONFIG-INTEGRITY.md").is_file(), "Missing staged installer config-integrity note.", errors)
    require((staging_root / "DOCS" / "WINDOWS-INTEGRATION.md").is_file(), "Missing staged installer Windows integration note.", errors)
    require((staging_root / "DOCS" / "INSTALLER-PACKAGING.md").is_file(), "Missing staged installer packaging note.", errors)
    require((staging_root / "DOCS" / "INSTALLER-LAYOUT.md").is_file(), "Missing staged installer layout note.", errors)
    require((staging_root / "DOCS" / "INCLUSION-MATRIX.md").is_file(), "Missing staged installer Core inclusion matrix copy.", errors)
    require((staging_root / "DOCS" / "RELEASE-CHANNELS.md").is_file(), "Missing staged installer release-channels copy.", errors)
    require((staging_root / "DOCS" / "CORE-DOCTRINE.md").is_file(), "Missing staged installer core-doctrine copy.", errors)
    require((staging_root / "DOCS" / "CHANNEL-MATRIX.md").is_file(), "Missing staged installer channel-matrix copy.", errors)
    require((staging_root / "DOCS" / "CHANNEL-MANIFEST.ini").is_file(), "Missing staged installer channel-manifest copy.", errors)
    require((staging_root / "DOCS" / "CHANGELOG.md").is_file(), "Missing staged installer changelog copy.", errors)
    require((staging_root / "DOCS" / "SOURCE-INSTALLER-MANIFEST.ini").is_file(), "Missing staged installer source-manifest copy.", errors)
    require((staging_root / "DOCS" / "ASSETS-LICENSES.md").is_file(), "Missing staged installer assets-license copy.", errors)
    require((staging_root / "DOCS" / "THIRD-PARTY-LICENSES.md").is_file(), "Missing staged installer third-party-license copy.", errors)
    require((staging_root / "DOCS" / "FILE-LIST.txt").is_file(), "Missing staged installer file list.", errors)
    require((staging_root / "DOCS" / "SHA256.txt").is_file(), "Missing staged installer hash list.", errors)
    require((staging_root / "README.txt").is_file(), "Missing staged installer README.txt.", errors)
    require(not (staging_root / "DOCS" / "RELEASE-CANDIDATE.md").exists(), "Installer must not stage the obsolete release-candidate note name.", errors)
    require(not (staging_root / "DOCS" / "RELEASE-READINESS.md").exists(), "Installer must not stage the obsolete release-readiness note name.", errors)

    portable_savers = saver_names(portable_root / "SAVERS")
    staged_savers = saver_names(staged_payload_root / "SAVERS")
    require(portable_savers == staged_savers, "Staged installer saver payload does not match the current portable payload.", errors)
    require(not any(path.name.lower() == "benchlab.exe" for path in staged_payload_root.rglob("*")), "BenchLab must not be included in the installer payload.", errors)
    require(not any("suite" in path.name.lower() for path in staged_payload_root.rglob("*") if path.is_file()), "Suite-level artifacts must not be included in the installer payload.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c16-installer-matrix.md")
    require("# C16 Installer Matrix" in note, "C16 installer note is missing its title.", errors)
    require("| `anthology` | `anthology.scr` | yes | yes |" in note, "C16 installer note must record anthology as present in the payload.", errors)
    require("`suite` remains excluded from the Installer payload." in note, "C16 installer note must record Suite as excluded.", errors)
    require("BenchLab remains excluded from the Installer payload." in note, "C16 installer note must record BenchLab as excluded.", errors)
    require("The SDK remains source-only and is not part of the Installer payload." in note, "C16 installer note must record the SDK exclusion.", errors)
    require("Extras remains empty in `C16` and is not part of the Installer payload." in note, "C16 installer note must record the Extras exclusion.", errors)

    readme = read_text(ROOT / "README.md")
    require("Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16`." in readme, "README.md must record C16 as complete.", errors)
    require("The continuation bridge is now closed and future work should resume in a new post-release `S`-series program." in readme, "README.md must record the post-C16 handoff.", errors)

    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    require("Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, the completed `C13` family-polish work, the completed `C14` release-hardening pass, the completed `C15` release-doctrine and channel-split pass, and the completed `C16` Core baseline freeze." in prompt_program, "prompt-program.md must record C16 as complete.", errors)
    require("`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in prompt_program, "prompt-program.md must record the completed C16 freeze.", errors)
    require("- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C14`, `C15`, and `C16` are complete." in prompt_program, "prompt-program.md must keep the completed C00-C12, C14-C16 line.", errors)

    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    require("`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16` are complete." in series_map, "series-map.md must record C16 as complete.", errors)
    require("The continuation bridge now extends through the frozen `C16` Core baseline. Future work should start in a new post-release `S`-series program." in series_map, "series-map.md must point to the post-C16 handoff.", errors)

    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    require("20. `C16` refreshes and freezes the actual Core baseline against the `C15` doctrine." in post_s15, "post-s15-plan.md must record the C16 freeze decision.", errors)
    require("21. `C16` closes the continuation bridge and hands off to a new post-release `S`-series program." in post_s15, "post-s15-plan.md must record the C16 handoff decision.", errors)
    require("The continuation bridge is closed after `C16`, and future work should start from the frozen baseline in a new post-release `S`-series program." in post_s15, "post-s15-plan.md must point to the post-C16 handoff.", errors)

    if errors:
        for message in errors:
            print(message, file=sys.stderr)
        return 1

    print("Installer layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
