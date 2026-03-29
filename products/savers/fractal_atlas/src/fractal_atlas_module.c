#include "fractal_atlas_internal.h"

static const screensave_saver_config_hooks g_fractal_atlas_config_hooks = {
    sizeof(fractal_atlas_config),
    fractal_atlas_config_set_defaults,
    fractal_atlas_config_clamp,
    fractal_atlas_config_load,
    fractal_atlas_config_save,
    fractal_atlas_config_show_dialog
};

static const screensave_saver_callbacks g_fractal_atlas_callbacks = {
    fractal_atlas_create_session,
    fractal_atlas_destroy_session,
    fractal_atlas_resize_session,
    fractal_atlas_step_session,
    fractal_atlas_render_session
};

static const screensave_saver_module g_fractal_atlas_module = {
    {
        "fractal_atlas",
        "Fractal Atlas",
        "Curated fractal voyage saver with staged refinement, restrained palettes, and calm mathematical travel."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_fractal_atlas_presets,
    FRACTAL_ATLAS_PRESET_COUNT,
    g_fractal_atlas_themes,
    FRACTAL_ATLAS_THEME_COUNT,
    &g_fractal_atlas_config_hooks,
    &g_fractal_atlas_callbacks
};

const screensave_saver_module *fractal_atlas_get_module(void)
{
    return &g_fractal_atlas_module;
}
