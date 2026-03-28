#include "oscilloscope_dreams_internal.h"

static const screensave_saver_config_hooks g_oscilloscope_dreams_config_hooks = {
    sizeof(oscilloscope_dreams_config),
    oscilloscope_dreams_config_set_defaults,
    oscilloscope_dreams_config_clamp,
    oscilloscope_dreams_config_load,
    oscilloscope_dreams_config_save,
    oscilloscope_dreams_config_show_dialog
};

static const screensave_saver_callbacks g_oscilloscope_dreams_callbacks = {
    oscilloscope_dreams_create_session,
    oscilloscope_dreams_destroy_session,
    oscilloscope_dreams_resize_session,
    oscilloscope_dreams_step_session,
    oscilloscope_dreams_render_session
};

static const screensave_saver_module g_oscilloscope_dreams_module = {
    {
        "oscilloscope_dreams",
        "Oscilloscope Dreams",
        "Vector-style phosphor saver with curated curve families and persistent trails."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_oscilloscope_dreams_presets,
    OSCILLOSCOPE_DREAMS_PRESET_COUNT,
    g_oscilloscope_dreams_themes,
    OSCILLOSCOPE_DREAMS_THEME_COUNT,
    &g_oscilloscope_dreams_config_hooks,
    &g_oscilloscope_dreams_callbacks
};

const screensave_saver_module *oscilloscope_dreams_get_module(void)
{
    return &g_oscilloscope_dreams_module;
}
