"""Validate the checked-in build scaffold and its key target relationships."""

from __future__ import annotations

import pathlib
import sys
import xml.etree.ElementTree as ET


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOLUTION = ROOT / "build" / "msvc" / "vs2022" / "ScreenSave.sln"
PLATFORM_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "screensave_platform.vcxproj"
BENCHLAB_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "benchlab.vcxproj"
SAVER_COMMON_PROPS = ROOT / "build" / "msvc" / "vs2022" / "saver_target_common.props"
MINGW_MAKEFILE = ROOT / "build" / "mingw" / "i686" / "Makefile"

NS = {"msb": "http://schemas.microsoft.com/developer/msbuild/2003"}
SAVER_UNITS = ("config", "module", "presets", "render", "sim", "themes")
BENCHLAB_APP_UNITS = ("app", "config", "diag", "main", "overlay", "session")

SAVERS = (
    ("nocturne", "Nocturne"),
    ("ricochet", "Ricochet"),
    ("deepfield", "Deepfield"),
    ("plasma", "Plasma"),
    ("phosphor", "Phosphor"),
    ("pipeworks", "Pipeworks"),
    ("lifeforms", "Lifeforms"),
    ("signals", "Signals"),
    ("mechanize", "Mechanize"),
    ("ecosystems", "Ecosystems"),
    ("stormglass", "Stormglass"),
    ("transit", "Transit"),
    ("observatory", "Observatory"),
    ("vector", "Vector"),
    ("explorer", "Explorer"),
    ("city", "City"),
    ("atlas", "Atlas"),
    ("gallery", "Gallery"),
)


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def parse_project(path: pathlib.Path) -> ET.Element:
    return ET.parse(path).getroot()


def collect_items(project_root: ET.Element, item_name: str) -> list[str]:
    return [node.attrib.get("Include", "") for node in project_root.findall(f".//msb:{item_name}", NS)]


def saver_project_path(saver_key: str) -> pathlib.Path:
    return ROOT / "build" / "msvc" / "vs2022" / f"{saver_key}.vcxproj"


def saver_source_paths(saver_key: str) -> list[str]:
    base = f"..\\..\\..\\products\\savers\\{saver_key}\\src"
    return [f"{base}\\{saver_key}_{unit}.c" for unit in SAVER_UNITS] + [f"{base}\\{saver_key}_entry.c"]


def saver_resource_paths(saver_key: str) -> list[str]:
    base = f"..\\..\\..\\products\\savers\\{saver_key}\\src"
    return [
        f"{base}\\{saver_key}_config.rc",
        f"{base}\\{saver_key}_version.rc",
        "..\\..\\..\\platform\\src\\host\\win32_scr\\screensave_host.rc",
    ]


def saver_required_paths(saver_key: str) -> list[pathlib.Path]:
    base = ROOT / "products" / "savers" / saver_key
    preset_names = {
        "nocturne": ("defaults.ini", "museum.ini", "night_modes.ini"),
        "ricochet": ("defaults.ini", "themed.ini"),
    }.get(saver_key, ("defaults.ini", "themed.ini", "performance.ini"))
    paths = [
        base / "manifest.ini",
        base / "README.md",
        base / "tests" / "README.md",
        base / "tests" / "smoke.c",
        base / "src" / f"{saver_key}_config.c",
        base / "src" / f"{saver_key}_config.rc",
        base / "src" / f"{saver_key}_entry.c",
        base / "src" / f"{saver_key}_internal.h",
        base / "src" / f"{saver_key}_module.c",
        base / "src" / f"{saver_key}_presets.c",
        base / "src" / f"{saver_key}_render.c",
        base / "src" / f"{saver_key}_resource.h",
        base / "src" / f"{saver_key}_sim.c",
        base / "src" / f"{saver_key}_themes.c",
        base / "src" / f"{saver_key}_version.rc",
    ]
    for preset_name in preset_names:
        paths.append(base / "presets" / preset_name)
    return paths


