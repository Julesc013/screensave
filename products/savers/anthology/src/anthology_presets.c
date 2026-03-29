#include <string.h>

#include "anthology_internal.h"

const screensave_preset_descriptor g_anthology_presets[] = {
    {
        "balanced_mix",
        "Balanced Mix",
        "Weighted cross-saver rotation across the full line with renderer-aware preference and stable pacing.",
        "anthology_neutral",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000801UL
    },
    {
        "weighted_showcase",
        "Weighted Showcase",
        "Bias toward heavyweight and renderer-showcase savers while keeping the full line eligible.",
        "anthology_neutral",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000802UL
    },
    {
        "places_and_ambient",
        "Places And Ambient",
        "Quiet, systems, and scenic anthology pass with a calmer duration profile.",
        "anthology_neutral",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000803UL
    }
};

static const anthology_preset_values g_anthology_preset_values[] = {
    {
        ANTHOLOGY_SELECTION_WEIGHTED_RANDOM,
        90UL,
        ANTHOLOGY_FAMILY_QUIET |
            ANTHOLOGY_FAMILY_MOTION |
            ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR |
            ANTHOLOGY_FAMILY_GRID_SIMULATION |
            ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT |
            ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE |
            ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS |
            ANTHOLOGY_FAMILY_SHOWCASE,
        0x0003FFFFUL,
        0UL,
        ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER,
        0,
        0,
        1,
        {
            100UL, 100UL, 100UL, 100UL, 100UL, 100UL, 100UL, 100UL, 100UL,
            100UL, 100UL, 100UL, 100UL, 100UL, 100UL, 100UL, 100UL, 100UL
        }
    },
    {
        ANTHOLOGY_SELECTION_WEIGHTED_RANDOM,
        75UL,
        ANTHOLOGY_FAMILY_QUIET |
            ANTHOLOGY_FAMILY_MOTION |
            ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR |
            ANTHOLOGY_FAMILY_GRID_SIMULATION |
            ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT |
            ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE |
            ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS |
            ANTHOLOGY_FAMILY_SHOWCASE,
        0x0003FFFFUL,
        0U |
            (1UL << 13) |
            (1UL << 14) |
            (1UL << 15) |
            (1UL << 16) |
            (1UL << 17),
        ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER,
        1,
        1,
        1,
        {
            60UL, 60UL, 60UL, 80UL, 80UL, 70UL, 70UL, 70UL, 70UL,
            70UL, 70UL, 70UL, 70UL, 140UL, 140UL, 140UL, 150UL, 180UL
        }
    },
    {
        ANTHOLOGY_SELECTION_FAVORITES_FIRST,
        120UL,
        ANTHOLOGY_FAMILY_QUIET |
            ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT |
            ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE,
        0x0003FFFFUL,
        0U |
            (1UL << 0) |
            (1UL << 9) |
            (1UL << 10) |
            (1UL << 11) |
            (1UL << 12),
        ANTHOLOGY_RENDERER_POLICY_BASELINE_COMPATIBLE,
        0,
        0,
        1,
        {
            120UL, 60UL, 60UL, 60UL, 60UL, 70UL, 70UL, 110UL, 110UL,
            120UL, 120UL, 120UL, 120UL, 60UL, 60UL, 60UL, 60UL, 40UL
        }
    }
};

const screensave_preset_descriptor *anthology_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_anthology_presets) / sizeof(g_anthology_presets[0]));
    }

    return g_anthology_presets;
}

const anthology_preset_values *anthology_find_preset_values(const char *preset_key)
{
    unsigned int index;

    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < ANTHOLOGY_PRESET_COUNT; ++index) {
        if (
            g_anthology_presets[index].preset_key != NULL &&
            strcmp(g_anthology_presets[index].preset_key, preset_key) == 0
        ) {
            return &g_anthology_preset_values[index];
        }
    }

    return NULL;
}

void anthology_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    anthology_config *product_config
)
{
    const screensave_preset_descriptor *preset_descriptor;
    const anthology_preset_values *preset_values;
    unsigned int index;

    preset_descriptor = screensave_find_preset(g_anthology_presets, ANTHOLOGY_PRESET_COUNT, preset_key);
    preset_values = anthology_find_preset_values(preset_key);
    if (common_config == NULL || product_config == NULL || preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->selection_mode = preset_values->selection_mode;
    product_config->interval_seconds = preset_values->interval_seconds;
    product_config->family_mask = preset_values->family_mask;
    product_config->include_mask = preset_values->include_mask;
    product_config->favorite_mask = preset_values->favorite_mask;
    product_config->renderer_policy = preset_values->renderer_policy;
    product_config->randomize_inner_presets = preset_values->randomize_inner_presets;
    product_config->randomize_inner_themes = preset_values->randomize_inner_themes;
    product_config->respect_inner_randomization = preset_values->respect_inner_randomization;

    for (index = 0U; index < ANTHOLOGY_INNER_SAVER_COUNT; ++index) {
        product_config->saver_weights[index] = preset_values->saver_weights[index];
    }
}
