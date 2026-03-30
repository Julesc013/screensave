"""Validate the refreshed C14 installer definition, staged package, and status docs."""

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
    parser = configparser.ConfigParser()
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
    require((ROOT / "packaging" / "release_notes" / "release-candidate-notes.md").exists(), "Missing packaging/release_notes/release-candidate-notes.md.", errors)
    require((ROOT / "validation" / "notes" / "c14-installer-matrix.md").exists(), "Missing validation/notes/c14-installer-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c14-release-readiness-matrix.md").exists(), "Missing validation/notes/c14-release-readiness-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c14-known-issues.md").exists(), "Missing validation/notes/c14-known-issues.md.", errors)
    require((ROOT / "validation" / "notes" / "c14-config-integrity.md").exists(), "Missing validation/notes/c14-config-integrity.md.", errors)

    if errors:
        for message in errors:
            print(message)
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
    require((staging_root / "DOCS" / "RELEASE-CANDIDATE.md").is_file(), "Missing staged installer release-candidate note.", errors)
    require((staging_root / "DOCS" / "RELEASE-READINESS.md").is_file(), "Missing staged installer release-readiness note.", errors)
    require((staging_root / "DOCS" / "KNOWN-ISSUES.md").is_file(), "Missing staged installer known-issues note.", errors)
    require((staging_root / "DOCS" / "CONFIG-INTEGRITY.md").is_file(), "Missing staged installer config-integrity note.", errors)
    require((staging_root / "README.txt").is_file(), "Missing staged installer README.txt.", errors)

    portable_savers = saver_names(portable_root / "SAVERS")
    staged_savers = saver_names(staged_payload_root / "SAVERS")
    require(portable_savers == staged_savers, "Staged installer saver payload does not match the current portable payload.", errors)
    require(not any(path.name.lower() == "benchlab.exe" for path in staged_payload_root.rglob("*")), "BenchLab must not be included in the installer payload.", errors)
    require(not any("suite" in path.name.lower() for path in staged_payload_root.rglob("*") if path.is_file()), "Suite-level artifacts must not be included in the installer payload.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c14-installer-matrix.md")
    require("# C14 Installer Matrix" in note, "C14 installer note is missing its title.", errors)
    require("`C14` closes the current continuation line" in note, "C14 installer note must record the release-candidate closeout.", errors)

    readme = read_text(ROOT / "README.md")
    require("Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, and `C14`." in readme, "README.md must record C13 Wave C as complete.", errors)
    require("The current continuation line is complete through `C14` final rerelease hardening." in readme, "README.md must point to C14.", errors)

    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    require("Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, the completed `C13` family-polish work, and the completed `C14` release-hardening pass." in prompt_program, "prompt-program.md must record completed C13 family-polish work.", errors)
    require("`C13` Wave A, Wave B, and Wave C are complete, and `C14` final rerelease hardening has closed the current continuation line with the release-candidate pass." in prompt_program, "prompt-program.md must point to C14.", errors)
    require("- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, and `C14` are complete." in prompt_program, "prompt-program.md must keep the completed C00-C12 line.", errors)

    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    require("`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, and `C14` are complete." in series_map, "series-map.md must record C13 Wave C as complete.", errors)
    require("The current continuation line is complete through `C14` final rerelease hardening." in series_map, "series-map.md must point to C14.", errors)

    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    require("10. `C09` completed the real `suite` browser, launcher, preview, and saver-settings surface before `C10` SDK stabilization." in post_s15, "post-s15-plan.md must record C09 as complete.", errors)
    require("11. `C10` completed the real SDK and contributor surface before `C11` backlog ingestion begins." in post_s15, "post-s15-plan.md must record C10 as complete.", errors)
    require("12. `C11` completed structured backlog ingestion and routing before `C12` cross-cutting polish begins." in post_s15, "post-s15-plan.md must record C11 as complete.", errors)
    require("13. `C12` completed the cross-cutting polish and quality-bar pass before the first `C13` saver-specific polish wave begins." in post_s15, "post-s15-plan.md must record C12 as complete.", errors)
    require("15. `C13` Wave B completed the second saver-specific polish pass for `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems`." in post_s15, "post-s15-plan.md must record C13 Wave B as complete.", errors)
    require("No further continuation prompt is scheduled on the current line after `C14` final rerelease hardening." in post_s15, "post-s15-plan.md must point to C14.", errors)

    if errors:
        for message in errors:
            print(message)
        return 1

    print("Installer layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
