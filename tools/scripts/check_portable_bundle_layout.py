"""Validate the frozen C16 Core bundle definition and staged output."""

from __future__ import annotations

import configparser
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "packaging" / "portable" / "bundle_manifest.ini"


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


def parse_saver_sections(manifest: configparser.ConfigParser) -> list[tuple[str, str]]:
    savers: list[tuple[str, str]] = []
    for section in manifest.sections():
        if section.startswith("saver."):
            saver = section.split(".", 1)[1]
            savers.append((saver, manifest.get(section, "binary_relpath")))
    return savers


def discover_binary(output_roots: list[pathlib.Path], relative_path: str) -> pathlib.Path | None:
    candidate_relative = pathlib.Path(relative_path)
    for root in output_roots:
        candidate = root / candidate_relative
        if candidate.exists() and candidate.is_file():
            return candidate
    return None


def check_source_controlled_defs(errors: list[str]) -> None:
    require(MANIFEST_PATH.exists(), "Missing packaging/portable/bundle_manifest.ini.", errors)
    require((ROOT / "packaging" / "portable" / "assemble_portable.py").exists(), "Missing packaging/portable/assemble_portable.py.", errors)
    require((ROOT / "packaging" / "portable" / "layout.md").exists(), "Missing packaging/portable/layout.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "portable-release-notes.md").exists(), "Missing packaging/release_notes/portable-release-notes.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "core-baseline-notes.md").exists(), "Missing packaging/release_notes/core-baseline-notes.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "core-compatibility-notes.md").exists(), "Missing packaging/release_notes/core-compatibility-notes.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-core-inclusion-matrix.md").exists(), "Missing validation/notes/c16-core-inclusion-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-release-baseline.md").exists(), "Missing validation/notes/c16-release-baseline.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-known-issues.md").exists(), "Missing validation/notes/c16-known-issues.md.", errors)
    require((ROOT / "validation" / "notes" / "c16-companion-channel-matrix.md").exists(), "Missing validation/notes/c16-companion-channel-matrix.md.", errors)
    require((ROOT / "validation" / "notes" / "c14-config-integrity.md").exists(), "Missing validation/notes/c14-config-integrity.md.", errors)
    require((ROOT / "validation" / "notes" / "c05-windows-integration-matrix.md").exists(), "Missing validation/notes/c05-windows-integration-matrix.md.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c16-core-inclusion-matrix.md")
    require("# C16 Core Inclusion Matrix" in note, "C16 inclusion note is missing its title.", errors)
    require("| `anthology` | included |" in note, "C16 inclusion note must record anthology as included.", errors)
    require("No current saver candidate was deferred to Extras in `C16`." in note, "C16 inclusion note must record the Extras deferral result.", errors)


