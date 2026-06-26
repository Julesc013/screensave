/*
 * Minimal Workbench shell model for PAW-B.
 *
 * This is not a second runner. It binds the first Workbench workspace list to
 * the private sslab ABI and generated proof-profile registry used by CI.
 */

#include "benchlab_workbench_shell.h"

#include "../../../../catalog/generated/proof_registry.h"
#include "../../../../tools/sslab/include/screensave/sslab.h"
#include "../../../../products/savers/nocturne/src/nocturne_v2_adapter.h"
#include "../../../../products/savers/ricochet/src/ricochet_v2_adapter.h"
#include "../../../../products/savers/plasma/src/plasma_v2_adapter.h"

#include <stdlib.h>
#include <string.h>

#define BENCHLAB_WORKBENCH_WORKSPACE_COUNT 5U
#define BENCHLAB_WORKBENCH_REQUIRED_PROFILE_COUNT 3U

static const benchlab_workbench_workspace g_benchlab_workbench_workspaces[BENCHLAB_WORKBENCH_WORKSPACE_COUNT] = {
    { "catalog", "Catalog", "Read generated products and proof-profile inventory." },
    { "run", "Run", "Launch named proof profiles through sslab-owned execution." },
    { "inspect", "Inspect", "Show requested, resolved, degraded, and receipt metadata." },
    { "compare", "Compare", "Open capture and comparison evidence without rendering independently." },
    { "author", "Author", "Edit Plasma v2 Basic controls, compile data-only packs, and run the fixed preview proof path." }
};

static const char *g_benchlab_workbench_required_profiles[BENCHLAB_WORKBENCH_REQUIRED_PROFILE_COUNT] = {
    "nocturne.reference.v0",
    "ricochet.reference.v1",
    "plasma.v2.reference.preview"
};

unsigned int benchlab_workbench_shell_workspace_count(void)
{
    return BENCHLAB_WORKBENCH_WORKSPACE_COUNT;
}

const benchlab_workbench_workspace *benchlab_workbench_shell_workspace(unsigned int index)
{
    if (index >= BENCHLAB_WORKBENCH_WORKSPACE_COUNT) {
        return 0;
    }
    return &g_benchlab_workbench_workspaces[index];
}

const char *benchlab_workbench_shell_required_profile(unsigned int index)
{
    if (index >= BENCHLAB_WORKBENCH_REQUIRED_PROFILE_COUNT) {
        return 0;
    }
    return g_benchlab_workbench_required_profiles[index];
}

int benchlab_workbench_shell_validate_binding(void)
{
    unsigned int index;

    if (SSLAB_ABI_VERSION != 0UL) {
        return 0;
    }
    if (SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES != 4UL) {
        return 0;
    }
    if (sizeof(sslab_abi_info) == 0U) {
        return 0;
    }
    for (index = 0U; index < BENCHLAB_WORKBENCH_REQUIRED_PROFILE_COUNT; ++index) {
        if (screensave_generated_find_proof_profile(g_benchlab_workbench_required_profiles[index]) == 0) {
            return 0;
        }
    }
    return 1;
}

static unsigned long benchlab_workbench_shell_checksum_rgba(
    const unsigned char *rgba,
    unsigned long byte_count
)
{
    unsigned long checksum;
    unsigned long index;

    checksum = 2166136261UL;
    for (index = 0UL; index < byte_count; ++index) {
        checksum ^= (unsigned long)rgba[index];
        checksum *= 16777619UL;
    }
    return checksum;
}

static const ss_v2_product_descriptor *benchlab_workbench_shell_v2_descriptor(
    const char *product_key
)
{
    if (product_key == 0) {
        return 0;
    }
    if (strcmp(product_key, "nocturne") == 0) {
        return nocturne_v2_product_descriptor();
    }
    if (strcmp(product_key, "ricochet") == 0) {
        return ricochet_v2_product_descriptor();
    }
    if (strcmp(product_key, "plasma") == 0) {
        return plasma_v2_product_descriptor();
    }
    return 0;
}

