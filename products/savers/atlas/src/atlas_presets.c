#include <string.h>

#include "atlas_internal.h"

const screensave_preset_descriptor g_atlas_presets[] = {
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
    },
    {
        "nebula_voyage",
        "Nebula Voyage",
        "Curated deep-space fractal route with slower holds and richer palette travel.",
        "nebula_voyage",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000E64UL
    },
    {
        "mathematics_plate",
        "Mathematics Plate",
        "Still mathematical plate study with restrained refinement and museum-print pacing.",
        "mathematics_plate",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000E65UL
    }
};

static const atlas_preset_values g_atlas_preset_values[] = {
    { ATLAS_MODE_ATLAS, ATLAS_SPEED_STILL, ATLAS_REFINEMENT_STANDARD },
    { ATLAS_MODE_VOYAGE, ATLAS_SPEED_STANDARD, ATLAS_REFINEMENT_FINE },
    { ATLAS_MODE_JULIA, ATLAS_SPEED_STANDARD, ATLAS_REFINEMENT_STANDARD },
    { ATLAS_MODE_VOYAGE, ATLAS_SPEED_BRISK, ATLAS_REFINEMENT_FINE },
    { ATLAS_MODE_ATLAS, ATLAS_SPEED_STILL, ATLAS_REFINEMENT_DRAFT },
    { ATLAS_MODE_VOYAGE, ATLAS_SPEED_STANDARD, ATLAS_REFINEMENT_FINE },
    { ATLAS_MODE_JULIA, ATLAS_SPEED_STILL, ATLAS_REFINEMENT_DRAFT }
};

const screensave_preset_descriptor *atlas_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_atlas_presets) / sizeof(g_atlas_presets[0]));
    }

    return g_atlas_presets;
}

const atlas_preset_values *atlas_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = atlas_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_atlas_preset_values[index];
        }
    }

    return NULL;
}

const char *atlas_mode_name(int mode)
{
    switch (mode) {
    case ATLAS_MODE_VOYAGE:
        return "voyage";
    case ATLAS_MODE_JULIA:
        return "julia";
    case ATLAS_MODE_ATLAS:
    default:
        return "atlas";
    }
}

const char *atlas_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case ATLAS_SPEED_STILL:
        return "still";
    case ATLAS_SPEED_BRISK:
        return "brisk";
    case ATLAS_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *atlas_refinement_mode_name(int refinement_mode)
{
    switch (refinement_mode) {
    case ATLAS_REFINEMENT_DRAFT:
        return "draft";
    case ATLAS_REFINEMENT_FINE:
        return "fine";
    case ATLAS_REFINEMENT_STANDARD:
    default:
        return "standard";
    }
}

void atlas_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    atlas_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const atlas_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = atlas_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = atlas_find_preset_values(preset_key);
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
