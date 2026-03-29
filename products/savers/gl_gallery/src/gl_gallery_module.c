#include "gl_gallery_internal.h"

static const screensave_saver_config_hooks g_gl_gallery_config_hooks = {
    sizeof(gl_gallery_config),
    gl_gallery_config_set_defaults,
    gl_gallery_config_clamp,
    gl_gallery_config_load,
    gl_gallery_config_save,
    gl_gallery_config_show_dialog
};

static const screensave_saver_callbacks g_gl_gallery_callbacks = {
    gl_gallery_create_session,
    gl_gallery_destroy_session,
    gl_gallery_resize_session,
    gl_gallery_step_session,
    gl_gallery_render_session
};

static const screensave_saver_module g_gl_gallery_module = {
    {
        "gl_gallery",
        "GL Gallery",
        "Renderer-showcase saver with compatibility, GL11 classic, and advanced showcase scene families."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI |
        SCREENSAVE_SAVER_CAP_OPTIONAL_GL11 |
        SCREENSAVE_SAVER_CAP_OPTIONAL_GL_PLUS |
        SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_gl_gallery_presets,
    GL_GALLERY_PRESET_COUNT,
    g_gl_gallery_themes,
    GL_GALLERY_THEME_COUNT,
    &g_gl_gallery_config_hooks,
    &g_gl_gallery_callbacks
};

const screensave_saver_module *gl_gallery_get_module(void)
{
    return &g_gl_gallery_module;
}