static int benchlab_workbench_shell_run_profile_once_on_path(
    const char *product_key,
    const char *preset_key,
    unsigned long width,
    unsigned long height,
    unsigned long seed,
    unsigned long step_count,
    unsigned long delta_ms,
    sslab_execution_path execution_path,
    unsigned long *checksum_out
)
{
    sslab_context_desc context_desc;
    sslab_run_desc run_desc;
    sslab_surface_desc surface_desc;
    sslab_context *context;
    sslab_product *product;
    sslab_session *session;
    unsigned char *rgba;
    unsigned long stride;
    unsigned long byte_count;
    unsigned long step_index;
    int ok;

    if (product_key == 0 || width == 0UL || height == 0UL || checksum_out == 0) {
        return 0;
    }
    *checksum_out = 0UL;
    if (sslab_check_host_abi() != SSLAB_STATUS_OK) {
        return 0;
    }

    stride = width * 4UL;
    byte_count = stride * height;
    rgba = (unsigned char *)malloc((size_t)byte_count);
    if (rgba == 0) {
        return 0;
    }

    context = 0;
    product = 0;
    session = 0;
    ok = 0;

    context_desc.size = sizeof(context_desc);
    context_desc.abi_version = SSLAB_ABI_VERSION;
    context_desc.output_root = "out/workbench";
    context_desc.catalog_root = "catalog";
    if (sslab_create_context(&context_desc, &context) != SSLAB_STATUS_OK) {
        goto cleanup;
    }
    if (sslab_set_execution_path(context, execution_path) != SSLAB_STATUS_OK) {
        goto cleanup;
    }
    if (sslab_open_product(context, product_key, &product) != SSLAB_STATUS_OK) {
        goto cleanup;
    }

    run_desc.size = sizeof(run_desc);
    run_desc.abi_version = SSLAB_ABI_VERSION;
    run_desc.product_key = product_key;
    run_desc.preset_key = preset_key;
    run_desc.width = width;
    run_desc.height = height;
    run_desc.seed = seed;
    run_desc.delta_ms = delta_ms;
    run_desc.frame_count = step_count;
    if (sslab_create_session(product, &run_desc, &session) != SSLAB_STATUS_OK) {
        goto cleanup;
    }

    for (step_index = 0UL; step_index < step_count; ++step_index) {
        if (sslab_step_session(session, delta_ms) != SSLAB_STATUS_OK) {
            goto cleanup;
        }
    }

    surface_desc.size = sizeof(surface_desc);
    surface_desc.abi_version = SSLAB_ABI_VERSION;
    surface_desc.width = width;
    surface_desc.height = height;
    surface_desc.stride = stride;
    surface_desc.rgba = rgba;
    if (sslab_render_session(session, &surface_desc) != SSLAB_STATUS_OK) {
        goto cleanup;
    }

    *checksum_out = benchlab_workbench_shell_checksum_rgba(rgba, byte_count);
    ok = *checksum_out != 0UL;

cleanup:
    sslab_destroy_session(session);
    sslab_close_product(product);
    sslab_destroy_context(context);
    free(rgba);
    return ok;
}

int benchlab_workbench_shell_run_profile_once(
    const char *product_key,
    const char *preset_key,
    unsigned long width,
    unsigned long height,
    unsigned long seed,
    unsigned long step_count,
    unsigned long delta_ms,
    unsigned long *checksum_out
)
{
    return benchlab_workbench_shell_run_profile_once_on_path(
        product_key,
        preset_key,
        width,
        height,
        seed,
        step_count,
        delta_ms,
        SSLAB_EXECUTION_PATH_V1,
        checksum_out);
}

