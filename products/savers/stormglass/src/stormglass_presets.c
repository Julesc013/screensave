#include <string.h>

#include "stormglass_internal.h"

const screensave_preset_descriptor g_stormglass_presets[] = {
    {
        "quiet_midnight_rain",
        "Quiet Midnight Rain",
        "Low-intensity rain-pane study with restrained city glow and soft pane mist.",
        "quiet_midnight_rain",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "sodium_city_window",
        "Sodium City Window",
        "Warmer wet-window urban glow with balanced rainfall and clearer glass.",
        "sodium_vapor_city",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D31UL
    },
    {
        "blue_storm",
        "Blue Storm",
        "Heavier rain with deeper pane condensation and restrained flash events.",
        "blue_storm",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000D32UL
    },
    {
        "winter_pane",
        "Winter Pane",
        "Cold glass drift with frostier layers and quieter snow-window motion.",
        "winter_pane",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D33UL
    },
    {
        "museum_weather",
        "Museum Weather",
        "Monochrome fogged pane for calmer, lower-detail long runs.",
        "museum_weather",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    },
    {
        "winter_streetlamp",
        "Winter Streetlamp",
        "Cold-pane streetlamp study with steadier snowfall drift and warmer distant light bleed.",
        "winter_streetlamp",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D34UL
    },
    {
        "monochrome_cold_pane",
        "Monochrome Cold Pane",
        "Quiet near-monochrome condensation pass for dark-room-safe long unattended runs.",
        "monochrome_cold_pane",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const stormglass_preset_values g_stormglass_preset_values[] = {
    { STORMGLASS_SCENE_RAIN, STORMGLASS_INTENSITY_QUIET, STORMGLASS_PANE_MISTED },
    { STORMGLASS_SCENE_RAIN, STORMGLASS_INTENSITY_STANDARD, STORMGLASS_PANE_CLEAR },
    { STORMGLASS_SCENE_RAIN, STORMGLASS_INTENSITY_HEAVY, STORMGLASS_PANE_CONDENSED },
    { STORMGLASS_SCENE_WINTER, STORMGLASS_INTENSITY_QUIET, STORMGLASS_PANE_CONDENSED },
    { STORMGLASS_SCENE_FOGGED, STORMGLASS_INTENSITY_QUIET, STORMGLASS_PANE_CONDENSED },
    { STORMGLASS_SCENE_WINTER, STORMGLASS_INTENSITY_STANDARD, STORMGLASS_PANE_CONDENSED },
    { STORMGLASS_SCENE_FOGGED, STORMGLASS_INTENSITY_QUIET, STORMGLASS_PANE_MISTED }
};

const screensave_preset_descriptor *stormglass_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_stormglass_presets) / sizeof(g_stormglass_presets[0]));
    }

    return g_stormglass_presets;
}

const stormglass_preset_values *stormglass_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = stormglass_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_stormglass_preset_values[index];
        }
    }

    return NULL;
}

const char *stormglass_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case STORMGLASS_SCENE_FOGGED:
        return "fogged";
    case STORMGLASS_SCENE_WINTER:
        return "winter";
    case STORMGLASS_SCENE_RAIN:
    default:
        return "rain";
    }
}

const char *stormglass_intensity_mode_name(int intensity_mode)
{
    switch (intensity_mode) {
    case STORMGLASS_INTENSITY_QUIET:
        return "quiet";
    case STORMGLASS_INTENSITY_HEAVY:
        return "heavy";
    case STORMGLASS_INTENSITY_STANDARD:
    default:
        return "standard";
    }
}

const char *stormglass_pane_mode_name(int pane_mode)
{
    switch (pane_mode) {
    case STORMGLASS_PANE_CLEAR:
        return "clear";
    case STORMGLASS_PANE_CONDENSED:
        return "condensed";
    case STORMGLASS_PANE_MISTED:
    default:
        return "misted";
    }
}

void stormglass_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    stormglass_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const stormglass_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = stormglass_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = stormglass_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->scene_mode = preset_values->scene_mode;
    product_config->intensity_mode = preset_values->intensity_mode;
    product_config->pane_mode = preset_values->pane_mode;
}
