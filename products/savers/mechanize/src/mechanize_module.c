#include "mechanize_internal.h"

static const screensave_saver_config_hooks g_mechanize_config_hooks = {
    sizeof(mechanize_config),
    mechanize_config_set_defaults,
    mechanize_config_clamp,
    mechanize_config_load,
    mechanize_config_save,
    mechanize_config_show_dialog
};

static const screensave_saver_callbacks g_mechanize_callbacks = {
    mechanize_create_session,
    mechanize_destroy_session,
    mechanize_resize_session,
    mechanize_step_session,
    mechanize_render_session
};

static const screensave_saver_module g_mechanize_module = {
    {
        "mechanize",
        "Mechanize",
        "Slow mechanical saver with coupled gears, cams, dials, and restrained event moments."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_mechanize_presets,
    MECHANIZE_PRESET_COUNT,
    g_mechanize_themes,
    MECHANIZE_THEME_COUNT,
    &g_mechanize_config_hooks,
    &g_mechanize_callbacks
};

const screensave_saver_module *mechanize_get_module(void)
{
    return &g_mechanize_module;
}
