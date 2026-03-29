#include "signals_internal.h"

static const screensave_saver_config_hooks g_signals_config_hooks = {
    sizeof(signals_config),
    signals_config_set_defaults,
    signals_config_clamp,
    signals_config_load,
    signals_config_save,
    signals_config_show_dialog
};

static const screensave_saver_callbacks g_signals_callbacks = {
    signals_create_session,
    signals_destroy_session,
    signals_resize_session,
    signals_step_session,
    signals_render_session
};

static const screensave_saver_module g_signals_module = {
    {
        "signals",
        "Signals",
        "Synthetic instrument saver with scopes, meters, counters, and restrained console activity."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_signals_presets,
    SIGNALS_PRESET_COUNT,
    g_signals_themes,
    SIGNALS_THEME_COUNT,
    &g_signals_config_hooks,
    &g_signals_callbacks
};

const screensave_saver_module *signals_get_module(void)
{
    return &g_signals_module;
}
