#include "../src/benchlab_workbench_shell.h"

#include <string.h>

int main(void)
{
    const benchlab_workbench_workspace *workspace;
    benchlab_workbench_v2_inspect nocturne_inspect;
    benchlab_workbench_v2_inspect ricochet_inspect;
    benchlab_workbench_v2_inspect plasma_inspect;
    benchlab_workbench_plasma_author plasma_author;
    benchlab_workbench_plasma_compare plasma_compare;
    benchlab_workbench_plasma_profile plasma_profile;
    benchlab_workbench_plasma_review plasma_review;
    benchlab_workbench_plasma_release_readiness plasma_release;
    benchlab_workbench_plasma_instrument_inspect plasma_instrument;
    unsigned long nocturne_checksum;
    unsigned long ricochet_checksum;

    if (benchlab_workbench_shell_workspace_count() != 8U) {
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
    workspace = benchlab_workbench_shell_workspace(4U);
    if (workspace == 0 || strcmp(workspace->key, "author") != 0) {
        return 4;
    }
    workspace = benchlab_workbench_shell_workspace(5U);
    if (workspace == 0 || strcmp(workspace->key, "profile") != 0) {
        return 39;
    }
    workspace = benchlab_workbench_shell_workspace(6U);
    if (workspace == 0 || strcmp(workspace->key, "review") != 0) {
        return 40;
    }
    workspace = benchlab_workbench_shell_workspace(7U);
    if (workspace == 0 || strcmp(workspace->key, "release-readiness") != 0) {
        return 47;
    }
    if (benchlab_workbench_shell_required_profile(0U) == 0) {
        return 5;
    }
    if (benchlab_workbench_shell_required_profile(1U) == 0) {
        return 6;
    }
    if (benchlab_workbench_shell_required_profile(2U) == 0) {
        return 7;
    }
    if (!benchlab_workbench_shell_validate_binding()) {
        return 8;
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
        return 9;
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
        return 10;
    }
    if (nocturne_checksum == ricochet_checksum) {
        return 11;
    }
    if (!benchlab_workbench_shell_inspect_profile_v2(
        "nocturne.reference.v0",
        &nocturne_inspect
    )) {
        return 12;
    }
    if (strcmp(nocturne_inspect.api_path, "v2_direct") != 0) {
        return 13;
    }
    if (strcmp(nocturne_inspect.product_key, "nocturne") != 0) {
        return 14;
    }
    if (nocturne_inspect.session_abi_version == 0UL) {
        return 15;
    }
    if (strcmp(nocturne_inspect.equivalence_status, "pass") != 0) {
        return 16;
    }
    if (nocturne_inspect.probe_checksum == 0UL) {
        return 17;
    }
    if (!benchlab_workbench_shell_inspect_profile_v2(
        "ricochet.reference.v1",
        &ricochet_inspect
    )) {
        return 18;
    }
    if (strcmp(ricochet_inspect.product_key, "ricochet") != 0) {
        return 19;
    }
    if (ricochet_inspect.clock_frame != 0UL) {
        return 20;
    }
    if (ricochet_inspect.width != 128UL || ricochet_inspect.height != 72UL) {
        return 21;
    }
    if (!benchlab_workbench_shell_inspect_profile_v2(
        "plasma.v2.reference.preview",
        &plasma_inspect
    )) {
        return 22;
    }
    if (strcmp(plasma_inspect.product_key, "plasma") != 0) {
        return 23;
    }
    if (strcmp(plasma_inspect.material, "plasma_lava") != 0) {
        return 24;
    }
    if (strcmp(plasma_inspect.treatment, "none") != 0) {
        return 25;
    }
    if (strcmp(plasma_inspect.proof_status, "pass") != 0) {
        return 26;
    }
    if (plasma_inspect.probe_checksum == 0UL) {
        return 27;
    }
    if (!benchlab_workbench_shell_author_plasma_v2(&plasma_author)) {
        return 28;
    }
    if (strcmp(plasma_author.product_key, "plasma") != 0) {
        return 29;
    }
    if (strcmp(plasma_author.profile_key, "plasma.v2.reference.preview") != 0) {
        return 30;
    }
    if (strcmp(plasma_author.degraded_facts, "none") != 0) {
        return 31;
    }
    if (plasma_author.preview_checksum == 0UL) {
        return 32;
    }
    if (!benchlab_workbench_shell_compare_plasma_v2(&plasma_compare)) {
        return 33;
    }
    if (strcmp(plasma_compare.product_key, "plasma") != 0) {
        return 34;
    }
    if (strcmp(plasma_compare.reference_profile_key, "plasma.v2.reference.preview") != 0) {
        return 35;
    }
    if (strcmp(plasma_compare.candidate_profile_key, "plasma.v2.visualintent.preview") != 0) {
        return 36;
    }
    if (strcmp(plasma_compare.review_status, "pending-human-preview-review") != 0) {
        return 37;
    }
    if (plasma_compare.left_rgba_sha256 == 0 || plasma_compare.right_rgba_sha256 == 0) {
        return 38;
    }
    if (!benchlab_workbench_shell_profile_plasma_v2(&plasma_profile)) {
        return 41;
    }
    if (strcmp(plasma_profile.candidate_id, "plasma_v2_realization_gl11_candidate") != 0) {
        return 42;
    }
    if (strcmp(plasma_profile.release_readiness, "blocked") != 0) {
        return 43;
    }
    if (!benchlab_workbench_shell_review_plasma_v2(&plasma_review)) {
        return 44;
    }
    if (strcmp(plasma_review.decision_class, "accepted-for-stable-candidate") != 0) {
        return 45;
    }
    if (strcmp(plasma_review.release_readiness, "blocked") != 0) {
        return 46;
    }
    if (!benchlab_workbench_shell_release_readiness_plasma_v2(&plasma_release)) {
        return 48;
    }
    if (strcmp(plasma_release.package_stage_status, "packaging/windows/plasma-v2-preview/manifest.toml") != 0) {
        return 49;
    }
    if (strcmp(plasma_release.manager_preview_status, "preview-ready") != 0) {
        return 50;
    }
    if (strcmp(plasma_release.promotion_status, "blocked") != 0) {
        return 51;
    }
    if (!benchlab_workbench_shell_inspect_plasma_v2_instrument(&plasma_instrument)) {
        return 52;
    }
    if (strcmp(plasma_instrument.product_key, "plasma") != 0) {
        return 53;
    }
    if (strcmp(plasma_instrument.promotion_status, "release-candidate-hold") != 0) {
        return 54;
    }
    if (strcmp(plasma_instrument.gl11_optionality, "gl11_optional=true; hidden_gl11_minimum=false") != 0) {
        return 55;
    }
    if (plasma_instrument.control_influence_status == 0 || plasma_instrument.material_response == 0) {
        return 56;
    }
    if (strcmp(plasma_instrument.legacy_source_kind, "preset/theme/pack/direct controls") != 0) {
        return 57;
    }
    if (strcmp(plasma_instrument.legacy_key, "plasma_lava") != 0) {
        return 58;
    }
    if (strcmp(plasma_instrument.authority, "v2_spec_plan_runtime") != 0) {
        return 59;
    }
    if (strcmp(plasma_instrument.legacy_authority, "migration_input_only") != 0) {
        return 60;
    }
    if (plasma_instrument.migrated_spec_summary == 0 || plasma_instrument.resolved_plan_summary == 0) {
        return 61;
    }
    return 0;
}
