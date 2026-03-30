#include "ricochet_internal.h"

static void ricochet_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(ricochet_config)) {
        return;
    }

    ricochet_apply_preset_to_config(preset_key, common_config, (ricochet_config *)product_config);
}

static const screensave_saver_config_hooks g_ricochet_config_hooks = {
    sizeof(ricochet_config),
    ricochet_config_set_defaults,
    ricochet_config_clamp,
    ricochet_config_load,
    ricochet_config_save,
    ricochet_config_show_dialog,
    ricochet_apply_shared_preset,
    ricochet_config_export_settings_entries,
    ricochet_config_import_settings_entry,
    ricochet_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_ricochet_callbacks = {
    ricochet_create_session,
    ricochet_destroy_session,
    ricochet_resize_session,
    ricochet_step_session,
    ricochet_render_session
};

static const screensave_saver_module g_ricochet_module = {
    {
        "ricochet",
        "Ricochet",
        "Polished bounce-motion saver with restrained trails, crisp starts, and curated motion presets."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_ricochet_presets,
    RICOCHET_PRESET_COUNT,
    g_ricochet_themes,
    RICOCHET_THEME_COUNT,
    &g_ricochet_config_hooks,
    &g_ricochet_callbacks
};

const screensave_saver_module *ricochet_get_module(void)
{
    return &g_ricochet_module;
}
