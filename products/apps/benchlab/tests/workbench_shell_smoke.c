#include "../src/benchlab_workbench_shell.h"

#include <string.h>

int main(void)
{
    const benchlab_workbench_workspace *workspace;

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
    return 0;
}
