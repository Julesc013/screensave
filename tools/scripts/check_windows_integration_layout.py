"""Validate the C05 Win32 saver lifecycle hardening and matrix note."""

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


def check_host_hardening(errors: list[str]) -> None:
    internal = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_internal.h")
    resource_ids = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "resource.h")
    host_resource = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "screensave_host.rc")
    config_dialog = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_config_dialog.c")
    window_code = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "scr_window.c")

    require("SM_CMONITORS" in internal, "scr_internal.h is missing the compatibility fallback for SM_CMONITORS.", errors)
    require("SM_CXVIRTUALSCREEN" in internal, "scr_internal.h is missing the compatibility fallback for virtual-screen sizing.", errors)
    require("IDD_SCR_SAVER_SHELL" in resource_ids, "resource.h is missing the single-saver shell dialog id.", errors)
    require('LTEXT "", IDC_SCR_INFO' in host_resource, "screensave_host.rc is missing the single-saver shell info text control.", errors)
    require('PUSHBUTTON "Settings...", IDC_SCR_PRODUCT_SETTINGS' in host_resource, "screensave_host.rc is missing the single-saver Settings button.", errors)
    require("scr_show_single_saver_dialog" in config_dialog, "scr_config_dialog.c is missing the single-saver shell path.", errors)
    require("scr_edit_module_settings_dialog" in config_dialog, "scr_config_dialog.c is missing the staged saver-settings editor helper.", errors)
    require("scr_save_renderer_request" in config_dialog, "scr_config_dialog.c is missing renderer-preference persistence in config mode.", errors)
    require("WM_DISPLAYCHANGE" in window_code, "scr_window.c is missing display-change resynchronization.", errors)
    require("scr_preview_parent_is_valid" in window_code, "scr_window.c is missing preview-parent validation.", errors)
    require("scr_sync_preview_window" in window_code, "scr_window.c is missing preview-window resynchronization.", errors)
    require("scr_sync_screen_window" in window_code, "scr_window.c is missing screen-window resynchronization.", errors)
    require("SM_XVIRTUALSCREEN" in window_code, "scr_window.c is missing virtual-desktop fullscreen sizing.", errors)


def check_saver_metadata(errors: list[str]) -> None:
    for saver in CANONICAL_SAVERS:
        manifest_path = SAVERS_ROOT / saver / "manifest.ini"
        version_path = SAVERS_ROOT / saver / "src" / f"{saver}_version.rc"

        require(manifest_path.exists(), f"Missing saver manifest: {manifest_path.relative_to(ROOT)}", errors)
        require(version_path.exists(), f"Missing saver version resource: {version_path.relative_to(ROOT)}", errors)
        if not manifest_path.exists() or not version_path.exists():
            continue

        manifest = read_ini(manifest_path)
        relative_manifest = manifest_path.relative_to(ROOT)
        require(manifest.get("product", "key", fallback="") == saver, f"{relative_manifest} has the wrong product key.", errors)
        require(manifest.get("product", "name", fallback="") != "", f"{relative_manifest} is missing the canonical product name.", errors)
        require(manifest.get("product", "kind", fallback="") == "saver", f"{relative_manifest} must declare kind=saver.", errors)
        require(manifest.get("identity", "summary", fallback="") != "", f"{relative_manifest} is missing the identity summary.", errors)
        require(manifest.get("identity", "default_preset", fallback="") != "", f"{relative_manifest} is missing default_preset.", errors)
        require(manifest.get("identity", "default_theme", fallback="") != "", f"{relative_manifest} is missing default_theme.", errors)
        require(manifest.get("capabilities", "preview_safe", fallback="") == "1", f"{relative_manifest} must declare preview_safe=1.", errors)
        require(manifest.get("capabilities", "long_run_stable", fallback="") == "1", f"{relative_manifest} must declare long_run_stable=1.", errors)

        version_text = read_text(version_path)
        require(f'#define SCREENSAVE_INTERNAL_NAME "{saver}.scr"' in version_text, f"{version_path.relative_to(ROOT)} is missing the canonical internal filename.", errors)
        require(f'#define SCREENSAVE_ORIGINAL_FILENAME "{saver}.scr"' in version_text, f"{version_path.relative_to(ROOT)} is missing the canonical original filename.", errors)


def check_status_docs(errors: list[str]) -> None:
    note = read_text(ROOT / "validation" / "notes" / "c05-windows-integration-matrix.md")
    require("# C05 Windows Integration Matrix" in note, "C05 validation note is missing its title.", errors)
    require("Single-saver `/c` now opens a bounded host shell" in note, "C05 validation note must record the single-saver config shell.", errors)
    require("- `C06` portable distribution bundle" in note, "C05 validation note must name the next continuation step.", errors)

    host_readme = read_text(ROOT / "platform" / "src" / "host" / "win32_scr" / "README.md")
    require("C05 hardens the real `.scr` lifecycle" in host_readme, "Win32 host README must describe the C05 hardening pass.", errors)


def main() -> int:
    errors: list[str] = []

    check_host_hardening(errors)
    check_saver_metadata(errors)
    check_status_docs(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Windows integration layout checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
