#include "plasma_v2_migration.h"

static const plasma_v2_migration_fixture g_plasma_v2_migration_fixtures[] = {
    {
        "plasma_lava",
        "plasma_lava",
        (ss_i32)1,
        (ss_i32)1,
        (ss_i32)1,
        (ss_i32)1,
        (ss_u32)104729U,
        SS_V2_TRUE,
        "classic",
        "plasma_lava",
        (ss_u32)420U,
        (ss_u32)420U,
        (ss_u32)520U,
        (ss_u32)180U,
        (ss_u32)120U,
        "soft",
        "continuous",
        "flat",
        "fixed"
    },
    {
        "aurora_plasma",
        "aurora_cool",
        (ss_i32)6,
        (ss_i32)2,
        (ss_i32)2,
        (ss_i32)1,
        (ss_u32)271828U,
        SS_V2_TRUE,
        "aurora",
        "aurora_cool",
        (ss_u32)700U,
        (ss_u32)560U,
        (ss_u32)600U,
        (ss_u32)320U,
        (ss_u32)120U,
        "soft",
        "continuous",
        "flat",
        "fixed"
    },
    {
        "ocean_interference",
        "oceanic_blue",
        (ss_i32)2,
        (ss_i32)2,
        (ss_i32)2,
        (ss_i32)0,
        (ss_u32)314159U,
        SS_V2_TRUE,
        "interference",
        "oceanic_blue",
        (ss_u32)700U,
        (ss_u32)360U,
        (ss_u32)700U,
        (ss_u32)280U,
        (ss_u32)120U,
        "none",
        "continuous",
        "flat",
        "fixed"
    },
    {
        "museum_phosphor",
        "museum_phosphor",
        (ss_i32)2,
        (ss_i32)0,
        (ss_i32)1,
        (ss_i32)2,
        (ss_u32)161803U,
        SS_V2_TRUE,
        "interference",
        "museum_phosphor",
        (ss_u32)220U,
        (ss_u32)420U,
        (ss_u32)520U,
        (ss_u32)260U,
        (ss_u32)120U,
        "phosphor",
        "continuous",
        "flat",
        "fixed"
    },
    {
        "quiet_darkroom",
        "quiet_darkroom",
        (ss_i32)1,
        (ss_i32)0,
        (ss_i32)0,
        (ss_i32)0,
        (ss_u32)424242U,
        SS_V2_TRUE,
        "fire",
        "quiet_darkroom",
        (ss_u32)180U,
        (ss_u32)680U,
        (ss_u32)360U,
        (ss_u32)140U,
        (ss_u32)180U,
        "none",
        "continuous",
        "flat",
        "fixed"
    }
};

static ss_u32 plasma_v2_migration_token_equals(const char *left, const char *right)
{
    if (left == 0 || right == 0) {
        return SS_V2_FALSE;
    }
    while (*left != '\0' && *right != '\0') {
        if (*left != *right) {
            return SS_V2_FALSE;
        }
        ++left;
        ++right;
    }
    return *left == *right ? SS_V2_TRUE : SS_V2_FALSE;
}

const plasma_v2_migration_fixture *plasma_v2_migration_fixtures(ss_u32 *count_out)
{
    if (count_out != 0) {
        *count_out = (ss_u32)(sizeof(g_plasma_v2_migration_fixtures) / sizeof(g_plasma_v2_migration_fixtures[0]));
    }
    return g_plasma_v2_migration_fixtures;
}

const char *plasma_v2_migration_rule_for_legacy_key(const char *legacy_key)
{
    if (plasma_v2_migration_token_equals(legacy_key, "effect_mode") == SS_V2_TRUE) {
        return "field_family";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "speed_mode") == SS_V2_TRUE) {
        return "speed";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "resolution_mode") == SS_V2_TRUE) {
        return "scale_complexity_softness";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "smoothing_mode") == SS_V2_TRUE) {
        return "softness_or_treatment";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "preset_key") == SS_V2_TRUE) {
        return "starter_plasma_v2_spec";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "theme_key") == SS_V2_TRUE) {
        return "material_key";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "output_mode") == SS_V2_TRUE) {
        return "output_kind";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "presentation_mode") == SS_V2_TRUE) {
        return "presentation_flat_for_stable";
    }
    if (plasma_v2_migration_token_equals(legacy_key, "deterministic_seed") == SS_V2_TRUE) {
        return "seed";
    }
    return "unsupported_and_hidden";
}

ss_u32 plasma_v2_migration_from_legacy_config(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_spec *spec_out
)
{
    ss_u32 status;

    status = plasma_v2_spec_from_legacy_config_view(legacy, spec_out);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    plasma_v2_spec_clamp(spec_out);
    return plasma_v2_spec_is_valid(spec_out) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_FAIL;
}
