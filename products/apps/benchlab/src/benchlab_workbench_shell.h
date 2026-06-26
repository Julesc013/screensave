#ifndef BENCHLAB_WORKBENCH_SHELL_H
#define BENCHLAB_WORKBENCH_SHELL_H

typedef struct benchlab_workbench_workspace_tag {
    const char *key;
    const char *label;
    const char *purpose;
} benchlab_workbench_workspace;

typedef struct benchlab_workbench_v2_inspect_tag {
    const char *api_path;
    const char *product_key;
    const char *profile_key;
    const char *product_descriptor_version;
    unsigned long session_abi_version;
    const char *config_schema_id;
    unsigned long config_schema_version;
    unsigned long seed;
    unsigned long width;
    unsigned long height;
    unsigned long clock_frame;
    unsigned long delta_ms;
    const char *surface_format;
    const char *draw_target_kind;
    const char *diagnostics;
    const char *equivalence_status;
    const char *claim_boundary;
    unsigned long probe_checksum;
} benchlab_workbench_v2_inspect;

unsigned int benchlab_workbench_shell_workspace_count(void);
const benchlab_workbench_workspace *benchlab_workbench_shell_workspace(unsigned int index);
int benchlab_workbench_shell_validate_binding(void);
const char *benchlab_workbench_shell_required_profile(unsigned int index);
int benchlab_workbench_shell_run_profile_once(
    const char *product_key,
    const char *preset_key,
    unsigned long width,
    unsigned long height,
    unsigned long seed,
    unsigned long step_count,
    unsigned long delta_ms,
    unsigned long *checksum_out
);
int benchlab_workbench_shell_inspect_profile_v2(
    const char *profile_key,
    benchlab_workbench_v2_inspect *inspect_out
);

#endif
