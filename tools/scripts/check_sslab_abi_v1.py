"""Validate the fixed-width libsslab ABI v1 tranche."""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[2]
HEADER = ROOT / "tools" / "sslab" / "include" / "screensave" / "sslab_v1.h"
SRC_V1 = ROOT / "tools" / "sslab" / "src_v1"
SSLAB_SRC = ROOT / "tools" / "sslab" / "src"
NOCTURNE_SRC = ROOT / "products" / "savers" / "nocturne" / "src"
RICOCHET_SRC = ROOT / "products" / "savers" / "ricochet" / "src"

SOURCES = [
    SRC_V1 / "context.c",
    SRC_V1 / "product.c",
    SRC_V1 / "session.c",
    SRC_V1 / "operations.c",
    SRC_V1 / "profile.c",
    SRC_V1 / "proof.c",
    SSLAB_SRC / "renderer_rgba8.c",
    NOCTURNE_SRC / "nocturne_core.c",
    NOCTURNE_SRC / "nocturne_v2_adapter.c",
    NOCTURNE_SRC / "nocturne_themes.c",
    RICOCHET_SRC / "ricochet_core.c",
    RICOCHET_SRC / "ricochet_v2_adapter.c",
    RICOCHET_SRC / "ricochet_themes.c",
    ROOT / "platform" / "src" / "v2" / "base_validate.c",
    ROOT / "platform" / "src" / "v2" / "config_view.c",
    ROOT / "platform" / "src" / "v2" / "surface_view.c",
    ROOT / "platform" / "src" / "v2" / "draw_target.c",
    ROOT / "platform" / "src" / "v2" / "session_helpers.c",
    ROOT / "platform" / "src" / "core" / "config" / "config.c",
    ROOT / "platform" / "src" / "surface" / "rgba8" / "surface_rgba8.c",
    ROOT / "platform" / "src" / "render" / "soft" / "soft_renderer.c",
]

INCLUDE_ARGS = [
    "-I",
    str(ROOT / "platform" / "include"),
    "-I",
    str(ROOT / "tools" / "sslab" / "include"),
    "-I",
    str(SSLAB_SRC),
    "-I",
    str(SRC_V1),
    "-I",
    str(NOCTURNE_SRC),
    "-I",
    str(RICOCHET_SRC),
]

SMOKE_SOURCE = r'''
#include "screensave/sslab_v1.h"

static void init_u64(ss_v2_u64_parts *value, ss_u32 low)
{
    value->struct_size = (ss_u32)sizeof(*value);
    value->abi_version = SS_V2_ABI_VERSION;
    value->low = low;
    value->high = 0U;
}

static void init_run(sslab_v1_run_desc *run, const char *product_key, ss_u32 seed)
{
    run->struct_size = (ss_u32)sizeof(*run);
    run->abi_version = SSLAB_V1_ABI_VERSION;
    run->product_key = product_key;
    run->preset_key = 0;
    run->dimensions.struct_size = (ss_u32)sizeof(run->dimensions);
    run->dimensions.abi_version = SS_V2_ABI_VERSION;
    run->dimensions.width = 96U;
    run->dimensions.height = 54U;
    run->seed.struct_size = (ss_u32)sizeof(run->seed);
    run->seed.abi_version = SS_V2_ABI_VERSION;
    run->seed.base_seed = seed;
    run->seed.stream_seed = 0U;
    run->seed.deterministic = SS_V2_TRUE;
    run->clock.struct_size = (ss_u32)sizeof(run->clock);
    run->clock.abi_version = SS_V2_ABI_VERSION;
    init_u64(&run->clock.frame_index, 0U);
    init_u64(&run->clock.elapsed_ms, 0U);
    run->clock.delta_ms = 100U;
    run->clock.fixed_step_ms = 100U;
}

static int smoke_product(sslab_v1_context *context, const char *product_key, ss_u32 seed)
{
    sslab_v1_run_desc run;
    sslab_v1_smoke_receipt receipt;

    init_run(&run, product_key, seed);
    if (sslab_v1_run_smoke_proof(context, &run, &receipt) != SSLAB_V1_STATUS_OK) {
        return 1;
    }
    if (receipt.status != SSLAB_V1_STATUS_OK || receipt.checksum == 0U) {
        return 1;
    }
    return 0;
}

int main(void)
{
    sslab_v1_abi_info info;
    sslab_v1_context_desc context_desc;
    sslab_v1_context *context;
    int failed;

    context = 0;
    failed = 0;
    if (sslab_v1_get_abi_info(&info) != SSLAB_V1_STATUS_OK) {
        return 1;
    }
    if (info.abi_version != SSLAB_V1_ABI_VERSION || info.portable_v2_abi_version != SS_V2_ABI_VERSION) {
        return 1;
    }
    context_desc.struct_size = (ss_u32)sizeof(context_desc);
    context_desc.abi_version = SSLAB_V1_ABI_VERSION;
    context_desc.output_root = "out/sslab-v1-smoke";
    context_desc.catalog_root = "catalog";
    if (sslab_v1_create_context(&context_desc, &context) != SSLAB_V1_STATUS_OK) {
        return 1;
    }
    failed |= smoke_product(context, "nocturne", 1536U);
    failed |= smoke_product(context, "ricochet", 1536U);
    sslab_v1_destroy_context(context);
    return failed != 0 ? 1 : 0;
}
'''


