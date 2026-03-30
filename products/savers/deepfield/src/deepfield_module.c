#include "deepfield_internal.h"

static void deepfield_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(deepfield_config)) {
        return;
    }

    deepfield_apply_preset_to_config(preset_key, common_config, (deepfield_config *)product_config);
}

static const screensave_saver_config_hooks g_deepfield_config_hooks = {
    sizeof(deepfield_config),
    deepfield_config_set_defaults,
    deepfield_config_clamp,
    deepfield_config_load,
    deepfield_config_save,
    deepfield_config_show_dialog,
    deepfield_apply_shared_preset,
    deepfield_config_export_settings_entries,
    deepfield_config_import_settings_entry,
    deepfield_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_deepfield_callbacks = {
    deepfield_create_session,
    deepfield_destroy_session,
    deepfield_resize_session,
    deepfield_step_session,
    deepfield_render_session
};

static const screensave_saver_module g_deepfield_module = {
    {
        "deepfield",
        "Deepfield",
        "Atmospheric drift and fly-through saver with calmer preview pacing, animated twinkle, and curated observatory travel presets."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_deepfield_presets,
    DEEPFIELD_PRESET_COUNT,
    g_deepfield_themes,
    DEEPFIELD_THEME_COUNT,
    &g_deepfield_config_hooks,
    &g_deepfield_callbacks
};

const screensave_saver_module *deepfield_get_module(void)
{
    return &g_deepfield_module;
}
