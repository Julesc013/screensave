/*
 * Minimal Workbench shell model for PAW-B.
 *
 * This is not a second runner. It binds the first Workbench workspace list to
 * the private sslab ABI and generated proof-profile registry used by CI.
 */

#include "benchlab_workbench_shell.h"

#include "../../../../catalog/generated/proof_registry.h"
#include "../../../../tools/sslab/include/screensave/sslab.h"

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
