"""Validate the C07 installer definition, staged package, and status docs."""

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
    require((ROOT / "validation" / "notes" / "c07-installer-matrix.md").exists(), "Missing validation/notes/c07-installer-matrix.md.", errors)

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
    require((staging_root / "README.txt").is_file(), "Missing staged installer README.txt.", errors)

    portable_savers = saver_names(portable_root / "SAVERS")
    staged_savers = saver_names(staged_payload_root / "SAVERS")
    require(portable_savers == staged_savers, "Staged installer saver payload does not match the current portable payload.", errors)
    require(not any(path.name.lower() == "benchlab.exe" for path in staged_payload_root.rglob("*")), "BenchLab must not be included in the installer payload.", errors)
    require(not any("suite" in path.name.lower() for path in staged_payload_root.rglob("*") if path.is_file()), "Suite-level artifacts must not be included in the installer payload.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c07-installer-matrix.md")
    require("# C07 Installer Matrix" in note, "C07 installer note is missing its title.", errors)
    require("- `C08` suite meta-saver for cross-saver randomization" in note, "C07 installer note must name the next continuation step.", errors)

    readme = read_text(ROOT / "README.md")
    require("Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, and `C07`." in readme, "README.md must record C07 as complete.", errors)
    require("The next continuation phase is `C08` for the suite meta-saver and cross-saver randomization." in readme, "README.md must point to C08.", errors)

    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    require("Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, and `C07` work plus later continuation prompts." in prompt_program, "prompt-program.md must record C07 as complete.", errors)
    require("C08 is the next planned implementation prompt after installer, registration, and uninstall flow." in prompt_program, "prompt-program.md must point to C08.", errors)
    require("- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, and `C07` are complete." in prompt_program, "prompt-program.md must mark C07 complete.", errors)

    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    require("`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, and `C07` are complete." in series_map, "series-map.md must mark C07 complete.", errors)
    require("The next planned implementation prompt is `C08` suite meta-saver for cross-saver randomization." in series_map, "series-map.md must point to C08.", errors)

    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    require("8. `C07` completed installer, registration, and uninstall work before `C08` suite meta-saver work." in post_s15, "post-s15-plan.md must record C07 as complete.", errors)
    require("The next continuation prompt should be `C08` for the suite meta-saver and cross-saver randomization." in post_s15, "post-s15-plan.md must point to C08.", errors)

    if errors:
        for message in errors:
            print(message)
        return 1

    print("C07 installer layout validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
