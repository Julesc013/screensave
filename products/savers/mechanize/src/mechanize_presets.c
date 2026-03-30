#include <string.h>

#include "mechanize_internal.h"

const screensave_preset_descriptor g_mechanize_presets[] = {
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
    },
    {
        "copper_counterworks",
        "Copper Counterworks",
        "Copper-toned counter assembly with slower exhibit pacing and cleaner event moments.",
        "copper_foundry",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000C44UL
    },
    {
        "ivory_gallery",
        "Ivory Gallery",
        "Ivory exhibit assembly tuned for patient museum-like mechanical motion.",
        "ivory_gallery",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000C45UL
    }
};

static const mechanize_preset_values g_mechanize_preset_values[] = {
    { MECHANIZE_SCENE_GEAR_TRAIN, MECHANIZE_SPEED_STANDARD, MECHANIZE_DENSITY_STANDARD },
    { MECHANIZE_SCENE_CAM_BANK, MECHANIZE_SPEED_STANDARD, MECHANIZE_DENSITY_DENSE },
    { MECHANIZE_SCENE_DIAL_ASSEMBLY, MECHANIZE_SPEED_PATIENT, MECHANIZE_DENSITY_STANDARD },
    { MECHANIZE_SCENE_GEAR_TRAIN, MECHANIZE_SPEED_BRISK, MECHANIZE_DENSITY_DENSE },
    { MECHANIZE_SCENE_DIAL_ASSEMBLY, MECHANIZE_SPEED_PATIENT, MECHANIZE_DENSITY_SPARSE },
    { MECHANIZE_SCENE_CAM_BANK, MECHANIZE_SPEED_STANDARD, MECHANIZE_DENSITY_STANDARD },
    { MECHANIZE_SCENE_DIAL_ASSEMBLY, MECHANIZE_SPEED_PATIENT, MECHANIZE_DENSITY_SPARSE }
};

const screensave_preset_descriptor *mechanize_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_mechanize_presets) / sizeof(g_mechanize_presets[0]));
    }

    return g_mechanize_presets;
}

const mechanize_preset_values *mechanize_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = mechanize_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_mechanize_preset_values[index];
        }
    }

    return NULL;
}

const char *mechanize_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case MECHANIZE_SCENE_CAM_BANK:
        return "cam_bank";

    case MECHANIZE_SCENE_DIAL_ASSEMBLY:
        return "dial_assembly";

    case MECHANIZE_SCENE_GEAR_TRAIN:
    default:
        return "gear_train";
    }
}

const char *mechanize_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case MECHANIZE_SPEED_PATIENT:
        return "patient";

    case MECHANIZE_SPEED_BRISK:
        return "brisk";

    case MECHANIZE_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *mechanize_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case MECHANIZE_DENSITY_SPARSE:
        return "sparse";

    case MECHANIZE_DENSITY_DENSE:
        return "dense";

    case MECHANIZE_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

void mechanize_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    mechanize_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const mechanize_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = mechanize_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = mechanize_find_preset_values(preset_key);
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
