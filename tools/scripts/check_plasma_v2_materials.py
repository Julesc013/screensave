"""Prove Plasma v2 material distinctness and restrained treatment bounds."""

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
CAPTURE_ROOT = ROOT / "validation" / "captures" / "plasma-v2" / "materials"
SUMMARY_PATH = CAPTURE_ROOT / "material-treatment-summary.json"
SMOKE_ROOT = ROOT / "out" / "checks" / "plasma-v2-materials"

MATERIAL_VARIANTS = [
    ("plasma_lava", "material_plasma_lava"),
    ("aurora_cool", "material_aurora_cool"),
    ("oceanic_blue", "material_oceanic_blue"),
    ("museum_phosphor", "material_museum_phosphor"),
    ("quiet_darkroom", "material_quiet_darkroom"),
]

TREATMENT_VARIANTS = [
    ("none", "treatment_none"),
    ("restrained_dither", "treatment_restrained_dither"),
    ("restrained_crt", "treatment_restrained_crt"),
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
    if (strcmp(variant, "material_plasma_lava") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MATERIAL, PLASMA_V2_MATERIAL_PLASMA_LAVA);
    }
    if (strcmp(variant, "material_aurora_cool") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MATERIAL, PLASMA_V2_MATERIAL_AURORA_COOL);
    }
    if (strcmp(variant, "material_oceanic_blue") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MATERIAL, PLASMA_V2_MATERIAL_OCEANIC_BLUE);
    }
    if (strcmp(variant, "material_museum_phosphor") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MATERIAL, PLASMA_V2_MATERIAL_MUSEUM_PHOSPHOR);
    }
    if (strcmp(variant, "material_quiet_darkroom") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_MATERIAL, PLASMA_V2_MATERIAL_QUIET_DARKROOM);
    }
    if (strcmp(variant, "treatment_none") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_TREATMENT, PLASMA_V2_TREATMENT_NONE);
    }
    if (strcmp(variant, "treatment_restrained_dither") == 0) {
        return plasma_spec_v2_apply_basic_control(spec, PLASMA_V2_BASIC_CONTROL_TREATMENT, PLASMA_V2_TREATMENT_RESTRAINED_DITHER);
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
    ss_u8 pixels[40U * 24U * 4U];
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

    fill_size(&size, 40U, 24U);
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
    surface.width = 40U;
    surface.height = 24U;
    surface.stride_bytes = 40U * 4U;
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
        errors.append("No C compiler found for Plasma v2 material capture compile.")
        return None
    for source in REQUIRED_SOURCES:
        require(source.exists(), f"Missing source for material capture: {repo_path(source)}", errors)
    if errors:
        return None

    SMOKE_ROOT.mkdir(parents=True, exist_ok=True)
    source_path = SMOKE_ROOT / "plasma_v2_material_capture.c"
    output_path = SMOKE_ROOT / "plasma_v2_material_capture.exe"
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
            "Plasma v2 material capture compile failed: "
            + (result.stdout.strip() + " " + result.stderr.strip()).strip()
        )
        return None
    return output_path


def luma(red: int, green: int, blue: int) -> int:
    return (red * 299 + green * 587 + blue * 114) // 1000


def sample_stats(path: pathlib.Path) -> dict[str, object]:
    data = path.read_bytes()
    count = len(data) // 4
    values: list[int] = []
    for index in range(count):
        offset = index * 4
        values.append(luma(data[offset], data[offset + 1], data[offset + 2]))
    mean = sum(values) / float(len(values))
    return {
        "sha256": hashlib.sha256(data).hexdigest(),
        "min_luminance": min(values),
        "max_luminance": max(values),
        "mean_luminance": round(mean, 3),
    }


def mean_absolute_rgb_delta(left: pathlib.Path, right: pathlib.Path) -> float:
    left_bytes = left.read_bytes()
    right_bytes = right.read_bytes()
    total = 0
    count = 0
    for index in range(0, min(len(left_bytes), len(right_bytes)), 4):
        total += abs(left_bytes[index] - right_bytes[index])
        total += abs(left_bytes[index + 1] - right_bytes[index + 1])
        total += abs(left_bytes[index + 2] - right_bytes[index + 2])
        count += 3
    return round(total / float(count), 3)


