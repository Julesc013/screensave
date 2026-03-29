#include <string.h>

#include "vector_internal.h"

const screensave_preset_descriptor g_vector_presets[] = {
    {
        "quiet_vector_hall",
        "Quiet Vector Hall",
        "Default calm wireframe hall with restrained depth and glide.",
        "quiet_midnight_grid",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "terminal_green_wireframe",
        "Terminal Green Wireframe",
        "Phosphor-green structure field with lower detail and steady motion.",
        "terminal_green_wireframe",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00001401UL
    },
    {
        "sgi_neon_tunnel",
        "SGI Neon Tunnel",
        "Neon tunnel flow with balanced depth cues and brighter rails.",
        "sgi_neon",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00001402UL
    },
    {
        "drafting_grid_flyover",
        "Drafting Grid Flyover",
        "Clean terrain and canyon-style flyover with higher detail.",
        "white_drafting_board",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00001403UL
    },
    {
        "museum_vector_exhibit",
        "Museum Vector Exhibit",
        "Quiet wireframe exhibit with low motion and long-run stability.",
        "museum_vector_exhibit",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const vector_preset_values g_vector_preset_values[] = {
    { VECTOR_SCENE_FIELD, VECTOR_SPEED_CALM, VECTOR_DETAIL_STANDARD },
    { VECTOR_SCENE_FIELD, VECTOR_SPEED_CALM, VECTOR_DETAIL_LOW },
    { VECTOR_SCENE_TUNNEL, VECTOR_SPEED_STANDARD, VECTOR_DETAIL_STANDARD },
    { VECTOR_SCENE_TERRAIN, VECTOR_SPEED_STANDARD, VECTOR_DETAIL_HIGH },
    { VECTOR_SCENE_FIELD, VECTOR_SPEED_CALM, VECTOR_DETAIL_LOW }
};

const screensave_preset_descriptor *vector_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_vector_presets) / sizeof(g_vector_presets[0]));
    }

    return g_vector_presets;
}

const vector_preset_values *vector_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = vector_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_vector_preset_values[index];
        }
    }

    return NULL;
}

const char *vector_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case VECTOR_SCENE_TUNNEL:
        return "tunnel";
    case VECTOR_SCENE_TERRAIN:
        return "terrain";
    case VECTOR_SCENE_FIELD:
    default:
        return "field";
    }
}

const char *vector_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case VECTOR_SPEED_STANDARD:
        return "standard";
    case VECTOR_SPEED_SURGE:
        return "surge";
    case VECTOR_SPEED_CALM:
    default:
        return "calm";
    }
}

const char *vector_detail_mode_name(int detail_mode)
{
    switch (detail_mode) {
    case VECTOR_DETAIL_LOW:
        return "low";
    case VECTOR_DETAIL_HIGH:
        return "high";
    case VECTOR_DETAIL_STANDARD:
    default:
        return "standard";
    }
}

void vector_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    vector_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const vector_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = vector_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = vector_find_preset_values(preset_key);
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
    product_config->detail_mode = preset_values->detail_mode;
}
