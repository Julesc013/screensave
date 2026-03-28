#include "ember_internal.h"

static const screensave_saver_config_hooks g_ember_config_hooks = {
    sizeof(ember_config),
    ember_config_set_defaults,
    ember_config_clamp,
    ember_config_load,
    ember_config_save,
    ember_config_show_dialog
};

static const screensave_saver_callbacks g_ember_callbacks = {
    ember_create_session,
    ember_destroy_session,
    ember_resize_session,
    ember_step_session,
    ember_render_session
};

static const screensave_saver_module g_ember_module = {
    {
        "ember",
        "Ember",
        "Palette-driven framebuffer saver with restrained plasma, fire, and interference studies."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_ember_presets,
    EMBER_PRESET_COUNT,
    g_ember_themes,
    EMBER_THEME_COUNT,
    &g_ember_config_hooks,
    &g_ember_callbacks
};

const screensave_saver_module *ember_get_module(void)
{
    return &g_ember_module;
}
