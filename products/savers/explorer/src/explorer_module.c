#include "explorer_internal.h"

static const screensave_saver_config_hooks g_explorer_config_hooks = {
    sizeof(explorer_config),
    explorer_config_set_defaults,
    explorer_config_clamp,
    explorer_config_load,
    explorer_config_save,
    explorer_config_show_dialog
};

static const screensave_saver_callbacks g_explorer_callbacks = {
    explorer_create_session,
    explorer_destroy_session,
    explorer_resize_session,
    explorer_step_session,
    explorer_render_session
};

static const screensave_saver_module g_explorer_module = {
    {
        "explorer",
        "Explorer",
        "Raycast-and-autopilot exploration saver with corridor, industrial, and canyon route studies."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_explorer_presets,
    EXPLORER_PRESET_COUNT,
    g_explorer_themes,
    EXPLORER_THEME_COUNT,
    &g_explorer_config_hooks,
    &g_explorer_callbacks
};

const screensave_saver_module *explorer_get_module(void)
{
    return &g_explorer_module;
}
