#include <stdlib.h>

#include "screensave/saver_api.h"

int screensave_saver_module_is_valid(const screensave_saver_module *module)
{
    const screensave_saver_config_hooks *config_hooks;

    if (module == NULL) {
        return 0;
    }

    if (
        module->identity.product_key == NULL ||
        module->identity.display_name == NULL ||
        module->identity.product_key[0] == '\0' ||
        module->identity.display_name[0] == '\0'
    ) {
        return 0;
    }

    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI) == 0UL) {
        return 0;
    }

    if (module->preset_count > 0U && module->presets == NULL) {
        return 0;
    }

    if (module->theme_count > 0U && module->themes == NULL) {
        return 0;
    }

    config_hooks = module->config_hooks;
    if (config_hooks != NULL) {
        if (config_hooks->set_defaults == NULL || config_hooks->clamp == NULL) {
            return 0;
        }
    }

    return 1;
}

static const screensave_saver_config_hooks *screensave_saver_get_config_hooks(
    const screensave_saver_module *module
)
{
    if (module == NULL) {
        return NULL;
    }

    return module->config_hooks;
}

int screensave_saver_config_state_init(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return 0;
    }

    ZeroMemory(config_state, sizeof(*config_state));
    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->product_config_size == 0U) {
        return 1;
    }

    config_state->product_config = malloc(config_hooks->product_config_size);
    if (config_state->product_config == NULL) {
        return 0;
    }

    config_state->product_config_size = config_hooks->product_config_size;
    ZeroMemory(config_state->product_config, config_state->product_config_size);
    return 1;
}

void screensave_saver_config_state_dispose(screensave_saver_config_state *config_state)
{
    if (config_state == NULL) {
        return;
    }

    if (config_state->product_config != NULL) {
        free(config_state->product_config);
    }

    ZeroMemory(config_state, sizeof(*config_state));
}

void screensave_saver_config_state_set_defaults(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return;
    }

    screensave_common_config_set_defaults(&config_state->common);
    if (config_state->product_config != NULL && config_state->product_config_size > 0U) {
        ZeroMemory(config_state->product_config, config_state->product_config_size);
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->set_defaults != NULL) {
        config_hooks->set_defaults(
            &config_state->common,
            config_state->product_config,
            config_state->product_config_size
        );
    }
}

void screensave_saver_config_state_clamp(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return;
    }

    screensave_common_config_clamp(&config_state->common);
    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->clamp != NULL) {
        config_hooks->clamp(
            &config_state->common,
            config_state->product_config,
            config_state->product_config_size
        );
    }
}

int screensave_saver_config_state_load(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return 0;
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->load_config == NULL) {
        return 1;
    }

    return config_hooks->load_config(
        module,
        &config_state->common,
        config_state->product_config,
        config_state->product_config_size,
        diagnostics
    );
}

int screensave_saver_config_state_save(
    const screensave_saver_module *module,
    const screensave_saver_config_state *config_state,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (config_state == NULL) {
        return 0;
    }

    config_hooks = screensave_saver_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->save_config == NULL) {
        return 1;
    }

    return config_hooks->save_config(
        module,
        &config_state->common,
        config_state->product_config,
        config_state->product_config_size,
        diagnostics
    );
}

const char *screensave_session_mode_name(screensave_session_mode mode)
{
    switch (mode) {
    case SCREENSAVE_SESSION_MODE_SCREEN:
        return "screen";

    case SCREENSAVE_SESSION_MODE_PREVIEW:
        return "preview";

    case SCREENSAVE_SESSION_MODE_WINDOWED:
        return "windowed";

    case SCREENSAVE_SESSION_MODE_CONFIG:
    default:
        return "config";
    }
}
