"""Prove Plasma v2 Basic-control output influence where claimed."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import shutil
import subprocess
import sys


ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "products" / "savers" / "plasma" / "src"
V2_SRC_ROOT = ROOT / "platform" / "src" / "v2"
CAPTURE_ROOT = ROOT / "validation" / "captures" / "plasma-v2" / "influence"
SUMMARY_PATH = CAPTURE_ROOT / "control-influence.json"
SMOKE_ROOT = ROOT / "out" / "checks" / "plasma-v2-influence"

MATERIAL_CONTROLS = [
    ("field_family", "field_family_radial_warped"),
    ("scale", "scale_070"),
    ("complexity", "complexity_088"),
    ("motion_speed", "motion_speed_085"),
    ("warp_amount", "warp_amount_060"),
    ("feedback_amount", "feedback_amount_045"),
    ("output_style", "output_style_contour"),
    ("material", "material_aurora_cool"),
    ("brightness", "brightness_070"),
    ("contrast", "contrast_060"),
    ("treatment", "treatment_restrained_crt"),
]

METADATA_CONTROLS = [
    ("seed_policy", "metadata-only: host seed selection policy is recorded in the spec but applied before session creation"),
    ("quality_intent", "metadata-only: PAW-DX admits only safe quality intent"),
]

REQUIRED_SOURCES = [
    SRC_ROOT / "plasma_spec_v2.c",
    SRC_ROOT / "plasma_migration_v2.c",
    SRC_ROOT / "plasma_v2_core.c",
    SRC_ROOT / "plasma_v2_adapter.c",
    V2_SRC_ROOT / "base_validate.c",
    V2_SRC_ROOT / "config_view.c",
    V2_SRC_ROOT / "surface_view.c",
    V2_SRC_ROOT / "draw_target.c",
    V2_SRC_ROOT / "session_helpers.c",
]

CAPTURE_SOURCE = r'''
#include <stdio.h>
#include <string.h>

#include "plasma_v2_adapter.h"
#include "plasma_spec_v2.h"

static void fill_size(ss_v2_size *size, ss_u32 width, ss_u32 height)
{
    size->struct_size = (ss_u32)sizeof(*size);
    size->abi_version = SS_V2_ABI_VERSION;
    size->width = width;
    size->height = height;
}

static void fill_parts(ss_v2_u64_parts *parts, ss_u32 low)
{
    parts->struct_size = (ss_u32)sizeof(*parts);
    parts->abi_version = SS_V2_ABI_VERSION;
    parts->low = low;
    parts->high = 0U;
}

static void fill_clock(ss_v2_clock *clock, ss_u32 frame, ss_u32 delta_ms)
{
    clock->struct_size = (ss_u32)sizeof(*clock);
    clock->abi_version = SS_V2_ABI_VERSION;
    fill_parts(&clock->frame_index, frame);
    fill_parts(&clock->elapsed_ms, frame * delta_ms);
    clock->delta_ms = delta_ms;
    clock->fixed_step_ms = delta_ms;
}

static ss_u32 apply_variant(plasma_spec_v2 *spec, const char *variant)
{
    if (strcmp(variant, "baseline") == 0) {
        return SS_V2_STATUS_OK;
    }
    if (strcmp(variant, "field_family_radial_warped") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_FIELD_FAMILY, PLASMA_V2_FIELD_RADIAL_WARPED);
    }
    if (strcmp(variant, "scale_070") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_SCALE, 70U);
    }
    if (strcmp(variant, "complexity_088") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_COMPLEXITY, 88U);
    }
    if (strcmp(variant, "motion_speed_085") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MOTION_SPEED, 85U);
    }
    if (strcmp(variant, "warp_amount_060") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_WARP_AMOUNT, 60U);
    }
    if (strcmp(variant, "feedback_amount_045") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_FEEDBACK_AMOUNT, 45U);
    }
    if (strcmp(variant, "output_style_contour") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_OUTPUT_STYLE, PLASMA_V2_OUTPUT_CONTOUR);
    }
    if (strcmp(variant, "material_aurora_cool") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MATERIAL, PLASMA_V2_MATERIAL_AURORA_COOL);
    }
    if (strcmp(variant, "brightness_070") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_BRIGHTNESS, 70U);
    }
    if (strcmp(variant, "contrast_060") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_CONTRAST, 60U);
    }
    if (strcmp(variant, "treatment_restrained_crt") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_TREATMENT, PLASMA_V2_TREATMENT_RESTRAINED_CRT);
    }
    return SS_V2_STATUS_UNSUPPORTED;
}

static int render_variant(const char *path, const char *variant)
{
    const ss_v2_product_descriptor *product;
    ss_v2_session *session;
    ss_v2_session_desc session_desc;
    ss_v2_advance_desc advance_desc;
    ss_v2_render_desc render_desc;
    ss_v2_surface_desc surface;
    ss_v2_config_view config;
    ss_v2_seed seed;
    ss_v2_clock clock;
    ss_v2_size size;
    plasma_spec_v2 spec;
    ss_u8 pixels[32U * 18U * 4U];
    ss_u32 index;
    FILE *handle;

    plasma_spec_v2_set_defaults(&spec);
    if (apply_variant(&spec, variant) != SS_V2_STATUS_OK) {
        return 10;
    }
    product = plasma_v2_product_descriptor();
    if (ss_v2_product_descriptor_is_valid(product) != SS_V2_STATUS_OK) {
        return 11;
    }

    fill_size(&size, 32U, 18U);
    fill_clock(&clock, 0U, 100U);
    seed.struct_size = (ss_u32)sizeof(seed);
    seed.abi_version = SS_V2_ABI_VERSION;
    seed.base_seed = 4096U;
    seed.stream_seed = 17U;
    seed.deterministic = SS_V2_TRUE;

    config.struct_size = (ss_u32)sizeof(config);
    config.abi_version = SS_V2_ABI_VERSION;
    config.product_schema_id = PLASMA_SPEC_V2_SCHEMA_ID;
    config.schema_version = PLASMA_SPEC_V2_SCHEMA_VERSION;
    config.bytes = &spec;
    config.byte_count = (ss_u32)sizeof(spec);

    session_desc.struct_size = (ss_u32)sizeof(session_desc);
    session_desc.abi_version = SS_V2_ABI_VERSION;
    session_desc.mode = SS_V2_SESSION_MODE_PROOF;
    session_desc.dimensions = size;
    session_desc.seed = seed;
    session_desc.clock = clock;
    session_desc.product_config = config;
    session_desc.diagnostics = 0;
    session = 0;
    if (product->session_ops->create(&session_desc, &session) != SS_V2_STATUS_OK || session == 0) {
        return 12;
    }

    advance_desc.struct_size = (ss_u32)sizeof(advance_desc);
    advance_desc.abi_version = SS_V2_ABI_VERSION;
    fill_clock(&advance_desc.clock, 1U, 100U);
    advance_desc.diagnostics = 0;
    if (product->session_ops->advance(session, &advance_desc) != SS_V2_STATUS_OK) {
        product->session_ops->destroy(session);
        return 13;
    }
    if (product->session_ops->advance(session, &advance_desc) != SS_V2_STATUS_OK) {
        product->session_ops->destroy(session);
        return 14;
    }

    for (index = 0U; index < (ss_u32)sizeof(pixels); ++index) {
        pixels[index] = 0U;
    }
    surface.struct_size = (ss_u32)sizeof(surface);
    surface.abi_version = SS_V2_ABI_VERSION;
    surface.width = 32U;
    surface.height = 18U;
    surface.stride_bytes = 32U * 4U;
    surface.format = SS_V2_SURFACE_FORMAT_RGBA8;
    surface.origin = SS_V2_SURFACE_ORIGIN_TOP_LEFT;
    surface.pixels = pixels;

    render_desc.struct_size = (ss_u32)sizeof(render_desc);
    render_desc.abi_version = SS_V2_ABI_VERSION;
    fill_clock(&render_desc.clock, 2U, 100U);
    render_desc.draw_target = 0;
    render_desc.surface = &surface;
    render_desc.diagnostics = 0;
    if (product->session_ops->render(session, &render_desc) != SS_V2_STATUS_OK) {
        product->session_ops->destroy(session);
        return 15;
    }
    product->session_ops->destroy(session);

    handle = fopen(path, "wb");
    if (handle == 0) {
        return 16;
    }
    if (fwrite(pixels, 1U, sizeof(pixels), handle) != sizeof(pixels)) {
        fclose(handle);
        return 17;
    }
    fclose(handle);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        return 1;
    }
    return render_variant(argv[1], argv[2]);
}
'''


def repo_path(path: pathlib.Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def c_compiler() -> str | None:
    for candidate in ["gcc", "cc", "clang"]:
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    return None


def compile_capture_tool(errors: list[str]) -> pathlib.Path | None:
    compiler = c_compiler()
    if compiler is None:
        errors.append("No C compiler found for Plasma v2 influence capture compile.")
        return None
    for source in REQUIRED_SOURCES:
        require(source.exists(), f"Missing source for influence capture: {repo_path(source)}", errors)
    if errors:
        return None

    SMOKE_ROOT.mkdir(parents=True, exist_ok=True)
    source_path = SMOKE_ROOT / "plasma_v2_influence_capture.c"
    output_path = SMOKE_ROOT / "plasma_v2_influence_capture.exe"
    source_path.write_text(CAPTURE_SOURCE.strip() + "\n", encoding="utf-8")

    command = [
        compiler,
        "-std=c89",
        "-pedantic",
        "-Werror",
        "-I",
        str(ROOT / "platform" / "include"),
        "-I",
        str(SRC_ROOT),
        str(source_path),
    ]
    command.extend(str(source) for source in REQUIRED_SOURCES)
    command.extend(["-o", str(output_path)])
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        errors.append(
            "Plasma v2 influence capture compile failed: "
            + (result.stdout.strip() + " " + result.stderr.strip()).strip()
        )
        return None
    return output_path


def render_hashes(tool_path: pathlib.Path, errors: list[str]) -> dict[str, str]:
    hashes: dict[str, str] = {}
    variants = [("baseline", "baseline")]
    variants.extend(MATERIAL_CONTROLS)
    for _control, variant in variants:
        output_path = SMOKE_ROOT / f"{variant}.rgba"
        result = subprocess.run(
            [str(tool_path), str(output_path), variant],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        if result.returncode != 0:
            errors.append(f"Influence capture failed for {variant}: returncode {result.returncode}")
            continue
        hashes[variant] = hashlib.sha256(output_path.read_bytes()).hexdigest()
    return hashes


def build_summary(hashes: dict[str, str]) -> dict:
    baseline_hash = hashes["baseline"]
    controls = []
    for control, variant in MATERIAL_CONTROLS:
        controls.append(
            {
                "control": control,
                "status": "materially_influences_output",
                "variant": variant,
                "baseline_sha256": baseline_hash,
                "variant_sha256": hashes[variant],
            }
        )
    for control, note in METADATA_CONTROLS:
        controls.append(
            {
                "control": control,
                "status": "metadata_only",
                "note": note,
            }
        )
    return {
        "schema_version": "screensave.plasma-v2.influence.v0",
        "profile": "plasma.v2.reference.preview",
        "surface": {"width": 32, "height": 18, "format": "rgba8"},
        "baseline_variant": "baseline",
        "baseline_sha256": baseline_hash,
        "controls": controls,
        "claim_boundary": "Basic-control influence for the PAW-DX Plasma v2 reference preview slice only; not artistic acceptance or release promotion.",
    }


def validate_summary(summary: dict, hashes: dict[str, str], errors: list[str]) -> None:
    require(summary.get("schema_version") == "screensave.plasma-v2.influence.v0", "Influence summary schema mismatch.", errors)
    require(summary.get("baseline_sha256") == hashes.get("baseline"), "Influence baseline hash is stale.", errors)
    seen = set()
    controls = summary.get("controls", [])
    require(isinstance(controls, list), "Influence summary controls must be a list.", errors)
    for item in controls:
        control = item.get("control")
        seen.add(control)
        if item.get("status") == "materially_influences_output":
            variant = item.get("variant")
            require(variant in hashes, f"Influence summary references unknown variant {variant!r}.", errors)
            require(item.get("variant_sha256") == hashes.get(variant), f"Influence hash for {control} is stale.", errors)
            require(
                hashes.get(variant) != hashes.get("baseline"),
                f"Control {control} did not materially change rendered output.",
                errors,
            )
        elif item.get("status") == "metadata_only":
            require("note" in item, f"Metadata-only control {control} must include a note.", errors)
        else:
            errors.append(f"Control {control} has unsupported status {item.get('status')!r}.")
    for control, _variant in MATERIAL_CONTROLS:
        require(control in seen, f"Influence summary missing material control {control}.", errors)
    for control, _note in METADATA_CONTROLS:
        require(control in seen, f"Influence summary missing metadata control {control}.", errors)
    require("artistic acceptance" in summary.get("claim_boundary", ""), "Influence claim boundary must block artistic acceptance.", errors)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Refresh committed influence summary JSON.")
    args = parser.parse_args()

    errors: list[str] = []
    tool_path = compile_capture_tool(errors)
    hashes: dict[str, str] = {}
    if tool_path is not None:
        hashes = render_hashes(tool_path, errors)
    if not errors and "baseline" not in hashes:
        errors.append("Influence capture did not produce a baseline hash.")

    if not errors and args.update:
        CAPTURE_ROOT.mkdir(parents=True, exist_ok=True)
        summary = build_summary(hashes)
        SUMMARY_PATH.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")

    if not errors:
        if not SUMMARY_PATH.exists():
            errors.append(f"Missing influence summary: {repo_path(SUMMARY_PATH)}")
        else:
            summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
            validate_summary(summary, hashes, errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 influence checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
