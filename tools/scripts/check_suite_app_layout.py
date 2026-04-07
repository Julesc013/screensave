"""Validate the C09 suite app product tree, build integration, and role boundaries."""

from __future__ import annotations

import configparser
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SUITE_ROOT = ROOT / "products" / "apps" / "suite"

CANONICAL_SAVERS = (
    "nocturne",
    "ricochet",
    "deepfield",
    "plasma",
    "phosphor",
    "pipeworks",
    "lifeforms",
    "signals",
    "mechanize",
    "ecosystems",
    "stormglass",
    "transit",
    "observatory",
    "vector",
    "explorer",
    "city",
    "atlas",
    "gallery",
    "anthology",
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


def main() -> int:
    errors: list[str] = []

    required_paths = (
        SUITE_ROOT / "README.md",
        SUITE_ROOT / "manifest.ini",
        SUITE_ROOT / "notes" / "README.md",
        SUITE_ROOT / "tests" / "README.md",
        SUITE_ROOT / "tests" / "smoke.c",
        SUITE_ROOT / "src" / "suite_app.c",
        SUITE_ROOT / "src" / "suite_browser.c",
        SUITE_ROOT / "src" / "suite_config.c",
        SUITE_ROOT / "src" / "suite_internal.h",
        SUITE_ROOT / "src" / "suite_launch.c",
        SUITE_ROOT / "src" / "suite_main.c",
        SUITE_ROOT / "src" / "suite_manifest.c",
        SUITE_ROOT / "src" / "suite_state.c",
        ROOT / "build" / "msvc" / "vs2022" / "suite.vcxproj",
        ROOT / "build" / "msvc" / "vs2022" / "suite_target_sources.props",
        ROOT / "validation" / "notes" / "c09-suite-app-matrix.md",
    )
    for path in required_paths:
        require(path.exists(), f"Missing required C09 path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    manifest = read_ini(SUITE_ROOT / "manifest.ini")
    available_savers = tuple(
        saver.strip()
        for saver in manifest.get("identity", "available_savers", fallback="").split(",")
        if saver.strip()
    )
    require(manifest.get("product", "key", fallback="") == "suite", "Suite manifest must use product key suite.", errors)
    require(manifest.get("product", "kind", fallback="") == "app", "Suite manifest must declare kind=app.", errors)
    require(manifest.get("identity", "default_saver", fallback="") == "nocturne", "Suite manifest must default to nocturne.", errors)
    require(available_savers == CANONICAL_SAVERS, "Suite manifest must list the full canonical saver line plus anthology.", errors)
    require(manifest.get("capabilities", "manifest_browser", fallback="") == "1", "Suite manifest must declare manifest_browser=1.", errors)
    require(manifest.get("capabilities", "embedded_preview", fallback="") == "1", "Suite manifest must declare embedded_preview=1.", errors)
    require(manifest.get("capabilities", "windowed_launch", fallback="") == "1", "Suite manifest must declare windowed_launch=1.", errors)
    require(manifest.get("capabilities", "saver_config_handoff", fallback="") == "1", "Suite manifest must declare saver_config_handoff=1.", errors)
    require(manifest.get("capabilities", "pack_visibility", fallback="") == "1", "Suite manifest must declare pack_visibility=1.", errors)

    readme = read_text(SUITE_ROOT / "README.md")
    for snippet in (
        "browser, launcher, preview, and configuration surface",
        "embedded live preview",
        "detached windowed run",
        "Settings...",
        "not BenchLab",
        "not the `anthology` meta-saver",
    ):
        require(snippet in readme, f"Suite README is missing {snippet!r}.", errors)

    notes = read_text(SUITE_ROOT / "notes" / "README.md")
    for snippet in (
        "`suite` is the end-user browser, launcher, preview, and configuration surface.",
        "`anthology` remains the suite meta-saver",
        "BenchLab remains the diagnostics harness.",
    ):
        require(snippet in notes, f"Suite notes are missing {snippet!r}.", errors)

    smoke = read_text(SUITE_ROOT / "tests" / "smoke.c")
    for snippet in (
        'suite_find_target_module("nocturne")',
        'suite_find_target_module("anthology")',
        'suite_find_target_module("suite") != NULL',
    ):
        require(snippet in smoke, f"Suite smoke is missing {snippet!r}.", errors)

    manifest_source = read_text(SUITE_ROOT / "src" / "suite_manifest.c")
    for saver in CANONICAL_SAVERS:
        require(f"{saver}_get_module" in manifest_source, f"suite_manifest.c must reference {saver}_get_module.", errors)
    for snippet in (
        "suite_get_available_module_count",
        "suite_find_target_module",
        "screensave_pack_manifest_discover",
        "module only",
        "suite_catalog_build",
    ):
        require(snippet in manifest_source, f"suite_manifest.c is missing {snippet!r}.", errors)

    app_source = read_text(SUITE_ROOT / "src" / "suite_app.c")
    internal_header = read_text(SUITE_ROOT / "src" / "suite_internal.h")
    for snippet in (
        "Run Windowed",
        "Settings...",
        "Restart Preview",
    ):
        require(snippet in app_source, f"suite_app.c is missing {snippet!r}.", errors)
    for snippet in (
        "IDC_SUITE_BROWSER",
        "IDC_SUITE_PRESET",
        "IDC_SUITE_THEME",
        "IDC_SUITE_RENDERER",
    ):
        require(snippet in internal_header, f"suite_internal.h is missing {snippet!r}.", errors)

    launch_source = read_text(SUITE_ROOT / "src" / "suite_launch.c")
    for snippet in (
        "SCREENSAVE_SESSION_MODE_PREVIEW",
        "SCREENSAVE_SESSION_MODE_WINDOWED",
        "suite_preview_start",
        "suite_open_windowed_run",
    ):
        require(snippet in launch_source, f"suite_launch.c is missing {snippet!r}.", errors)

    config_source = read_text(SUITE_ROOT / "src" / "suite_config.c")
    for snippet in (
        "suite_save_working_config",
        "suite_handle_preset_combo_change",
        "suite_handle_theme_combo_change",
        "suite_handle_renderer_combo_change",
        "suite_show_selected_settings_dialog",
    ):
        require(snippet in config_source, f"suite_config.c is missing {snippet!r}.", errors)

    solution_text = read_text(ROOT / "build" / "msvc" / "vs2022" / "ScreenSave.sln")
    makefile_text = read_text(ROOT / "build" / "mingw" / "i686" / "Makefile")
    require("suite.vcxproj" in solution_text, "ScreenSave.sln must include suite.vcxproj.", errors)
    require("SUITE_TARGET :=" in makefile_text, "MinGW Makefile must declare the suite target.", errors)
    require("suite.exe" in makefile_text, "MinGW Makefile must emit suite.exe.", errors)
    require("SUITE_SAVER_template" in makefile_text, "MinGW Makefile must define suite saver object rules.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c09-suite-app-matrix.md")
    require("# C09 Suite App Matrix" in note, "C09 suite note is missing its title.", errors)
    require("- `C10` SDK / contributor surface" in note, "C09 suite note must name the next continuation step.", errors)
    require("static only" in note.lower(), "C09 suite note must record static-only evidence honestly.", errors)

    root_readme = read_text(ROOT / "README.md")
    require("Post-`C16` work now proceeds as a short `SS` release-closure series first:" in root_readme, "README.md must record the post-C16 handoff.", errors)
    require("`benchlab` and `suite` are now the real non-saver apps in the tree." in root_readme, "README.md must describe the real app pair.", errors)

    prompt_program = read_text(ROOT / "docs" / "roadmap" / "prompt-program.md")
    require("`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in prompt_program, "prompt-program.md must record the completed C16 freeze.", errors)

    series_map = read_text(ROOT / "docs" / "roadmap" / "series-map.md")
    require("`SS` runs first as the short release-closure bridge." in series_map, "series-map.md must point to the post-C16 handoff.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Suite app layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

