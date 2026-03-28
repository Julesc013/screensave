#include <string.h>

#include "oscilloscope_dreams_internal.h"

const screensave_preset_descriptor g_oscilloscope_dreams_presets[] = {
    {
        "amber_scope",
        "Amber Scope",
        "Warm laboratory-style lissajous with restrained persistence.",
        "amber_scope",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "phase_harmonics",
        "Phase Harmonics",
        "Cool harmonograph presentation with longer phosphor drift.",
        "blue_lab",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A21UL
    },
    {
        "green_vector",
        "Green Vector",
        "Classic green scope presentation with mirrored dense curves.",
        "green_vector",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A22UL
    },
    {
        "drafting_board",
        "Drafting Board",
        "Blue-on-paper vector study with moderate persistence and quiet drift.",
        "drafting_board",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "museum_quiet",
        "Museum Quiet",
        "Low-detail quiet presentation for long unattended runs.",
        "museum_quiet",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000A23UL
    }
};

static const oscilloscope_dreams_preset_values g_oscilloscope_dreams_preset_values[] = {
    { OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS, OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD, OSCILLOSCOPE_DREAMS_DRIFT_STANDARD, OSCILLOSCOPE_DREAMS_MIRROR_NONE },
    { OSCILLOSCOPE_DREAMS_CURVE_HARMONOGRAPH, OSCILLOSCOPE_DREAMS_PERSISTENCE_LONG, OSCILLOSCOPE_DREAMS_DRIFT_STANDARD, OSCILLOSCOPE_DREAMS_MIRROR_HORIZONTAL },
    { OSCILLOSCOPE_DREAMS_CURVE_DENSE, OSCILLOSCOPE_DREAMS_PERSISTENCE_LONG, OSCILLOSCOPE_DREAMS_DRIFT_STANDARD, OSCILLOSCOPE_DREAMS_MIRROR_QUAD },
    { OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS, OSCILLOSCOPE_DREAMS_PERSISTENCE_SHORT, OSCILLOSCOPE_DREAMS_DRIFT_CALM, OSCILLOSCOPE_DREAMS_MIRROR_HORIZONTAL },
    { OSCILLOSCOPE_DREAMS_CURVE_HARMONOGRAPH, OSCILLOSCOPE_DREAMS_PERSISTENCE_SHORT, OSCILLOSCOPE_DREAMS_DRIFT_CALM, OSCILLOSCOPE_DREAMS_MIRROR_NONE }
};

const screensave_preset_descriptor *oscilloscope_dreams_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_oscilloscope_dreams_presets) / sizeof(g_oscilloscope_dreams_presets[0]));
    }

    return g_oscilloscope_dreams_presets;
}

const oscilloscope_dreams_preset_values *oscilloscope_dreams_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = oscilloscope_dreams_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_oscilloscope_dreams_preset_values[index];
        }
    }

    return NULL;
}

const char *oscilloscope_dreams_curve_mode_name(int curve_mode)
{
    switch (curve_mode) {
    case OSCILLOSCOPE_DREAMS_CURVE_HARMONOGRAPH:
        return "harmonograph";

    case OSCILLOSCOPE_DREAMS_CURVE_DENSE:
        return "dense";

    case OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS:
    default:
        return "lissajous";
    }
}

const char *oscilloscope_dreams_persistence_mode_name(int persistence_mode)
{
    switch (persistence_mode) {
    case OSCILLOSCOPE_DREAMS_PERSISTENCE_SHORT:
        return "short";

    case OSCILLOSCOPE_DREAMS_PERSISTENCE_LONG:
        return "long";

    case OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD:
    default:
        return "standard";
    }
}

const char *oscilloscope_dreams_drift_mode_name(int drift_mode)
{
    switch (drift_mode) {
    case OSCILLOSCOPE_DREAMS_DRIFT_CALM:
        return "calm";

    case OSCILLOSCOPE_DREAMS_DRIFT_WIDE:
        return "wide";

    case OSCILLOSCOPE_DREAMS_DRIFT_STANDARD:
    default:
        return "standard";
    }
}

const char *oscilloscope_dreams_mirror_mode_name(int mirror_mode)
{
    switch (mirror_mode) {
    case OSCILLOSCOPE_DREAMS_MIRROR_HORIZONTAL:
        return "horizontal";

    case OSCILLOSCOPE_DREAMS_MIRROR_QUAD:
        return "quad";

    case OSCILLOSCOPE_DREAMS_MIRROR_NONE:
    default:
        return "none";
    }
}

void oscilloscope_dreams_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    oscilloscope_dreams_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const oscilloscope_dreams_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = oscilloscope_dreams_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = oscilloscope_dreams_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->curve_mode = preset_values->curve_mode;
    product_config->persistence_mode = preset_values->persistence_mode;
    product_config->drift_mode = preset_values->drift_mode;
    product_config->mirror_mode = preset_values->mirror_mode;
}