def required_paths() -> list[pathlib.Path]:
    paths = [
        ROOT / "build" / "README.md",
        ROOT / "build" / "msvc" / "README.md",
        ROOT / "build" / "msvc" / "vs6" / "README.md",
        ROOT / "build" / "msvc" / "vs2008" / "README.md",
        ROOT / "build" / "msvc" / "vs2022" / "README.md",
        ROOT / "build" / "mingw" / "README.md",
        ROOT / "build" / "mingw" / "i686" / "README.md",
        ROOT / "build" / "ci" / "README.md",
        SOLUTION,
        PLATFORM_PROJECT,
        BENCHLAB_PROJECT,
        SAVER_COMMON_PROPS,
        MINGW_MAKEFILE,
        ROOT / "platform" / "include" / "screensave" / "config_api.h",
        ROOT / "platform" / "include" / "screensave" / "diagnostics_api.h",
        ROOT / "platform" / "include" / "screensave" / "grid_buffer_api.h",
        ROOT / "platform" / "include" / "screensave" / "renderer_api.h",
        ROOT / "platform" / "include" / "screensave" / "saver_api.h",
        ROOT / "platform" / "include" / "screensave" / "types.h",
        ROOT / "platform" / "include" / "screensave" / "version.h",
        ROOT / "platform" / "include" / "screensave" / "visual_buffer_api.h",
        ROOT / "platform" / "src" / "core" / "grid" / "grid_buffer.c",
        ROOT / "platform" / "src" / "core" / "visual" / "visual_buffer.c",
        ROOT / "platform" / "src" / "host" / "win32_scr" / "screensave_host.rc",
        ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_product_version.rc",
        ROOT / "platform" / "src" / "render" / "gl21" / "README.md",
        ROOT / "platform" / "src" / "render" / "gl21" / "gl21_backend.c",
        ROOT / "platform" / "src" / "render" / "gl21" / "gl21_caps.c",
        ROOT / "platform" / "src" / "render" / "gl21" / "gl21_context.c",
        ROOT / "platform" / "src" / "render" / "gl21" / "gl21_present.c",
        ROOT / "platform" / "src" / "render" / "gl21" / "gl21_primitives.c",
        ROOT / "platform" / "src" / "render" / "gl21" / "gl21_state.c",
        ROOT / "platform" / "src" / "render" / "gl33" / "README.md",
        ROOT / "platform" / "src" / "render" / "gl33" / "gl33_backend.c",
        ROOT / "platform" / "src" / "render" / "gl46" / "README.md",
        ROOT / "platform" / "src" / "render" / "gl46" / "gl46_backend.c",
        ROOT / "platform" / "src" / "render" / "null" / "README.md",
        ROOT / "platform" / "src" / "render" / "null" / "null_backend.c",
        ROOT / "products" / "apps" / "benchlab" / "README.md",
        ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_session.c",
        ROOT / "products" / "apps" / "benchlab" / "tests" / "smoke.c",
    ]
    for saver_key, _ in SAVERS:
        paths.append(saver_project_path(saver_key))
        paths.extend(saver_required_paths(saver_key))
    return paths


def require_all(items: list[str], expected_items: list[str], label: str, errors: list[str]) -> None:
    for expected in expected_items:
        require(expected in items, f"{label} is missing {expected!r}.", errors)


def require_exact(items: list[str], expected_items: list[str], label: str, errors: list[str]) -> None:
    require(set(items) == set(expected_items), f"{label} has unexpected items.", errors)


def require_saver_project(project_path: pathlib.Path, saver_key: str, errors: list[str]) -> None:
    project_root = parse_project(project_path)
    sources = collect_items(project_root, "ClCompile")
    resources = collect_items(project_root, "ResourceCompile")
    project_refs = collect_items(project_root, "ProjectReference")
    project_text = project_path.read_text(encoding="utf-8")

    require_exact(sources, saver_source_paths(saver_key), project_path.name, errors)
    require_exact(resources, saver_resource_paths(saver_key), project_path.name, errors)
    require("screensave_platform.vcxproj" in project_refs, f"{project_path.name} must reference the platform project.", errors)
    require("<TargetExt>.scr</TargetExt>" in project_text, f"{project_path.name} must emit a .scr target.", errors)
    require(f"<TargetName>{saver_key}</TargetName>" in project_text, f"{project_path.name} must set the expected target name.", errors)
    require("saver_target_common.props" in project_text, f"{project_path.name} must import saver_target_common.props.", errors)


