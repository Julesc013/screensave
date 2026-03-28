"""Validate the checked-in build scaffold and its key target relationships."""

from __future__ import annotations

import pathlib
import sys
import xml.etree.ElementTree as ET


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOLUTION = ROOT / "build" / "msvc" / "vs2022" / "ScreenSave.sln"
PLATFORM_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "screensave_platform.vcxproj"
NOCTURNE_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "nocturne.vcxproj"
RICOCHET_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "ricochet.vcxproj"
DEEPFIELD_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "deepfield.vcxproj"
EMBER_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "ember.vcxproj"
OSCILLOSCOPE_DREAMS_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "oscilloscope_dreams.vcxproj"
PIPEWORKS_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "pipeworks.vcxproj"
LIFEFORMS_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "lifeforms.vcxproj"
BENCHLAB_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "benchlab.vcxproj"
MINGW_MAKEFILE = ROOT / "build" / "mingw" / "i686" / "Makefile"

NS = {"msb": "http://schemas.microsoft.com/developer/msbuild/2003"}

COMMON_SAVER_SOURCES = [
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_config.c",
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_module.c",
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_presets.c",
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_render.c",
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_sim.c",
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_themes.c",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_config.c",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_module.c",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_presets.c",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_render.c",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_sim.c",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_themes.c",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_config.c",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_module.c",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_presets.c",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_render.c",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_sim.c",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_themes.c",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_config.c",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_module.c",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_presets.c",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_render.c",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_sim.c",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_themes.c",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_config.c",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_module.c",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_presets.c",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_render.c",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_sim.c",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_themes.c",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_config.c",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_module.c",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_presets.c",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_render.c",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_sim.c",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_themes.c",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_config.c",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_module.c",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_presets.c",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_render.c",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_sim.c",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_themes.c",
]

COMMON_SAVER_RESOURCES = [
    "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_config.rc",
    "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_config.rc",
    "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_config.rc",
    "..\\..\\..\\products\\savers\\ember\\src\\ember_config.rc",
    "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_config.rc",
    "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_config.rc",
    "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_config.rc",
]

