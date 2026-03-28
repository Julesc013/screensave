#include <string.h>

#include "pipeworks_internal.h"

const screensave_preset_descriptor g_pipeworks_presets[] = {
    {
        "industrial_pipes",
        "Industrial Pipes",
        "Warm industrial growth with balanced branching and a readable rebuild rhythm.",
        "industrial_pipes",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "circuit_trace",
        "Circuit Trace",
        "Finer grid construction with brisk growth and brighter trace flow.",
        "circuit_trace",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000B11UL
    },
    {
        "clean_workstation",
        "Clean Workstation",
        "Open white-gray layout with restrained growth and clear structural rhythm.",
        "clean_workstation",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000B12UL
    },
    {
        "phosphor_grid",
        "Phosphor Grid",
        "Terminal-like green presentation with steady growth and restrained travel highlights.",
        "phosphor_grid",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000B13UL
    },
    {
        "quiet_night",
        "Quiet Night",
        "Low-distraction dark presentation with patient growth and slower rebuilds.",
        "quiet_night",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const pipeworks_preset_values g_pipeworks_preset_values[] = {
    { PIPEWORKS_DENSITY_STANDARD, PIPEWORKS_SPEED_STANDARD, PIPEWORKS_BRANCH_BALANCED, PIPEWORKS_REBUILD_CYCLE },
    { PIPEWORKS_DENSITY_TIGHT, PIPEWORKS_SPEED_BRISK, PIPEWORKS_BRANCH_WILD, PIPEWORKS_REBUILD_FREQUENT },
    { PIPEWORKS_DENSITY_OPEN, PIPEWORKS_SPEED_PATIENT, PIPEWORKS_BRANCH_ORDERLY, PIPEWORKS_REBUILD_PATIENT },
    { PIPEWORKS_DENSITY_STANDARD, PIPEWORKS_SPEED_STANDARD, PIPEWORKS_BRANCH_BALANCED, PIPEWORKS_REBUILD_CYCLE },
    { PIPEWORKS_DENSITY_OPEN, PIPEWORKS_SPEED_PATIENT, PIPEWORKS_BRANCH_ORDERLY, PIPEWORKS_REBUILD_PATIENT }
};

const screensave_preset_descriptor *pipeworks_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_pipeworks_presets) / sizeof(g_pipeworks_presets[0]));
    }

    return g_pipeworks_presets;
}

const pipeworks_preset_values *pipeworks_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = pipeworks_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_pipeworks_preset_values[index];
        }
    }

    return NULL;
}

const char *pipeworks_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case PIPEWORKS_DENSITY_TIGHT:
        return "tight";

    case PIPEWORKS_DENSITY_OPEN:
        return "open";

    case PIPEWORKS_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

const char *pipeworks_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case PIPEWORKS_SPEED_PATIENT:
        return "patient";

    case PIPEWORKS_SPEED_BRISK:
        return "brisk";

    case PIPEWORKS_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *pipeworks_branch_mode_name(int branch_mode)
{
    switch (branch_mode) {
    case PIPEWORKS_BRANCH_ORDERLY:
        return "orderly";

    case PIPEWORKS_BRANCH_WILD:
        return "wild";

    case PIPEWORKS_BRANCH_BALANCED:
    default:
        return "balanced";
    }
}

const char *pipeworks_rebuild_mode_name(int rebuild_mode)
{
    switch (rebuild_mode) {
    case PIPEWORKS_REBUILD_PATIENT:
        return "patient";

    case PIPEWORKS_REBUILD_FREQUENT:
        return "frequent";

    case PIPEWORKS_REBUILD_CYCLE:
    default:
        return "cycle";
    }
}

void pipeworks_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    pipeworks_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const pipeworks_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = pipeworks_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = pipeworks_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->density_mode = preset_values->density_mode;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->branch_mode = preset_values->branch_mode;
    product_config->rebuild_mode = preset_values->rebuild_mode;
}
