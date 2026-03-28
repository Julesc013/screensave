#include "ecosystems_internal.h"

static const screensave_saver_config_hooks g_ecosystems_config_hooks = {
    sizeof(ecosystems_config),
    ecosystems_config_set_defaults,
    ecosystems_config_clamp,
    ecosystems_config_load,
    ecosystems_config_save,
    ecosystems_config_show_dialog
};

static const screensave_saver_callbacks g_ecosystems_callbacks = {
    ecosystems_create_session,
    ecosystems_destroy_session,
    ecosystems_resize_session,
    ecosystems_step_session,
    ecosystems_render_session
};

static const screensave_saver_module g_ecosystems_module = {
    {
        "ecosystems",
        "Ecosystems",
        "Ambient habitat saver with calm grouping, drift, and restrained light-driven behavior."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_ecosystems_presets,
    ECOSYSTEMS_PRESET_COUNT,
    g_ecosystems_themes,
    ECOSYSTEMS_THEME_COUNT,
    &g_ecosystems_config_hooks,
    &g_ecosystems_callbacks
};

const screensave_saver_module *ecosystems_get_module(void)
{
    return &g_ecosystems_module;
}
