#include <string.h>

#include "nocturne_internal.h"

const screensave_preset_descriptor g_nocturne_presets[] = {
    {
        "pure_black",
        "Pure Black",
        "No visible motion; a true black-room blanker.",
        "pure_black",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    },
    {
        "amber_terminal",
        "Amber Terminal",
        "Dim amber line work with a restrained phosphor feel.",
        "amber_black",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000606UL
    },
    {
        "green_phosphor",
        "Green Phosphor",
        "Muted green trace motion for long dark-room runs.",
        "green_black",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000607UL
    },
    {
        "phosphor_night",
        "Phosphor Night",
        "Green-blue line motion with a dim observatory floor and slow drift refresh.",
        "phosphor_night",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000609UL
    },
    {
        "blue_quiet",
        "Blue Quiet",
        "Cool blue motion with a wide, calm cycle.",
        "blue_black",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "gray_museum",
        "Gray Museum",
        "Neutral monolith motion with restrained fade pacing.",
        "gray_black",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000608UL
    },
    {
        "museum_amber",
        "Museum Amber",
        "Near-black amber monolith motion tuned for exhibit and lobby darkness.",
        "museum_amber",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x0000060AUL
    },
    {
        "deep_sleep",
        "Deep Sleep",
        "Near-black pulse intended for very dark rooms.",
        "gray_black",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    },
    {
        "observatory_night",
        "Observatory Night",
        "The default dark-room profile with subtle but visible life.",
        "gray_black",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    }
};

static const nocturne_preset_values g_nocturne_preset_values[] = {
    { NOCTURNE_MOTION_NONE, NOCTURNE_FADE_GENTLE, NOCTURNE_STRENGTH_STILL },
    { NOCTURNE_MOTION_QUIET_LINE, NOCTURNE_FADE_STANDARD, NOCTURNE_STRENGTH_SUBTLE },
    { NOCTURNE_MOTION_QUIET_LINE, NOCTURNE_FADE_GENTLE, NOCTURNE_STRENGTH_SUBTLE },
    { NOCTURNE_MOTION_QUIET_LINE, NOCTURNE_FADE_GENTLE, NOCTURNE_STRENGTH_SOFT },
    { NOCTURNE_MOTION_DRIFT_MARK, NOCTURNE_FADE_GENTLE, NOCTURNE_STRENGTH_SUBTLE },
    { NOCTURNE_MOTION_MONOLITH, NOCTURNE_FADE_SLOW, NOCTURNE_STRENGTH_STILL },
    { NOCTURNE_MOTION_MONOLITH, NOCTURNE_FADE_SLOW, NOCTURNE_STRENGTH_SUBTLE },
    { NOCTURNE_MOTION_BREATH, NOCTURNE_FADE_GENTLE, NOCTURNE_STRENGTH_STILL },
    { NOCTURNE_MOTION_MONOLITH, NOCTURNE_FADE_GENTLE, NOCTURNE_STRENGTH_SUBTLE }
};

const screensave_preset_descriptor *nocturne_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_nocturne_presets) / sizeof(g_nocturne_presets[0]));
    }

    return g_nocturne_presets;
}

const nocturne_preset_values *nocturne_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = nocturne_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_nocturne_preset_values[index];
        }
    }

    return NULL;
}

const char *nocturne_motion_mode_name(int motion_mode)
{
    switch (motion_mode) {
    case NOCTURNE_MOTION_NONE:
        return "none";

    case NOCTURNE_MOTION_DRIFT_MARK:
        return "drift_mark";

    case NOCTURNE_MOTION_QUIET_LINE:
        return "quiet_line";

    case NOCTURNE_MOTION_MONOLITH:
        return "monolith";

    case NOCTURNE_MOTION_BREATH:
    default:
        return "breath";
    }
}

const char *nocturne_fade_speed_name(int fade_speed)
{
    switch (fade_speed) {
    case NOCTURNE_FADE_SLOW:
        return "slow";

    case NOCTURNE_FADE_GENTLE:
        return "gentle";

    case NOCTURNE_FADE_STANDARD:
    default:
        return "standard";
    }
}

const char *nocturne_motion_strength_name(int motion_strength)
{
    switch (motion_strength) {
    case NOCTURNE_STRENGTH_STILL:
        return "still";

    case NOCTURNE_STRENGTH_SOFT:
        return "soft";

    case NOCTURNE_STRENGTH_SUBTLE:
    default:
        return "subtle";
    }
}

void nocturne_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    nocturne_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const nocturne_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = nocturne_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = nocturne_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->motion_mode = preset_values->motion_mode;
    product_config->fade_speed = preset_values->fade_speed;
    product_config->motion_strength = preset_values->motion_strength;
}
