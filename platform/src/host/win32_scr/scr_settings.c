#include "scr_internal.h"

int scr_settings_init(const screensave_saver_module *module, scr_settings *settings)
{
    return screensave_saver_config_state_init(module, settings);
}

void scr_settings_dispose(scr_settings *settings)
{
    screensave_saver_config_state_dispose(settings);
}

void scr_settings_set_defaults(const screensave_saver_module *module, scr_settings *settings)
{
    screensave_saver_config_state_set_defaults(module, settings);
}

void scr_settings_clamp(const screensave_saver_module *module, scr_settings *settings)
{
    screensave_saver_config_state_clamp(module, settings);
}

int scr_settings_load(
    const screensave_saver_module *module,
    scr_settings *settings,
    screensave_diag_context *diagnostics
)
{
    return screensave_saver_config_state_load(module, settings, diagnostics);
}

int scr_settings_save(
    const screensave_saver_module *module,
    const scr_settings *settings,
    screensave_diag_context *diagnostics
)
{
    return screensave_saver_config_state_save(module, settings, diagnostics);
}
