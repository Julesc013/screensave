#include <string.h>

#include "lifeforms_internal.h"

const screensave_preset_descriptor g_lifeforms_presets[] = {
    {
        "classic_life",
        "Classic Life",
        "Standard Conway Life with restrained density and steady reseed discipline.",
        "classic_mono",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "age_lab",
        "Age Lab",
        "Denser Conway field with age-colored population studies.",
        "laboratory_age",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000C21UL
    },
    {
        "amber_conservatory",
        "Amber Conservatory",
        "Sparser warm phosphor field tuned for longer quiet runs.",
        "amber_phosphor",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000C22UL
    },
    {
        "green_highlife",
        "Green HighLife",
        "HighLife growth with denser seeding and more assertive reseed response.",
        "green_phosphor",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000C23UL
    },
    {
        "quiet_museum",
        "Quiet Museum",
        "Low-distraction sparse field with conservative reseed behavior.",
        "museum_quiet",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    },
    {
        "garden_bloom",
        "Garden Bloom",
        "Balanced Conway field with garden-like bloom seeding and calm reseed timing.",
        "garden_green",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000C24UL
    },
    {
        "blue_archive",
        "Blue Archive",
        "Cool archive study with sparse seeds, patient reseeds, and calmer long runs.",
        "blue_archive",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000C25UL
    }
};

static const lifeforms_preset_values g_lifeforms_preset_values[] = {
    { LIFEFORMS_RULE_CONWAY, LIFEFORMS_DENSITY_STANDARD, LIFEFORMS_SEED_BALANCED, LIFEFORMS_RESEED_STANDARD },
    { LIFEFORMS_RULE_CONWAY, LIFEFORMS_DENSITY_DENSE, LIFEFORMS_SEED_BLOOM, LIFEFORMS_RESEED_STANDARD },
    { LIFEFORMS_RULE_CONWAY, LIFEFORMS_DENSITY_SPARSE, LIFEFORMS_SEED_BALANCED, LIFEFORMS_RESEED_PATIENT },
    { LIFEFORMS_RULE_HIGHLIFE, LIFEFORMS_DENSITY_DENSE, LIFEFORMS_SEED_BLOOM, LIFEFORMS_RESEED_ALERT },
    { LIFEFORMS_RULE_CONWAY, LIFEFORMS_DENSITY_SPARSE, LIFEFORMS_SEED_SPARSE, LIFEFORMS_RESEED_PATIENT },
    { LIFEFORMS_RULE_CONWAY, LIFEFORMS_DENSITY_STANDARD, LIFEFORMS_SEED_BLOOM, LIFEFORMS_RESEED_STANDARD },
    { LIFEFORMS_RULE_CONWAY, LIFEFORMS_DENSITY_SPARSE, LIFEFORMS_SEED_BALANCED, LIFEFORMS_RESEED_PATIENT }
};

const screensave_preset_descriptor *lifeforms_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_lifeforms_presets) / sizeof(g_lifeforms_presets[0]));
    }

    return g_lifeforms_presets;
}

const lifeforms_preset_values *lifeforms_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = lifeforms_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &g_lifeforms_preset_values[index];
        }
    }

    return NULL;
}

const char *lifeforms_rule_mode_name(int rule_mode)
{
    switch (rule_mode) {
    case LIFEFORMS_RULE_HIGHLIFE:
        return "highlife";

    case LIFEFORMS_RULE_CONWAY:
    default:
        return "conway";
    }
}

const char *lifeforms_density_mode_name(int density_mode)
{
    switch (density_mode) {
    case LIFEFORMS_DENSITY_SPARSE:
        return "sparse";

    case LIFEFORMS_DENSITY_DENSE:
        return "dense";

    case LIFEFORMS_DENSITY_STANDARD:
    default:
        return "standard";
    }
}

const char *lifeforms_seed_mode_name(int seed_mode)
{
    switch (seed_mode) {
    case LIFEFORMS_SEED_SPARSE:
        return "sparse";

    case LIFEFORMS_SEED_BLOOM:
        return "bloom";

    case LIFEFORMS_SEED_BALANCED:
    default:
        return "balanced";
    }
}

const char *lifeforms_reseed_mode_name(int reseed_mode)
{
    switch (reseed_mode) {
    case LIFEFORMS_RESEED_PATIENT:
        return "patient";

    case LIFEFORMS_RESEED_ALERT:
        return "alert";

    case LIFEFORMS_RESEED_STANDARD:
    default:
        return "standard";
    }
}

void lifeforms_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    lifeforms_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const lifeforms_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = lifeforms_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = lifeforms_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->rule_mode = preset_values->rule_mode;
    product_config->density_mode = preset_values->density_mode;
    product_config->seed_mode = preset_values->seed_mode;
    product_config->reseed_mode = preset_values->reseed_mode;
}
