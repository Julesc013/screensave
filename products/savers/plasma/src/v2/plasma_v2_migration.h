#ifndef PLASMA_V2_MIGRATION_H
#define PLASMA_V2_MIGRATION_H

#include "plasma_v2_spec.h"

#define PLASMA_V2_MIGRATION_SCHEMA_VERSION ((ss_u32)1U)

const char *plasma_v2_migration_rule_for_legacy_key(const char *legacy_key);
ss_u32 plasma_v2_migration_from_legacy_config(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_spec *spec_out
);

#endif /* PLASMA_V2_MIGRATION_H */
