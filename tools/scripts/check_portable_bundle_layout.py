"""Validate the C06 portable bundle definition and staged output."""

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


def check_source_controlled_defs(manifest: configparser.ConfigParser, errors: list[str]) -> None:
    require(MANIFEST_PATH.exists(), "Missing packaging/portable/bundle_manifest.ini.", errors)
    require((ROOT / "packaging" / "portable" / "assemble_portable.py").exists(), "Missing packaging/portable/assemble_portable.py.", errors)
    require((ROOT / "packaging" / "portable" / "layout.md").exists(), "Missing packaging/portable/layout.md.", errors)
    require((ROOT / "packaging" / "release_notes" / "portable-release-notes.md").exists(), "Missing packaging/release_notes/portable-release-notes.md.", errors)
    require((ROOT / "validation" / "notes" / "c06-portable-bundle-matrix.md").exists(), "Missing validation/notes/c06-portable-bundle-matrix.md.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c06-portable-bundle-matrix.md")
    require("# C06 Portable Bundle Matrix" in note, "C06 portable note is missing its title.", errors)
    require("The next continuation step is `C07` installer, registration, and uninstall flow." in note, "C06 portable note must name the next continuation step.", errors)


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
    require(not (staging_root / "OPTIONAL" / "benchlab.exe").exists(), "BenchLab must not be staged in the end-user portable bundle.", errors)

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
        pathlib.Path("DOCS") / "PORTABLE-BUNDLE.md",
        pathlib.Path("DOCS") / "PORTABLE-LAYOUT.md",
        pathlib.Path("DOCS") / "PORTABLE-RELEASE-NOTES.md",
        pathlib.Path("LICENSES") / "ASSETS-LICENSES.md",
        pathlib.Path("LICENSES") / "THIRD-PARTY-LICENSES.md",
        pathlib.Path("PRESETS") / "README.txt",
        pathlib.Path("THEMES") / "README.txt",
        pathlib.Path("PACKS") / "README.txt",
        pathlib.Path("OPTIONAL") / "README.txt",
    ):
        require((staging_root / relative_path).is_file(), f"Missing staged bundle file: {(staging_root / relative_path).relative_to(ROOT)}", errors)


def check_status_docs(errors: list[str]) -> None:
    readme = read_text(ROOT / "README.md")
    require("Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, and `C06`." in readme, "README.md must record C06 as complete.", errors)
    require("The next continuation phase is `C07` for installer, registration, and uninstall flow." in readme, "README.md must point to C07.", errors)

    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    require("Post-`S15` work now follows committed `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, and `C06` work plus later continuation prompts." in prompt_program, "prompt-program.md must record C06 as complete.", errors)
    require("C07 is the next planned implementation prompt after the portable distribution bundle." in prompt_program, "prompt-program.md must point to C07.", errors)
    require("- `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, and `C06` are complete." in prompt_program, "prompt-program.md must mark C06 complete.", errors)

    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    require("`C00`, `C01`, `C02`, `C03`, `C04`, `C05`, and `C06` are complete." in series_map, "series-map.md must mark C06 complete.", errors)
    require("The next planned implementation prompt is `C07` installer, registration, and uninstall flow." in series_map, "series-map.md must point to C07.", errors)

    post_s15 = read_text(ROOT / "docs" / "roadmap" / "post-s15-plan.md")
    require("7. `C06` completed portable distribution before `C07` installer, registration, and uninstall work." in post_s15, "post-s15-plan.md must record C06 as complete.", errors)
    require("The next continuation prompt should be `C07` for installer, registration, and uninstall flow." in post_s15, "post-s15-plan.md must point to C07.", errors)


def main() -> int:
    errors: list[str] = []
    manifest = read_ini(MANIFEST_PATH)

    check_source_controlled_defs(manifest, errors)
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
