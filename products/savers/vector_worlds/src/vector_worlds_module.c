#include "vector_worlds_internal.h"

static const screensave_saver_config_hooks g_vector_worlds_config_hooks = {
    sizeof(vector_worlds_config),
    vector_worlds_config_set_defaults,
    vector_worlds_config_clamp,
    vector_worlds_config_load,
    vector_worlds_config_save,
    vector_worlds_config_show_dialog
};

static const screensave_saver_callbacks g_vector_worlds_callbacks = {
    vector_worlds_create_session,
    vector_worlds_destroy_session,
    vector_worlds_resize_session,
    vector_worlds_step_session,
    vector_worlds_render_session
};

static const screensave_saver_module g_vector_worlds_module = {
    {
        "vector_worlds",
        "Vector Worlds",
        "Software-3D wireframe flyovers with curated structure, tunnel, and terrain scenes."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_vector_worlds_presets,
    VECTOR_WORLDS_PRESET_COUNT,
    g_vector_worlds_themes,
    VECTOR_WORLDS_THEME_COUNT,
    &g_vector_worlds_config_hooks,
    &g_vector_worlds_callbacks
};

const screensave_saver_module *vector_worlds_get_module(void)
{
    return &g_vector_worlds_module;
}
