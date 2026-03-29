#include <string.h>

#include "transit_internal.h"

const screensave_preset_descriptor g_transit_presets[] = {
    {
        "wet_motorway",
        "Wet Motorway",
        "Balanced motorway glide with lane lights, reflections, and moderate speed.",
        "wet_motorway",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "freight_night_run",
        "Freight Night Run",
        "Rail corridor study with slower cadence and stronger signal rhythm.",
        "freight_night_run",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D41UL
    },
    {
        "neon_tunnel",
        "Neon Tunnel",
        "Denser tunnel lighting with faster forward cadence and brighter trim.",
        "neon_tunnel",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000D42UL
    },
    {
        "harbor_lights",
        "Harbor Lights",
        "Channel and docklight glide with calm reflections and lower speed.",
        "harbor_lights",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D43UL
    },
    {
        "quiet_industrial_edge",
        "Quiet Industrial Edge",
        "Sparse corridor preset tuned for subdued long-run night ambience.",
        "quiet_industrial_edge",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const transit_preset_values g_transit_preset_values[] = {
    { TRANSIT_SCENE_MOTORWAY, TRANSIT_SPEED_CRUISE, TRANSIT_LIGHTS_STANDARD },
    { TRANSIT_SCENE_RAIL, TRANSIT_SPEED_GLIDE, TRANSIT_LIGHTS_STANDARD },
    { TRANSIT_SCENE_MOTORWAY, TRANSIT_SPEED_EXPRESS, TRANSIT_LIGHTS_DENSE },
    { TRANSIT_SCENE_HARBOR, TRANSIT_SPEED_GLIDE, TRANSIT_LIGHTS_STANDARD },
    { TRANSIT_SCENE_RAIL, TRANSIT_SPEED_GLIDE, TRANSIT_LIGHTS_SPARSE }
};

const screensave_preset_descriptor *transit_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_transit_presets) / sizeof(g_transit_presets[0]));
    }

    return g_transit_presets;
}

const transit_preset_values *transit_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = transit_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_transit_preset_values[index];
        }
    }

    return NULL;
}

const char *transit_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case TRANSIT_SCENE_RAIL:
        return "rail";
    case TRANSIT_SCENE_HARBOR:
        return "harbor";
    case TRANSIT_SCENE_MOTORWAY:
    default:
        return "motorway";
    }
}

const char *transit_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case TRANSIT_SPEED_GLIDE:
        return "glide";
    case TRANSIT_SPEED_EXPRESS:
        return "express";
    case TRANSIT_SPEED_CRUISE:
    default:
        return "cruise";
    }
}

const char *transit_light_mode_name(int light_mode)
{
    switch (light_mode) {
    case TRANSIT_LIGHTS_SPARSE:
        return "sparse";
    case TRANSIT_LIGHTS_DENSE:
        return "dense";
    case TRANSIT_LIGHTS_STANDARD:
    default:
        return "standard";
    }
}

void transit_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    transit_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const transit_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = transit_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = transit_find_preset_values(preset_key);
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
    product_config->light_mode = preset_values->light_mode;
}
