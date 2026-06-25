#include "../src/benchlab_workbench_shell.h"

#include <string.h>

int main(void)
{
    const benchlab_workbench_workspace *workspace;
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
    return 0;
}
