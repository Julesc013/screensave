#include <string.h>

#include "observatory_internal.h"

const screensave_preset_descriptor g_observatory_presets[] = {
    {
        "brass_orrery",
        "Brass Orrery",
        "Balanced exhibit-style orrery with calm ring motion and moderate detail.",
        "brass_observatory",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "chart_room",
        "Chart Room",
        "Slower monochrome celestial chart study with restrained markers and sparse motion.",
        "monochrome_chart_room",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D51UL
    },
    {
        "deep_blue_dome",
        "Deep Blue Dome",
        "Richer dome-style star study with brighter ring and sweep presentation.",
        "deep_blue_dome",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000D52UL
    },
    {
        "eclipse_watch",
        "Eclipse Watch",
        "Sparse eclipse-watch exhibit with darker tones and restrained central pulse.",
        "eclipse_watch",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000D53UL
    },
    {
        "quiet_museum",
        "Quiet Museum",
        "Lower-detail astronomy exhibit tuned for darker, calmer long runs.",
        "quiet_museum_astronomy",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const observatory_preset_values g_observatory_preset_values[] = {
    { OBSERVATORY_SCENE_ORRERY, OBSERVATORY_SPEED_STANDARD, OBSERVATORY_DETAIL_STANDARD },
    { OBSERVATORY_SCENE_CHART_ROOM, OBSERVATORY_SPEED_STILL, OBSERVATORY_DETAIL_STANDARD },
    { OBSERVATORY_SCENE_DOME, OBSERVATORY_SPEED_STANDARD, OBSERVATORY_DETAIL_RICH },
    { OBSERVATORY_SCENE_DOME, OBSERVATORY_SPEED_STILL, OBSERVATORY_DETAIL_SPARSE },
    { OBSERVATORY_SCENE_CHART_ROOM, OBSERVATORY_SPEED_STILL, OBSERVATORY_DETAIL_SPARSE }
};

const screensave_preset_descriptor *observatory_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_observatory_presets) / sizeof(g_observatory_presets[0]));
    }

    return g_observatory_presets;
}

const observatory_preset_values *observatory_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = observatory_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_observatory_preset_values[index];
        }
    }

    return NULL;
}

const char *observatory_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case OBSERVATORY_SCENE_CHART_ROOM:
        return "chart_room";
    case OBSERVATORY_SCENE_DOME:
        return "dome";
    case OBSERVATORY_SCENE_ORRERY:
    default:
        return "orrery";
    }
}

const char *observatory_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case OBSERVATORY_SPEED_STILL:
        return "still";
    case OBSERVATORY_SPEED_BRISK:
        return "brisk";
    case OBSERVATORY_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *observatory_detail_mode_name(int detail_mode)
{
    switch (detail_mode) {
    case OBSERVATORY_DETAIL_SPARSE:
        return "sparse";
    case OBSERVATORY_DETAIL_RICH:
        return "rich";
    case OBSERVATORY_DETAIL_STANDARD:
    default:
        return "standard";
    }
}

void observatory_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    observatory_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const observatory_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = observatory_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = observatory_find_preset_values(preset_key);
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
