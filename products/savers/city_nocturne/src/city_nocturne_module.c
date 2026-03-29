#include "city_nocturne_internal.h"

static const screensave_saver_config_hooks g_city_nocturne_config_hooks = {
    sizeof(city_nocturne_config),
    city_nocturne_config_set_defaults,
    city_nocturne_config_clamp,
    city_nocturne_config_load,
    city_nocturne_config_save,
    city_nocturne_config_show_dialog
};

static const screensave_saver_callbacks g_city_nocturne_callbacks = {
    city_nocturne_create_session,
    city_nocturne_destroy_session,
    city_nocturne_resize_session,
    city_nocturne_step_session,
    city_nocturne_render_session
};

static const screensave_saver_module g_city_nocturne_module = {
    {
        "city_nocturne",
        "City Nocturne",
        "Urban night-world saver with skyline, harbor, and rooftop/window-river studies."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_city_nocturne_presets,
    CITY_NOCTURNE_PRESET_COUNT,
    g_city_nocturne_themes,
    CITY_NOCTURNE_THEME_COUNT,
    &g_city_nocturne_config_hooks,
    &g_city_nocturne_callbacks
};

const screensave_saver_module *city_nocturne_get_module(void)
{
    return &g_city_nocturne_module;
}
