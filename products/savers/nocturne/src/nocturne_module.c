#include "nocturne_internal.h"

static const screensave_saver_config_hooks g_nocturne_config_hooks = {
    sizeof(nocturne_config),
    nocturne_config_set_defaults,
    nocturne_config_clamp,
    nocturne_config_load,
    nocturne_config_save,
    nocturne_config_show_dialog
};

static const screensave_saver_callbacks g_nocturne_callbacks = {
    nocturne_create_session,
    nocturne_destroy_session,
    nocturne_resize_session,
    nocturne_step_session,
    nocturne_render_session
};

static const screensave_saver_module g_nocturne_module = {
    {
        "nocturne",
        "Nocturne",
        "A restrained dark-room saver with subtle motion and curated night presets."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_nocturne_presets,
    NOCTURNE_PRESET_COUNT,
    g_nocturne_themes,
    NOCTURNE_THEME_COUNT,
    &g_nocturne_config_hooks,
    &g_nocturne_callbacks
};

const screensave_saver_module *nocturne_get_module(void)
{
    return &g_nocturne_module;
}
