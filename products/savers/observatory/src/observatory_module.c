#include "observatory_internal.h"

static const screensave_saver_config_hooks g_observatory_config_hooks = {
    sizeof(observatory_config),
    observatory_config_set_defaults,
    observatory_config_clamp,
    observatory_config_load,
    observatory_config_save,
    observatory_config_show_dialog
};

static const screensave_saver_callbacks g_observatory_callbacks = {
    observatory_create_session,
    observatory_destroy_session,
    observatory_resize_session,
    observatory_step_session,
    observatory_render_session
};

static const screensave_saver_module g_observatory_module = {
    {
        "observatory",
        "Observatory",
        "Celestial exhibit saver with calm orrery motion, chart framing, and dome-watch atmosphere."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_observatory_presets,
    OBSERVATORY_PRESET_COUNT,
    g_observatory_themes,
    OBSERVATORY_THEME_COUNT,
    &g_observatory_config_hooks,
    &g_observatory_callbacks
};

const screensave_saver_module *observatory_get_module(void)
{
    return &g_observatory_module;
}
