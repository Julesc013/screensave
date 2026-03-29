#include <string.h>

#include "screensave/config_api.h"

static int screensave_text_is_empty(const char *text)
{
    return text == NULL || text[0] == '\0';
}

void screensave_common_config_set_defaults(screensave_common_config *config)
{
    if (config == NULL) {
        return;
    }

    config->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    config->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    config->diagnostics_overlay_enabled = 0;
    config->use_deterministic_seed = 0;
    config->deterministic_seed = 0UL;
    config->randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_OFF;
    config->randomization_scope = 0UL;
    config->preset_key = NULL;
    config->theme_key = NULL;
}

void screensave_common_config_clamp(screensave_common_config *config)
{
    if (config == NULL) {
        return;
    }

    if (config->schema_version != SCREENSAVE_CONFIG_SCHEMA_VERSION) {
        config->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    }

    if (config->detail_level < SCREENSAVE_DETAIL_LEVEL_LOW) {
        config->detail_level = SCREENSAVE_DETAIL_LEVEL_LOW;
    } else if (config->detail_level > SCREENSAVE_DETAIL_LEVEL_HIGH) {
        config->detail_level = SCREENSAVE_DETAIL_LEVEL_HIGH;
    }

    config->diagnostics_overlay_enabled = config->diagnostics_overlay_enabled != 0;
    config->use_deterministic_seed = config->use_deterministic_seed != 0;
    if (
        config->randomization_mode != SCREENSAVE_RANDOMIZATION_MODE_OFF &&
        config->randomization_mode != SCREENSAVE_RANDOMIZATION_MODE_SESSION
    ) {
        config->randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_OFF;
    }
    config->randomization_scope &=
        SCREENSAVE_RANDOMIZATION_SCOPE_PRESET |
        SCREENSAVE_RANDOMIZATION_SCOPE_THEME |
        SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL |
        SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT;
    if (
        config->randomization_mode == SCREENSAVE_RANDOMIZATION_MODE_SESSION &&
        config->randomization_scope == 0UL
    ) {
        config->randomization_scope = screensave_randomization_default_scope();
    }
    if (config->randomization_mode == SCREENSAVE_RANDOMIZATION_MODE_OFF) {
        config->randomization_scope = 0UL;
    }

    if (screensave_text_is_empty(config->preset_key)) {
        config->preset_key = NULL;
    }

    if (screensave_text_is_empty(config->theme_key)) {
        config->theme_key = NULL;
    }
}

int screensave_common_config_validate(const screensave_common_config *config, unsigned long *issue_flags)
{
    unsigned long flags;

    flags = 0UL;
    if (config == NULL) {
        flags = SCREENSAVE_CONFIG_ISSUE_BAD_DETAIL;
    } else {
        if (config->schema_version != SCREENSAVE_CONFIG_SCHEMA_VERSION) {
            flags |= SCREENSAVE_CONFIG_ISSUE_BAD_SCHEMA_VERSION;
        }

        if (
            config->detail_level < SCREENSAVE_DETAIL_LEVEL_LOW ||
            config->detail_level > SCREENSAVE_DETAIL_LEVEL_HIGH
        ) {
            flags |= SCREENSAVE_CONFIG_ISSUE_BAD_DETAIL;
        }

        if (
            config->randomization_mode != SCREENSAVE_RANDOMIZATION_MODE_OFF &&
            config->randomization_mode != SCREENSAVE_RANDOMIZATION_MODE_SESSION
        ) {
            flags |= SCREENSAVE_CONFIG_ISSUE_BAD_RANDOMIZATION_MODE;
        }

        if ((config->randomization_scope & ~(
            SCREENSAVE_RANDOMIZATION_SCOPE_PRESET |
            SCREENSAVE_RANDOMIZATION_SCOPE_THEME |
            SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL |
            SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT
        )) != 0UL) {
            flags |= SCREENSAVE_CONFIG_ISSUE_BAD_RANDOMIZATION_SCOPE;
        }

        if (config->preset_key != NULL && config->preset_key[0] == '\0') {
            flags |= SCREENSAVE_CONFIG_ISSUE_EMPTY_PRESET;
        }

        if (config->theme_key != NULL && config->theme_key[0] == '\0') {
            flags |= SCREENSAVE_CONFIG_ISSUE_EMPTY_THEME;
        }
    }

    if (issue_flags != NULL) {
        *issue_flags = flags;
    }

    return flags == 0UL;
}

void screensave_config_binding_init(
    screensave_config_binding *binding,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size
)
{
    if (binding == NULL) {
        return;
    }

    binding->common_config = common_config;
    binding->product_config = product_config;
    binding->product_config_size = product_config_size;
}

const screensave_preset_descriptor *screensave_find_preset(
    const screensave_preset_descriptor *presets,
    unsigned int preset_count,
    const char *preset_key
)
{
    unsigned int index;

    if (presets == NULL || preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (
            presets[index].preset_key != NULL &&
            strcmp(presets[index].preset_key, preset_key) == 0
        ) {
            return &presets[index];
        }
    }

    return NULL;
}

const screensave_theme_descriptor *screensave_find_theme(
    const screensave_theme_descriptor *themes,
    unsigned int theme_count,
    const char *theme_key
)
{
    unsigned int index;

    if (themes == NULL || theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < theme_count; ++index) {
        if (
            themes[index].theme_key != NULL &&
            strcmp(themes[index].theme_key, theme_key) == 0
        ) {
            return &themes[index];
        }
    }

    return NULL;
}

const char *screensave_detail_level_name(screensave_detail_level level)
{
    switch (level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        return "low";

    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        return "high";

    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        return "standard";
    }
}

const char *screensave_seed_mode_name(const screensave_common_config *config)
{
    if (config != NULL && config->use_deterministic_seed) {
        return "fixed";
    }

    return "session";
}

const char *screensave_randomization_mode_name(screensave_randomization_mode mode)
{
    switch (mode) {
    case SCREENSAVE_RANDOMIZATION_MODE_SESSION:
        return "session";

    case SCREENSAVE_RANDOMIZATION_MODE_OFF:
    default:
        return "off";
    }
}

unsigned long screensave_randomization_default_scope(void)
{
    return
        SCREENSAVE_RANDOMIZATION_SCOPE_PRESET |
        SCREENSAVE_RANDOMIZATION_SCOPE_THEME |
        SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL |
        SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT;
}
