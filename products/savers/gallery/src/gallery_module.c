#include "gallery_internal.h"

static const screensave_saver_config_hooks g_gallery_config_hooks = {
    sizeof(gallery_config),
    gallery_config_set_defaults,
    gallery_config_clamp,
    gallery_config_load,
    gallery_config_save,
    gallery_config_show_dialog
};

static const screensave_saver_callbacks g_gallery_callbacks = {
    gallery_create_session,
    gallery_destroy_session,
    gallery_resize_session,
    gallery_step_session,
    gallery_render_session
};

static const screensave_saver_module g_gallery_module = {
    {
        "gallery",
        "Gallery",
        "Renderer-showcase saver with compatibility, GL11 classic, and advanced showcase scene families."
    },
    SCREENSAVE_SAVER_CAP_GDI |
        SCREENSAVE_SAVER_CAP_GL11 |
        SCREENSAVE_SAVER_CAP_GL21 |
        SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_gallery_presets,
    GALLERY_PRESET_COUNT,
    g_gallery_themes,
    GALLERY_THEME_COUNT,
    &g_gallery_config_hooks,
    &g_gallery_callbacks
};

const screensave_saver_module *gallery_get_module(void)
{
    return &g_gallery_module;
}
