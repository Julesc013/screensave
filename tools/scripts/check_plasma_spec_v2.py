"""Validate the Plasma v2 product specification and U09 migration law."""

from __future__ import annotations

import pathlib
import re
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
CONTRACT = ROOT / "contracts" / "plasma_spec_v2.md"
SRC_ROOT = ROOT / "products" / "savers" / "plasma" / "src"
SPEC_HEADER = SRC_ROOT / "plasma_spec_v2.h"
SPEC_SOURCE = SRC_ROOT / "plasma_spec_v2.c"
MIGRATION_HEADER = SRC_ROOT / "plasma_migration_v2.h"
MIGRATION_SOURCE = SRC_ROOT / "plasma_migration_v2.c"
DIRECT_V2_ROOT = SRC_ROOT / "v2"
DIRECT_TYPES = DIRECT_V2_ROOT / "plasma_v2_types.h"
DIRECT_SPEC_HEADER = DIRECT_V2_ROOT / "plasma_v2_spec.h"
DIRECT_SPEC_SOURCE = DIRECT_V2_ROOT / "plasma_v2_spec.c"
SMOKE_ROOT = ROOT / "out" / "checks" / "plasma-spec-v2"

REQUIRED_FILES = [
    CONTRACT,
    SPEC_HEADER,
    SPEC_SOURCE,
    MIGRATION_HEADER,
    MIGRATION_SOURCE,
    DIRECT_TYPES,
    DIRECT_SPEC_HEADER,
    DIRECT_SPEC_SOURCE,
]

REQUIRED_TOKENS = [
    "screensave.plasma.spec.v2",
    "classic_interference",
    "radial_warped",
    "continuous",
    "banded",
    "contour",
    "plasma_lava",
    "aurora_cool",
    "oceanic_blue",
    "museum_phosphor",
    "quiet_darkroom",
    "restrained_dither",
    "restrained_crt",
    "U09",
    "not a public SDK ABI",
]

STABLE_PRESETS = [
    "plasma_lava",
    "aurora_plasma",
    "ocean_interference",
    "museum_phosphor",
    "quiet_darkroom",
]

STABLE_THEMES = [
    "plasma_lava",
    "aurora_cool",
    "oceanic_blue",
    "museum_phosphor",
    "quiet_darkroom",
]

FORBIDDEN_TERMS = [
    "windows.h",
    "HWND",
    "HDC",
    "HINSTANCE",
    "screensave_saver_environment",
    "screensave_saver_session",
    "screensave/saver_api.h",
    "registry",
    "unsigned long",
    " long ",
    "DWORD",
]

C89_FORBIDDEN = [
    "//",
    "_Static_assert",
    "static_assert",
    "stdint.h",
    "stdbool.h",
    "inline",
    "restrict",
]

DIRECT_SPEC_FIELDS = [
    "schema_version",
    "field_family",
    "motion_kind",
    "output_kind",
    "treatment_kind",
    "scale",
    "complexity",
    "speed",
    "warp",
    "feedback",
    "contrast",
    "brightness",
    "softness",
    "seed",
    "deterministic",
    "material_key",
    "profile_key",
]

DIRECT_REQUIRED_TOKENS = [
    "PLASMA_V2_INSTRUMENT_SCHEMA_ID",
    "PLASMA_V2_INSTRUMENT_SCHEMA_VERSION",
    "PLASMA_V2_CONTROL_MIN",
    "PLASMA_V2_CONTROL_MAX",
    "PLASMA_V2_FIELD_CLASSIC",
    "PLASMA_V2_FIELD_INTERFERENCE",
    "PLASMA_V2_FIELD_FIRE",
    "plasma_v2_spec_set_defaults",
    "plasma_v2_spec_clamp",
    "plasma_v2_spec_is_valid",
    "plasma_v2_spec_apply_control",
    "plasma_v2_spec_from_legacy_config_view",
    "plasma_v2_field_family_from_token",
    "plasma_v2_treatment_kind_token",
]

