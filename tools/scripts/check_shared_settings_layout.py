"""Validate the C04 shared settings, preset, randomization, and pack scaffold."""

from __future__ import annotations

import configparser
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SAVERS_ROOT = ROOT / "products" / "savers"

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
)

REPRESENTATIVE_SAVERS = ("nocturne", "plasma", "vector", "ecosystems", "transit", "atlas")


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


def valid_color(value: str) -> bool:
    if len(value) != 7 or not value.startswith("#"):
        return False
    return all(character in "0123456789ABCDEFabcdef" for character in value[1:])


def check_module_contract(errors: list[str]) -> None:
    for saver in CANONICAL_SAVERS:
        module_path = SAVERS_ROOT / saver / "src" / f"{saver}_module.c"
        text = read_text(module_path)
        require("apply_shared_preset" in text, f"{module_path.relative_to(ROOT)} is missing the shared preset hook.", errors)
        require("SCREENSAVE_CONFIG_SCHEMA_VERSION" in text, f"{module_path.relative_to(ROOT)} is missing the shared schema version marker.", errors)
        require("SCREENSAVE_SETTINGS_CAP_PRESET_FILES" in text, f"{module_path.relative_to(ROOT)} is missing settings capability flags.", errors)

    for saver in REPRESENTATIVE_SAVERS:
        module_path = SAVERS_ROOT / saver / "src" / f"{saver}_module.c"
        config_path = SAVERS_ROOT / saver / "src" / f"{saver}_config.c"
        internal_path = SAVERS_ROOT / saver / "src" / f"{saver}_internal.h"
        require(f"{saver}_config_export_settings_entries" in read_text(module_path), f"{module_path.relative_to(ROOT)} is missing export hook wiring.", errors)
        require(f"{saver}_config_import_settings_entry" in read_text(module_path), f"{module_path.relative_to(ROOT)} is missing import hook wiring.", errors)
        require(f"{saver}_config_randomize_settings" in read_text(module_path), f"{module_path.relative_to(ROOT)} is missing randomization hook wiring.", errors)
        require(f"{saver}_config_export_settings_entries" in read_text(config_path), f"{config_path.relative_to(ROOT)} is missing export implementation.", errors)
        require(f"{saver}_config_import_settings_entry" in read_text(config_path), f"{config_path.relative_to(ROOT)} is missing import implementation.", errors)
        require(f"{saver}_config_randomize_settings" in read_text(config_path), f"{config_path.relative_to(ROOT)} is missing randomization implementation.", errors)
        require(f"{saver}_config_export_settings_entries" in read_text(internal_path), f"{internal_path.relative_to(ROOT)} is missing export declarations.", errors)


def check_host_and_benchlab(errors: list[str]) -> None:
    config_dialog = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_config_dialog.c")
    host_resource = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "screensave_host.rc")
    host_ids = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "resource.h")
    benchlab_session = read_text(ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_session.c")
    benchlab_overlay = read_text(ROOT / "products" / "apps" / "benchlab" / "src" / "benchlab_overlay.c")

    require("IDC_SCR_RANDOMIZE_SESSION" in host_ids, "Fallback host dialog is missing the shared randomization control id.", errors)
    require("Randomize each session" in host_resource, "Fallback host dialog is missing the shared randomization checkbox.", errors)
    require("screensave_randomization_default_scope()" in config_dialog, "Fallback host dialog is missing shared randomization scope handling.", errors)
    require("resolved_config" in benchlab_session, "BenchLab must resolve a per-session config state.", errors)
    require("Randomization:" in benchlab_overlay, "BenchLab overlay must surface the active randomization state.", errors)


def check_pack_samples(errors: list[str]) -> None:
    manifests = sorted(SAVERS_ROOT.glob("*/packs/*/pack.ini"))
    require(len(manifests) >= 3, "Expected at least three built-in sample pack manifests for C04.", errors)

    for manifest_path in manifests:
        parser = read_ini(manifest_path)
        relative_manifest = manifest_path.relative_to(ROOT)
        require(parser.has_section("pack"), f"{relative_manifest} is missing the [pack] section.", errors)
        require(parser.has_section("files"), f"{relative_manifest} is missing the [files] section.", errors)
        if not parser.has_section("pack") or not parser.has_section("files"):
            continue

        product_key = parser["pack"].get("product_key", "")
        require(parser["pack"].get("format", "") == "screensave-pack", f"{relative_manifest} must declare the screensave-pack format.", errors)
        require(parser["pack"].get("version", "") == "1", f"{relative_manifest} must declare version 1.", errors)
        require(product_key in CANONICAL_SAVERS, f"{relative_manifest} references an unknown saver product key.", errors)
        require(parser["pack"].get("pack_key", "") != "", f"{relative_manifest} is missing pack_key.", errors)
        require(parser["pack"].get("display_name", "") != "", f"{relative_manifest} is missing display_name.", errors)

        for key, relative_file in parser["files"].items():
            target_path = manifest_path.parent / relative_file
            require(target_path.exists(), f"{relative_manifest} references missing file {relative_file!r}.", errors)
            if not target_path.exists():
                continue

            if key.startswith("preset_"):
                preset = read_ini(target_path)
                require(preset.get("format", "kind", fallback="") == "preset", f"{target_path.relative_to(ROOT)} must be a preset export.", errors)
                require(preset.get("format", "version", fallback="") == "1", f"{target_path.relative_to(ROOT)} must declare format version 1.", errors)
                require(preset.get("product", "key", fallback="") == product_key, f"{target_path.relative_to(ROOT)} must target {product_key}.", errors)
                require(preset.has_section("common"), f"{target_path.relative_to(ROOT)} is missing the [common] section.", errors)
            elif key.startswith("theme_"):
                theme = read_ini(target_path)
                require(theme.get("format", "kind", fallback="") == "theme", f"{target_path.relative_to(ROOT)} must be a theme export.", errors)
                require(theme.get("product", "key", fallback="") == product_key, f"{target_path.relative_to(ROOT)} must target {product_key}.", errors)
                require(theme.has_section("theme"), f"{target_path.relative_to(ROOT)} is missing the [theme] section.", errors)
                require(valid_color(theme.get("theme", "primary_color", fallback="")), f"{target_path.relative_to(ROOT)} has an invalid primary color.", errors)
                require(valid_color(theme.get("theme", "accent_color", fallback="")), f"{target_path.relative_to(ROOT)} has an invalid accent color.", errors)
            elif key.startswith("scene_"):
                scene = read_ini(target_path)
                require(scene.has_section("scene"), f"{target_path.relative_to(ROOT)} is missing the [scene] section.", errors)


def main() -> int:
    errors: list[str] = []

    check_module_contract(errors)
    check_host_and_benchlab(errors)
    check_pack_samples(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Shared settings layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
