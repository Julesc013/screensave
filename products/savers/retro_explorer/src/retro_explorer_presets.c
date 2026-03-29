#include <string.h>

#include "retro_explorer_internal.h"

const screensave_preset_descriptor g_retro_explorer_presets[] = {
    {
        "quiet_night_run",
        "Quiet Night Run",
        "Calm canyon or corridor travel with sparse cues and soft pacing.",
        "quiet_night_run",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000E11UL
    },
    {
        "cold_lab_passage",
        "Cold Lab Passage",
        "Cool corridor travel with clean geometry and measured forward motion.",
        "cold_lab",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000E12UL
    },
    {
        "industrial_tunnel",
        "Industrial Tunnel",
        "Balanced service passage travel with signage and structural rhythm.",
        "industrial_tunnel",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000E13UL
    },
    {
        "neon_maze",
        "Neon Maze",
        "Brisk industrial travel with stronger highlights and denser route cues.",
        "neon_maze",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000E14UL
    },
    {
        "dusty_ruin_run",
        "Dusty Ruin Run",
        "Open canyon travel with warmer tones and a longer scenic horizon.",
        "dusty_ruin",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000E15UL
    }
};

static const retro_explorer_preset_values g_retro_explorer_preset_values[] = {
    { RETRO_EXPLORER_SCENE_CANYON, RETRO_EXPLORER_SPEED_CALM },
    { RETRO_EXPLORER_SCENE_CORRIDOR, RETRO_EXPLORER_SPEED_CALM },
    { RETRO_EXPLORER_SCENE_INDUSTRIAL, RETRO_EXPLORER_SPEED_STANDARD },
    { RETRO_EXPLORER_SCENE_INDUSTRIAL, RETRO_EXPLORER_SPEED_BRISK },
    { RETRO_EXPLORER_SCENE_CANYON, RETRO_EXPLORER_SPEED_STANDARD }
};

const screensave_preset_descriptor *retro_explorer_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_retro_explorer_presets) / sizeof(g_retro_explorer_presets[0]));
    }

    return g_retro_explorer_presets;
}

const retro_explorer_preset_values *retro_explorer_find_preset_values(const char *preset_key)
{
    unsigned int count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = retro_explorer_get_presets(&count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_retro_explorer_preset_values[index];
        }
    }

    return NULL;
}

const char *retro_explorer_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case RETRO_EXPLORER_SCENE_INDUSTRIAL:
        return "industrial";
    case RETRO_EXPLORER_SCENE_CANYON:
        return "canyon";
    case RETRO_EXPLORER_SCENE_CORRIDOR:
    default:
        return "corridor";
    }
}

const char *retro_explorer_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case RETRO_EXPLORER_SPEED_CALM:
        return "calm";
    case RETRO_EXPLORER_SPEED_BRISK:
        return "brisk";
    case RETRO_EXPLORER_SPEED_STANDARD:
    default:
        return "standard";
    }
}

void retro_explorer_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    retro_explorer_config *product_config
)
{
    unsigned int count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const retro_explorer_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = retro_explorer_get_presets(&count);
    preset_descriptor = screensave_find_preset(presets, count, preset_key);
    preset_values = retro_explorer_find_preset_values(preset_key);
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
}
