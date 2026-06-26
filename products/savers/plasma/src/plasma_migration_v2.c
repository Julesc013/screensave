#include <string.h>

#include "plasma_migration_v2.h"

static const plasma_u09_preset_migration_v2 g_plasma_u09_presets[] = {
    {
        "plasma_lava",
        "plasma_lava",
        "plasma_lava",
        PLASMA_V2_FIELD_CLASSIC_INTERFERENCE,
        (ss_u32)100U,
        (ss_u32)56U,
        (ss_u32)32U,
        (ss_u32)18U,
        (ss_u32)8U,
        PLASMA_V2_OUTPUT_CONTINUOUS
    },
    {
        "aurora_plasma",
        "aurora_plasma",
        "aurora_cool",
        PLASMA_V2_FIELD_RADIAL_WARPED,
        (ss_u32)92U,
        (ss_u32)68U,
        (ss_u32)44U,
        (ss_u32)38U,
        (ss_u32)10U,
        PLASMA_V2_OUTPUT_CONTINUOUS
    },
    {
        "ocean_interference",
        "ocean_interference",
        "oceanic_blue",
        PLASMA_V2_FIELD_CLASSIC_INTERFERENCE,
        (ss_u32)88U,
        (ss_u32)72U,
        (ss_u32)40U,
        (ss_u32)28U,
        (ss_u32)12U,
        PLASMA_V2_OUTPUT_CONTINUOUS
    },
    {
        "museum_phosphor",
        "museum_phosphor",
        "museum_phosphor",
        PLASMA_V2_FIELD_CLASSIC_INTERFERENCE,
        (ss_u32)104U,
        (ss_u32)44U,
        (ss_u32)24U,
        (ss_u32)12U,
        (ss_u32)6U,
        PLASMA_V2_OUTPUT_CONTINUOUS
    },
    {
        "quiet_darkroom",
        "quiet_darkroom",
        "quiet_darkroom",
        PLASMA_V2_FIELD_RADIAL_WARPED,
        (ss_u32)108U,
        (ss_u32)36U,
        (ss_u32)18U,
        (ss_u32)20U,
        (ss_u32)5U,
        PLASMA_V2_OUTPUT_CONTINUOUS
    }
};

static const plasma_u09_theme_migration_v2 g_plasma_u09_themes[] = {
    {
        "plasma_lava",
        "plasma_lava",
        PLASMA_V2_MATERIAL_PLASMA_LAVA,
        (ss_u32)100U,
        (ss_u32)100U,
        PLASMA_V2_TREATMENT_NONE
    },
    {
        "aurora_cool",
        "aurora_cool",
        PLASMA_V2_MATERIAL_AURORA_COOL,
        (ss_u32)96U,
        (ss_u32)96U,
        PLASMA_V2_TREATMENT_RESTRAINED_DITHER
    },
    {
        "oceanic_blue",
        "oceanic_blue",
        PLASMA_V2_MATERIAL_OCEANIC_BLUE,
        (ss_u32)98U,
        (ss_u32)104U,
        PLASMA_V2_TREATMENT_NONE
    },
    {
        "museum_phosphor",
        "museum_phosphor",
        PLASMA_V2_MATERIAL_MUSEUM_PHOSPHOR,
        (ss_u32)92U,
        (ss_u32)108U,
        PLASMA_V2_TREATMENT_RESTRAINED_CRT
    },
    {
        "quiet_darkroom",
        "quiet_darkroom",
        PLASMA_V2_MATERIAL_QUIET_DARKROOM,
        (ss_u32)76U,
        (ss_u32)86U,
        PLASMA_V2_TREATMENT_RESTRAINED_CRT
    }
};

static const char *plasma_migration_v2_canonical_key(const char *key)
{
    if (key == 0 || key[0] == '\0') {
        return "plasma_lava";
    }
    if (strcmp(key, "ember_lava") == 0) {
        return "plasma_lava";
    }

    return key;
}

const plasma_u09_preset_migration_v2 *plasma_migration_v2_find_u09_preset(const char *key)
{
    const char *canonical_key;
    ss_u32 index;
    ss_u32 count;

    canonical_key = plasma_migration_v2_canonical_key(key);
    count = (ss_u32)(sizeof(g_plasma_u09_presets) / sizeof(g_plasma_u09_presets[0]));
    for (index = (ss_u32)0U; index < count; ++index) {
        if (strcmp(g_plasma_u09_presets[index].canonical_key, canonical_key) == 0) {
            return &g_plasma_u09_presets[index];
        }
    }

    return 0;
}

const plasma_u09_theme_migration_v2 *plasma_migration_v2_find_u09_theme(const char *key)
{
    const char *canonical_key;
    ss_u32 index;
    ss_u32 count;

    canonical_key = plasma_migration_v2_canonical_key(key);
    count = (ss_u32)(sizeof(g_plasma_u09_themes) / sizeof(g_plasma_u09_themes[0]));
    for (index = (ss_u32)0U; index < count; ++index) {
        if (strcmp(g_plasma_u09_themes[index].canonical_key, canonical_key) == 0) {
            return &g_plasma_u09_themes[index];
        }
    }

    return 0;
}

ss_u32 plasma_migration_v2_apply_u09(
    const char *preset_key,
    const char *theme_key,
    plasma_spec_v2 *spec
)
{
    const plasma_u09_preset_migration_v2 *preset;
    const plasma_u09_theme_migration_v2 *theme;

    if (spec == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plasma_spec_v2_set_defaults(spec);
    preset = plasma_migration_v2_find_u09_preset(preset_key);
    if (preset == 0) {
        return SS_V2_STATUS_UNSUPPORTED;
    }

    if (theme_key == 0 || theme_key[0] == '\0') {
        theme = plasma_migration_v2_find_u09_theme(preset->default_theme_key);
    } else {
        theme = plasma_migration_v2_find_u09_theme(theme_key);
    }
    if (theme == 0) {
        return SS_V2_STATUS_UNSUPPORTED;
    }

    spec->field_family = preset->field_family;
    spec->scale = preset->scale;
    spec->complexity = preset->complexity;
    spec->motion_speed = preset->motion_speed;
    spec->warp_amount = preset->warp_amount;
    spec->feedback_amount = preset->feedback_amount;
    spec->output_style = preset->output_style;
    spec->material_id = theme->material_id;
    spec->brightness = theme->brightness;
    spec->contrast = theme->contrast;
    spec->treatment_flags = theme->treatment_flags;
    spec->quality_intent = PLASMA_V2_QUALITY_SAFE;
    spec->seed_policy = PLASMA_V2_SEED_SESSION;
    spec->presentation = PLASMA_V2_PRESENTATION_FLAT;
    plasma_spec_v2_clamp(spec);

    return plasma_spec_v2_is_valid(spec) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_FAIL;
}
