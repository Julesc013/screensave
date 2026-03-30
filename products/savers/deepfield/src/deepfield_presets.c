#include <string.h>

#include "deepfield_internal.h"

const screensave_preset_descriptor g_deepfield_presets[] = {
    {
        "quiet_observatory",
        "Quiet Observatory",
        "The default calm drift profile with low-distraction depth layering.",
        "quiet_observatory",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "warp_travel",
        "Warp Travel",
        "Restrained fly-through motion with periodic warp pulses.",
        "warp_travel",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000941UL
    },
    {
        "phosphor_drift",
        "Phosphor Drift",
        "Green monochrome drift with denser layered motion.",
        "phosphor_space",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000942UL
    },
    {
        "blue_vault",
        "Blue Vault",
        "Cool fly-through presentation with mild pulse cues.",
        "blue_vault",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "museum_terminal",
        "Museum Terminal",
        "Low-density monochrome drift for long quiet runs.",
        "museum_terminal",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000943UL
    },
    {
        "preview_lane",
        "Preview Lane",
        "Sparse drift profile intended for preview and low-detail validation.",
        "quiet_observatory",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000944UL
    },
    {
        "deep_midnight",
        "Deep Midnight",
        "Rich deep-blue observatory drift with a slower camera arc and soft twinkle pulses.",
        "deep_midnight",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000945UL
    },
    {
        "observatory_cruise",
        "Observatory Cruise",
        "Calmer fly-through travel with lower density and gentler depth pacing.",
        "quiet_observatory",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000946UL
    }
};

static const deepfield_preset_values g_deepfield_preset_values[] = {
    { DEEPFIELD_SCENE_PARALLAX, DEEPFIELD_DENSITY_STANDARD, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_OBSERVE, DEEPFIELD_PULSE_NONE },
    { DEEPFIELD_SCENE_FLYTHROUGH, DEEPFIELD_DENSITY_RICH, DEEPFIELD_SPEED_CRUISE, DEEPFIELD_CAMERA_ARC, DEEPFIELD_PULSE_WARP },
    { DEEPFIELD_SCENE_PARALLAX, DEEPFIELD_DENSITY_RICH, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_DRIFT, DEEPFIELD_PULSE_SOFT },
    { DEEPFIELD_SCENE_FLYTHROUGH, DEEPFIELD_DENSITY_STANDARD, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_OBSERVE, DEEPFIELD_PULSE_SOFT },
    { DEEPFIELD_SCENE_PARALLAX, DEEPFIELD_DENSITY_SPARSE, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_OBSERVE, DEEPFIELD_PULSE_NONE },
    { DEEPFIELD_SCENE_PARALLAX, DEEPFIELD_DENSITY_SPARSE, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_DRIFT, DEEPFIELD_PULSE_NONE },
    { DEEPFIELD_SCENE_PARALLAX, DEEPFIELD_DENSITY_RICH, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_ARC, DEEPFIELD_PULSE_SOFT },
    { DEEPFIELD_SCENE_FLYTHROUGH, DEEPFIELD_DENSITY_SPARSE, DEEPFIELD_SPEED_CALM, DEEPFIELD_CAMERA_DRIFT, DEEPFIELD_PULSE_SOFT }
};

const screensave_preset_descriptor *deepfield_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_deepfield_presets) / sizeof(g_deepfield_presets[0]));
    }

    return g_deepfield_presets;
}

const deepfield_preset_values *deepfield_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = deepfield_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_deepfield_preset_values[index];
        }
    }

    return NULL;
}

const char *deepfield_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case DEEPFIELD_SCENE_FLYTHROUGH:
        return "flythrough";

    case DEEPFIELD_SCENE_PARALLAX:
    default:
        return "parallax";
    }
}

const char *deepfield_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case DEEPFIELD_DENSITY_SPARSE:
        return "sparse";

    case DEEPFIELD_DENSITY_RICH:
        return "rich";

    case DEEPFIELD_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

const char *deepfield_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case DEEPFIELD_SPEED_CALM:
        return "calm";

    case DEEPFIELD_SPEED_SURGE:
        return "surge";

    case DEEPFIELD_SPEED_CRUISE:
    default:
        return "cruise";
    }
}

const char *deepfield_camera_mode_name(int camera_mode)
{
    switch (camera_mode) {
    case DEEPFIELD_CAMERA_DRIFT:
        return "drift";

    case DEEPFIELD_CAMERA_ARC:
        return "arc";

    case DEEPFIELD_CAMERA_OBSERVE:
    default:
        return "observe";
    }
}

const char *deepfield_pulse_mode_name(int pulse_mode)
{
    switch (pulse_mode) {
    case DEEPFIELD_PULSE_SOFT:
        return "soft";

    case DEEPFIELD_PULSE_WARP:
        return "warp";

    case DEEPFIELD_PULSE_NONE:
    default:
        return "none";
    }
}

void deepfield_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    deepfield_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const deepfield_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = deepfield_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = deepfield_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->scene_mode = preset_values->scene_mode;
    product_config->density_mode = preset_values->density_mode;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->camera_mode = preset_values->camera_mode;
    product_config->pulse_mode = preset_values->pulse_mode;
}
