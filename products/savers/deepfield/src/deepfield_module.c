#include "deepfield_internal.h"

static const screensave_saver_config_hooks g_deepfield_config_hooks = {
    sizeof(deepfield_config),
    deepfield_config_set_defaults,
    deepfield_config_clamp,
    deepfield_config_load,
    deepfield_config_save,
    deepfield_config_show_dialog
};

static const screensave_saver_callbacks g_deepfield_callbacks = {
    deepfield_create_session,
    deepfield_destroy_session,
    deepfield_resize_session,
    deepfield_step_session,
    deepfield_render_session
};

static const screensave_saver_module g_deepfield_module = {
    {
        "deepfield",
        "Deepfield",
        "Atmospheric drift and fly-through saver with restrained depth cues and curated space presets."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_deepfield_presets,
    DEEPFIELD_PRESET_COUNT,
    g_deepfield_themes,
    DEEPFIELD_THEME_COUNT,
    &g_deepfield_config_hooks,
    &g_deepfield_callbacks
};

const screensave_saver_module *deepfield_get_module(void)
{
    return &g_deepfield_module;
}
