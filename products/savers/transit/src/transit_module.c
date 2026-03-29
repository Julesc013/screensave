#include "transit_internal.h"

static const screensave_saver_config_hooks g_transit_config_hooks = {
    sizeof(transit_config),
    transit_config_set_defaults,
    transit_config_clamp,
    transit_config_load,
    transit_config_save,
    transit_config_show_dialog
};

static const screensave_saver_callbacks g_transit_callbacks = {
    transit_create_session,
    transit_destroy_session,
    transit_resize_session,
    transit_step_session,
    transit_render_session
};

static const screensave_saver_module g_transit_module = {
    {
        "transit",
        "Transit",
        "Night infrastructure saver with route progression, distant lights, and restrained reflections."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_transit_presets,
    TRANSIT_PRESET_COUNT,
    g_transit_themes,
    TRANSIT_THEME_COUNT,
    &g_transit_config_hooks,
    &g_transit_callbacks
};

const screensave_saver_module *transit_get_module(void)
{
    return &g_transit_module;
}