REQUIRED_PATHS = [
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
    NOCTURNE_PROJECT,
    RICOCHET_PROJECT,
    DEEPFIELD_PROJECT,
    EMBER_PROJECT,
    OSCILLOSCOPE_DREAMS_PROJECT,
    PIPEWORKS_PROJECT,
    LIFEFORMS_PROJECT,
    BENCHLAB_PROJECT,
    MINGW_MAKEFILE,
    ROOT / "platform" / "include" / "screensave" / "types.h",
    ROOT / "platform" / "include" / "screensave" / "version.h",
    ROOT / "platform" / "include" / "screensave" / "diagnostics_api.h",
    ROOT / "platform" / "include" / "screensave" / "config_api.h",
    ROOT / "platform" / "include" / "screensave" / "renderer_api.h",
    ROOT / "platform" / "include" / "screensave" / "saver_api.h",
    ROOT / "platform" / "include" / "screensave" / "grid_buffer_api.h",
    ROOT / "platform" / "include" / "screensave" / "visual_buffer_api.h",
    ROOT / "platform" / "src" / "core" / "base" / "renderer_dispatch.c",
    ROOT / "platform" / "src" / "core" / "base" / "renderer_private.h",
    ROOT / "platform" / "src" / "core" / "base" / "saver_contract.c",
    ROOT / "platform" / "src" / "core" / "base" / "saver_registry.c",
    ROOT / "platform" / "src" / "core" / "base" / "saver_registry.h",
    ROOT / "platform" / "src" / "core" / "config" / "config.c",
    ROOT / "platform" / "src" / "core" / "diagnostics" / "diagnostics.c",
    ROOT / "platform" / "src" / "core" / "grid" / "grid_buffer.c",
    ROOT / "platform" / "src" / "core" / "rng" / "rng.c",
    ROOT / "platform" / "src" / "core" / "rng" / "rng_internal.h",
    ROOT / "platform" / "src" / "core" / "timing" / "timing.c",
    ROOT / "platform" / "src" / "core" / "timing" / "timing_internal.h",
    ROOT / "platform" / "src" / "core" / "version" / "version.c",
    ROOT / "platform" / "src" / "core" / "visual" / "visual_buffer.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "README.md",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_internal.h",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_backend.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_surface.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_present.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_primitives.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_bitmap.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_state.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "README.md",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_internal.h",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_backend.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_bitmap.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_caps.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_context.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_present.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_primitives.c",
    ROOT / "platform" / "src" / "render" / "gl11" / "gl11_state.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_internal.h",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_host_entry.h",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_args.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_config_dialog.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_diagnostics.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_entry.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_settings.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_validation_scene.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_window.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "resource.h",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "screensave_host.rc",
    ROOT / "products" / "savers" / "nocturne" / "manifest.ini",
    ROOT / "products" / "savers" / "nocturne" / "README.md",
    ROOT / "products" / "savers" / "nocturne" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "nocturne" / "presets" / "museum.ini",
    ROOT / "products" / "savers" / "nocturne" / "presets" / "night_modes.ini",
    ROOT / "products" / "savers" / "nocturne" / "src" / "README.md",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_config.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_config.rc",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_entry.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_internal.h",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_module.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_presets.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_render.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_resource.h",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_sim.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_themes.c",
    ROOT / "products" / "savers" / "nocturne" / "tests" / "README.md",
    ROOT / "products" / "savers" / "nocturne" / "tests" / "smoke.c",
    ROOT / "products" / "savers" / "ricochet" / "manifest.ini",
    ROOT / "products" / "savers" / "ricochet" / "README.md",
    ROOT / "products" / "savers" / "ricochet" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "ricochet" / "presets" / "themed.ini",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_config.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_config.rc",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_entry.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_internal.h",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_module.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_presets.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_render.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_resource.h",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_sim.c",
    ROOT / "products" / "savers" / "ricochet" / "src" / "ricochet_themes.c",
    ROOT / "products" / "savers" / "ricochet" / "tests" / "README.md",
    ROOT / "products" / "savers" / "ricochet" / "tests" / "smoke.c",
    ROOT / "products" / "savers" / "deepfield" / "manifest.ini",
    ROOT / "products" / "savers" / "deepfield" / "README.md",
    ROOT / "products" / "savers" / "deepfield" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "deepfield" / "presets" / "themed.ini",
    ROOT / "products" / "savers" / "deepfield" / "presets" / "performance.ini",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_config.c",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_config.rc",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_entry.c",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_internal.h",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_module.c",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_presets.c",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_render.c",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_resource.h",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_sim.c",
    ROOT / "products" / "savers" / "deepfield" / "src" / "deepfield_themes.c",
    ROOT / "products" / "savers" / "deepfield" / "tests" / "README.md",
    ROOT / "products" / "savers" / "deepfield" / "tests" / "smoke.c",
    ROOT / "products" / "savers" / "ember" / "manifest.ini",
    ROOT / "products" / "savers" / "ember" / "README.md",
    ROOT / "products" / "savers" / "ember" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "ember" / "presets" / "themed.ini",
    ROOT / "products" / "savers" / "ember" / "presets" / "performance.ini",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_config.c",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_config.rc",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_entry.c",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_internal.h",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_module.c",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_presets.c",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_render.c",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_resource.h",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_sim.c",
    ROOT / "products" / "savers" / "ember" / "src" / "ember_themes.c",
    ROOT / "products" / "savers" / "ember" / "tests" / "README.md",
    ROOT / "products" / "savers" / "ember" / "tests" / "smoke.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "manifest.ini",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "README.md",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "presets" / "themed.ini",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "presets" / "performance.ini",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_config.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_config.rc",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_entry.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_internal.h",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_module.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_presets.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_render.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_resource.h",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_sim.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "src" / "oscilloscope_dreams_themes.c",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "tests" / "README.md",
    ROOT / "products" / "savers" / "oscilloscope_dreams" / "tests" / "smoke.c",
    ROOT / "products" / "savers" / "pipeworks" / "manifest.ini",
    ROOT / "products" / "savers" / "pipeworks" / "README.md",
    ROOT / "products" / "savers" / "pipeworks" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "pipeworks" / "presets" / "themed.ini",
    ROOT / "products" / "savers" / "pipeworks" / "presets" / "performance.ini",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_config.c",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_config.rc",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_entry.c",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_internal.h",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_module.c",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_presets.c",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_render.c",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_resource.h",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_sim.c",
    ROOT / "products" / "savers" / "pipeworks" / "src" / "pipeworks_themes.c",
    ROOT / "products" / "savers" / "pipeworks" / "tests" / "README.md",
    ROOT / "products" / "savers" / "pipeworks" / "tests" / "smoke.c",
    ROOT / "products" / "savers" / "lifeforms" / "manifest.ini",
    ROOT / "products" / "savers" / "lifeforms" / "README.md",
    ROOT / "products" / "savers" / "lifeforms" / "presets" / "defaults.ini",
    ROOT / "products" / "savers" / "lifeforms" / "presets" / "themed.ini",
    ROOT / "products" / "savers" / "lifeforms" / "presets" / "performance.ini",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_config.c",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_config.rc",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_entry.c",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_internal.h",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_module.c",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_presets.c",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_render.c",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_resource.h",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_sim.c",
    ROOT / "products" / "savers" / "lifeforms" / "src" / "lifeforms_themes.c",
    ROOT / "products" / "savers" / "lifeforms" / "tests" / "README.md",
    ROOT / "products" / "savers" / "lifeforms" / "tests" / "smoke.c",
    ROOT / "products" / "apps" / "benchlab" / "manifest.ini",
    ROOT / "products" / "apps" / "benchlab" / "README.md",
    ROOT / "products" / "apps" / "benchlab" / "captures" / "README.md",
    ROOT / "products" / "apps" / "benchlab" / "notes" / "README.md",
    ROOT / "products" / "apps" / "benchlab" / "src" / "README.md",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_app.c",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_config.c",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_diag.c",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_internal.h",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_main.c",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_overlay.c",
    ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_session.c",
    ROOT / "products" / "apps" / "benchlab" / "tests" / "README.md",
    ROOT / "products" / "apps" / "benchlab" / "tests" / "smoke.c",
]


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def parse_project(path: pathlib.Path) -> ET.Element:
    return ET.parse(path).getroot()


