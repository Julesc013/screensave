#include "atlas_internal.h"

static void atlas_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(atlas_config)) {
        return;
    }

    atlas_apply_preset_to_config(preset_key, common_config, (atlas_config *)product_config);
}

static const screensave_saver_config_hooks g_atlas_config_hooks = {
    sizeof(atlas_config),
    atlas_config_set_defaults,
    atlas_config_clamp,
    atlas_config_load,
    atlas_config_save,
    atlas_config_show_dialog,
    atlas_apply_shared_preset,
    atlas_config_export_settings_entries,
    atlas_config_import_settings_entry,
    atlas_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
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
