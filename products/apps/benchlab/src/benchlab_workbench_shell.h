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
    const char *field_family;
    const char *output_style;
    const char *material;
    const char *treatment;
    const char *capture_refs;
    const char *proof_status;
    const char *diagnostics;
    const char *equivalence_status;
    const char *claim_boundary;
    unsigned long probe_checksum;
} benchlab_workbench_v2_inspect;

typedef struct benchlab_workbench_plasma_author_tag {
    const char *product_key;
    const char *profile_key;
    const char *spec_schema_id;
    unsigned long spec_schema_version;
    const char *pack_input_ref;
    const char *packc_validate_command;
    const char *packc_compile_command;
    const char *proof_command;
    const char *requested_facts;
    const char *resolved_facts;
    const char *degraded_facts;
    const char *claim_boundary;
    unsigned long preview_checksum;
} benchlab_workbench_plasma_author;

typedef struct benchlab_workbench_plasma_compare_tag {
    const char *product_key;
    const char *reference_profile_key;
    const char *candidate_profile_key;
    const char *left_capture_ref;
    const char *right_capture_ref;
    const char *left_rgba_sha256;
    const char *right_rgba_sha256;
    const char *diff_ref;
    const char *contact_sheet_ref;
    const char *proof_bundle_ref;
    const char *review_status;
    const char *candidate_ranking_notes;
    const char *claim_boundary;
} benchlab_workbench_plasma_compare;

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
int benchlab_workbench_shell_author_plasma_v2(benchlab_workbench_plasma_author *author_out);
int benchlab_workbench_shell_compare_plasma_v2(benchlab_workbench_plasma_compare *compare_out);

#endif
