"""Validate the C08 anthology meta-saver product, hooks, and status docs."""

from __future__ import annotations

import configparser
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
ANTHOLOGY_ROOT = ROOT / "products" / "savers" / "anthology"


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
        ANTHOLOGY_ROOT / "README.md",
        ANTHOLOGY_ROOT / "manifest.ini",
        ANTHOLOGY_ROOT / "presets" / "defaults.ini",
        ANTHOLOGY_ROOT / "presets" / "weighted.ini",
        ANTHOLOGY_ROOT / "presets" / "families.ini",
        ANTHOLOGY_ROOT / "src" / "anthology_config.c",
        ANTHOLOGY_ROOT / "src" / "anthology_config.rc",
        ANTHOLOGY_ROOT / "src" / "anthology_entry.c",
        ANTHOLOGY_ROOT / "src" / "anthology_internal.h",
        ANTHOLOGY_ROOT / "src" / "anthology_module.c",
        ANTHOLOGY_ROOT / "src" / "anthology_presets.c",
        ANTHOLOGY_ROOT / "src" / "anthology_render.c",
        ANTHOLOGY_ROOT / "src" / "anthology_resource.h",
        ANTHOLOGY_ROOT / "src" / "anthology_sim.c",
        ANTHOLOGY_ROOT / "src" / "anthology_themes.c",
        ANTHOLOGY_ROOT / "src" / "anthology_version.rc",
        ANTHOLOGY_ROOT / "tests" / "README.md",
        ANTHOLOGY_ROOT / "tests" / "smoke.c",
        ROOT / "validation" / "notes" / "c08-meta-saver-matrix.md",
    )
    for path in required_paths:
        require(path.exists(), f"Missing required C08 path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    manifest = read_ini(ANTHOLOGY_ROOT / "manifest.ini")
    require(manifest.get("product", "key", fallback="") == "anthology", "Anthology manifest must use product key anthology.", errors)
    require(manifest.get("product", "kind", fallback="") == "saver", "Anthology manifest must declare kind=saver.", errors)
    require(manifest.get("identity", "default_preset", fallback="") == "balanced_mix", "Anthology manifest must point to balanced_mix.", errors)
    require(manifest.get("identity", "default_theme", fallback="") == "anthology_neutral", "Anthology manifest must point to anthology_neutral.", errors)
    require(manifest.get("capabilities", "gdi", fallback="") == "1", "Anthology manifest must declare gdi support.", errors)
    require(manifest.get("capabilities", "gl11", fallback="") == "1", "Anthology manifest must declare gl11 support.", errors)
    require(manifest.get("capabilities", "gl21", fallback="") == "1", "Anthology manifest must declare gl21 support.", errors)

    readme = read_text(ANTHOLOGY_ROOT / "README.md")
    require("cross-saver randomization" in readme, "Anthology README must describe cross-saver randomization.", errors)
    require("real `suite` app" in readme, "Anthology README must keep the real suite app out of scope.", errors)
    require("in-process" in readme, "Anthology README must describe the in-process orchestration model.", errors)

    module_text = read_text(ANTHOLOGY_ROOT / "src" / "anthology_module.c")
    require('"anthology"' in module_text, "anthology_module.c must declare the anthology product identity.", errors)
    require("SCREENSAVE_SAVER_CAP_GDI" in module_text, "anthology_module.c must keep GDI support.", errors)
    require("SCREENSAVE_SAVER_CAP_GL11" in module_text, "anthology_module.c must keep GL11 support.", errors)
    require("SCREENSAVE_SAVER_CAP_GL21" in module_text, "anthology_module.c must keep GL21 support.", errors)

    sim_text = read_text(ANTHOLOGY_ROOT / "src" / "anthology_sim.c")
    for snippet in (
        'lstrcmpiA(canonical_key, "anthology") == 0',
        "ANTHOLOGY_SELECTION_WEIGHTED_RANDOM",
        "ANTHOLOGY_SELECTION_FAVORITES_FIRST",
        "anthology_module_supports_renderer",
        "anthology_switch_inner_session",
        "black_fallback_no_candidate",
        "black_fallback_start_failure",
    ):
        require(snippet in sim_text, f"anthology_sim.c is missing {snippet!r}.", errors)

    config_text = read_text(ANTHOLOGY_ROOT / "src" / "anthology_config.c")
    for snippet in (
        '"selection_mode"',
        '"family_filter"',
        '"include_keys"',
        '"favorite_keys"',
        '"renderer_policy"',
        '"randomize_inner_presets"',
        '"randomize_inner_themes"',
        '"respect_inner_randomization"',
    ):
        require(snippet in config_text, f"anthology_config.c is missing {snippet!r}.", errors)

    benchlab_session = read_text(ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_session.c")
    benchlab_overlay = read_text(ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_overlay.c")
    require("anthology_get_module" in benchlab_session, "BenchLab session list must include anthology_get_module.", errors)
    require("anthology_session_build_summary" in benchlab_overlay, "BenchLab overlay must include anthology summary support.", errors)

    solution_text = read_text(ROOT / "build" / "msvc" / "vs2022" / "ScreenSave.sln")
    makefile_text = read_text(ROOT / "build" / "mingw" / "i686" / "Makefile")
    require("anthology.vcxproj" in solution_text, "ScreenSave.sln must include anthology.vcxproj.", errors)
    require("ANTHOLOGY_TARGET :=" in makefile_text, "MinGW Makefile must declare the anthology target.", errors)

    note = read_text(ROOT / "validation" / "notes" / "c08-meta-saver-matrix.md")
    require("# C08 Meta-Saver Matrix" in note, "C08 meta-saver note is missing its title.", errors)
    require("- `C09` `suite` app" in note, "C08 meta-saver note must name the next continuation step.", errors)
    require("predate `anthology`" in note, "C08 meta-saver note must record the packaging limitation honestly.", errors)

    root_readme = read_text(ROOT / "README.md")
    require("The current continuation line is complete through `C14` final rerelease hardening." in root_readme, "README.md must point to C14.", errors)
    require("`anthology` is the real suite meta-saver `.scr`" in root_readme, "README.md must describe anthology explicitly.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Meta-saver layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
