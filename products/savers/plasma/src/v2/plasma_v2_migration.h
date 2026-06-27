#ifndef PLASMA_V2_MIGRATION_H
#define PLASMA_V2_MIGRATION_H

#include "plasma_v2_spec.h"

#define PLASMA_V2_MIGRATION_SCHEMA_VERSION ((ss_u32)1U)
#define PLASMA_V2_MIGRATION_FIXTURE_COUNT ((ss_u32)5U)

typedef struct plasma_v2_migration_fixture_tag {
    const char *legacy_preset_key;
    const char *legacy_theme_key;
    ss_i32 effect_mode;
    ss_i32 speed_mode;
    ss_i32 resolution_mode;
    ss_i32 smoothing_mode;
    ss_u32 seed;
    ss_u32 deterministic;
    const char *expected_field_family;
    const char *expected_material_key;
    ss_u32 expected_speed;
    ss_u32 expected_scale;
    ss_u32 expected_complexity;
    ss_u32 expected_warp;
    ss_u32 expected_feedback;
    const char *expected_treatment;
    const char *expected_output_kind;
    const char *expected_presentation;
    const char *expected_seed_policy;
} plasma_v2_migration_fixture;

const char *plasma_v2_migration_rule_for_legacy_key(const char *legacy_key);
const plasma_v2_migration_fixture *plasma_v2_migration_fixtures(ss_u32 *count_out);
ss_u32 plasma_v2_migration_from_legacy_config(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_spec *spec_out
);

#endif /* PLASMA_V2_MIGRATION_H */
