#include "retro_explorer_internal.h"

static const screensave_saver_config_hooks g_retro_explorer_config_hooks = {
    sizeof(retro_explorer_config),
    retro_explorer_config_set_defaults,
    retro_explorer_config_clamp,
    retro_explorer_config_load,
    retro_explorer_config_save,
    retro_explorer_config_show_dialog
};

static const screensave_saver_callbacks g_retro_explorer_callbacks = {
    retro_explorer_create_session,
    retro_explorer_destroy_session,
    retro_explorer_resize_session,
    retro_explorer_step_session,
    retro_explorer_render_session
};

static const screensave_saver_module g_retro_explorer_module = {
    {
        "retro_explorer",
        "Retro Explorer",
        "Raycast-and-autopilot exploration saver with corridor, industrial, and canyon route studies."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_retro_explorer_presets,
    RETRO_EXPLORER_PRESET_COUNT,
    g_retro_explorer_themes,
    RETRO_EXPLORER_THEME_COUNT,
    &g_retro_explorer_config_hooks,
    &g_retro_explorer_callbacks
};

const screensave_saver_module *retro_explorer_get_module(void)
{
    return &g_retro_explorer_module;
}