def collect_items(project_root: ET.Element, item_name: str) -> list[str]:
    return [node.attrib.get("Include", "") for node in project_root.findall(f".//msb:{item_name}", NS)]


def require_all(items: list[str], expected_items: list[str], message_prefix: str, errors: list[str]) -> None:
    for expected in expected_items:
        require(expected in items, f"{message_prefix} is missing {expected!r}.", errors)


def require_saver_project(
    project_path: pathlib.Path,
    entry_source: str,
    project_name: str,
    errors: list[str],
) -> None:
    project_root = parse_project(project_path)
    sources = collect_items(project_root, "ClCompile")
    resources = collect_items(project_root, "ResourceCompile")
    project_refs = collect_items(project_root, "ProjectReference")
    project_text = project_path.read_text(encoding="utf-8")

    require_all(
        sources,
        COMMON_SAVER_SOURCES + [entry_source],
        f"{project_path.name}",
        errors,
    )
    require_all(
        resources,
        COMMON_SAVER_RESOURCES + ["..\\..\\..\\platform\\src\\host\\win32_scr\\screensave_host.rc"],
        f"{project_path.name}",
        errors,
    )
    require(
        "screensave_platform.vcxproj" in project_refs,
        f"{project_path.name} must reference the platform project.",
        errors,
    )
    require("<TargetExt>.scr</TargetExt>" in project_text, f"{project_path.name} must emit a .scr target.", errors)
    require(
        "user32.lib;gdi32.lib;advapi32.lib;opengl32.lib" in project_text,
        f"{project_path.name} must link the host-support Win32 and OpenGL libraries.",
        errors,
    )
    require(
        "out\\msvc\\vs2022" in project_text,
        f"{project_path.name} must use the documented MSVC output root.",
        errors,
    )
    require(
        f"<TargetName>{project_name}</TargetName>" in project_text,
        f"{project_path.name} must set the expected target name.",
        errors,
    )


