#include "lifeforms_internal.h"

static const screensave_saver_config_hooks g_lifeforms_config_hooks = {
    sizeof(lifeforms_config),
    lifeforms_config_set_defaults,
    lifeforms_config_clamp,
    lifeforms_config_load,
    lifeforms_config_save,
    lifeforms_config_show_dialog
};

static const screensave_saver_callbacks g_lifeforms_callbacks = {
    lifeforms_create_session,
    lifeforms_destroy_session,
    lifeforms_resize_session,
    lifeforms_step_session,
    lifeforms_render_session
};

static const screensave_saver_module g_lifeforms_module = {
    {
        "lifeforms",
        "Lifeforms",
        "Contemplative cellular automata saver with curated rules, reseed discipline, and age-tinted population studies."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_lifeforms_presets,
    LIFEFORMS_PRESET_COUNT,
    g_lifeforms_themes,
    LIFEFORMS_THEME_COUNT,
    &g_lifeforms_config_hooks,
    &g_lifeforms_callbacks
};

const screensave_saver_module *lifeforms_get_module(void)
{
    return &g_lifeforms_module;
}
