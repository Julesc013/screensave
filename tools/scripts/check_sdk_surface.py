"""Validate the C10 SDK and contributor surface."""

from __future__ import annotations

import argparse
import configparser
import pathlib
import re
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SDK_ROOT = ROOT / "products" / "sdk"
TEMPLATE_ROOT = ROOT / "products" / "savers" / "_template"
EXAMPLE_PACK_ROOT = SDK_ROOT / "examples" / "template_pack"

SLUG_RE = re.compile(r"^[a-z0-9_]+$")
REQUIRED_TEMPLATE_SUFFIXES = (
    "_entry.c",
    "_internal.h",
    "_module.c",
    "_config.c",
    "_presets.c",
    "_themes.c",
    "_sim.c",
    "_render.c",
    "_config.rc",
    "_resource.h",
    "_version.rc",
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


def validate_sdk_baseline(errors: list[str]) -> None:
    required_paths = (
        SDK_ROOT / "README.md",
        SDK_ROOT / "saver-authoring.md",
        SDK_ROOT / "pack-authoring.md",
        SDK_ROOT / "manifest-reference.md",
        SDK_ROOT / "contributor-checklist.md",
        SDK_ROOT / "examples" / "README.md",
        SDK_ROOT / "examples" / "template_pack" / "README.md",
        SDK_ROOT / "examples" / "template_pack" / "pack.ini",
        SDK_ROOT / "examples" / "template_pack" / "presets" / "starter_showcase.preset.ini",
        SDK_ROOT / "examples" / "template_pack" / "themes" / "starter_showcase.theme.ini",
        SDK_ROOT / "template_saver" / "README.md",
        SDK_ROOT / "template_saver" / "copy-map.md",
        SDK_ROOT / "template_saver" / "validation-checklist.md",
        TEMPLATE_ROOT / "README.md",
        TEMPLATE_ROOT / "manifest.ini",
        TEMPLATE_ROOT / "tests" / "smoke.c",
        ROOT / "validation" / "notes" / "c10-sdk-contributor-surface.md",
    )
    for path in required_paths:
        require(path.exists(), f"Missing required C10 path: {path.relative_to(ROOT)}", errors)

    if errors:
        return

    validate_saver_root(TEMPLATE_ROOT, errors)
    validate_pack_root(EXAMPLE_PACK_ROOT, errors)

    require(
        "real contributor-facing templates" in read_text(SDK_ROOT / "README.md"),
        "products/sdk/README.md must describe the real contributor-facing surface.",
        errors,
    )
    require(
        "`C10` turns `_template` into the canonical starter saver surface" in read_text(TEMPLATE_ROOT / "README.md"),
        "products/savers/_template/README.md must record the C10 template baseline.",
        errors,
    )
    require(
        "Post-`SS` work now proceeds in the bounded `SX` substrate series." in read_text(ROOT / "README.md"),
        "README.md must record the post-C16 handoff.",
        errors,
    )
    require(
        "`C13` Wave A, Wave B, and Wave C are complete, `C14` final rerelease hardening is complete, `C15` release doctrine and channel split are complete, and `C16` Core release refresh and baseline freeze is complete." in read_text(
            ROOT / "docs" / "roadmap" / "prompt-program.md"
        ),
        "prompt-program.md must record the completed C16 freeze.",
        errors,
    )
    require(
        "`SX` is now active as the bounded substrate-hardening and capability-expansion series." in read_text(
            ROOT / "docs" / "roadmap" / "series-map.md"
        ),
        "series-map.md must point to the post-C16 handoff.",
        errors,
    )
    require(
        "This file remains as historical record for the completed bridge; active post-`C16` planning now treats `SS` as closed, `SX` as active, and `PL` as the later flagship program from the frozen Core baseline." in read_text(
            ROOT / "docs" / "roadmap" / "post-s15-plan.md"
        ),
        "post-s15-plan.md must point to the post-C16 handoff.",
        errors,
    )
    require(
        "# C10 SDK Contributor Surface" in read_text(ROOT / "validation" / "notes" / "c10-sdk-contributor-surface.md"),
        "C10 baseline note must exist with the correct title.",
        errors,
    )
    require(
        "- `C11` backlog ingestion and routing of new ideas" in read_text(
            ROOT / "validation" / "notes" / "c10-sdk-contributor-surface.md"
        ),
        "C10 baseline note must name the next continuation step.",
        errors,
    )
    require(
        "check_sdk_surface.py" in read_text(ROOT / "tools" / "scripts" / "README.md"),
        "tools/scripts/README.md must mention check_sdk_surface.py.",
        errors,
    )


def validate_saver_root(path: pathlib.Path, errors: list[str]) -> None:
    manifest_path = path / "manifest.ini"
    readme_path = path / "README.md"
    presets_dir = path / "presets"
    src_dir = path / "src"
    tests_dir = path / "tests"

    require(manifest_path.exists(), f"{path} is missing manifest.ini.", errors)
    require(readme_path.exists(), f"{path} is missing README.md.", errors)
    require(presets_dir.is_dir(), f"{path} is missing presets/.", errors)
    require(src_dir.is_dir(), f"{path} is missing src/.", errors)
    require(tests_dir.is_dir(), f"{path} is missing tests/.", errors)
    if errors:
        return

    manifest = read_ini(manifest_path)
    require(manifest.has_section("product"), f"{manifest_path} is missing [product].", errors)
    require(manifest.has_section("identity"), f"{manifest_path} is missing [identity].", errors)
    require(manifest.has_section("capabilities"), f"{manifest_path} is missing [capabilities].", errors)
    if errors:
        return

    product_key = manifest.get("product", "key", fallback="")
    default_preset = manifest.get("identity", "default_preset", fallback="")
    default_theme = manifest.get("identity", "default_theme", fallback="")

    require(bool(SLUG_RE.match(product_key)), f"{manifest_path} must use a safe lowercase product key.", errors)
    require(manifest.get("product", "kind", fallback="") == "saver", f"{manifest_path} must declare kind=saver.", errors)
    require(default_preset != "", f"{manifest_path} must declare a default_preset.", errors)
    require(default_theme != "", f"{manifest_path} must declare a default_theme.", errors)
    require(manifest.get("capabilities", "gdi", fallback="") == "1", f"{manifest_path} must declare gdi=1.", errors)

    for suffix in REQUIRED_TEMPLATE_SUFFIXES:
        require(
            (src_dir / f"{product_key}{suffix}").exists(),
            f"{path.relative_to(ROOT)} is missing src/{product_key}{suffix}.",
            errors,
        )

    preset_files = tuple(sorted(presets_dir.glob("*.ini")))
    require(bool(preset_files), f"{path.relative_to(ROOT)} must include preset .ini files.", errors)
    for preset_file in preset_files:
        text = read_text(preset_file)
        require("[preset]" in text, f"{preset_file.relative_to(ROOT)} must contain at least one [preset] block.", errors)

    smoke_path = tests_dir / "smoke.c"
    require(smoke_path.exists(), f"{path.relative_to(ROOT)} is missing tests/smoke.c.", errors)
    require(
        product_key in read_text(smoke_path),
        f"{smoke_path.relative_to(ROOT)} must reference the product key {product_key}.",
        errors,
    )

    internal_header = src_dir / f"{product_key}_internal.h"
    if internal_header.exists():
        require(
            f"Software\\\\Julesc013\\\\ScreenSave\\\\Products\\\\{product_key}" in read_text(internal_header),
            f"{internal_header.relative_to(ROOT)} must declare the canonical registry root for {product_key}.",
            errors,
        )


def validate_preset_file(path: pathlib.Path, errors: list[str]) -> None:
    parser = read_ini(path)

    require(parser.get("format", "kind", fallback="") == "preset", f"{path.relative_to(ROOT)} must declare kind=preset.", errors)
    require(parser.get("format", "version", fallback="") == "1", f"{path.relative_to(ROOT)} must declare version=1.", errors)
    require(parser.get("product", "key", fallback="") != "", f"{path.relative_to(ROOT)} must declare product key.", errors)
    require(parser.get("product", "schema_version", fallback="") == "1", f"{path.relative_to(ROOT)} must declare schema_version=1.", errors)
    require(parser.get("common", "preset_key", fallback="") != "", f"{path.relative_to(ROOT)} must declare common preset_key.", errors)
    require(parser.get("common", "theme_key", fallback="") != "", f"{path.relative_to(ROOT)} must declare common theme_key.", errors)


def validate_theme_file(path: pathlib.Path, errors: list[str]) -> None:
    parser = read_ini(path)

    require(parser.get("format", "kind", fallback="") == "theme", f"{path.relative_to(ROOT)} must declare kind=theme.", errors)
    require(parser.get("format", "version", fallback="") == "1", f"{path.relative_to(ROOT)} must declare version=1.", errors)
    require(parser.get("product", "key", fallback="") != "", f"{path.relative_to(ROOT)} must declare product key.", errors)
    require(parser.get("product", "schema_version", fallback="") == "1", f"{path.relative_to(ROOT)} must declare schema_version=1.", errors)
    require(parser.get("theme", "theme_key", fallback="") != "", f"{path.relative_to(ROOT)} must declare theme_key.", errors)
    require(parser.get("theme", "display_name", fallback="") != "", f"{path.relative_to(ROOT)} must declare display_name.", errors)


def validate_pack_root(path: pathlib.Path, errors: list[str]) -> None:
    manifest_path = path / "pack.ini"
    require(manifest_path.exists(), f"{path.relative_to(ROOT)} is missing pack.ini.", errors)
    if errors:
        return

    parser = read_ini(manifest_path)
    require(parser.get("pack", "format", fallback="") == "screensave-pack", f"{manifest_path.relative_to(ROOT)} must declare format=screensave-pack.", errors)
    require(parser.get("pack", "version", fallback="") == "1", f"{manifest_path.relative_to(ROOT)} must declare version=1.", errors)
    require(parser.get("pack", "schema_version", fallback="") == "1", f"{manifest_path.relative_to(ROOT)} must declare schema_version=1.", errors)
    require(parser.get("pack", "product_key", fallback="") != "", f"{manifest_path.relative_to(ROOT)} must declare product_key.", errors)
    require(parser.get("pack", "display_name", fallback="") != "", f"{manifest_path.relative_to(ROOT)} must declare display_name.", errors)

    file_items = parser.items("files") if parser.has_section("files") else []
    require(bool(file_items), f"{manifest_path.relative_to(ROOT)} must declare at least one file entry.", errors)
    for entry_name, relative_path in file_items:
        entry_path = pathlib.PurePosixPath(relative_path)
        require(not entry_path.is_absolute(), f"{manifest_path.relative_to(ROOT)} entry {entry_name} must be relative.", errors)
        require(".." not in entry_path.parts, f"{manifest_path.relative_to(ROOT)} entry {entry_name} must not escape the pack root.", errors)
        resolved = path / pathlib.Path(relative_path)
        require(resolved.exists(), f"{manifest_path.relative_to(ROOT)} entry {entry_name} points to a missing file: {relative_path}", errors)
        if resolved.suffixes[-2:] == [".preset", ".ini"]:
            validate_preset_file(resolved, errors)
        elif resolved.suffixes[-2:] == [".theme", ".ini"]:
            validate_theme_file(resolved, errors)


def validate_path(path: pathlib.Path, errors: list[str]) -> None:
    resolved = path.resolve()
    if not resolved.exists():
        errors.append(f"Path does not exist: {path}")
        return

    if resolved.is_dir() and (resolved / "manifest.ini").exists():
        validate_saver_root(resolved, errors)
        return
    if resolved.is_dir() and (resolved / "pack.ini").exists():
        validate_pack_root(resolved, errors)
        return
    if resolved.name.endswith(".preset.ini"):
        validate_preset_file(resolved, errors)
        return
    if resolved.name.endswith(".theme.ini"):
        validate_theme_file(resolved, errors)
        return

    errors.append(
        "Unsupported path type for SDK validation. Expected a saver root, pack root, .preset.ini, or .theme.ini file."
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the C10 SDK and contributor surface.")
    parser.add_argument("paths", nargs="*", help="Optional saver roots, pack roots, or preset/theme files to validate.")
    args = parser.parse_args()

    errors: list[str] = []
    if args.paths:
        for path_text in args.paths:
            validate_path((ROOT / path_text) if not pathlib.Path(path_text).is_absolute() else pathlib.Path(path_text), errors)
    else:
        validate_sdk_baseline(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("SDK contributor surface checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

