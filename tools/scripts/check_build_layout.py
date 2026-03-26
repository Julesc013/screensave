"""Validate the checked-in build scaffold and its key target relationships."""

from __future__ import annotations

import pathlib
import sys
import xml.etree.ElementTree as ET


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOLUTION = ROOT / "build" / "msvc" / "vs2022" / "ScreenSave.sln"
PLATFORM_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "screensave_platform_stub.vcxproj"
NOCTURNE_PROJECT = ROOT / "build" / "msvc" / "vs2022" / "nocturne_stub.vcxproj"
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
    ROOT / "platform" / "src" / "core" / "_series02_core_stub.c",
    ROOT / "platform" / "src" / "host" / "win32_scr" / "_series02_host_stub.c",
    ROOT / "products" / "savers" / "nocturne" / "src" / "README.md",
    ROOT / "products" / "savers" / "nocturne" / "src" / "_series02_nocturne_stub.c",
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
    for phrase in ("checked-in per-toolchain lanes", "out/", "concrete MSVC VS2022 solution", "MinGW i686"):
        require(phrase in build_readme, f"build/README.md is missing expected phrase: {phrase!r}", errors)

    for path in (ROOT / "build" / "msvc" / "vs6" / "README.md", ROOT / "build" / "msvc" / "vs2008" / "README.md"):
        content = path.read_text(encoding="utf-8").lower()
        require("deferred" in content, f"{path.relative_to(ROOT)} must explicitly describe deferred work.", errors)

    solution_text = SOLUTION.read_text(encoding="utf-8")
    for expected in ("screensave_platform_stub.vcxproj", "nocturne_stub.vcxproj", "Debug|Win32", "Release|Win32"):
        require(expected in solution_text, f"ScreenSave.sln is missing {expected!r}.", errors)

    platform_project = parse_project(PLATFORM_PROJECT)
    nocturne_project = parse_project(NOCTURNE_PROJECT)

    platform_sources = [node.attrib.get("Include", "") for node in platform_project.findall(".//msb:ClCompile", NS)]
    nocturne_sources = [node.attrib.get("Include", "") for node in nocturne_project.findall(".//msb:ClCompile", NS)]
    project_refs = [node.attrib.get("Include", "") for node in nocturne_project.findall(".//msb:ProjectReference", NS)]

    require(
        "..\\..\\..\\platform\\src\\core\\_series02_core_stub.c" in platform_sources,
        "screensave_platform_stub.vcxproj must compile the core stub.",
        errors,
    )
    require(
        "..\\..\\..\\platform\\src\\host\\win32_scr\\_series02_host_stub.c" in platform_sources,
        "screensave_platform_stub.vcxproj must compile the host stub.",
        errors,
    )
    require(
        "..\\..\\..\\products\\savers\\nocturne\\src\\_series02_nocturne_stub.c" in nocturne_sources,
        "nocturne_stub.vcxproj must compile the nocturne stub.",
        errors,
    )
    require("screensave_platform_stub.vcxproj" in project_refs, "nocturne_stub.vcxproj must reference the platform stub project.", errors)

    nocturne_project_text = NOCTURNE_PROJECT.read_text(encoding="utf-8")
    require("<TargetExt>.scr</TargetExt>" in nocturne_project_text, "nocturne_stub.vcxproj must emit a .scr target.", errors)
    require("out\\msvc\\vs2022" in nocturne_project_text, "nocturne_stub.vcxproj must use the documented MSVC output root.", errors)

    makefile_text = MINGW_MAKEFILE.read_text(encoding="utf-8")
    for expected in ("mingw/i686", "OUTROOT := $(ROOT)/out", "_series02_core_stub.c", "_series02_host_stub.c", "_series02_nocturne_stub.c", "nocturne.scr"):
        require(expected in makefile_text, f"Makefile is missing {expected!r}.", errors)

    for path in (
        ROOT / "platform" / "src" / "core" / "_series02_core_stub.c",
        ROOT / "platform" / "src" / "host" / "win32_scr" / "_series02_host_stub.c",
        ROOT / "products" / "savers" / "nocturne" / "src" / "_series02_nocturne_stub.c",
    ):
        content = path.read_text(encoding="utf-8")
        require("Series 02 build-only scaffolding" in content, f"{path.relative_to(ROOT)} must identify itself as Series 02 scaffolding.", errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Build layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
