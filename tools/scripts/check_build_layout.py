"""Validate the checked-in build scaffold and its key target relationships."""

from __future__ import annotations

import pathlib
import sys
import xml.etree.ElementTree as ET


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOLUTION = ROOT / "build" / "msvc" / "vs2022" / "ScreenSave.sln"
PLATFORM_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "screensave_platform.vcxproj"
NOCTURNE_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "nocturne.vcxproj"
MINGW_MAKEFILE = ROOT / "build" / "mingw" / "i686" / "Makefile"

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
    MINGW_MAKEFILE,
    ROOT / "platform" / "include" / "screensave" / "types.h",
    ROOT / "platform" / "include" / "screensave" / "version.h",
    ROOT / "platform" / "include" / "screensave" / "diagnostics_api.h",
    ROOT / "platform" / "include" / "screensave" / "config_api.h",
    ROOT / "platform" / "include" / "screensave" / "renderer_api.h",
    ROOT / "platform" / "include" / "screensave" / "saver_api.h",
    ROOT / "platform" / "src" / "core" / "base" / "renderer_dispatch.c",
    ROOT / "platform" / "src" / "core" / "base" / "renderer_private.h",
    ROOT / "platform" / "src" / "core" / "base" / "saver_contract.c",
    ROOT / "platform" / "src" / "core" / "config" / "config.c",
    ROOT / "platform" / "src" / "core" / "diagnostics" / "diagnostics.c",
    ROOT / "platform" / "src" / "core" / "rng" / "rng.c",
    ROOT / "platform" / "src" / "core" / "rng" / "rng_internal.h",
    ROOT / "platform" / "src" / "core" / "timing" / "timing.c",
    ROOT / "platform" / "src" / "core" / "timing" / "timing_internal.h",
    ROOT / "platform" / "src" / "core" / "version" / "version.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "README.md",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_internal.h",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_backend.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_surface.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_present.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_primitives.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_bitmap.c",
    ROOT / "platform" / "src" / "render" / "gdi" / "gdi_state.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_internal.h",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_args.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_config_dialog.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_diagnostics.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_entry.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_settings.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_validation_scene.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_window.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "resource.h",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "screensave_host.rc",
    ROOT / "products" / "savers" / "nocturne" / "src" / "README.md",
    ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_entry.c",
]

NS = {"msb": "http://schemas.microsoft.com/developer/msbuild/2003"}


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def parse_project(path: pathlib.Path) -> ET.Element:
    return ET.parse(path).getroot()