SMOKE_SOURCE = r'''
#include "plasma_migration_v2.h"

int main(void)
{
    plasma_spec_v2 spec;
    ss_u32 status;

    plasma_spec_v2_set_defaults(&spec);
    if (plasma_spec_v2_is_valid(&spec) != SS_V2_TRUE) {
        return 1;
    }
    if (plasma_spec_v2_schema_id()[0] == '\0') {
        return 2;
    }
    status = plasma_migration_v2_apply_u09("ember_lava", 0, &spec);
    if (status != SS_V2_STATUS_OK) {
        return 3;
    }
    if (spec.material_id != PLASMA_V2_MATERIAL_PLASMA_LAVA) {
        return 4;
    }
    status = plasma_migration_v2_apply_u09("museum_phosphor", "museum_phosphor", &spec);
    if (status != SS_V2_STATUS_OK) {
        return 5;
    }
    if (spec.treatment_flags != PLASMA_V2_TREATMENT_RESTRAINED_CRT) {
        return 6;
    }
    if (plasma_migration_v2_apply_u09("unsupported", 0, &spec) != SS_V2_STATUS_UNSUPPORTED) {
        return 7;
    }

    return 0;
}
'''


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def read_text(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def scan_files(errors: list[str]) -> None:
    for path in REQUIRED_FILES:
        require(path.exists(), f"Missing Plasma v2 spec path: {repo_path(path)}", errors)
    if errors:
        return

    contract_text = read_text(CONTRACT)
    for token in REQUIRED_TOKENS:
        require(token in contract_text, f"Plasma v2 contract missing {token!r}", errors)

    header_text = read_text(SPEC_HEADER)
    for needle in [
        "typedef struct plasma_spec_v2_tag",
        "ss_u32 struct_size;",
        "ss_u32 schema_version;",
        "PLASMA_SPEC_V2_SCHEMA_ID",
        "PLASMA_SPEC_V2_SCHEMA_VERSION",
        "PLASMA_V2_BASIC_CONTROL_FIELD_FAMILY",
        "plasma_spec_v2_apply_basic_control",
        "PLASMA_V2_PRESENTATION_FLAT",
    ]:
        require(needle in header_text, f"plasma_spec_v2.h missing {needle!r}", errors)

    for path in [SPEC_HEADER, SPEC_SOURCE, MIGRATION_HEADER, MIGRATION_SOURCE]:
        text = read_text(path)
        relative = repo_path(path)
        for term in FORBIDDEN_TERMS:
            require(term not in text, f"{relative} contains forbidden product-spec term {term!r}", errors)
        for term in C89_FORBIDDEN:
            require(term not in text, f"{relative} contains non-C89 token {term!r}", errors)
        require("screensave/v2/base.h" in header_text, "plasma_spec_v2.h must use portable v2 fixed-width aliases.", errors)

    migration_text = read_text(MIGRATION_SOURCE)
    for key in STABLE_PRESETS:
        require(key in migration_text, f"U09 migration source missing stable preset {key!r}", errors)
    for key in STABLE_THEMES:
        require(key in migration_text, f"U09 migration source missing stable theme {key!r}", errors)
    require("ember_lava" in migration_text, "U09 migration must preserve ember_lava compatibility alias.", errors)
    require(
        re.search(r"PLASMA_V2_TREATMENT_RESTRAINED_CRT", migration_text) is not None,
        "U09 migration must include restrained CRT treatment mapping.",
        errors,
    )

    direct_text = "\n".join(
        [
            read_text(DIRECT_TYPES),
            read_text(DIRECT_SPEC_HEADER),
            read_text(DIRECT_SPEC_SOURCE),
        ]
    )
    for field in DIRECT_SPEC_FIELDS:
        require(
            re.search(r"\b" + re.escape(field) + r"\b", read_text(DIRECT_TYPES)) is not None,
            f"direct Plasma v2 spec missing semantic field {field!r}",
            errors,
        )
    for token in DIRECT_REQUIRED_TOKENS:
        require(token in direct_text, f"direct Plasma v2 spec missing {token!r}", errors)
    for normalized in ["scale", "complexity", "speed", "warp", "feedback", "contrast", "brightness", "softness"]:
        require(
            re.search(r"spec->" + normalized + r"\s*=\s*plasma_v2_clamp_normalized", read_text(DIRECT_SPEC_SOURCE)) is not None,
            f"direct Plasma v2 spec must clamp normalized field {normalized!r}",
            errors,
        )
    for path in [DIRECT_TYPES, DIRECT_SPEC_HEADER, DIRECT_SPEC_SOURCE]:
        text = read_text(path)
        relative = repo_path(path)
        for term in FORBIDDEN_TERMS:
            require(term not in text, f"{relative} contains forbidden direct-spec term {term!r}", errors)
        for term in C89_FORBIDDEN:
            require(term not in text, f"{relative} contains non-C89 token {term!r}", errors)
    direct_source = read_text(DIRECT_SPEC_SOURCE)
    for token in ["preset_key", "theme_key", "plasma_v2_apply_legacy_preset", "plasma_v2_apply_legacy_theme"]:
        require(token in direct_source, f"direct Plasma v2 legacy-to-spec path missing {token!r}", errors)


def c_compiler() -> str | None:
    for candidate in ["gcc", "cc", "clang"]:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def compile_and_run(errors: list[str]) -> None:
    compiler = c_compiler()
    if compiler is None:
        errors.append("No C compiler found for Plasma v2 spec C89 smoke compile.")
        return

    SMOKE_ROOT.mkdir(parents=True, exist_ok=True)
    smoke_source = SMOKE_ROOT / "plasma_spec_v2_smoke.c"
    output = SMOKE_ROOT / "plasma_spec_v2_smoke.exe"
    smoke_source.write_text(SMOKE_SOURCE.strip() + "\n", encoding="utf-8")

    command = [
        compiler,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-I",
        str(SRC_ROOT),
        str(smoke_source),
        str(SPEC_SOURCE),
        str(MIGRATION_SOURCE),
        "-o",
        str(output),
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(
            "Plasma v2 spec C89 compile failed: "
            + (result.stdout.strip() + " " + result.stderr.strip()).strip()
        )
        return

    run_result = subprocess.run([str(output)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if run_result.returncode != 0:
        errors.append(
            "Plasma v2 spec smoke program failed: "
            + (run_result.stdout.strip() + " " + run_result.stderr.strip()).strip()
        )


def main() -> int:
    errors: list[str] = []
    scan_files(errors)
    if not errors:
        compile_and_run(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 specification checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
