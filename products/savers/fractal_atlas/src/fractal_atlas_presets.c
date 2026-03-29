#include <string.h>

#include "fractal_atlas_internal.h"

const screensave_preset_descriptor g_fractal_atlas_presets[] = {
    {
        "atlas_monochrome",
        "Atlas Monochrome",
        "Curated monochrome Mandelbrot atlas with slow settling refinement.",
        "atlas_monochrome",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "liquid_nebula",
        "Liquid Nebula",
        "Richer voyage preset with brighter color travel through curated deep structures.",
        "liquid_nebula",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000E61UL
    },
    {
        "terminal_mathematics",
        "Terminal Mathematics",
        "Green Julia-study presentation with restrained palette drift and standard refinement.",
        "terminal_mathematics",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000E62UL
    },
    {
        "deep_ultraviolet",
        "Deep Ultraviolet",
        "Faster zoom voyage through darker ultraviolet coordinates with fine refinement.",
        "deep_ultraviolet",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000E63UL
    },
    {
        "museum_print_room",
        "Museum Print Room",
        "Low-detail still atlas tuned for calmer long unattended runs.",
        "museum_print_room",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const fractal_atlas_preset_values g_fractal_atlas_preset_values[] = {
    { FRACTAL_ATLAS_MODE_ATLAS, FRACTAL_ATLAS_SPEED_STILL, FRACTAL_ATLAS_REFINEMENT_STANDARD },
    { FRACTAL_ATLAS_MODE_VOYAGE, FRACTAL_ATLAS_SPEED_STANDARD, FRACTAL_ATLAS_REFINEMENT_FINE },
    { FRACTAL_ATLAS_MODE_JULIA, FRACTAL_ATLAS_SPEED_STANDARD, FRACTAL_ATLAS_REFINEMENT_STANDARD },
    { FRACTAL_ATLAS_MODE_VOYAGE, FRACTAL_ATLAS_SPEED_BRISK, FRACTAL_ATLAS_REFINEMENT_FINE },
    { FRACTAL_ATLAS_MODE_ATLAS, FRACTAL_ATLAS_SPEED_STILL, FRACTAL_ATLAS_REFINEMENT_DRAFT }
};

const screensave_preset_descriptor *fractal_atlas_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_fractal_atlas_presets) / sizeof(g_fractal_atlas_presets[0]));
    }

    return g_fractal_atlas_presets;
}

const fractal_atlas_preset_values *fractal_atlas_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = fractal_atlas_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_fractal_atlas_preset_values[index];
        }
    }

    return NULL;
}

const char *fractal_atlas_mode_name(int mode)
{
    switch (mode) {
    case FRACTAL_ATLAS_MODE_VOYAGE:
        return "voyage";
    case FRACTAL_ATLAS_MODE_JULIA:
        return "julia";
    case FRACTAL_ATLAS_MODE_ATLAS:
    default:
        return "atlas";
    }
}

const char *fractal_atlas_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case FRACTAL_ATLAS_SPEED_STILL:
        return "still";
    case FRACTAL_ATLAS_SPEED_BRISK:
        return "brisk";
    case FRACTAL_ATLAS_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *fractal_atlas_refinement_mode_name(int refinement_mode)
{
    switch (refinement_mode) {
    case FRACTAL_ATLAS_REFINEMENT_DRAFT:
        return "draft";
    case FRACTAL_ATLAS_REFINEMENT_FINE:
        return "fine";
    case FRACTAL_ATLAS_REFINEMENT_STANDARD:
    default:
        return "standard";
    }
}

void fractal_atlas_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    fractal_atlas_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const fractal_atlas_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = fractal_atlas_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = fractal_atlas_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->mode = preset_values->mode;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->refinement_mode = preset_values->refinement_mode;
}
