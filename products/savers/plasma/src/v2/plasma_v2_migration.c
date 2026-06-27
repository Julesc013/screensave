#include "plasma_v2_migration.h"

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
