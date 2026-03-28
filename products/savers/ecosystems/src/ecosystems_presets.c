#include <string.h>

#include "ecosystems_internal.h"

const screensave_preset_descriptor g_ecosystems_presets[] = {
    {
        "aquarium_current",
        "Aquarium Current",
        "Balanced schooling aquarium with cool current motion.",
        "aquarium_current",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "aviary_silhouettes",
        "Aviary Silhouettes",
        "Calm flock study with higher sky spacing and quieter activity.",
        "aviary_silhouettes",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000C61UL
    },
    {
        "dusk_fireflies",
        "Dusk Fireflies",
        "Lusher firefly field with brighter dusk pulses.",
        "dusk_fireflies",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C62UL
    },
    {
        "wetland_reeds",
        "Wetland Reeds",
        "Sparser firefly habitat framed by calm reed silhouettes.",
        "wetland_reeds",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C63UL
    },
    {
        "dark_oceanic",
        "Dark Oceanic",
        "Low-motion oceanic aquarium tuned for darker rooms.",
        "dark_oceanic",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const ecosystems_preset_values g_ecosystems_preset_values[] = {
    { ECOSYSTEMS_HABITAT_AQUARIUM, ECOSYSTEMS_DENSITY_STANDARD, ECOSYSTEMS_ACTIVITY_STANDARD },
    { ECOSYSTEMS_HABITAT_AVIARY, ECOSYSTEMS_DENSITY_STANDARD, ECOSYSTEMS_ACTIVITY_CALM },
    { ECOSYSTEMS_HABITAT_FIREFLIES, ECOSYSTEMS_DENSITY_LUSH, ECOSYSTEMS_ACTIVITY_STANDARD },
    { ECOSYSTEMS_HABITAT_FIREFLIES, ECOSYSTEMS_DENSITY_STANDARD, ECOSYSTEMS_ACTIVITY_CALM },
    { ECOSYSTEMS_HABITAT_AQUARIUM, ECOSYSTEMS_DENSITY_SPARSE, ECOSYSTEMS_ACTIVITY_CALM }
};

const screensave_preset_descriptor *ecosystems_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_ecosystems_presets) / sizeof(g_ecosystems_presets[0]));
    }

    return g_ecosystems_presets;
}

const ecosystems_preset_values *ecosystems_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = ecosystems_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_ecosystems_preset_values[index];
        }
    }

    return NULL;
}

const char *ecosystems_habitat_mode_name(int habitat_mode)
{
    switch (habitat_mode) {
    case ECOSYSTEMS_HABITAT_AVIARY:
        return "aviary";

    case ECOSYSTEMS_HABITAT_FIREFLIES:
        return "fireflies";

    case ECOSYSTEMS_HABITAT_AQUARIUM:
    default:
        return "aquarium";
    }
}

const char *ecosystems_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case ECOSYSTEMS_DENSITY_SPARSE:
        return "sparse";

    case ECOSYSTEMS_DENSITY_LUSH:
        return "lush";

    case ECOSYSTEMS_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

const char *ecosystems_activity_mode_name(int activity_mode)
{
    switch (activity_mode) {
    case ECOSYSTEMS_ACTIVITY_CALM:
        return "calm";

    case ECOSYSTEMS_ACTIVITY_LIVELY:
        return "lively";

    case ECOSYSTEMS_ACTIVITY_STANDARD:
    default:
        return "standard";
    }
}

void ecosystems_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ecosystems_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const ecosystems_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = ecosystems_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = ecosystems_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->habitat_mode = preset_values->habitat_mode;
    product_config->density_mode = preset_values->density_mode;
    product_config->activity_mode = preset_values->activity_mode;
}
