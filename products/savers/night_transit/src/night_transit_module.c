#include "night_transit_internal.h"

static const screensave_saver_config_hooks g_night_transit_config_hooks = {
    sizeof(night_transit_config),
    night_transit_config_set_defaults,
    night_transit_config_clamp,
    night_transit_config_load,
    night_transit_config_save,
    night_transit_config_show_dialog
};

static const screensave_saver_callbacks g_night_transit_callbacks = {
    night_transit_create_session,
    night_transit_destroy_session,
    night_transit_resize_session,
    night_transit_step_session,
    night_transit_render_session
};

static const screensave_saver_module g_night_transit_module = {
    {
        "night_transit",
        "Night Transit",
        "Night infrastructure saver with route progression, distant lights, and restrained reflections."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_night_transit_presets,
    NIGHT_TRANSIT_PRESET_COUNT,
    g_night_transit_themes,
    NIGHT_TRANSIT_THEME_COUNT,
    &g_night_transit_config_hooks,
    &g_night_transit_callbacks
};

const screensave_saver_module *night_transit_get_module(void)
{
    return &g_night_transit_module;
}
