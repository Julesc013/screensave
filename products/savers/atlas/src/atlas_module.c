#include "atlas_internal.h"

static const screensave_saver_config_hooks g_atlas_config_hooks = {
    sizeof(atlas_config),
    atlas_config_set_defaults,
    atlas_config_clamp,
    atlas_config_load,
    atlas_config_save,
    atlas_config_show_dialog
};

static const screensave_saver_callbacks g_atlas_callbacks = {
    atlas_create_session,
    atlas_destroy_session,
    atlas_resize_session,
    atlas_step_session,
    atlas_render_session
};

static const screensave_saver_module g_atlas_module = {
    {
        "atlas",
        "Atlas",
        "Curated fractal voyage saver with staged refinement, restrained palettes, and calm mathematical travel."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_atlas_presets,
    ATLAS_PRESET_COUNT,
    g_atlas_themes,
    ATLAS_THEME_COUNT,
    &g_atlas_config_hooks,
    &g_atlas_callbacks
};

const screensave_saver_module *atlas_get_module(void)
{
    return &g_atlas_module;
}