def check_staged_bundle(manifest: configparser.ConfigParser, errors: list[str]) -> None:
    output_roots = [ROOT / manifest.get("output_roots", option) for option in manifest.options("output_roots")]
    saver_sections = parse_saver_sections(manifest)
    expected_staged = sorted(
        saver for saver, relative_path in saver_sections if discover_binary(output_roots, relative_path) is not None
    )
    canonical_savers = {saver for saver, _relative_path in saver_sections}

    staging_root = ROOT / "out" / "portable" / manifest.get("bundle", "staging_name")
    zip_path = ROOT / "out" / "portable" / manifest.get("bundle", "zip_name")

    require(staging_root.is_dir(), f"Missing staged portable directory: {staging_root.relative_to(ROOT)}", errors)
    require(zip_path.is_file(), f"Missing staged portable zip: {zip_path.relative_to(ROOT)}", errors)
    if not staging_root.is_dir():
        return

    staged_savers = sorted(path.stem for path in (staging_root / "SAVERS").glob("*.scr"))
    require(staged_savers == expected_staged, "Staged SAVERS contents do not match the currently discovered canonical saver outputs.", errors)
    require(all(saver in canonical_savers for saver in staged_savers), "Staged SAVERS contains a non-canonical saver artifact.", errors)
    require(not (staging_root / "OPTIONAL" / "benchlab.exe").exists(), "BenchLab must not be staged in the end-user Core bundle.", errors)
    require(not any(path.name.lower() == "suite.exe" for path in staging_root.rglob("*") if path.is_file()), "Suite must not be staged in the end-user Core bundle.", errors)

    staged_manifests = sorted(
        path.name.replace(".manifest.ini", "")
        for path in (staging_root / "MANIFESTS").glob("*.manifest.ini")
    )
    require(staged_manifests == expected_staged, "Staged MANIFESTS contents do not match the staged saver set.", errors)

    for relative_path in (
        pathlib.Path("README.txt"),
        pathlib.Path("DOCS") / "BUNDLE-STATUS.md",
        pathlib.Path("DOCS") / "FILE-LIST.txt",
        pathlib.Path("DOCS") / "SHA256.txt",
        pathlib.Path("DOCS") / "INCLUSION-MATRIX.md",
        pathlib.Path("DOCS") / "CORE-RELEASE-NOTES.md",
        pathlib.Path("DOCS") / "CORE-BASELINE.md",
        pathlib.Path("DOCS") / "RELEASE-BASELINE.md",
        pathlib.Path("DOCS") / "KNOWN-ISSUES.md",
        pathlib.Path("DOCS") / "COMPATIBILITY-NOTES.md",
        pathlib.Path("DOCS") / "COMPANION-CHANNELS.md",
        pathlib.Path("DOCS") / "CONFIG-INTEGRITY.md",
        pathlib.Path("DOCS") / "WINDOWS-INTEGRATION.md",
        pathlib.Path("DOCS") / "PORTABLE-BUNDLE.md",
        pathlib.Path("DOCS") / "PORTABLE-LAYOUT.md",
        pathlib.Path("DOCS") / "RELEASE-CHANNELS.md",
        pathlib.Path("DOCS") / "CORE-DOCTRINE.md",
        pathlib.Path("DOCS") / "CHANNEL-MATRIX.md",
        pathlib.Path("DOCS") / "CHANNEL-MANIFEST.ini",
        pathlib.Path("DOCS") / "CHANGELOG.md",
        pathlib.Path("DOCS") / "SOURCE-BUNDLE-MANIFEST.ini",
        pathlib.Path("LICENSES") / "ASSETS-LICENSES.md",
        pathlib.Path("LICENSES") / "THIRD-PARTY-LICENSES.md",
        pathlib.Path("PRESETS") / "README.txt",
        pathlib.Path("THEMES") / "README.txt",
        pathlib.Path("PACKS") / "README.txt",
        pathlib.Path("OPTIONAL") / "README.txt",
    ):
        require((staging_root / relative_path).is_file(), f"Missing staged bundle file: {(staging_root / relative_path).relative_to(ROOT)}", errors)

    require(not (staging_root / "DOCS" / "RELEASE-CANDIDATE.md").exists(), "Portable bundle must not stage the obsolete release-candidate note name.", errors)
    require(not (staging_root / "DOCS" / "RELEASE-READINESS.md").exists(), "Portable bundle must not stage the obsolete release-readiness note name.", errors)


def check_status_docs(errors: list[str]) -> None:
    readme = read_text(ROOT / "README.md")
    require("Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16`." in readme, "README.md must record C16 as complete.", errors)
    require("Post-`C16` work now proceeds as a short `SS` release-closure series first:" in readme, "README.md must record the post-C16 handoff.", errors)

    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    require("Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, the completed `C13` family-polish work, the completed `C14` release-hardening pass, the completed `C15` release-doctrine and channel-split pass, and the completed `C16` Core baseline freeze." in prompt_program, "prompt-program.md must record C16 as complete.", errors)
    require("`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in prompt_program, "prompt-program.md must record the completed C16 freeze.", errors)
    require("- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C14`, `C15`, and `C16` are complete." in prompt_program, "prompt-program.md must keep the completed C00-C12, C14-C16 line.", errors)

    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    require("`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, `C10`, `C11`, `C12`, `C13` Wave A, Wave B, and Wave C, `C14`, `C15`, and `C16` are complete." in series_map, "series-map.md must record C16 as complete.", errors)
    require("`SS` runs first as the short release-closure bridge." in series_map, "series-map.md must point to the post-C16 handoff.", errors)

    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    require("20. `C16` refreshes and freezes the actual Core baseline against the `C15` doctrine." in post_s15, "post-s15-plan.md must record the C16 freeze decision.", errors)
    require("21. `C16` closes the continuation bridge and hands off to the short `SS` release-closure series, followed by the `PL` flagship program." in post_s15, "post-s15-plan.md must record the C16 handoff decision.", errors)
    require("This file remains as historical record for the completed bridge; active post-`C16` planning now runs `SS` first and `PL` second from the frozen Core baseline." in post_s15, "post-s15-plan.md must point to the post-C16 handoff.", errors)

    release_channels = read_text(ROOT / "docs" / "roadmap" / "release-channels.md")
    require("## Current C16 State" in release_channels, "release-channels.md must expose the current C16 state section.", errors)
    require("`C16` froze `ScreenSave Core` at `out/portable/screensave-core-c16-baseline/` with a matching zip beside it." in release_channels, "release-channels.md must record the frozen C16 Core output.", errors)


def main() -> int:
    errors: list[str] = []
    manifest = read_ini(MANIFEST_PATH)

    check_source_controlled_defs(errors)
    check_staged_bundle(manifest, errors)
    check_status_docs(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Portable bundle layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