def main() -> int:
    errors = []

    for path in REQUIRED_PATHS:
        require(path.exists(), f"Missing required build path: {path.relative_to(ROOT)}", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    build_readme = (ROOT / "build" / "README.md").read_text(encoding="utf-8")
    for phrase in ("checked-in per-toolchain lanes", "out/", "concrete MSVC VS2022 solution", "mandatory GDI backend"):
        require(phrase in build_readme, f"build/README.md is missing expected phrase: {phrase!r}", errors)

    for path in (ROOT / "build" / "msvc" / "vs6" / "README.md", ROOT / "build" / "msvc" / "vs2008" / "README.md"):
        content = path.read_text(encoding="utf-8").lower()
        require("deferred" in content, f"{path.relative_to(ROOT)} must explicitly describe deferred work.", errors)

    solution_text = SOLUTION.read_text(encoding="utf-8")
    for expected in ("screensave_platform.vcxproj", "nocturne.vcxproj", "Debug|Win32", "Release|Win32"):
        require(expected in solution_text, f"ScreenSave.sln is missing {expected!r}.", errors)

    platform_project = parse_project(PLATFORM_PROJECT)
    nocturne_project = parse_project(NOCTURNE_PROJECT)

    platform_sources = [node.attrib.get("Include", "") for node in platform_project.findall(".//msb:ClCompile", NS)]
    nocturne_sources = [node.attrib.get("Include", "") for node in nocturne_project.findall(".//msb:ClCompile", NS)]
    nocturne_resources = [node.attrib.get("Include", "") for node in nocturne_project.findall(".//msb:ResourceCompile", NS)]
    project_refs = [node.attrib.get("Include", "") for node in nocturne_project.findall(".//msb:ProjectReference", NS)]

    for expected in (
        "..\\..\\..\\platform\\src\\core\\base\\renderer_dispatch.c",
        "..\\..\\..\\platform\\src\\core\\base\\saver_contract.c",
        "..\\..\\..\\platform\\src\\core\\config\\config.c",
        "..\\..\\..\\platform\\src\\core\\diagnostics\\diagnostics.c",
        "..\\..\\..\\platform\\src\\core\\rng\\rng.c",
        "..\\..\\..\\platform\\src\\core\\timing\\timing.c",
        "..\\..\\..\\platform\\src\\core\\version\\version.c",
        "..\\..\\..\\platform\\src\\render\\gdi\\gdi_backend.c",
        "..\\..\\..\\platform\\src\\render\\gdi\\gdi_bitmap.c",
        "..\\..\\..\\platform\\src\\render\\gdi\\gdi_present.c",
        "..\\..\\..\\platform\\src\\render\\gdi\\gdi_primitives.c",
        "..\\..\\..\\platform\\src\\render\\gdi\\gdi_state.c",
        "..\\..\\..\\platform\\src\\render\\gdi\\gdi_surface.c",
        "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_entry.c",
        "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_validation_scene.c",
        "..\\..\\..\\platform\\src\\host\\win32_scr\\scr_window.c",
    ):
        require(expected in platform_sources, f"screensave_platform.vcxproj is missing {expected!r}.", errors)
    require(
        "..\\..\\..\\products\\savers\\nocturne\\src\\nocturne_entry.c" in nocturne_sources,
        "nocturne.vcxproj must compile the product entry.",
        errors,
    )
    require(
        "..\\..\\..\\platform\\src\\host\\win32_scr\\screensave_host.rc" in nocturne_resources,
        "nocturne.vcxproj must compile the host resource script.",
        errors,
    )
    require("screensave_platform.vcxproj" in project_refs, "nocturne.vcxproj must reference the platform project.", errors)

    nocturne_project_text = NOCTURNE_PROJECT.read_text(encoding="utf-8")
    require("<TargetExt>.scr</TargetExt>" in nocturne_project_text, "nocturne.vcxproj must emit a .scr target.", errors)
    require("user32.lib;gdi32.lib;advapi32.lib" in nocturne_project_text, "nocturne.vcxproj must link the host-support Win32 libraries.", errors)
    require("out\\msvc\\vs2022" in nocturne_project_text, "nocturne.vcxproj must use the documented MSVC output root.", errors)

    makefile_text = MINGW_MAKEFILE.read_text(encoding="utf-8")
    for expected in (
        "mingw/i686",
        "OUTROOT := $(ROOT)/out",
        "renderer_dispatch.c",
        "saver_contract.c",
        "config.c",
        "diagnostics.c",
        "rng.c",
        "timing.c",
        "version.c",
        "gdi_backend.c",
        "gdi_bitmap.c",
        "gdi_present.c",
        "gdi_primitives.c",
        "gdi_state.c",
        "gdi_surface.c",
        "scr_entry.c",
        "scr_validation_scene.c",
        "scr_window.c",
        "nocturne_entry.c",
        "screensave_host.rc",
        "nocturne.scr",
        "windres",
    ):
        require(expected in makefile_text, f"Makefile is missing {expected!r}.", errors)

    host_entry_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_entry.c").read_text(encoding="utf-8")
    require("screensave_scr_main" in host_entry_text, "scr_entry.c must define the shared saver host entry.", errors)
    require("screensave_saver_module_is_valid" in host_entry_text, "scr_entry.c must validate the public saver module contract.", errors)

    host_window_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_window.c").read_text(encoding="utf-8")
    require("screensave_gdi_renderer_create" in host_window_text, "scr_window.c must initialize the reusable GDI renderer.", errors)
    require("scr_render_validation_scene" in host_window_text, "scr_window.c must route the fallback validation scene through the shared renderer.", errors)
    require("SCREENSAVE_SESSION_MODE_PREVIEW" in host_window_text, "scr_window.c must distinguish preview mode.", errors)

    gdi_backend_text = (ROOT / "platform" / "src" / "render" / "gdi" / "gdi_backend.c").read_text(encoding="utf-8")
    require("SCREENSAVE_RENDERER_KIND_GDI" in gdi_backend_text, "gdi_backend.c must identify itself as the GDI renderer.", errors)

    gdi_surface_text = (ROOT / "platform" / "src" / "render" / "gdi" / "gdi_surface.c").read_text(encoding="utf-8")
    require("CreateDIBSection" in gdi_surface_text, "gdi_surface.c must create a real offscreen GDI backbuffer.", errors)

    validation_scene_text = (ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_validation_scene.c").read_text(encoding="utf-8")
    require("screensave_renderer_blit_bitmap" in validation_scene_text, "scr_validation_scene.c must exercise bitmap blit support.", errors)

    product_entry_text = (ROOT / "products" / "savers" / "nocturne" / "src" / "nocturne_entry.c").read_text(encoding="utf-8")
    require("screensave_scr_main" in product_entry_text, "nocturne_entry.c must delegate into the host entry.", errors)
    require("screensave_saver_module" in product_entry_text, "nocturne_entry.c must define a saver-module descriptor.", errors)
    require("Nocturne" in product_entry_text, "nocturne_entry.c must declare the product identity.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Build layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
