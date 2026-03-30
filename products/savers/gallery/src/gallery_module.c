#include "gallery_internal.h"

static void gallery_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(gallery_config)) {
        return;
    }

    gallery_apply_preset_to_config(preset_key, common_config, (gallery_config *)product_config);
}

static const screensave_saver_config_hooks g_gallery_config_hooks = {
    sizeof(gallery_config),
    gallery_config_set_defaults,
    gallery_config_clamp,
    gallery_config_load,
    gallery_config_save,
    gallery_config_show_dialog,
    gallery_apply_shared_preset,
    gallery_config_export_settings_entries,
    gallery_config_import_settings_entry,
    gallery_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
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
