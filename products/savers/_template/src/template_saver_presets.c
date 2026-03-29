#include <string.h>

#include "template_saver_internal.h"

static const template_saver_preset_values g_template_saver_preset_values[TEMPLATE_SAVER_PRESET_COUNT] = {
    { TEMPLATE_SAVER_MOTION_DRIFT, TEMPLATE_SAVER_ACCENT_FRAME, TEMPLATE_SAVER_SPACING_BALANCED },
    { TEMPLATE_SAVER_MOTION_FOCUS, TEMPLATE_SAVER_ACCENT_PULSE, TEMPLATE_SAVER_SPACING_BALANCED },
    { TEMPLATE_SAVER_MOTION_CALM, TEMPLATE_SAVER_ACCENT_FRAME, TEMPLATE_SAVER_SPACING_WIDE },
    { TEMPLATE_SAVER_MOTION_FOCUS, TEMPLATE_SAVER_ACCENT_BANDS, TEMPLATE_SAVER_SPACING_TIGHT },
    { TEMPLATE_SAVER_MOTION_CALM, TEMPLATE_SAVER_ACCENT_FRAME, TEMPLATE_SAVER_SPACING_BALANCED },
    { TEMPLATE_SAVER_MOTION_DRIFT, TEMPLATE_SAVER_ACCENT_PULSE, TEMPLATE_SAVER_SPACING_WIDE }
};

const screensave_preset_descriptor g_template_saver_presets[TEMPLATE_SAVER_PRESET_COUNT] = {
    {
        "starter_drift",
        "Starter Drift",
        "Default contributor starter preset with balanced spacing and a restrained frame accent.",
        "starter_blue",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "starter_focus",
        "Starter Focus",
        "Slightly sharper motion and pulse accent for the template product.",
        "starter_amber",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "night_grid",
        "Night Grid",
        "Wide, low-distraction presentation tuned for dark-room style products.",
        "starter_mono",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        404UL
    },
    {
        "signal_lab",
        "Signal Lab",
        "Tighter spacing and band accents to show a second product-owned style direction.",
        "starter_blue",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        808UL
    },
    {
        "quiet_preview",
        "Quiet Preview",
        "Low-detail preset for preview and compatibility-focused validation.",
        "starter_mono",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        120UL
    },
    {
        "window_demo",
        "Window Demo",
        "Balanced preset for windowed development checks and configuration handoff tests.",
        "starter_amber",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    }
};

const screensave_preset_descriptor *template_saver_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = TEMPLATE_SAVER_PRESET_COUNT;
    }

    return g_template_saver_presets;
}

const template_saver_preset_values *template_saver_find_preset_values(const char *preset_key)
{
    unsigned int index;

    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < TEMPLATE_SAVER_PRESET_COUNT; ++index) {
        if (strcmp(g_template_saver_presets[index].preset_key, preset_key) == 0) {
            return &g_template_saver_preset_values[index];
        }
    }

    return NULL;
}

const char *template_saver_motion_mode_name(int motion_mode)
{
    switch (motion_mode) {
    case TEMPLATE_SAVER_MOTION_FOCUS:
        return "focus";
    case TEMPLATE_SAVER_MOTION_CALM:
        return "calm";
    case TEMPLATE_SAVER_MOTION_DRIFT:
    default:
        return "drift";
    }
}

const char *template_saver_accent_mode_name(int accent_mode)
{
    switch (accent_mode) {
    case TEMPLATE_SAVER_ACCENT_PULSE:
        return "pulse";
    case TEMPLATE_SAVER_ACCENT_BANDS:
        return "bands";
    case TEMPLATE_SAVER_ACCENT_FRAME:
    default:
        return "frame";
    }
}

const char *template_saver_spacing_mode_name(int spacing_mode)
{
    switch (spacing_mode) {
    case TEMPLATE_SAVER_SPACING_TIGHT:
        return "tight";
    case TEMPLATE_SAVER_SPACING_WIDE:
        return "wide";
    case TEMPLATE_SAVER_SPACING_BALANCED:
    default:
        return "balanced";
    }
}

int template_saver_motion_mode_from_name(const char *name, int fallback_value)
{
    if (name == NULL) {
        return fallback_value;
    }
    if (strcmp(name, "drift") == 0) {
        return TEMPLATE_SAVER_MOTION_DRIFT;
    }
    if (strcmp(name, "focus") == 0) {
        return TEMPLATE_SAVER_MOTION_FOCUS;
    }
    if (strcmp(name, "calm") == 0) {
        return TEMPLATE_SAVER_MOTION_CALM;
    }
    return fallback_value;
}

int template_saver_accent_mode_from_name(const char *name, int fallback_value)
{
    if (name == NULL) {
        return fallback_value;
    }
    if (strcmp(name, "frame") == 0) {
        return TEMPLATE_SAVER_ACCENT_FRAME;
    }
    if (strcmp(name, "pulse") == 0) {
        return TEMPLATE_SAVER_ACCENT_PULSE;
    }
    if (strcmp(name, "bands") == 0) {
        return TEMPLATE_SAVER_ACCENT_BANDS;
    }
    return fallback_value;
}

int template_saver_spacing_mode_from_name(const char *name, int fallback_value)
{
    if (name == NULL) {
        return fallback_value;
    }
    if (strcmp(name, "tight") == 0) {
        return TEMPLATE_SAVER_SPACING_TIGHT;
    }
    if (strcmp(name, "balanced") == 0) {
        return TEMPLATE_SAVER_SPACING_BALANCED;
    }
    if (strcmp(name, "wide") == 0) {
        return TEMPLATE_SAVER_SPACING_WIDE;
    }
    return fallback_value;
}

void template_saver_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    template_saver_config *product_config
)
{
    const screensave_preset_descriptor *descriptor;
    const template_saver_preset_values *values;
    unsigned int preset_count;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    descriptor = screensave_find_preset(
        template_saver_get_presets(&preset_count),
        preset_count,
        preset_key != NULL ? preset_key : TEMPLATE_SAVER_DEFAULT_PRESET_KEY
    );
    if (descriptor == NULL) {
        descriptor = screensave_find_preset(
            template_saver_get_presets(&preset_count),
            preset_count,
            TEMPLATE_SAVER_DEFAULT_PRESET_KEY
        );
    }
    if (descriptor == NULL) {
        return;
    }

    values = template_saver_find_preset_values(descriptor->preset_key);
    if (values == NULL) {
        return;
    }

    common_config->preset_key = descriptor->preset_key;
    common_config->theme_key = descriptor->theme_key;
    common_config->detail_level = descriptor->detail_level;
    common_config->use_deterministic_seed = descriptor->use_fixed_seed;
    common_config->deterministic_seed = descriptor->fixed_seed;
    product_config->motion_mode = values->motion_mode;
    product_config->accent_mode = values->accent_mode;
    product_config->spacing_mode = values->spacing_mode;
}
