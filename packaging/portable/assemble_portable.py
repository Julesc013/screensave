"""Assemble the current ScreenSave Core candidate from real discovered outputs."""

from __future__ import annotations

import configparser
import hashlib
import pathlib
import shutil
import sys
import zipfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
MANIFEST_PATH = pathlib.Path(__file__).with_name("bundle_manifest.ini")


def read_manifest(path: pathlib.Path) -> configparser.ConfigParser:
    parser = configparser.ConfigParser(interpolation=None)
    parser.optionxform = str
    with path.open("r", encoding="utf-8") as handle:
        parser.read_file(handle)
    return parser


def split_list(value: str) -> list[str]:
    items: list[str] = []
    for raw_item in value.replace(",", ";").split(";"):
        item = raw_item.strip()
        if item:
            items.append(item)
    return items


def ensure_directory(path: pathlib.Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def write_text(path: pathlib.Path, text: str) -> None:
    ensure_directory(path.parent)
    path.write_text(text, encoding="utf-8", newline="\n")


def copy_file(source: pathlib.Path, destination: pathlib.Path) -> None:
    ensure_directory(destination.parent)
    shutil.copy2(source, destination)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def format_list(items: list[str]) -> str:
    if not items:
        return "(none)"
    return ", ".join(items)


def parse_savers(manifest: configparser.ConfigParser) -> list[dict[str, object]]:
    savers: list[dict[str, object]] = []
    for section in manifest.sections():
        if not section.startswith("saver."):
            continue

        saver_key = section.split(".", 1)[1]
        pack_dirs = split_list(manifest.get(section, "pack_dirs", fallback=""))
        savers.append(
            {
                "key": saver_key,
                "binary_relpath": manifest.get(section, "binary_relpath"),
                "manifest": pathlib.Path(manifest.get(section, "manifest")),
                "pack_dirs": [pathlib.Path(item) for item in pack_dirs],
            }
        )

    return savers


def discover_binary(output_roots: list[pathlib.Path], binary_relpath: str) -> pathlib.Path | None:
    relative_path = pathlib.Path(binary_relpath)
    for root in output_roots:
        candidate = root / relative_path
        if candidate.exists() and candidate.is_file():
            return candidate
    return None


def discover_legacy_outputs(output_roots: list[pathlib.Path], canonical_savers: set[str]) -> list[str]:
    legacy_outputs: list[str] = []
    for root in output_roots:
        if not root.exists():
            continue
        for candidate in sorted(root.rglob("*.scr")):
            if candidate.stem not in canonical_savers:
                legacy_outputs.append(str(candidate.relative_to(ROOT)))
    return legacy_outputs


def copy_packs(pack_dirs: list[pathlib.Path], destination_root: pathlib.Path) -> list[str]:
    staged_paths: list[str] = []
    for pack_root in pack_dirs:
        if not pack_root.exists():
            continue

        for pack_dir in sorted(pack_root.iterdir()):
            if not pack_dir.is_dir():
                continue
            destination = destination_root / pack_dir.name
            shutil.copytree(pack_dir, destination, dirs_exist_ok=True)
            staged_paths.append(str(destination.relative_to(destination_root.parent.parent)))
    return staged_paths


def sha256_for_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        while True:
            chunk = handle.read(65536)
            if not chunk:
                break
            digest.update(chunk)
    return digest.hexdigest()


def build_root_readme(
    bundle_name: str,
    included_savers: list[str],
    missing_savers: list[str],
    legacy_outputs: list[str],
) -> str:
    lines = [
        f"ScreenSave Core Candidate ({bundle_name})",
        "",
        "This is the current portable ZIP candidate for ScreenSave Core under the C15 release doctrine.",
        "",
        "Included saver binaries:",
        f"- {format_list(included_savers)}",
        "",
        "Missing canonical saver binaries in the current output roots:",
        f"- {format_list(missing_savers)}",
        "",
        "Manual use:",
        "1. Open the SAVERS directory.",
        "2. Choose the saver .scr you want to use.",
        "3. Use Windows screen saver settings or the file context menu to install it manually.",
        "4. Use the saver's own Settings dialog for configuration.",
        "",
        "This Core candidate does not include installer, registration, or uninstall automation.",
        "Installed distribution is available separately through the optional ScreenSave Installer channel.",
        "BenchLab, Suite, SDK material, and Extras are separate channels and are intentionally excluded from Core by default.",
        "Anthology is treated as a normal saver product and is staged only when its real binary exists in the configured output roots.",
        "",
        "Renderer notes:",
        "- GDI remains the guaranteed floor.",
        "- GL11 remains optional.",
        "- GL21 is available only where the saver and the system support it.",
        "- GL33 and GL46 are not shipped as real user-facing requirements in this bundle.",
        "",
        "See DOCS\\BUNDLE-STATUS.md, DOCS\\INCLUSION-MATRIX.md, DOCS\\RELEASE-READINESS.md, and DOCS\\KNOWN-ISSUES.md for details.",
    ]

    if legacy_outputs:
        lines.extend(
            [
                "",
                "Legacy outputs detected and excluded from this bundle:",
                f"- {format_list(legacy_outputs)}",
            ]
        )

    return "\n".join(lines) + "\n"


def build_status_note(
    bundle_name: str,
    output_roots: list[pathlib.Path],
    included: list[dict[str, object]],
    missing: list[dict[str, object]],
    legacy_outputs: list[str],
    benchlab_path: pathlib.Path | None,
    suite_path: pathlib.Path | None,
) -> str:
    lines = [
        f"# {bundle_name} Bundle Status",
        "",
        "## Assembly Provenance",
        "",
        "- Source-controlled manifest: `packaging/portable/bundle_manifest.ini`",
        "- Assembly script: `packaging/portable/assemble_portable.py`",
        "- Output roots inspected:",
    ]

    for root in output_roots:
        lines.append(f"  - `{root.relative_to(ROOT)}`")

    lines.extend(
        [
            "- Toolchains were not available in this environment, so the bundle was assembled from pre-existing output files only.",
            "",
            "## Included Canonical Saver Binaries",
            "",
        ]
    )

    if included:
        for entry in included:
            lines.append(
                f"- `{entry['key']}` from `{pathlib.Path(entry['binary_source']).relative_to(ROOT)}`"
            )
    else:
        lines.append("- None")

    lines.extend(
        [
            "",
            "## Missing Canonical Saver Binaries",
            "",
        ]
    )
    if missing:
        for entry in missing:
            lines.append(f"- `{entry['key']}` expected at `{entry['binary_relpath']}`")
    else:
        lines.append("- None")

    lines.extend(
        [
            "",
            "## Legacy Outputs Excluded",
            "",
        ]
    )
    if legacy_outputs:
        for item in legacy_outputs:
            lines.append(f"- `{item}`")
    else:
        lines.append("- None")

    lines.extend(
        [
            "",
            "## Bundle Policy",
            "",
            "- Anthology is part of the canonical saver line and is staged when its real binary exists.",
            "- BenchLab is excluded from the Core candidate payload.",
            "- Suite, SDK material, and Extras are excluded from the Core candidate payload unless a later doctrine update says otherwise.",
            "- File-backed packs are staged only when the owning saver binary is present.",
            "- Missing binaries are recorded explicitly instead of replaced with placeholders.",
            "",
            "## App Output Discovery",
            "",
        ]
    )

    if benchlab_path is not None:
        lines.append(
            f"- Developer-only artifact detected at `{benchlab_path.relative_to(ROOT)}` and excluded by policy."
        )
    else:
        lines.append("- No BenchLab output was discovered in the configured roots.")

    if suite_path is not None:
        lines.append(
            f"- Suite app artifact detected at `{suite_path.relative_to(ROOT)}` and excluded from the saver-only portable bundle by policy."
        )
    else:
        lines.append("- No Suite app output was discovered in the configured roots.")

    lines.extend(
        [
            "",
            "## Known Limitations",
            "",
            "- This Core candidate stage remains partial when canonical saver outputs are missing from the local output roots.",
            "- The included binaries were not rebuilt during C14 because no supported toolchain was available here.",
            "- BenchLab, Suite, SDK material, and Extras remain separate channels and are not part of Core by default.",
            "- C16 still has to apply the Core inclusion gate and freeze the actual Core baseline.",
        ]
    )

    return "\n".join(lines) + "\n"


def write_file_inventory(staging_root: pathlib.Path) -> None:
    files = sorted(
        path.relative_to(staging_root)
        for path in staging_root.rglob("*")
        if path.is_file()
    )

    file_list = "\n".join(str(path).replace("/", "\\") for path in files) + "\n"
    write_text(staging_root / "DOCS" / "FILE-LIST.txt", file_list)

    hash_lines = []
    for relative_path in files:
        if str(relative_path).replace("\\", "/") == "DOCS/SHA256.txt":
            continue
        absolute_path = staging_root / relative_path
        relative_path_windows = str(relative_path).replace("/", "\\")
        hash_lines.append(
            f"{sha256_for_file(absolute_path)}  {relative_path_windows}"
        )
    write_text(staging_root / "DOCS" / "SHA256.txt", "\n".join(hash_lines) + "\n")


def write_placeholder_notes(staging_root: pathlib.Path, included_pack_count: int) -> None:
    write_text(
        staging_root / "PRESETS" / "README.txt",
        "Standalone portable preset-export files are not staged separately in the current Core candidate baseline.\n"
        "File-backed preset examples ship inside PACKS only when the owning saver binary is present.\n",
    )
    write_text(
        staging_root / "THEMES" / "README.txt",
        "Standalone portable theme-export files are not staged separately in the current Core candidate baseline.\n"
        "File-backed theme examples ship inside PACKS only when the owning saver binary is present.\n",
    )

    if included_pack_count == 0:
        pack_note = (
            "No file-backed pack content was staged in this bundle.\n"
            "Current sample packs belong to savers whose canonical binaries were not discovered in the configured output roots.\n"
        )
    else:
        pack_note = (
            "This directory contains file-backed packs for staged savers only.\n"
            "Invalid or unsupported packs are ignored safely by the shared settings architecture.\n"
        )
    write_text(staging_root / "PACKS" / "README.txt", pack_note)

    write_text(
        staging_root / "OPTIONAL" / "README.txt",
        "No optional Extras content is included in the current Core candidate bundle.\n"
        "BenchLab, Suite, and SDK material remain separate channels and are intentionally excluded here.\n",
    )


def create_zip(staging_root: pathlib.Path, zip_path: pathlib.Path) -> None:
    if zip_path.exists():
        zip_path.unlink()

    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(staging_root.rglob("*")):
            if path.is_file():
                archive.write(path, staging_root.name + "/" + str(path.relative_to(staging_root)))


def main() -> int:
    manifest = read_manifest(MANIFEST_PATH)
    bundle_name = manifest.get("bundle", "staging_name")
    zip_name = manifest.get("bundle", "zip_name")
    output_roots = [
        ROOT / manifest.get("output_roots", option)
        for option in manifest.options("output_roots")
    ]
    savers = parse_savers(manifest)
    canonical_keys = {str(entry["key"]) for entry in savers}

    staging_root = ROOT / "out" / "portable" / bundle_name
    zip_path = ROOT / "out" / "portable" / zip_name

    if staging_root.exists():
        shutil.rmtree(staging_root)
    ensure_directory(staging_root)

    ensure_directory(staging_root / "SAVERS")
    ensure_directory(staging_root / "MANIFESTS")
    ensure_directory(staging_root / "PRESETS")
    ensure_directory(staging_root / "THEMES")
    ensure_directory(staging_root / "PACKS")
    ensure_directory(staging_root / "DOCS")
    ensure_directory(staging_root / "LICENSES")
    ensure_directory(staging_root / "OPTIONAL")

    included: list[dict[str, object]] = []
    missing: list[dict[str, object]] = []
    included_pack_count = 0

    for entry in savers:
        binary_source = discover_binary(output_roots, str(entry["binary_relpath"]))
        manifest_source = ROOT / pathlib.Path(str(entry["manifest"]))

        if binary_source is None:
            missing.append(entry)
            continue

        copy_file(binary_source, staging_root / "SAVERS" / f"{entry['key']}.scr")
        copy_file(manifest_source, staging_root / "MANIFESTS" / f"{entry['key']}.manifest.ini")

        staged_pack_root = staging_root / "PACKS" / str(entry["key"])
        staged_pack_paths = copy_packs(list(entry["pack_dirs"]), staged_pack_root)
        if staged_pack_paths:
            included_pack_count += len(staged_pack_paths)

        included.append(
            {
                "key": entry["key"],
                "binary_source": binary_source,
                "binary_relpath": entry["binary_relpath"],
                "staged_packs": staged_pack_paths,
            }
        )

    legacy_outputs = discover_legacy_outputs(output_roots, canonical_keys)
    benchlab_path = discover_binary(output_roots, "benchlab/benchlab.exe")
    suite_path = discover_binary(output_roots, "suite/suite.exe")

    docs = manifest["docs"]
    copy_file(ROOT / docs["portable_readme"], staging_root / "DOCS" / "PORTABLE-BUNDLE.md")
    copy_file(ROOT / docs["portable_layout"], staging_root / "DOCS" / "PORTABLE-LAYOUT.md")
    copy_file(ROOT / docs["release_notes"], staging_root / "DOCS" / "PORTABLE-RELEASE-NOTES.md")
    copy_file(ROOT / docs["bundle_matrix"], staging_root / "DOCS" / "INCLUSION-MATRIX.md")
    copy_file(ROOT / docs["release_channels"], staging_root / "DOCS" / "RELEASE-CHANNELS.md")
    copy_file(ROOT / docs["core_doctrine"], staging_root / "DOCS" / "CORE-DOCTRINE.md")
    copy_file(ROOT / docs["channel_matrix"], staging_root / "DOCS" / "CHANNEL-MATRIX.md")
    copy_file(ROOT / docs["channel_manifest"], staging_root / "DOCS" / "CHANNEL-MANIFEST.ini")
    copy_file(ROOT / docs["release_candidate_notes"], staging_root / "DOCS" / "RELEASE-CANDIDATE.md")
    copy_file(ROOT / docs["release_readiness"], staging_root / "DOCS" / "RELEASE-READINESS.md")
    copy_file(ROOT / docs["known_issues"], staging_root / "DOCS" / "KNOWN-ISSUES.md")
    copy_file(ROOT / docs["integrity_note"], staging_root / "DOCS" / "CONFIG-INTEGRITY.md")
    copy_file(ROOT / docs["windows_validation"], staging_root / "DOCS" / "WINDOWS-INTEGRATION.md")
    copy_file(ROOT / docs["project_changelog"], staging_root / "DOCS" / "CHANGELOG.md")
    copy_file(ROOT / docs["assets_license"], staging_root / "LICENSES" / "ASSETS-LICENSES.md")
    copy_file(ROOT / docs["third_party_license"], staging_root / "LICENSES" / "THIRD-PARTY-LICENSES.md")
    copy_file(MANIFEST_PATH, staging_root / "DOCS" / "SOURCE-BUNDLE-MANIFEST.ini")

    write_text(
        staging_root / "README.txt",
        build_root_readme(
            bundle_name,
            [str(entry["key"]) for entry in included],
            [str(entry["key"]) for entry in missing],
            legacy_outputs,
        ),
    )
    write_text(
        staging_root / "DOCS" / "BUNDLE-STATUS.md",
        build_status_note(bundle_name, output_roots, included, missing, legacy_outputs, benchlab_path, suite_path),
    )
    write_placeholder_notes(staging_root, included_pack_count)
    write_file_inventory(staging_root)
    create_zip(staging_root, zip_path)

    print(f"Staged bundle: {staging_root.relative_to(ROOT)}")
    print(f"Zip artifact: {zip_path.relative_to(ROOT)}")
    print(f"Included savers: {format_list([str(entry['key']) for entry in included])}")
    print(f"Missing savers: {format_list([str(entry['key']) for entry in missing])}")
    if legacy_outputs:
        print(f"Legacy outputs excluded: {format_list(legacy_outputs)}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
