#include "pipeworks_internal.h"

static const screensave_saver_config_hooks g_pipeworks_config_hooks = {
    sizeof(pipeworks_config),
    pipeworks_config_set_defaults,
    pipeworks_config_clamp,
    pipeworks_config_load,
    pipeworks_config_save,
    pipeworks_config_show_dialog
};

static const screensave_saver_callbacks g_pipeworks_callbacks = {
    pipeworks_create_session,
    pipeworks_destroy_session,
    pipeworks_resize_session,
    pipeworks_step_session,
    pipeworks_render_session
};

static const screensave_saver_module g_pipeworks_module = {
    {
        "pipeworks",
        "Pipeworks",
        "Grid-grown network saver with branching build cycles, restrained pulse flow, and staged rebuilds."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_pipeworks_presets,
    PIPEWORKS_PRESET_COUNT,
    g_pipeworks_themes,
    PIPEWORKS_THEME_COUNT,
    &g_pipeworks_config_hooks,
    &g_pipeworks_callbacks
};

const screensave_saver_module *pipeworks_get_module(void)
{
    return &g_pipeworks_module;
}