def require(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def validate_text(errors: list[str]) -> None:
    require(HEADER.exists(), "Missing tools/sslab/include/screensave/sslab_v1.h", errors)
    if not HEADER.exists():
        return
    text = HEADER.read_text(encoding="utf-8")
    require("screensave/v2.h" in text, "sslab_v1.h must include the portable v2 aggregate header.", errors)
    require("SSLAB_V1_ABI_VERSION" in text, "sslab_v1.h must define SSLAB_V1_ABI_VERSION.", errors)
    require("unsigned long" not in text, "sslab_v1.h must not expose unsigned long.", errors)
    require(" long" not in text and "\tlong" not in text, "sslab_v1.h must not expose long.", errors)
    require("SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES" not in text, "sslab_v1.h must not require unsigned-long width.", errors)
    for source in SOURCES:
        require(source.exists(), f"Missing ABI v1 source input: {source.relative_to(ROOT)}", errors)
        if source.exists():
            source_text = source.read_text(encoding="utf-8")
            require(
                "SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES" not in source_text,
                f"{source.relative_to(ROOT)} must not depend on ABI v0 unsigned-long width preconditions.",
                errors,
            )
            require(
                "sizeof(unsigned long)" not in source_text,
                f"{source.relative_to(ROOT)} must not require sizeof(unsigned long).",
                errors,
            )


def compile_and_run(errors: list[str]) -> None:
    if shutil.which("gcc") is None:
        errors.append("gcc must be available to compile the sslab ABI v1 smoke program.")
        return
    with tempfile.TemporaryDirectory(prefix="screensave-sslab-v1-") as temp_root:
        temp_path = pathlib.Path(temp_root)
        smoke_c = temp_path / "sslab_v1_smoke.c"
        smoke_exe = temp_path / "sslab_v1_smoke.exe"
        smoke_c.write_text(SMOKE_SOURCE, encoding="ascii")
        command = [
            "gcc",
            "-std=c89",
            "-Wall",
            "-Wextra",
            "-pedantic",
            *INCLUDE_ARGS,
            *(str(source) for source in SOURCES),
            str(smoke_c),
            "-o",
            str(smoke_exe),
        ]
        result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        if result.returncode != 0:
            errors.append("sslab ABI v1 smoke compile failed:\n" + result.stderr.strip())
            return
        run = subprocess.run([str(smoke_exe)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        if run.returncode != 0:
            errors.append("sslab ABI v1 smoke execution failed:\n" + run.stderr.strip())


def main() -> int:
    errors: list[str] = []

    validate_text(errors)
    if not errors:
        compile_and_run(errors)

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("sslab ABI v1 checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
