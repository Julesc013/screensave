#include <string.h>

#include "city_nocturne_internal.h"

const screensave_preset_descriptor g_city_nocturne_presets[] = {
    {
        "quiet_city_sleep",
        "Quiet City Sleep",
        "Low-noise nocturnal study with subdued lights and calm movement.",
        "quiet_city_sleep",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x000014C1UL
    },
    {
        "skyline_vista",
        "Skyline Vista",
        "Balanced skyline composition with moderate density and slow drift.",
        "skyline_vista",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x000014C2UL
    },
    {
        "harbor_sodium_night",
        "Harbor Sodium Night",
        "Harbor-edge scene with warm sodium lights, reflections, and moderate motion.",
        "harbor_sodium_night",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x000014C3UL
    },
    {
        "wet_rooftop",
        "Wet Rooftop",
        "Denser rooftop flyover with rain-sheen reflections and slightly brisker drift.",
        "wet_rooftop",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x000014C4UL
    },
    {
        "industrial_port",
        "Industrial Port",
        "Sparse industrial edge with slower lights and a restrained profile.",
        "industrial_port",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x000014C5UL
    }
};

static const city_nocturne_preset_values g_city_nocturne_preset_values[] = {
    { CITY_NOCTURNE_SCENE_WINDOW_RIVER, CITY_NOCTURNE_SPEED_STILL, CITY_NOCTURNE_DENSITY_SPARSE },
    { CITY_NOCTURNE_SCENE_SKYLINE, CITY_NOCTURNE_SPEED_STANDARD, CITY_NOCTURNE_DENSITY_STANDARD },
    { CITY_NOCTURNE_SCENE_HARBOR, CITY_NOCTURNE_SPEED_STANDARD, CITY_NOCTURNE_DENSITY_STANDARD },
    { CITY_NOCTURNE_SCENE_WINDOW_RIVER, CITY_NOCTURNE_SPEED_BRISK, CITY_NOCTURNE_DENSITY_DENSE },
    { CITY_NOCTURNE_SCENE_HARBOR, CITY_NOCTURNE_SPEED_STILL, CITY_NOCTURNE_DENSITY_SPARSE }
};

const screensave_preset_descriptor *city_nocturne_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_city_nocturne_presets) / sizeof(g_city_nocturne_presets[0]));
    }

    return g_city_nocturne_presets;
}

const city_nocturne_preset_values *city_nocturne_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = city_nocturne_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_city_nocturne_preset_values[index];
        }
    }

    return NULL;
}

const char *city_nocturne_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case CITY_NOCTURNE_SCENE_HARBOR:
        return "harbor";
    case CITY_NOCTURNE_SCENE_WINDOW_RIVER:
        return "window_river";
    case CITY_NOCTURNE_SCENE_SKYLINE:
    default:
        return "skyline";
    }
}

const char *city_nocturne_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case CITY_NOCTURNE_SPEED_STILL:
        return "still";
    case CITY_NOCTURNE_SPEED_BRISK:
        return "brisk";
    case CITY_NOCTURNE_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *city_nocturne_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case CITY_NOCTURNE_DENSITY_SPARSE:
        return "sparse";
    case CITY_NOCTURNE_DENSITY_DENSE:
        return "dense";
    case CITY_NOCTURNE_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

void city_nocturne_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    city_nocturne_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const city_nocturne_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = city_nocturne_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = city_nocturne_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->scene_mode = preset_values->scene_mode;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->density_mode = preset_values->density_mode;
}
