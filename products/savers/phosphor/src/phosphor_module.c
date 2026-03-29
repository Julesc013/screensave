#include "phosphor_internal.h"

static const screensave_saver_config_hooks g_phosphor_config_hooks = {
    sizeof(phosphor_config),
    phosphor_config_set_defaults,
    phosphor_config_clamp,
    phosphor_config_load,
    phosphor_config_save,
    phosphor_config_show_dialog
};

static const screensave_saver_callbacks g_phosphor_callbacks = {
    phosphor_create_session,
    phosphor_destroy_session,
    phosphor_resize_session,
    phosphor_step_session,
    phosphor_render_session
};

static const screensave_saver_module g_phosphor_module = {
    {
        "phosphor",
        "Phosphor",
        "Vector-style phosphor saver with curated curve families and persistent trails."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_phosphor_presets,
    PHOSPHOR_PRESET_COUNT,
    g_phosphor_themes,
    PHOSPHOR_THEME_COUNT,
    &g_phosphor_config_hooks,
    &g_phosphor_callbacks
};

const screensave_saver_module *phosphor_get_module(void)
{
    return &g_phosphor_module;
}
