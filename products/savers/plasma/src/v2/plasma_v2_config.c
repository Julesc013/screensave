#include "plasma_v2_config.h"

static void plasma_v2_config_copy_source(char *dst, ss_u32 dst_size, const char *src)
{
    ss_u32 index;

    if (dst == 0 || dst_size == 0U) {
        return;
    }
    dst[0] = '\0';
    if (src == 0) {
        return;
    }
    index = 0U;
    while (index + 1U < dst_size && src[index] != '\0') {
        dst[index] = src[index];
        ++index;
    }
    dst[index] = '\0';
}

void plasma_v2_config_set_defaults(plasma_v2_config *config)
{
    if (config == 0) {
        return;
    }
    config->struct_size = (ss_u32)sizeof(*config);
    config->schema_version = PLASMA_V2_CONFIG_SCHEMA_VERSION;
    plasma_v2_spec_set_defaults(&config->spec);
    plasma_v2_config_copy_source(config->source_kind, PLASMA_V2_CONFIG_SOURCE_LENGTH, "default");
}

ss_u32 plasma_v2_config_from_legacy(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_config *config_out
)
{
    ss_u32 status;

    if (legacy == 0 || config_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    plasma_v2_config_set_defaults(config_out);
    status = plasma_v2_migration_from_legacy_config(legacy, &config_out->spec);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    plasma_v2_config_copy_source(config_out->source_kind, PLASMA_V2_CONFIG_SOURCE_LENGTH, "legacy-migrated");
    return plasma_v2_config_is_valid(config_out) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_FAIL;
}

ss_u32 plasma_v2_config_is_valid(const plasma_v2_config *config)
{
    if (config == 0) {
        return SS_V2_FALSE;
    }
    if (config->struct_size < (ss_u32)sizeof(*config)) {
        return SS_V2_FALSE;
    }
    if (config->schema_version != PLASMA_V2_CONFIG_SCHEMA_VERSION) {
        return SS_V2_FALSE;
    }
    if (config->source_kind[0] == '\0') {
        return SS_V2_FALSE;
    }
    return plasma_v2_spec_is_valid(&config->spec);
}
