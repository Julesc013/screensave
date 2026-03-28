#include <string.h>

#include "mechanical_dreams_internal.h"

const screensave_preset_descriptor g_mechanical_dreams_presets[] = {
    {
        "brass_gear_train",
        "Brass Gear Train",
        "Balanced workshop gear assembly with warm brass highlights.",
        "brass_workshop",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "steel_cam_bank",
        "Steel Cam Bank",
        "Cooler machine-room cam bank with denser followers and pulse moments.",
        "steel_machine_room",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C41UL
    },
    {
        "black_instrument",
        "Black Instrument",
        "Black enamel dial cluster with slower instrument-like choreography.",
        "black_enamel_instrument",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000C42UL
    },
    {
        "industrial_green",
        "Industrial Green",
        "Brisker gear scene with industrial green machine-room tones.",
        "industrial_green",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C43UL
    },
    {
        "quiet_museum",
        "Quiet Museum",
        "Sparse museum display tuned for patient long runs.",
        "quiet_museum",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const mechanical_dreams_preset_values g_mechanical_dreams_preset_values[] = {
    { MECHANICAL_DREAMS_SCENE_GEAR_TRAIN, MECHANICAL_DREAMS_SPEED_STANDARD, MECHANICAL_DREAMS_DENSITY_STANDARD },
    { MECHANICAL_DREAMS_SCENE_CAM_BANK, MECHANICAL_DREAMS_SPEED_STANDARD, MECHANICAL_DREAMS_DENSITY_DENSE },
    { MECHANICAL_DREAMS_SCENE_DIAL_ASSEMBLY, MECHANICAL_DREAMS_SPEED_PATIENT, MECHANICAL_DREAMS_DENSITY_STANDARD },
    { MECHANICAL_DREAMS_SCENE_GEAR_TRAIN, MECHANICAL_DREAMS_SPEED_BRISK, MECHANICAL_DREAMS_DENSITY_DENSE },
    { MECHANICAL_DREAMS_SCENE_DIAL_ASSEMBLY, MECHANICAL_DREAMS_SPEED_PATIENT, MECHANICAL_DREAMS_DENSITY_SPARSE }
};

const screensave_preset_descriptor *mechanical_dreams_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_mechanical_dreams_presets) / sizeof(g_mechanical_dreams_presets[0]));
    }

    return g_mechanical_dreams_presets;
}

const mechanical_dreams_preset_values *mechanical_dreams_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = mechanical_dreams_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_mechanical_dreams_preset_values[index];
        }
    }

    return NULL;
}

const char *mechanical_dreams_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case MECHANICAL_DREAMS_SCENE_CAM_BANK:
        return "cam_bank";

    case MECHANICAL_DREAMS_SCENE_DIAL_ASSEMBLY:
        return "dial_assembly";

    case MECHANICAL_DREAMS_SCENE_GEAR_TRAIN:
    default:
        return "gear_train";
    }
}

const char *mechanical_dreams_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case MECHANICAL_DREAMS_SPEED_PATIENT:
        return "patient";

    case MECHANICAL_DREAMS_SPEED_BRISK:
        return "brisk";

    case MECHANICAL_DREAMS_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *mechanical_dreams_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case MECHANICAL_DREAMS_DENSITY_SPARSE:
        return "sparse";

    case MECHANICAL_DREAMS_DENSITY_DENSE:
        return "dense";

    case MECHANICAL_DREAMS_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

void mechanical_dreams_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    mechanical_dreams_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const mechanical_dreams_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = mechanical_dreams_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = mechanical_dreams_find_preset_values(preset_key);
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
    product_config->density_mode = preset_values->density_mode;
}
