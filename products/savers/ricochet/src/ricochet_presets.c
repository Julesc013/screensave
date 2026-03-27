#include <string.h>

#include "ricochet_internal.h"

const screensave_preset_descriptor g_ricochet_presets[] = {
    {
        "classic_clean",
        "Classic Clean",
        "Single bright block motion with a short, readable trail.",
        "classic_clean",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "phosphor_arc",
        "Phosphor Arc",
        "Green disc motion with a longer phosphor-style trail.",
        "phosphor_green",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000921UL
    },
    {
        "quiet_modern",
        "Quiet Modern",
        "Three emblem bodies with calm motion and a cool restrained trail.",
        "quiet_modern",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "corner_watch",
        "Corner Watch",
        "Single faster block with no trail and rare corner celebration flashes.",
        "minimal_slate",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000922UL
    },
    {
        "minimal_lobby",
        "Minimal Lobby",
        "Multi-object emblem motion tuned for quiet public-space presentation.",
        "minimal_slate",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const ricochet_preset_values g_ricochet_preset_values[] = {
    { RICOCHET_OBJECT_BLOCK, 1, RICOCHET_SPEED_STANDARD, RICOCHET_TRAIL_SHORT },
    { RICOCHET_OBJECT_DISC, 3, RICOCHET_SPEED_STANDARD, RICOCHET_TRAIL_PHOSPHOR },
    { RICOCHET_OBJECT_EMBLEM, 3, RICOCHET_SPEED_CALM, RICOCHET_TRAIL_SHORT },
    { RICOCHET_OBJECT_BLOCK, 1, RICOCHET_SPEED_LIVELY, RICOCHET_TRAIL_NONE },
    { RICOCHET_OBJECT_EMBLEM, 5, RICOCHET_SPEED_STANDARD, RICOCHET_TRAIL_SHORT }
};

const screensave_preset_descriptor *ricochet_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_ricochet_presets) / sizeof(g_ricochet_presets[0]));
    }

    return g_ricochet_presets;
}

const ricochet_preset_values *ricochet_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = ricochet_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_ricochet_preset_values[index];
        }
    }

    return NULL;
}

const char *ricochet_object_mode_name(int object_mode)
{
    switch (object_mode) {
    case RICOCHET_OBJECT_BLOCK:
        return "block";

    case RICOCHET_OBJECT_DISC:
        return "disc";

    case RICOCHET_OBJECT_EMBLEM:
    default:
        return "emblem";
    }
}

const char *ricochet_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case RICOCHET_SPEED_CALM:
        return "calm";

    case RICOCHET_SPEED_LIVELY:
        return "lively";

    case RICOCHET_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *ricochet_trail_mode_name(int trail_mode)
{
    switch (trail_mode) {
    case RICOCHET_TRAIL_NONE:
        return "none";

    case RICOCHET_TRAIL_PHOSPHOR:
        return "phosphor";

    case RICOCHET_TRAIL_SHORT:
    default:
        return "short";
    }
}

void ricochet_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    ricochet_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *preset_descriptor;
    const ricochet_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    preset_descriptor = screensave_find_preset(ricochet_get_presets(&preset_count), preset_count, preset_key);
    preset_values = ricochet_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->object_mode = preset_values->object_mode;
    product_config->object_count = preset_values->object_count;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->trail_mode = preset_values->trail_mode;
}
