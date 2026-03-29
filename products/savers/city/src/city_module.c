#include "city_internal.h"

static const screensave_saver_config_hooks g_city_config_hooks = {
    sizeof(city_config),
    city_config_set_defaults,
    city_config_clamp,
    city_config_load,
    city_config_save,
    city_config_show_dialog
};

static const screensave_saver_callbacks g_city_callbacks = {
    city_create_session,
    city_destroy_session,
    city_resize_session,
    city_step_session,
    city_render_session
};

static const screensave_saver_module g_city_module = {
    {
        "city",
        "City",
        "Urban night-world saver with skyline, harbor, and rooftop/window-river studies."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_city_presets,
    CITY_PRESET_COUNT,
    g_city_themes,
    CITY_THEME_COUNT,
    &g_city_config_hooks,
    &g_city_callbacks
};

const screensave_saver_module *city_get_module(void)
{
    return &g_city_module;
}