def render_variants(tool_path: pathlib.Path, errors: list[str]) -> dict[str, pathlib.Path]:
    outputs: dict[str, pathlib.Path] = {}
    variants = MATERIAL_VARIANTS + TREATMENT_VARIANTS
    for _name, variant in variants:
        output_path = SMOKE_ROOT / f"{variant}.rgba"
        result = subprocess.run(
            [str(tool_path), str(output_path), variant],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        if result.returncode != 0:
            errors.append(f"Material capture failed for {variant}: returncode {result.returncode}")
            continue
        outputs[variant] = output_path
    return outputs


def build_summary(outputs: dict[str, pathlib.Path]) -> dict[str, object]:
    material_records = []
    treatment_records = []
    for name, variant in MATERIAL_VARIANTS:
        stats = sample_stats(outputs[variant])
        material_records.append(
            {
                "name": name,
                "variant": variant,
                "sha256": stats["sha256"],
                "min_luminance": stats["min_luminance"],
                "max_luminance": stats["max_luminance"],
                "mean_luminance": stats["mean_luminance"],
            }
        )

    baseline = outputs["treatment_none"]
    for name, variant in TREATMENT_VARIANTS:
        stats = sample_stats(outputs[variant])
        treatment_records.append(
            {
                "name": name,
                "variant": variant,
                "sha256": stats["sha256"],
                "min_luminance": stats["min_luminance"],
                "max_luminance": stats["max_luminance"],
                "mean_luminance": stats["mean_luminance"],
                "mean_abs_rgb_delta_from_none": mean_absolute_rgb_delta(baseline, outputs[variant]),
            }
        )

    return {
        "schema_version": "screensave.plasma-v2.materials.v0",
        "profile": "plasma.v2.reference.preview",
        "surface": {"width": 40, "height": 24, "format": "rgba8"},
        "materials": material_records,
        "treatments": treatment_records,
        "declared_luminance_bounds": {
            "all_materials": {"min_mean": 20.0, "max_mean": 180.0},
            "quiet_darkroom": {"max_mean": 85.0},
        },
        "treatment_bounds": {
            "restrained_dither": {"max_mean_abs_rgb_delta_from_none": 8.0},
            "restrained_crt": {"max_mean_abs_rgb_delta_from_none": 24.0},
        },
        "claim_boundary": "Material and restrained treatment proof for the PAW-DX Plasma v2 reference preview slice only; not a presentation-family expansion, artistic acceptance, or release promotion.",
    }


def validate_summary(summary: dict[str, object], outputs: dict[str, pathlib.Path], errors: list[str]) -> None:
    require(summary.get("schema_version") == "screensave.plasma-v2.materials.v0", "Material summary schema mismatch.", errors)
    require("artistic acceptance" in str(summary.get("claim_boundary", "")), "Material claim boundary must block artistic acceptance.", errors)
    require("release promotion" in str(summary.get("claim_boundary", "")), "Material claim boundary must block release promotion.", errors)
    require("presentation-family expansion" in str(summary.get("claim_boundary", "")), "Material claim boundary must block presentation-family expansion.", errors)

    material_by_name = {item.get("name"): item for item in summary.get("materials", []) if isinstance(item, dict)}
    treatment_by_name = {item.get("name"): item for item in summary.get("treatments", []) if isinstance(item, dict)}
    material_hashes = set()
    for name, variant in MATERIAL_VARIANTS:
        require(name in material_by_name, f"Missing material proof record for {name}.", errors)
        record = material_by_name.get(name, {})
        fresh = sample_stats(outputs[variant])
        require(record.get("sha256") == fresh["sha256"], f"Material hash for {name} is stale.", errors)
        require(record.get("min_luminance") == fresh["min_luminance"], f"Material min luminance for {name} is stale.", errors)
        require(record.get("max_luminance") == fresh["max_luminance"], f"Material max luminance for {name} is stale.", errors)
        require(record.get("mean_luminance") == fresh["mean_luminance"], f"Material mean luminance for {name} is stale.", errors)
        material_hashes.add(record.get("sha256"))
        mean_luminance = float(record.get("mean_luminance", -1.0))
        require(20.0 <= mean_luminance <= 180.0, f"Material {name} mean luminance is outside declared bounds.", errors)
    require(len(material_hashes) == len(MATERIAL_VARIANTS), "Each Plasma v2 material must produce distinct output.", errors)
    quiet = material_by_name.get("quiet_darkroom", {})
    require(float(quiet.get("mean_luminance", 999.0)) <= 85.0, "quiet_darkroom must remain lower-band honest.", errors)

    baseline = outputs["treatment_none"]
    for name, variant in TREATMENT_VARIANTS:
        require(name in treatment_by_name, f"Missing treatment proof record for {name}.", errors)
        record = treatment_by_name.get(name, {})
        fresh = sample_stats(outputs[variant])
        fresh_delta = mean_absolute_rgb_delta(baseline, outputs[variant])
        require(record.get("sha256") == fresh["sha256"], f"Treatment hash for {name} is stale.", errors)
        require(record.get("mean_abs_rgb_delta_from_none") == fresh_delta, f"Treatment delta for {name} is stale.", errors)
    require(treatment_by_name.get("none", {}).get("mean_abs_rgb_delta_from_none") == 0.0, "Treatment none must be the baseline.", errors)
    require(
        0.0 < float(treatment_by_name.get("restrained_dither", {}).get("mean_abs_rgb_delta_from_none", 999.0)) <= 8.0,
        "restrained_dither must be visible but bounded.",
        errors,
    )
    require(
        0.0 < float(treatment_by_name.get("restrained_crt", {}).get("mean_abs_rgb_delta_from_none", 999.0)) <= 24.0,
        "restrained_crt must be visible but bounded.",
        errors,
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--update", action="store_true", help="Refresh committed material/treatment summary JSON.")
    args = parser.parse_args()

    errors: list[str] = []
    tool_path = compile_capture_tool(errors)
    outputs: dict[str, pathlib.Path] = {}
    if tool_path is not None:
        outputs = render_variants(tool_path, errors)
    for _name, variant in MATERIAL_VARIANTS + TREATMENT_VARIANTS:
        if variant not in outputs:
            errors.append(f"Material checker did not produce {variant}.")

    if not errors and args.update:
        CAPTURE_ROOT.mkdir(parents=True, exist_ok=True)
        summary = build_summary(outputs)
        SUMMARY_PATH.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8")

    if not errors:
        if not SUMMARY_PATH.exists():
            errors.append(f"Missing material summary: {repo_path(SUMMARY_PATH)}")
        else:
            summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
            validate_summary(summary, outputs, errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("Plasma v2 material checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
