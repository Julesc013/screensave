#ifndef BENCHLAB_WORKBENCH_SHELL_H
#define BENCHLAB_WORKBENCH_SHELL_H

typedef struct benchlab_workbench_workspace_tag {
    const char *key;
    const char *label;
    const char *purpose;
} benchlab_workbench_workspace;

unsigned int benchlab_workbench_shell_workspace_count(void);
const benchlab_workbench_workspace *benchlab_workbench_shell_workspace(unsigned int index);
int benchlab_workbench_shell_validate_binding(void);
const char *benchlab_workbench_shell_required_profile(unsigned int index);

#endif
