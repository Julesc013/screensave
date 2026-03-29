#include "nocturne_internal.h"

static void nocturne_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(nocturne_config)) {
        return;
    }

    nocturne_apply_preset_to_config(preset_key, common_config, (nocturne_config *)product_config);
}

static const screensave_saver_config_hooks g_nocturne_config_hooks = {
    sizeof(nocturne_config),
    nocturne_config_set_defaults,
    nocturne_config_clamp,
    nocturne_config_load,
    nocturne_config_save,
    nocturne_config_show_dialog,
    nocturne_apply_shared_preset,
    nocturne_config_export_settings_entries,
    nocturne_config_import_settings_entry,
    nocturne_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_nocturne_callbacks = {
    nocturne_create_session,
    nocturne_destroy_session,
    nocturne_resize_session,
    nocturne_step_session,
    nocturne_render_session
};

static const screensave_saver_module g_nocturne_module = {
    {
        "nocturne",
        "Nocturne",
        "A restrained dark-room saver with subtle motion and curated night presets."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_nocturne_presets,
    NOCTURNE_PRESET_COUNT,
    g_nocturne_themes,
    NOCTURNE_THEME_COUNT,
    &g_nocturne_config_hooks,
    &g_nocturne_callbacks
};

const screensave_saver_module *nocturne_get_module(void)
{
    return &g_nocturne_module;
}
