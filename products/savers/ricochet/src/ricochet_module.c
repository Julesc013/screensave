#include "ricochet_internal.h"

static const screensave_saver_config_hooks g_ricochet_config_hooks = {
    sizeof(ricochet_config),
    ricochet_config_set_defaults,
    ricochet_config_clamp,
    ricochet_config_load,
    ricochet_config_save,
    ricochet_config_show_dialog
};

static const screensave_saver_callbacks g_ricochet_callbacks = {
    ricochet_create_session,
    ricochet_destroy_session,
    ricochet_resize_session,
    ricochet_step_session,
    ricochet_render_session
};

static const screensave_saver_module g_ricochet_module = {
    {
        "ricochet",
        "Ricochet",
        "Polished bounce-motion saver with restrained trails and curated motion presets."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_ricochet_presets,
    RICOCHET_PRESET_COUNT,
    g_ricochet_themes,
    RICOCHET_THEME_COUNT,
    &g_ricochet_config_hooks,
    &g_ricochet_callbacks
};

const screensave_saver_module *ricochet_get_module(void)
{
    return &g_ricochet_module;
}
