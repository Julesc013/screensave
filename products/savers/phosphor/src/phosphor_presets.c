#include <string.h>

#include "phosphor_internal.h"

const screensave_preset_descriptor g_phosphor_presets[] = {
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
    },
    {
        "amber_harmonics",
        "Amber Harmonics",
        "Warm harmonograph presentation with longer amber persistence and restrained mirror symmetry.",
        "amber_harmonics",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A24UL
    },
    {
        "white_instrument",
        "White Instrument",
        "Pale laboratory trace tuned for cleaner preview-safe rhythm and drafting-room clarity.",
        "white_instrument",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A25UL
    }
};

static const phosphor_preset_values g_phosphor_preset_values[] = {
    { PHOSPHOR_CURVE_LISSAJOUS, PHOSPHOR_PERSISTENCE_STANDARD, PHOSPHOR_DRIFT_STANDARD, PHOSPHOR_MIRROR_NONE },
    { PHOSPHOR_CURVE_HARMONOGRAPH, PHOSPHOR_PERSISTENCE_LONG, PHOSPHOR_DRIFT_STANDARD, PHOSPHOR_MIRROR_HORIZONTAL },
    { PHOSPHOR_CURVE_DENSE, PHOSPHOR_PERSISTENCE_LONG, PHOSPHOR_DRIFT_STANDARD, PHOSPHOR_MIRROR_QUAD },
    { PHOSPHOR_CURVE_LISSAJOUS, PHOSPHOR_PERSISTENCE_SHORT, PHOSPHOR_DRIFT_CALM, PHOSPHOR_MIRROR_HORIZONTAL },
    { PHOSPHOR_CURVE_HARMONOGRAPH, PHOSPHOR_PERSISTENCE_SHORT, PHOSPHOR_DRIFT_CALM, PHOSPHOR_MIRROR_NONE },
    { PHOSPHOR_CURVE_HARMONOGRAPH, PHOSPHOR_PERSISTENCE_LONG, PHOSPHOR_DRIFT_CALM, PHOSPHOR_MIRROR_HORIZONTAL },
    { PHOSPHOR_CURVE_LISSAJOUS, PHOSPHOR_PERSISTENCE_STANDARD, PHOSPHOR_DRIFT_CALM, PHOSPHOR_MIRROR_HORIZONTAL }
};

const screensave_preset_descriptor *phosphor_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_phosphor_presets) / sizeof(g_phosphor_presets[0]));
    }

    return g_phosphor_presets;
}

const phosphor_preset_values *phosphor_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = phosphor_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_phosphor_preset_values[index];
        }
    }

    return NULL;
}

const char *phosphor_curve_mode_name(int curve_mode)
{
    switch (curve_mode) {
    case PHOSPHOR_CURVE_HARMONOGRAPH:
        return "harmonograph";

    case PHOSPHOR_CURVE_DENSE:
        return "dense";

    case PHOSPHOR_CURVE_LISSAJOUS:
    default:
        return "lissajous";
    }
}

const char *phosphor_persistence_mode_name(int persistence_mode)
{
    switch (persistence_mode) {
    case PHOSPHOR_PERSISTENCE_SHORT:
        return "short";

    case PHOSPHOR_PERSISTENCE_LONG:
        return "long";

    case PHOSPHOR_PERSISTENCE_STANDARD:
    default:
        return "standard";
    }
}

const char *phosphor_drift_mode_name(int drift_mode)
{
    switch (drift_mode) {
    case PHOSPHOR_DRIFT_CALM:
        return "calm";

    case PHOSPHOR_DRIFT_WIDE:
        return "wide";

    case PHOSPHOR_DRIFT_STANDARD:
    default:
        return "standard";
    }
}

const char *phosphor_mirror_mode_name(int mirror_mode)
{
    switch (mirror_mode) {
    case PHOSPHOR_MIRROR_HORIZONTAL:
        return "horizontal";

    case PHOSPHOR_MIRROR_QUAD:
        return "quad";

    case PHOSPHOR_MIRROR_NONE:
    default:
        return "none";
    }
}

void phosphor_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    phosphor_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const phosphor_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = phosphor_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = phosphor_find_preset_values(preset_key);
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
