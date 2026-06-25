/*
 * Minimal Workbench shell model for PAW-B.
 *
 * This is not a second runner. It binds the first Workbench workspace list to
 * the private sslab ABI and generated proof-profile registry used by CI.
 */

#include "benchlab_workbench_shell.h"

#include "../../../../catalog/generated/proof_registry.h"
#include "../../../../tools/sslab/include/screensave/sslab.h"

#include <stdlib.h>

#define BENCHLAB_WORKBENCH_WORKSPACE_COUNT 4U
#define BENCHLAB_WORKBENCH_REQUIRED_PROFILE_COUNT 2U

static const benchlab_workbench_workspace g_benchlab_workbench_workspaces[BENCHLAB_WORKBENCH_WORKSPACE_COUNT] = {
    { "catalog", "Catalog", "Read generated products and proof-profile inventory." },
    { "run", "Run", "Launch named proof profiles through sslab-owned execution." },
    { "inspect", "Inspect", "Show requested, resolved, degraded, and receipt metadata." },
    { "compare", "Compare", "Open capture and comparison evidence without rendering independently." }
};

static const char *g_benchlab_workbench_required_profiles[BENCHLAB_WORKBENCH_REQUIRED_PROFILE_COUNT] = {
    "nocturne.reference.v0",
    "ricochet.reference.v1"
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
