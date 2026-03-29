#include "plasma_internal.h"

static const screensave_saver_config_hooks g_plasma_config_hooks = {
    sizeof(plasma_config),
    plasma_config_set_defaults,
    plasma_config_clamp,
    plasma_config_load,
    plasma_config_save,
    plasma_config_show_dialog
};

static const screensave_saver_callbacks g_plasma_callbacks = {
    plasma_create_session,
    plasma_destroy_session,
    plasma_resize_session,
    plasma_step_session,
    plasma_render_session
};

static const screensave_saver_module g_plasma_module = {
    {
        "plasma",
        "Plasma",
        "Palette-driven framebuffer saver with restrained plasma, fire, and interference studies."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_plasma_presets,
    PLASMA_PRESET_COUNT,
    g_plasma_themes,
    PLASMA_THEME_COUNT,
    &g_plasma_config_hooks,
    &g_plasma_callbacks
};

const screensave_saver_module *plasma_get_module(void)
{
    return &g_plasma_module;
}