def main() -> int:
    errors: list[str] = []

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing required build path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    build_readme = (ROOT / "build" / "README.md").read_text(encoding="utf-8")
    for phrase in (
        "checked-in per-toolchain lanes",
        "out/",
        "concrete MSVC VS2022 solution",
        "mandatory GDI backend",
        "BenchLab",
        "Ricochet",
        "Deepfield",
        "Ember",
        "Oscilloscope Dreams",
        "Pipeworks",
        "Lifeforms",
    ):
        require(phrase.lower() in build_readme.lower(), f"build/README.md is missing expected phrase: {phrase!r}", errors)

    for path in (ROOT / "build" / "msvc" / "vs6" / "README.md", ROOT / "build" / "msvc" / "vs2008" / "README.md"):
        content = path.read_text(encoding="utf-8").lower()
        require("deferred" in content, f"{path.relative_to(ROOT)} must explicitly describe deferred work.", errors)

    solution_text = SOLUTION.read_text(encoding="utf-8")
    for expected in (
        "screensave_platform.vcxproj",
        "nocturne.vcxproj",
        "ricochet.vcxproj",
        "deepfield.vcxproj",
        "ember.vcxproj",
        "oscilloscope_dreams.vcxproj",
        "pipeworks.vcxproj",
        "lifeforms.vcxproj",
        "benchlab.vcxproj",
        "Debug|Win32",
        "Release|Win32",
    ):
        require(expected in solution_text, f"ScreenSave.sln is missing {expected!r}.", errors)

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
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_bitmap.c",
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_present.c",
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_primitives.c",
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_state.c",
            "..\\..\\..\\platform\\src\\render\\gdi\\gdi_surface.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_backend.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_bitmap.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_caps.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_context.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_present.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_primitives.c",
            "..\\..\\..\\platform\\src\\render\\gl11\\gl11_state.c",
            "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_entry.c",
            "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_validation_scene.c",
            "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_window.c",
        ],
        "screensave_platform.vcxproj",
        errors,
    )

    require_saver_project(
        NOCTURNE_PROJECT,
        "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_entry.c",
        "nocturne",
        errors,
    )
    require_saver_project(
        RICOCHET_PROJECT,
        "..\\..\\..\\products\\savers\\ricochet\\src\\ricochet_entry.c",
        "ricochet",
        errors,
    )
    require_saver_project(
        DEEPFIELD_PROJECT,
        "..\\..\\..\\products\\savers\\deepfield\\src\\deepfield_entry.c",
        "deepfield",
        errors,
    )
    require_saver_project(
        EMBER_PROJECT,
        "..\\..\\..\\products\\savers\\ember\\src\\ember_entry.c",
        "ember",
        errors,
    )
    require_saver_project(
        OSCILLOSCOPE_DREAMS_PROJECT,
        "..\\..\\..\\products\\savers\\oscilloscope_dreams\\src\\oscilloscope_dreams_entry.c",
        "oscilloscope_dreams",
        errors,
    )
    require_saver_project(
        PIPEWORKS_PROJECT,
        "..\\..\\..\\products\\savers\\pipeworks\\src\\pipeworks_entry.c",
        "pipeworks",
        errors,
    )
    require_saver_project(
        LIFEFORMS_PROJECT,
        "..\\..\\..\\products\\savers\\lifeforms\\src\\lifeforms_entry.c",
        "lifeforms",
        errors,
    )

    benchlab_project = parse_project(BENCHLAB_PROJECT)
    benchlab_sources = collect_items(benchlab_project, "ClCompile")
    benchlab_resources = collect_items(benchlab_project, "ResourceCompile")
    benchlab_project_refs = collect_items(benchlab_project, "ProjectReference")
    benchlab_project_text = BENCHLAB_PROJECT.read_text(encoding="utf-8")

    require_all(
        benchlab_sources,
        [
            "..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_app.c",
            "..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_config.c",
            "..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_diag.c",
            "..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_main.c",
            "..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_overlay.c",
            "..\\..\\..\\products\\apps\\benchlab\\src\\benchlab_session.c",
        ]
        + COMMON_SAVER_SOURCES,
        "benchlab.vcxproj",
        errors,
    )
    require_all(
        benchlab_resources,
        COMMON_SAVER_RESOURCES,
        "benchlab.vcxproj",
        errors,
    )
    require(
        "screensave_platform.vcxproj" in benchlab_project_refs,
        "benchlab.vcxproj must reference the platform project.",
        errors,
    )
    require("<TargetExt>.exe</TargetExt>" in benchlab_project_text, "benchlab.vcxproj must emit an .exe target.", errors)
    require(
        "user32.lib;gdi32.lib;advapi32.lib;opengl32.lib" in benchlab_project_text,
        "benchlab.vcxproj must link the app-support Win32 and OpenGL libraries.",
        errors,
    )
    require("out\\msvc\\vs2022" in benchlab_project_text, "benchlab.vcxproj must use the documented MSVC output root.", errors)

    makefile_text = MINGW_MAKEFILE.read_text(encoding="utf-8")
    for expected in (
        "mingw/i686",
        "OUTROOT := $(ROOT)/out",
        "$(PLATFORM_DIR)/renderer_dispatch.o",
        "$(PLATFORM_DIR)/saver_contract.o",
        "$(PLATFORM_DIR)/saver_registry.o",
        "$(PLATFORM_DIR)/config.o",
        "$(PLATFORM_DIR)/diagnostics.o",
        "$(PLATFORM_DIR)/grid_buffer.o",
        "$(PLATFORM_DIR)/rng.o",
        "$(PLATFORM_DIR)/timing.o",
        "$(PLATFORM_DIR)/version.o",
        "$(PLATFORM_DIR)/gdi_backend.o",
        "$(PLATFORM_DIR)/gdi_bitmap.o",
        "$(PLATFORM_DIR)/gdi_present.o",
        "$(PLATFORM_DIR)/gdi_primitives.o",
        "$(PLATFORM_DIR)/gdi_state.o",
        "$(PLATFORM_DIR)/gdi_surface.o",
        "$(PLATFORM_DIR)/gl11_backend.o",
        "$(PLATFORM_DIR)/gl11_bitmap.o",
        "$(PLATFORM_DIR)/gl11_caps.o",
        "$(PLATFORM_DIR)/gl11_context.o",
        "$(PLATFORM_DIR)/gl11_present.o",
        "$(PLATFORM_DIR)/gl11_primitives.o",
        "$(PLATFORM_DIR)/gl11_state.o",
        "$(PLATFORM_DIR)/scr_entry.o",
        "$(PLATFORM_DIR)/scr_validation_scene.o",
        "$(PLATFORM_DIR)/scr_window.o",
        "$(NOCTURNE_DIR)/nocturne_config.o",
        "$(NOCTURNE_DIR)/nocturne_entry.o",
        "$(RICOCHET_DIR)/ricochet_config.o",
        "$(RICOCHET_DIR)/ricochet_entry.o",
        "$(DEEPFIELD_DIR)/deepfield_config.o",
        "$(DEEPFIELD_DIR)/deepfield_entry.o",
        "$(EMBER_DIR)/ember_config.o",
        "$(EMBER_DIR)/ember_entry.o",
        "$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_config.o",
        "$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_entry.o",
        "$(PIPEWORKS_DIR)/pipeworks_config.o",
        "$(PIPEWORKS_DIR)/pipeworks_entry.o",
        "$(LIFEFORMS_DIR)/lifeforms_config.o",
        "$(LIFEFORMS_DIR)/lifeforms_entry.o",
        "screensave_host.rc",
        "nocturne.scr",
        "ricochet.scr",
        "deepfield.scr",
        "ember.scr",
        "oscilloscope_dreams.scr",
        "pipeworks.scr",
        "lifeforms.scr",
        "$(BENCHLAB_DIR)/benchlab_app.o",
        "$(BENCHLAB_DIR)/benchlab_config.o",
        "$(BENCHLAB_DIR)/benchlab_diag.o",
        "$(BENCHLAB_DIR)/benchlab_main.o",
        "$(BENCHLAB_DIR)/benchlab_overlay.o",
        "$(BENCHLAB_DIR)/benchlab_session.o",
        "benchlab.exe",
        "opengl32",
        "windres",
    ):
        require(expected in makefile_text, f"Makefile is missing {expected!r}.", errors)

    host_entry_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_entry.c").read_text(encoding="utf-8")
    require("screensave_scr_main" in host_entry_text, "scr_entry.c must define the shared saver host entry.", errors)
    require("screensave_scr_main_with_registry" in host_entry_text, "scr_entry.c must define the registry-aware host entry.", errors)
    require("screensave_saver_registry_find" in host_entry_text, "scr_entry.c must resolve the selected built-in saver through the shared private registry helper.", errors)
    require("scr_load_selected_product_key" in host_entry_text, "scr_entry.c must load the persisted current product key.", errors)

    host_config_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_config_dialog.c").read_text(encoding="utf-8")
    require("IDD_SCR_PRODUCT_SELECT" in host_config_text, "scr_config_dialog.c must provide the built-in saver selection dialog path.", errors)
    require("IDC_SCR_PRODUCT_SETTINGS" in host_config_text, "scr_config_dialog.c must allow saver-specific settings from the selector dialog.", errors)
    require("scr_run_module_settings_dialog" in host_config_text, "scr_config_dialog.c must delegate settings to the selected saver module.", errors)

    host_window_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_window.c").read_text(encoding="utf-8")
    require("screensave_renderer_create_for_window" in host_window_text, "scr_window.c must initialize the reusable renderer through the shared public contract.", errors)
    require("SCREENSAVE_RENDERER_KIND_UNKNOWN" in host_window_text, "scr_window.c must use the automatic renderer-selection path.", errors)
    require("scr_render_validation_scene" in host_window_text, "scr_window.c must route the fallback validation scene through the shared renderer.", errors)
    require("SCREENSAVE_SESSION_MODE_PREVIEW" in host_window_text, "scr_window.c must distinguish preview mode.", errors)

    renderer_api_text = (ROOT / "platform" / "include" / "screensave" / "renderer_api.h").read_text(encoding="utf-8")
    require("selection_reason" in renderer_api_text, "renderer_api.h must expose selection-reason reporting.", errors)
    require("fallback_reason" in renderer_api_text, "renderer_api.h must expose fallback-reason reporting.", errors)
    require("vendor_name" in renderer_api_text, "renderer_api.h must expose backend identity strings for GL diagnostics.", errors)
    require("screensave_bitmap_view" in renderer_api_text, "renderer_api.h must expose bitmap-view presentation for framebuffer-backed savers.", errors)

    renderer_dispatch_text = (ROOT / "platform" / "src" / "core" / "base" / "renderer_dispatch.c").read_text(encoding="utf-8")
    require("SCREENSAVE_RENDERER_KIND_GL11" in renderer_dispatch_text, "renderer_dispatch.c must route the optional GL11 backend.", errors)
    require("auto-fallback-gdi" in renderer_dispatch_text, "renderer_dispatch.c must report automatic fallback to GDI.", errors)
    require("force-gl11-fallback-gdi" in renderer_dispatch_text, "renderer_dispatch.c must report forced-GL11 fallback to GDI.", errors)

    gdi_backend_text = (ROOT / "platform" / "src" / "render" / "gdi" / "gdi_backend.c").read_text(encoding="utf-8")
    require("SCREENSAVE_RENDERER_KIND_GDI" in gdi_backend_text, "gdi_backend.c must identify itself as the GDI renderer.", errors)

    gl11_backend_text = (ROOT / "platform" / "src" / "render" / "gl11" / "gl11_backend.c").read_text(encoding="utf-8")
    require("SCREENSAVE_RENDERER_KIND_GL11" in gl11_backend_text, "gl11_backend.c must identify itself as the GL11 renderer.", errors)

    gl11_context_text = (ROOT / "platform" / "src" / "render" / "gl11" / "gl11_context.c").read_text(encoding="utf-8")
    require("wglCreateContext" in gl11_context_text, "gl11_context.c must create a real WGL context.", errors)
    require("SetPixelFormat" in gl11_context_text, "gl11_context.c must set a real pixel format.", errors)

    gl11_caps_text = (ROOT / "platform" / "src" / "render" / "gl11" / "gl11_caps.c").read_text(encoding="utf-8")
    require("glGetString(GL_VENDOR)" in gl11_caps_text, "gl11_caps.c must capture GL vendor information.", errors)
    require("glGetString(GL_RENDERER)" in gl11_caps_text, "gl11_caps.c must capture GL renderer information.", errors)
    require("glGetString(GL_VERSION)" in gl11_caps_text, "gl11_caps.c must capture GL version information.", errors)

    gdi_surface_text = (ROOT / "platform" / "src" / "render" / "gdi" / "gdi_surface.c").read_text(encoding="utf-8")
    require("CreateDIBSection" in gdi_surface_text, "gdi_surface.c must create a real offscreen GDI backbuffer.", errors)

    validation_scene_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_validation_scene.c").read_text(encoding="utf-8")
    require("screensave_renderer_blit_bitmap" in validation_scene_text, "scr_validation_scene.c must exercise bitmap blit support.", errors)

    for product_key, entry_name, module_name in (
        ("nocturne", "nocturne_entry.c", "nocturne_get_module"),
        ("ricochet", "ricochet_entry.c", "ricochet_get_module"),
        ("deepfield", "deepfield_entry.c", "deepfield_get_module"),
        ("ember", "ember_entry.c", "ember_get_module"),
        ("oscilloscope_dreams", "oscilloscope_dreams_entry.c", "oscilloscope_dreams_get_module"),
        ("pipeworks", "pipeworks_entry.c", "pipeworks_get_module"),
        ("lifeforms", "lifeforms_entry.c", "lifeforms_get_module"),
    ):
        entry_text = (ROOT / "products" / "savers" / product_key / "src" / entry_name).read_text(encoding="utf-8")
        module_text = (ROOT / "products" / "savers" / product_key / "src" / f"{product_key}_module.c").read_text(encoding="utf-8")
        require(
            "screensave_scr_main_with_registry" in entry_text,
            f"{entry_name} must delegate into the registry-aware host entry.",
            errors,
        )
        require(module_name in entry_text, f"{entry_name} must dispatch through the real product module.", errors)
        require("screensave_saver_module" in module_text, f"{product_key}_module.c must define a saver-module descriptor.", errors)

    benchlab_main_text = (ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_main.c").read_text(encoding="utf-8")
    benchlab_app_text = (ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_app.c").read_text(encoding="utf-8")
    benchlab_session_text = (ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_session.c").read_text(encoding="utf-8")
    benchlab_overlay_text = (ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_overlay.c").read_text(encoding="utf-8")

    require("benchlab_app_run" in benchlab_main_text, "benchlab_main.c must delegate into the real BenchLab app runner.", errors)
    require("screensave_saver_module_is_valid" in benchlab_app_text, "benchlab_app.c must validate the selected saver module before creating the harness.", errors)
    require("IDM_BENCHLAB_PRODUCT_FIRST" in benchlab_app_text, "benchlab_app.c must expose saver-selection commands.", errors)
    require("benchlab_handle_module_request" in benchlab_app_text, "benchlab_app.c must support switching the active saver product.", errors)
    require("screensave_renderer_create_for_window" in benchlab_session_text, "benchlab_session.c must create the shared renderer through the public renderer contract.", errors)
    require("screensave_saver_registry_find" in benchlab_session_text, "benchlab_session.c must resolve selected saver products through the shared private registry helper.", errors)
    require("ricochet_get_module" in benchlab_session_text, "benchlab_session.c must bind Ricochet for the current stage.", errors)
    require("deepfield_get_module" in benchlab_session_text, "benchlab_session.c must bind Deepfield for the current stage.", errors)
    require("ember_get_module" in benchlab_session_text, "benchlab_session.c must bind Ember for the current stage.", errors)
    require("oscilloscope_dreams_get_module" in benchlab_session_text, "benchlab_session.c must bind Oscilloscope Dreams for the current stage.", errors)
    require("pipeworks_get_module" in benchlab_session_text, "benchlab_session.c must bind Pipeworks for the current stage.", errors)
    require("lifeforms_get_module" in benchlab_session_text, "benchlab_session.c must bind Lifeforms for the current stage.", errors)
    require("show_config_dialog" in benchlab_session_text, "benchlab_session.c must support product-owned saver configuration dialogs.", errors)
    require("fallback_reason" in benchlab_overlay_text, "benchlab_overlay.c must report renderer fallback reasons.", errors)
    require("vendor_name" in benchlab_overlay_text, "benchlab_overlay.c must report GL identity strings when available.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Build layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
