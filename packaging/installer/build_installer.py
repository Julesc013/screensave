"""Stage the C14 current-user installer refresh from the real portable payload."""

from __future__ import annotations

import configparser
import hashlib
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "packaging" / "installer" / "installer_manifest.ini"


def read_manifest() -> configparser.ConfigParser:
    parser = configparser.ConfigParser()
    with MANIFEST_PATH.open("r", encoding="ascii") as handle:
        parser.read_file(handle)
    return parser


def ensure_clean_directory(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def copy_file(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, destination)


def copy_tree(source: Path, destination: Path) -> None:
    if destination.exists():
        shutil.rmtree(destination)
    shutil.copytree(source, destination)


def sha256_for_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def discover_payload_savers(payload_root: Path) -> list[str]:
    savers_root = payload_root / "SAVERS"
    if not savers_root.is_dir():
        raise SystemExit(f"Portable payload is missing SAVERS/: {savers_root}")
    return sorted(path.stem for path in savers_root.glob("*.scr"))


def write_readme(staging_root: Path, savers: list[str], manifest: configparser.ConfigParser) -> None:
    display_name = manifest.get("bundle", "display_name")
    lines = [
        display_name,
        "",
        "This is the C14 scripted current-user installer refresh for the ScreenSave saver line.",
        "It complements the current portable bundle and does not replace portable distribution.",
        "",
        "How to use this package:",
        "1. Extract this package to a writable directory.",
        "2. Run install_screensave.ps1 from PowerShell.",
        "3. Optionally pass -SetActiveSaver <slug> to set the current-user active saver.",
        "4. Use the installed uninstall script under INSTALLER/ to remove the package later.",
        "",
        "Supported install mode:",
        "- current-user only",
        "",
        "Current payload saver coverage:",
        f"- Included saver binaries: {', '.join(savers) if savers else 'none'}",
        "",
        "Important limits:",
        "- This installer package is only as complete as the real portable payload it consumes.",
        "- Machine-wide install is deferred after C07.",
        "- BenchLab and Suite are separate app products and are not part of this package.",
        "- Anthology is treated as a normal saver product and is included only when its binary exists in the current payload.",
    ]
    (staging_root / "README.txt").write_text("\n".join(lines) + "\n", encoding="ascii")


def write_status_note(
    staging_root: Path,
    savers: list[str],
    missing_payload: bool,
    manifest: configparser.ConfigParser,
) -> None:
    lines = [
        "# Installer Status",
        "",
        "This note describes the staged C14 installer package.",
        "",
        "## Package Source",
        "",
        f"- Installer manifest: `{MANIFEST_PATH.relative_to(ROOT).as_posix()}`",
        f"- Portable payload source: `{manifest.get('portable_source', 'staging_root')}`",
        "",
        "## Current Payload",
        "",
        f"- Included saver binaries: {', '.join(savers) if savers else 'none'}",
        "- BenchLab is excluded from the installed end-user package.",
        "- Suite is a separate app product and is excluded from the installed end-user package.",
        "- Anthology is included only when it exists in the current portable payload.",
        "",
        "## Policy Summary",
        "",
        "- Supported install mode: current-user only",
        "- Active saver selection assistance: opt-in only",
        "- Uninstall record: current-user uninstall key",
        "- Update behavior: conservative overlay; no payload-absent pruning by default",
        "- User config cleanup: preserved outside the dedicated install root",
        "",
        "## Known Limits",
        "",
        "- The installer package depends on the current portable payload for saver coverage.",
        "- Machine-wide install remains deferred after C07.",
    ]
    if missing_payload:
        lines.append("- The current payload is partial because some canonical saver outputs are still missing from the local output roots.")
    (staging_root / "DOCS" / "INSTALLER-STATUS.md").write_text("\n".join(lines) + "\n", encoding="ascii")


def write_file_list(staging_root: Path) -> None:
    entries = []
    for path in sorted(staging_root.rglob("*")):
        if path.is_file():
            entries.append(path.relative_to(staging_root).as_posix())
    (staging_root / "DOCS" / "FILE-LIST.txt").write_text("\n".join(entries) + "\n", encoding="ascii")


def write_hashes(staging_root: Path) -> None:
    entries = []
    for path in sorted(staging_root.rglob("*")):
        if path.is_file() and path.name != "SHA256.txt":
            digest = sha256_for_file(path)
            entries.append(f"{digest}  {path.relative_to(staging_root).as_posix()}")
    (staging_root / "DOCS" / "SHA256.txt").write_text("\n".join(entries) + "\n", encoding="ascii")


def build_zip(staging_root: Path, zip_path: Path) -> None:
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    base_name = zip_path.with_suffix("")
    if zip_path.exists():
        zip_path.unlink()
    shutil.make_archive(str(base_name), "zip", root_dir=staging_root.parent, base_dir=staging_root.name)


def main() -> None:
    manifest = read_manifest()

    portable_root = ROOT / manifest.get("portable_source", "staging_root")
    if not portable_root.is_dir():
        raise SystemExit(f"Portable payload root is missing: {portable_root}")

    staging_root = ROOT / "out" / "installer" / manifest.get("bundle", "staging_name")
    zip_path = ROOT / "out" / "installer" / manifest.get("bundle", "zip_name")
    ensure_clean_directory(staging_root)
    (staging_root / "DOCS").mkdir(parents=True, exist_ok=True)

    for filename in (
        "installer_manifest.ini",
        "installer_common.ps1",
        "install_screensave.ps1",
        "uninstall_screensave.ps1",
    ):
        copy_file(ROOT / "packaging" / "installer" / filename, staging_root / filename)

    copy_tree(portable_root, staging_root / "PAYLOAD")

    docs = manifest["docs"]
    copy_file(ROOT / docs["installer_readme"], staging_root / "DOCS" / "INSTALLER-PACKAGING.md")
    copy_file(ROOT / docs["installer_layout"], staging_root / "DOCS" / "INSTALLER-LAYOUT.md")
    copy_file(ROOT / docs["release_notes"], staging_root / "DOCS" / "INSTALLER-RELEASE-NOTES.md")
    copy_file(ROOT / docs["installer_matrix"], staging_root / "DOCS" / "INSTALL-UNINSTALL-MATRIX.md")
    copy_file(ROOT / docs["portable_matrix"], staging_root / "DOCS" / "PORTABLE-BUNDLE-MATRIX.md")
    copy_file(ROOT / docs["release_candidate_notes"], staging_root / "DOCS" / "RELEASE-CANDIDATE.md")
    copy_file(ROOT / docs["release_readiness"], staging_root / "DOCS" / "RELEASE-READINESS.md")
    copy_file(ROOT / docs["known_issues"], staging_root / "DOCS" / "KNOWN-ISSUES.md")
    copy_file(ROOT / docs["integrity_note"], staging_root / "DOCS" / "CONFIG-INTEGRITY.md")
    copy_file(ROOT / docs["windows_validation"], staging_root / "DOCS" / "WINDOWS-INTEGRATION.md")
    copy_file(ROOT / docs["project_changelog"], staging_root / "DOCS" / "CHANGELOG.md")
    copy_file(ROOT / docs["assets_license"], staging_root / "DOCS" / "ASSETS-LICENSES.md")
    copy_file(ROOT / docs["third_party_license"], staging_root / "DOCS" / "THIRD-PARTY-LICENSES.md")
    copy_file(MANIFEST_PATH, staging_root / "DOCS" / "SOURCE-INSTALLER-MANIFEST.ini")

    savers = discover_payload_savers(staging_root / "PAYLOAD")
    missing_payload = len(savers) < 19
    write_readme(staging_root, savers, manifest)
    write_status_note(staging_root, savers, missing_payload, manifest)
    write_file_list(staging_root)
    write_hashes(staging_root)
    build_zip(staging_root, zip_path)

    print(f"Staged installer package at {staging_root.relative_to(ROOT)}")
    print(f"Created installer zip at {zip_path.relative_to(ROOT)}")
    if savers:
        print("Included saver payload: " + ", ".join(savers))
    else:
        print("Included saver payload: none")


if __name__ == "__main__":
    main()
