#ifndef PLASMA_V2_CONFIG_H
#define PLASMA_V2_CONFIG_H

#include "plasma_v2_migration.h"

#define PLASMA_V2_CONFIG_SCHEMA_VERSION ((ss_u32)1U)
#define PLASMA_V2_CONFIG_SOURCE_LENGTH ((ss_u32)32U)

typedef struct plasma_v2_config_tag {
    ss_u32 struct_size;
    ss_u32 schema_version;
    plasma_v2_spec spec;
    char source_kind[PLASMA_V2_CONFIG_SOURCE_LENGTH];
} plasma_v2_config;

void plasma_v2_config_set_defaults(plasma_v2_config *config);
ss_u32 plasma_v2_config_from_legacy(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_config *config_out
);
ss_u32 plasma_v2_config_is_valid(const plasma_v2_config *config);

#endif /* PLASMA_V2_CONFIG_H */