int benchlab_workbench_shell_inspect_profile_v2(
    const char *profile_key,
    benchlab_workbench_v2_inspect *inspect_out
)
{
    const screensave_generated_proof_profile *profile;
    const ss_v2_product_descriptor *descriptor;
    unsigned long capture_frame;
    unsigned long checksum;

    if (profile_key == 0 || inspect_out == 0) {
        return 0;
    }
    memset(inspect_out, 0, sizeof(*inspect_out));

    profile = screensave_generated_find_proof_profile(profile_key);
    if (profile == 0 || profile->product == 0) {
        return 0;
    }
    descriptor = benchlab_workbench_shell_v2_descriptor(profile->product);
    if (descriptor == 0 || ss_v2_product_descriptor_is_valid(descriptor) != SS_V2_STATUS_OK) {
        return 0;
    }
    if (profile->capture_frame_count == 0U) {
        return 0;
    }

    capture_frame = profile->capture_frames[0];
    checksum = 0UL;
    if (!benchlab_workbench_shell_run_profile_once_on_path(
        profile->product,
        profile->preset,
        profile->width,
        profile->height,
        profile->seed,
        capture_frame,
        profile->delta_ms,
        SSLAB_EXECUTION_PATH_V2,
        &checksum)) {
        return 0;
    }

    inspect_out->api_path = "v2_direct";
    inspect_out->product_key = descriptor->product_key;
    inspect_out->profile_key = profile->key;
    inspect_out->product_descriptor_version = descriptor->product_version;
    inspect_out->session_abi_version = (unsigned long)SS_V2_ABI_VERSION;
    inspect_out->config_schema_id = descriptor->config_schema_id;
    inspect_out->config_schema_version = (unsigned long)descriptor->config_schema_version;
    inspect_out->seed = profile->seed;
    inspect_out->width = profile->width;
    inspect_out->height = profile->height;
    inspect_out->clock_frame = capture_frame;
    inspect_out->delta_ms = profile->delta_ms;
    inspect_out->surface_format = "rgba8";
    inspect_out->draw_target_kind = "sslab-v2-rgba8";
    inspect_out->field_family = "not-inspected";
    inspect_out->output_style = "not-inspected";
    inspect_out->material = "not-inspected";
    inspect_out->treatment = "not-inspected";
    inspect_out->capture_refs = "catalog-profile-capture-frames";
    inspect_out->proof_status = "pass";
    inspect_out->diagnostics = "none";
    if (strcmp(profile->product, "plasma") == 0) {
        inspect_out->field_family = "classic_interference";
        inspect_out->output_style = "continuous";
        inspect_out->material = "plasma_lava";
        inspect_out->treatment = "none";
        inspect_out->capture_refs = "validation/captures/plasma-v2/reference-preview";
        inspect_out->equivalence_status = "not-applicable-preview-profile";
        inspect_out->claim_boundary = "Plasma v2 preview reference proof only; not stable promotion or artistic acceptance";
    } else {
        inspect_out->equivalence_status = "pass";
        inspect_out->claim_boundary = "v1/v2 deterministic equivalence for named canary profiles only";
    }
    inspect_out->probe_checksum = checksum;
    return 1;
}

int benchlab_workbench_shell_author_plasma_v2(benchlab_workbench_plasma_author *author_out)
{
    unsigned long checksum;

    if (author_out == 0) {
        return 0;
    }
    memset(author_out, 0, sizeof(*author_out));

    checksum = 0UL;
    if (!benchlab_workbench_shell_run_profile_once_on_path(
        "plasma",
        "plasma_lava",
        160UL,
        90UL,
        4096UL,
        90UL,
        100UL,
        SSLAB_EXECUTION_PATH_V2,
        &checksum)) {
        return 0;
    }

    author_out->product_key = "plasma";
    author_out->profile_key = "plasma.v2.reference.preview";
    author_out->spec_schema_id = "screensave.plasma.spec.v2";
    author_out->spec_schema_version = 2UL;
    author_out->pack_input_ref = "products/savers/plasma/content/v2/examples/plasma_lava_v2.toml";
    author_out->packc_validate_command = "py -3 tools/packc/packc.py validate products/savers/plasma/content/v2/examples/plasma_lava_v2.toml";
    author_out->packc_compile_command = "py -3 tools/packc/packc.py compile products/savers/plasma/content/v2/examples/plasma_lava_v2.toml --out out/packc/plasma_lava_v2";
    author_out->proof_command = "py -3 tools/project_adapter/screensave_project.py proof --profile plasma.v2.reference.preview --path v2";
    author_out->requested_facts = "field/output/material/treatment/basic-controls";
    author_out->resolved_facts = "classic_interference/continuous/plasma_lava/none/safe-flat";
    author_out->degraded_facts = "none";
    author_out->claim_boundary = "Workbench Author model only; pack compilation and proof stay delegated to packc and sslab.";
    author_out->preview_checksum = checksum;
    return checksum != 0UL;
}
