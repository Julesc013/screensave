#include <stdlib.h>

#include "scr_internal.h"

static const screensave_saver_config_hooks *scr_get_config_hooks(const screensave_saver_module *module)
{
    if (module == NULL) {
        return NULL;
    }

    return module->config_hooks;
}

int scr_settings_init(const screensave_saver_module *module, scr_settings *settings)
{
    const screensave_saver_config_hooks *config_hooks;

    if (settings == NULL) {
        return 0;
    }

    ZeroMemory(settings, sizeof(*settings));
    config_hooks = scr_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->product_config_size == 0U) {
        return 1;
    }

    settings->product_config = malloc(config_hooks->product_config_size);
    if (settings->product_config == NULL) {
        return 0;
    }

    settings->product_config_size = config_hooks->product_config_size;
    ZeroMemory(settings->product_config, settings->product_config_size);
    return 1;
}

void scr_settings_dispose(scr_settings *settings)
{
    if (settings == NULL) {
        return;
    }

    if (settings->product_config != NULL) {
        free(settings->product_config);
    }

    ZeroMemory(settings, sizeof(*settings));
}

void scr_settings_set_defaults(const screensave_saver_module *module, scr_settings *settings)
{
    const screensave_saver_config_hooks *config_hooks;

    if (settings == NULL) {
        return;
    }

    screensave_common_config_set_defaults(&settings->common);
    if (settings->product_config != NULL && settings->product_config_size > 0U) {
        ZeroMemory(settings->product_config, settings->product_config_size);
    }

    config_hooks = scr_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->set_defaults != NULL) {
        config_hooks->set_defaults(&settings->common, settings->product_config, settings->product_config_size);
    }
}

void scr_settings_clamp(const screensave_saver_module *module, scr_settings *settings)
{
    const screensave_saver_config_hooks *config_hooks;

    if (settings == NULL) {
        return;
    }

    screensave_common_config_clamp(&settings->common);
    config_hooks = scr_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->clamp != NULL) {
        config_hooks->clamp(&settings->common, settings->product_config, settings->product_config_size);
    }
}

int scr_settings_load(
    const screensave_saver_module *module,
    scr_settings *settings,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (settings == NULL) {
        return 0;
    }

    config_hooks = scr_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->load_config == NULL) {
        return 1;
    }

    return config_hooks->load_config(
        module,
        &settings->common,
        settings->product_config,
        settings->product_config_size,
        diagnostics
    );
}

int scr_settings_save(
    const screensave_saver_module *module,
    const scr_settings *settings,
    screensave_diag_context *diagnostics
)
{
    const screensave_saver_config_hooks *config_hooks;

    if (settings == NULL) {
        return 0;
    }

    config_hooks = scr_get_config_hooks(module);
    if (config_hooks == NULL || config_hooks->save_config == NULL) {
        return 1;
    }

    return config_hooks->save_config(
        module,
        &settings->common,
        settings->product_config,
        settings->product_config_size,
        diagnostics
    );
}
