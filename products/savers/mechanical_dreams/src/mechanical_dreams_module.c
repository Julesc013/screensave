#include "mechanical_dreams_internal.h"

static const screensave_saver_config_hooks g_mechanical_dreams_config_hooks = {
    sizeof(mechanical_dreams_config),
    mechanical_dreams_config_set_defaults,
    mechanical_dreams_config_clamp,
    mechanical_dreams_config_load,
    mechanical_dreams_config_save,
    mechanical_dreams_config_show_dialog
};

static const screensave_saver_callbacks g_mechanical_dreams_callbacks = {
    mechanical_dreams_create_session,
    mechanical_dreams_destroy_session,
    mechanical_dreams_resize_session,
    mechanical_dreams_step_session,
    mechanical_dreams_render_session
};

static const screensave_saver_module g_mechanical_dreams_module = {
    {
        "mechanical_dreams",
        "Mechanical Dreams",
        "Slow mechanical saver with coupled gears, cams, dials, and restrained event moments."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_mechanical_dreams_presets,
    MECHANICAL_DREAMS_PRESET_COUNT,
    g_mechanical_dreams_themes,
    MECHANICAL_DREAMS_THEME_COUNT,
    &g_mechanical_dreams_config_hooks,
    &g_mechanical_dreams_callbacks
};

const screensave_saver_module *mechanical_dreams_get_module(void)
{
    return &g_mechanical_dreams_module;
}
