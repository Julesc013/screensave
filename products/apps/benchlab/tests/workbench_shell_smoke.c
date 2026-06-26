#include "../src/benchlab_workbench_shell.h"

#include <string.h>

int main(void)
{
    const benchlab_workbench_workspace *workspace;
    benchlab_workbench_v2_inspect nocturne_inspect;
    benchlab_workbench_v2_inspect ricochet_inspect;
    unsigned long nocturne_checksum;
    unsigned long ricochet_checksum;

    if (benchlab_workbench_shell_workspace_count() != 4U) {
        return 1;
    }
    workspace = benchlab_workbench_shell_workspace(0U);
    if (workspace == 0 || strcmp(workspace->key, "catalog") != 0) {
        return 2;
    }
    workspace = benchlab_workbench_shell_workspace(3U);
    if (workspace == 0 || strcmp(workspace->key, "compare") != 0) {
        return 3;
    }
    if (benchlab_workbench_shell_required_profile(0U) == 0) {
        return 4;
    }
    if (benchlab_workbench_shell_required_profile(1U) == 0) {
        return 5;
    }
    if (!benchlab_workbench_shell_validate_binding()) {
        return 6;
    }
    if (!benchlab_workbench_shell_run_profile_once(
        "nocturne",
        "observatory_night",
        96UL,
        54UL,
        1536UL,
        8UL,
        100UL,
        &nocturne_checksum
    )) {
        return 7;
    }
    if (!benchlab_workbench_shell_run_profile_once(
        "ricochet",
        "classic_clean",
        128UL,
        72UL,
        2048UL,
        32UL,
        100UL,
        &ricochet_checksum
    )) {
        return 8;
    }
    if (nocturne_checksum == ricochet_checksum) {
        return 9;
    }
    if (!benchlab_workbench_shell_inspect_profile_v2(
        "nocturne.reference.v0",
        &nocturne_inspect
    )) {
        return 10;
    }
    if (strcmp(nocturne_inspect.api_path, "v2_direct") != 0) {
        return 11;
    }
    if (strcmp(nocturne_inspect.product_key, "nocturne") != 0) {
        return 12;
    }
    if (nocturne_inspect.session_abi_version == 0UL) {
        return 13;
    }
    if (strcmp(nocturne_inspect.equivalence_status, "pass") != 0) {
        return 14;
    }
    if (nocturne_inspect.probe_checksum == 0UL) {
        return 15;
    }
    if (!benchlab_workbench_shell_inspect_profile_v2(
        "ricochet.reference.v1",
        &ricochet_inspect
    )) {
        return 16;
    }
    if (strcmp(ricochet_inspect.product_key, "ricochet") != 0) {
        return 17;
    }
    if (ricochet_inspect.clock_frame != 0UL) {
        return 18;
    }
    if (ricochet_inspect.width != 128UL || ricochet_inspect.height != 72UL) {
        return 19;
    }
    return 0;
}
