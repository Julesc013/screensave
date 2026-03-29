#include "stormglass_internal.h"

static const screensave_saver_config_hooks g_stormglass_config_hooks = {
    sizeof(stormglass_config),
    stormglass_config_set_defaults,
    stormglass_config_clamp,
    stormglass_config_load,
    stormglass_config_save,
    stormglass_config_show_dialog
};

static const screensave_saver_callbacks g_stormglass_callbacks = {
    stormglass_create_session,
    stormglass_destroy_session,
    stormglass_resize_session,
    stormglass_step_session,
    stormglass_render_session
};

static const screensave_saver_module g_stormglass_module = {
    {
        "stormglass",
        "Stormglass",
        "Weather-on-glass saver with restrained pane, droplet, fog, and distant-light atmosphere."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_stormglass_presets,
    STORMGLASS_PRESET_COUNT,
    g_stormglass_themes,
    STORMGLASS_THEME_COUNT,
    &g_stormglass_config_hooks,
    &g_stormglass_callbacks
};

const screensave_saver_module *stormglass_get_module(void)
{
    return &g_stormglass_module;
}
