#include <string.h>

#include "signal_lab_internal.h"

const screensave_preset_descriptor g_signal_lab_presets[] = {
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

static const signal_lab_preset_values g_signal_lab_preset_values[] = {
    { SIGNAL_LAB_LAYOUT_OPERATIONS, SIGNAL_LAB_ACTIVITY_STANDARD, SIGNAL_LAB_OVERLAY_GRID },
    { SIGNAL_LAB_LAYOUT_SPECTRUM, SIGNAL_LAB_ACTIVITY_BUSY, SIGNAL_LAB_OVERLAY_SCAN },
    { SIGNAL_LAB_LAYOUT_TELEMETRY, SIGNAL_LAB_ACTIVITY_STANDARD, SIGNAL_LAB_OVERLAY_GRID },
    { SIGNAL_LAB_LAYOUT_TELEMETRY, SIGNAL_LAB_ACTIVITY_QUIET, SIGNAL_LAB_OVERLAY_GRID },
    { SIGNAL_LAB_LAYOUT_OPERATIONS, SIGNAL_LAB_ACTIVITY_QUIET, SIGNAL_LAB_OVERLAY_OFF }
};

const screensave_preset_descriptor *signal_lab_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_signal_lab_presets) / sizeof(g_signal_lab_presets[0]));
    }

    return g_signal_lab_presets;
}

const signal_lab_preset_values *signal_lab_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = signal_lab_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_signal_lab_preset_values[index];
        }
    }

    return NULL;
}

const char *signal_lab_layout_mode_name(int layout_mode)
{
    switch (layout_mode) {
    case SIGNAL_LAB_LAYOUT_SPECTRUM:
        return "spectrum";

    case SIGNAL_LAB_LAYOUT_TELEMETRY:
        return "telemetry";

    case SIGNAL_LAB_LAYOUT_OPERATIONS:
    default:
        return "operations";
    }
}

const char *signal_lab_activity_mode_name(int activity_mode)
{
    switch (activity_mode) {
    case SIGNAL_LAB_ACTIVITY_QUIET:
        return "quiet";

    case SIGNAL_LAB_ACTIVITY_BUSY:
        return "busy";

    case SIGNAL_LAB_ACTIVITY_STANDARD:
    default:
        return "standard";
    }
}

const char *signal_lab_overlay_mode_name(int overlay_mode)
{
    switch (overlay_mode) {
    case SIGNAL_LAB_OVERLAY_OFF:
        return "off";

    case SIGNAL_LAB_OVERLAY_SCAN:
        return "scan";

    case SIGNAL_LAB_OVERLAY_GRID:
    default:
        return "grid";
    }
}

void signal_lab_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    signal_lab_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const signal_lab_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = signal_lab_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = signal_lab_find_preset_values(preset_key);
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
