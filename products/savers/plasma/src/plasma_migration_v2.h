#ifndef PLASMA_MIGRATION_V2_H
#define PLASMA_MIGRATION_V2_H

#include "plasma_spec_v2.h"

typedef struct plasma_u09_preset_migration_v2_tag {
    const char *source_key;
    const char *canonical_key;
    const char *default_theme_key;
    ss_u32 field_family;
    ss_u32 scale;
    ss_u32 complexity;
    ss_u32 motion_speed;
    ss_u32 warp_amount;
    ss_u32 feedback_amount;
    ss_u32 output_style;
} plasma_u09_preset_migration_v2;

typedef struct plasma_u09_theme_migration_v2_tag {
    const char *source_key;
    const char *canonical_key;
    ss_u32 material_id;
    ss_u32 brightness;
    ss_u32 contrast;
    ss_u32 treatment_flags;
} plasma_u09_theme_migration_v2;

const plasma_u09_preset_migration_v2 *plasma_migration_v2_find_u09_preset(const char *key);
const plasma_u09_theme_migration_v2 *plasma_migration_v2_find_u09_theme(const char *key);
ss_u32 plasma_migration_v2_apply_u09(
    const char *preset_key,
    const char *theme_key,
    plasma_spec_v2 *spec
);

#endif /* PLASMA_MIGRATION_V2_H */
