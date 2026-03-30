#include <string.h>

#include "plasma_internal.h"

static const char *plasma_canonical_preset_key(const char *preset_key)
{
    if (preset_key != NULL && lstrcmpiA(preset_key, "ember_lava") == 0) {
        return "plasma_lava";
    }

    return preset_key;
}

const screensave_preset_descriptor g_plasma_presets[] = {
    {
        "plasma_lava",
        "Plasma Lava",
        "Warm fire presentation with restrained palette cycling and soft diffusion.",
        "plasma_lava",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "aurora_plasma",
        "Aurora Plasma",
        "Cool multi-wave plasma tuned for richer internal color motion.",
        "aurora_cool",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A11UL
    },
    {
        "ocean_interference",
        "Ocean Interference",
        "Layered wave interference with restrained blue palette drift.",
        "oceanic_blue",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A12UL
    },
    {
        "museum_phosphor",
        "Museum Phosphor",
        "Low-motion monochrome plasma for quiet long-run presentation.",
        "museum_phosphor",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000A13UL
    },
    {
        "quiet_darkroom",
        "Quiet Darkroom",
        "Subdued plasma-fire preset with lower resolution and softer motion.",
        "quiet_darkroom",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "midnight_interference",
        "Midnight Interference",
        "Cool dark-room interference field with softer resolution and slower palette drift.",
        "midnight_interference",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A14UL
    },
    {
        "amber_terminal",
        "Amber Terminal",
        "Warm monochrome plasma tuned for quieter museum-style presentation.",
        "amber_terminal",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000A15UL
    }
};

static const plasma_preset_values g_plasma_preset_values[] = {
    { PLASMA_EFFECT_FIRE, PLASMA_SPEED_GENTLE, PLASMA_RESOLUTION_STANDARD, PLASMA_SMOOTHING_SOFT },
    { PLASMA_EFFECT_PLASMA, PLASMA_SPEED_STANDARD, PLASMA_RESOLUTION_FINE, PLASMA_SMOOTHING_GLOW },
    { PLASMA_EFFECT_INTERFERENCE, PLASMA_SPEED_GENTLE, PLASMA_RESOLUTION_FINE, PLASMA_SMOOTHING_SOFT },
    { PLASMA_EFFECT_PLASMA, PLASMA_SPEED_GENTLE, PLASMA_RESOLUTION_COARSE, PLASMA_SMOOTHING_OFF },
    { PLASMA_EFFECT_FIRE, PLASMA_SPEED_GENTLE, PLASMA_RESOLUTION_COARSE, PLASMA_SMOOTHING_SOFT },
    { PLASMA_EFFECT_INTERFERENCE, PLASMA_SPEED_GENTLE, PLASMA_RESOLUTION_STANDARD, PLASMA_SMOOTHING_SOFT },
    { PLASMA_EFFECT_PLASMA, PLASMA_SPEED_GENTLE, PLASMA_RESOLUTION_COARSE, PLASMA_SMOOTHING_SOFT }
};

const screensave_preset_descriptor *plasma_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_plasma_presets) / sizeof(g_plasma_presets[0]));
    }

    return g_plasma_presets;
}

const plasma_preset_values *plasma_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = plasma_get_presets(&preset_count);
    preset_key = plasma_canonical_preset_key(preset_key);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_plasma_preset_values[index];
        }
    }

    return NULL;
}

const char *plasma_effect_mode_name(int effect_mode)
{
    switch (effect_mode) {
    case PLASMA_EFFECT_FIRE:
        return "fire";

    case PLASMA_EFFECT_INTERFERENCE:
        return "interference";

    case PLASMA_EFFECT_PLASMA:
    default:
        return "plasma";
    }
}

const char *plasma_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case PLASMA_SPEED_GENTLE:
        return "gentle";

    case PLASMA_SPEED_LIVELY:
        return "lively";

    case PLASMA_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *plasma_resolution_mode_name(int resolution_mode)
{
    switch (resolution_mode) {
    case PLASMA_RESOLUTION_COARSE:
        return "coarse";

    case PLASMA_RESOLUTION_FINE:
        return "fine";

    case PLASMA_RESOLUTION_STANDARD:
    default:
        return "standard";
    }
}

const char *plasma_smoothing_mode_name(int smoothing_mode)
{
    switch (smoothing_mode) {
    case PLASMA_SMOOTHING_OFF:
        return "off";

    case PLASMA_SMOOTHING_GLOW:
        return "glow";

    case PLASMA_SMOOTHING_SOFT:
    default:
        return "soft";
    }
}

void plasma_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    plasma_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const plasma_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    preset_key = plasma_canonical_preset_key(preset_key);
    presets = plasma_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = plasma_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->effect_mode = preset_values->effect_mode;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->resolution_mode = preset_values->resolution_mode;
    product_config->smoothing_mode = preset_values->smoothing_mode;
}
