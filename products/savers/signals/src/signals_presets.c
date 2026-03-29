#include <string.h>

#include "signals_internal.h"

const screensave_preset_descriptor g_signals_presets[] = {
    {
        "green_terminal",
        "Green Terminal",
        "Balanced multi-panel console with restrained grid overlays.",
        "green_terminal",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "amber_scope",
        "Amber Scope",
        "Wide sweep and meter presentation with warmer instrument tones.",
        "amber_scope",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C21UL
    },
    {
        "blue_lab",
        "Blue Lab",
        "Telemetry-heavy board layout with cooler laboratory styling.",
        "blue_lab",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C22UL
    },
    {
        "white_board",
        "White Board",
        "Quiet technical board with brighter framing and restrained updates.",
        "white_technical_board",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000C23UL
    },
    {
        "quiet_console",
        "Quiet Console",
        "Lower-activity night console tuned for calmer long runs.",
        "quiet_night_console",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const signals_preset_values g_signals_preset_values[] = {
    { SIGNALS_LAYOUT_OPERATIONS, SIGNALS_ACTIVITY_STANDARD, SIGNALS_OVERLAY_GRID },
    { SIGNALS_LAYOUT_SPECTRUM, SIGNALS_ACTIVITY_BUSY, SIGNALS_OVERLAY_SCAN },
    { SIGNALS_LAYOUT_TELEMETRY, SIGNALS_ACTIVITY_STANDARD, SIGNALS_OVERLAY_GRID },
    { SIGNALS_LAYOUT_TELEMETRY, SIGNALS_ACTIVITY_QUIET, SIGNALS_OVERLAY_GRID },
    { SIGNALS_LAYOUT_OPERATIONS, SIGNALS_ACTIVITY_QUIET, SIGNALS_OVERLAY_OFF }
};

const screensave_preset_descriptor *signals_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_signals_presets) / sizeof(g_signals_presets[0]));
    }

    return g_signals_presets;
}

const signals_preset_values *signals_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = signals_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_signals_preset_values[index];
        }
    }

    return NULL;
}

const char *signals_layout_mode_name(int layout_mode)
{
    switch (layout_mode) {
    case SIGNALS_LAYOUT_SPECTRUM:
        return "spectrum";

    case SIGNALS_LAYOUT_TELEMETRY:
        return "telemetry";

    case SIGNALS_LAYOUT_OPERATIONS:
    default:
        return "operations";
    }
}

const char *signals_activity_mode_name(int activity_mode)
{
    switch (activity_mode) {
    case SIGNALS_ACTIVITY_QUIET:
        return "quiet";

    case SIGNALS_ACTIVITY_BUSY:
        return "busy";

    case SIGNALS_ACTIVITY_STANDARD:
    default:
        return "standard";
    }
}

const char *signals_overlay_mode_name(int overlay_mode)
{
    switch (overlay_mode) {
    case SIGNALS_OVERLAY_OFF:
        return "off";

    case SIGNALS_OVERLAY_SCAN:
        return "scan";

    case SIGNALS_OVERLAY_GRID:
    default:
        return "grid";
    }
}

void signals_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    signals_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const signals_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = signals_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = signals_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->layout_mode = preset_values->layout_mode;
    product_config->activity_mode = preset_values->activity_mode;
    product_config->overlay_mode = preset_values->overlay_mode;
}