def main() -> int:
    errors: list[str] = []

    for path in required_paths():
        require(path.exists(), f"Missing required build path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    solution_text = SOLUTION.read_text(encoding="utf-8")
    for expected in (
        "screensave_platform.vcxproj",
        "benchlab.vcxproj",
        "Debug|Win32",
        "Release|Win32",
    ):
        require(expected in solution_text, f"ScreenSave.sln is missing {expected!r}.", errors)
    for saver_key, _ in SAVERS:
        require(f"{saver_key}.vcxproj" in solution_text, f"ScreenSave.sln is missing {saver_key}.vcxproj.", errors)

    platform_project = parse_project(PLATFORM_PROJECT)
    platform_sources = collect_items(platform_project, "ClCompile")
    require_all(
        platform_sources,
        [
            "..\\..\\..\\platform\\src\\core\\base\\renderer_dispatch.c",
            "..\\..\\..\\platform\\src\\core\\base\\saver_contract.c",
            "..\\..\\..\\platform\\src\\core\\base\\saver_registry.c",
            "..\\..\\..\\platform\\src\\core\\config\\config.c",
            "..\\..\\..\\platform\\src\\core\\diagnostics\\diagnostics.c",
            "..\\..\\..\\platform\\src\\core\\grid\\grid_buffer.c",
            "..\\..\\..\\platform\\src\\core\\rng\\rng.c",
            "..\\..\\..\\platform\\src\\core\\timing\\timing.c",
            "..\\..\\..\\platform\\src\\core\\version\\version.c",
            "..\\..\\..\\platform\\src\\core\\visual\\visual_buffer.c",
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_backend.c",
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_primitives.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_backend.c",
            "..\\..\\..\\platform\\src\\render\\gl21\\gl21_backend.c",
            "..\\..\\..\\platform\\src\\render\\gl21\\gl21_caps.c",
            "..\\..\\..\\platform\\src\\render\\gl21\\gl21_context.c",
            "..\\..\\..\\platform\\src\\render\\gl21\\gl21_present.c",
            "..\\..\\..\\platform\\src\\render\\gl21\\gl21_primitives.c",
            "..\\..\\..\\platform\\src\\render\\gl21\\gl21_state.c",
            "..\\..\\..\\platform\\src\\render\\gl33\\gl33_backend.c",
            "..\\..\\..\\platform\\src\\render\\gl46\\gl46_backend.c",
            "..\\..\\..\\platform\\src\\render\\null\\null_backend.c",
            "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_entry.c",
            "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_window.c",
        ],
        "screensave_platform.vcxproj",
        errors,
    )

    for saver_key, _ in SAVERS:
        require_saver_project(saver_project_path(saver_key), saver_key, errors)

    benchlab_project = parse_project(BENCHLAB_PROJECT)
    benchlab_sources = collect_items(benchlab_project, "ClCompile")
    benchlab_resources = collect_items(benchlab_project, "ResourceCompile")
    benchlab_project_refs = collect_items(benchlab_project, "ProjectReference")
    benchlab_project_text = BENCHLAB_PROJECT.read_text(encoding="utf-8")

    require_all(
        benchlab_sources,
        [f"..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_{unit}.c" for unit in BENCHLAB_APP_UNITS]
        + [source for saver_key, _ in SAVERS for source in saver_source_paths(saver_key)[:-1]],
        "benchlab.vcxproj",
        errors,
    )
    require_all(
        benchlab_resources,
        [f"..\\..\\..\\products\\savers\\{saver_key}\\src\\{saver_key}_config.rc" for saver_key, _ in SAVERS],
        "benchlab.vcxproj",
        errors,
    )
    require("screensave_platform.vcxproj" in benchlab_project_refs, "benchlab.vcxproj must reference the platform project.", errors)
    require("<TargetExt>.exe</TargetExt>" in benchlab_project_text, "benchlab.vcxproj must emit an .exe target.", errors)

    makefile_text = MINGW_MAKEFILE.read_text(encoding="utf-8")
    for expected in (
        "SAVERS :=",
        "SAVER_template",
        "_version_res.o",
        "screensave_host_res.o",
        "benchlab.exe",
        "windres",
        "gl21_backend",
        "gl33_backend",
        "gl46_backend",
        "null_backend",
    ):
        require(expected in makefile_text, f"Makefile is missing {expected!r}.", errors)
    require("$(1)_version_res.o" in makefile_text, "Makefile must compile per-saver version resources.", errors)
    require("$(1)_version.rc" in makefile_text, "Makefile must reference the per-saver version resource template.", errors)

    host_entry_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_entry.c").read_text(encoding="utf-8")
    require("screensave_scr_main" in host_entry_text, "scr_entry.c must define the shared saver host entry.", errors)
    require("screensave_scr_main_with_registry" in host_entry_text, "scr_entry.c must preserve the registry-aware host entry.", errors)

    benchlab_session_text = (ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_session.c").read_text(encoding="utf-8")
    for saver_key, _ in SAVERS:
        require(f"{saver_key}_get_module" in benchlab_session_text, f"benchlab_session.c must reference {saver_key}_get_module.", errors)

    benchlab_smoke_text = (ROOT / "products" / "apps" / "benchlab" / "tests" / "smoke.c").read_text(encoding="utf-8")
    for saver_key, _ in SAVERS:
        require(saver_key in benchlab_smoke_text, f"benchlab smoke must mention {saver_key}.", errors)

    for saver_key, _ in SAVERS:
        entry_text = (ROOT / "products" / "savers" / saver_key / "src" / f"{saver_key}_entry.c").read_text(encoding="utf-8")
        require("screensave_scr_main(" in entry_text, f"{saver_key}_entry.c must use the single-saver host entry.", errors)
        require("screensave_scr_main_with_registry" not in entry_text, f"{saver_key}_entry.c must not use the registry host entry.", errors)
        require(f"{saver_key}_get_module" in entry_text, f"{saver_key}_entry.c must bind {saver_key}_get_module.", errors)
        for other_saver, _ in SAVERS:
            if other_saver != saver_key:
                require(f"{other_saver}_get_module" not in entry_text, f"{saver_key}_entry.c must not reference {other_saver}_get_module.", errors)

    build_readme = (ROOT / "build" / "README.md").read_text(encoding="utf-8").lower()
    for phrase in (
        "one true `.scr` output per saver",
        "benchlab",
        "gl21",
        "gl33",
        "gl46",
        "null",
    ):
        require(phrase in build_readme, f"build/README.md is missing expected phrase: {phrase!r}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Build layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
