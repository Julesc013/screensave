#include <string.h>

#include "ember_internal.h"

const screensave_preset_descriptor g_ember_presets[] = {
    {
        "ember_lava",
        "Ember Lava",
        "Warm fire presentation with restrained palette cycling and soft diffusion.",
        "ember_lava",
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
        "Subdued ember-fire preset with lower resolution and softer motion.",
        "quiet_darkroom",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    }
};

static const ember_preset_values g_ember_preset_values[] = {
    { EMBER_EFFECT_FIRE, EMBER_SPEED_GENTLE, EMBER_RESOLUTION_STANDARD, EMBER_SMOOTHING_SOFT },
    { EMBER_EFFECT_PLASMA, EMBER_SPEED_STANDARD, EMBER_RESOLUTION_FINE, EMBER_SMOOTHING_GLOW },
    { EMBER_EFFECT_INTERFERENCE, EMBER_SPEED_GENTLE, EMBER_RESOLUTION_FINE, EMBER_SMOOTHING_SOFT },
    { EMBER_EFFECT_PLASMA, EMBER_SPEED_GENTLE, EMBER_RESOLUTION_COARSE, EMBER_SMOOTHING_OFF },
    { EMBER_EFFECT_FIRE, EMBER_SPEED_GENTLE, EMBER_RESOLUTION_COARSE, EMBER_SMOOTHING_SOFT }
};

const screensave_preset_descriptor *ember_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_ember_presets) / sizeof(g_ember_presets[0]));
    }

    return g_ember_presets;
}

const ember_preset_values *ember_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = ember_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_ember_preset_values[index];
        }
    }

    return NULL;
}

const char *ember_effect_mode_name(int effect_mode)
{
    switch (effect_mode) {
    case EMBER_EFFECT_FIRE:
        return "fire";

    case EMBER_EFFECT_INTERFERENCE:
        return "interference";

    case EMBER_EFFECT_PLASMA:
    default:
        return "plasma";
    }
}

const char *ember_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case EMBER_SPEED_GENTLE:
        return "gentle";

    case EMBER_SPEED_LIVELY:
        return "lively";

    case EMBER_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *ember_resolution_mode_name(int resolution_mode)
{
    switch (resolution_mode) {
    case EMBER_RESOLUTION_COARSE:
        return "coarse";

    case EMBER_RESOLUTION_FINE:
        return "fine";

    case EMBER_RESOLUTION_STANDARD:
    default:
        return "standard";
    }
}

const char *ember_smoothing_mode_name(int smoothing_mode)
{
    switch (smoothing_mode) {
    case EMBER_SMOOTHING_OFF:
        return "off";

    case EMBER_SMOOTHING_GLOW:
        return "glow";

    case EMBER_SMOOTHING_SOFT:
    default:
        return "soft";
    }
}

void ember_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ember_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const ember_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = ember_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = ember_find_preset_values(preset_key);
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
