#include "signal_lab_internal.h"

static const screensave_saver_config_hooks g_signal_lab_config_hooks = {
    sizeof(signal_lab_config),
    signal_lab_config_set_defaults,
    signal_lab_config_clamp,
    signal_lab_config_load,
    signal_lab_config_save,
    signal_lab_config_show_dialog
};

static const screensave_saver_callbacks g_signal_lab_callbacks = {
    signal_lab_create_session,
    signal_lab_destroy_session,
    signal_lab_resize_session,
    signal_lab_step_session,
    signal_lab_render_session
};

static const screensave_saver_module g_signal_lab_module = {
    {
        "signal_lab",
        "Signal Lab",
        "Synthetic instrument saver with scopes, meters, counters, and restrained console activity."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_signal_lab_presets,
    SIGNAL_LAB_PRESET_COUNT,
    g_signal_lab_themes,
    SIGNAL_LAB_THEME_COUNT,
    &g_signal_lab_config_hooks,
    &g_signal_lab_callbacks
};

const screensave_saver_module *signal_lab_get_module(void)
{
    return &g_signal_lab_module;
}
