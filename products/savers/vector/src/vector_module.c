#include "vector_internal.h"

static const screensave_saver_config_hooks g_vector_config_hooks = {
    sizeof(vector_config),
    vector_config_set_defaults,
    vector_config_clamp,
    vector_config_load,
    vector_config_save,
    vector_config_show_dialog
};

static const screensave_saver_callbacks g_vector_callbacks = {
    vector_create_session,
    vector_destroy_session,
    vector_resize_session,
    vector_step_session,
    vector_render_session
};

static const screensave_saver_module g_vector_module = {
    {
        "vector",
        "Vector",
        "Software-3D wireframe flyovers with curated structure, tunnel, and terrain scenes."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_vector_presets,
    VECTOR_PRESET_COUNT,
    g_vector_themes,
    VECTOR_THEME_COUNT,
    &g_vector_config_hooks,
    &g_vector_callbacks
};

const screensave_saver_module *vector_get_module(void)
{
    return &g_vector_module;
}
