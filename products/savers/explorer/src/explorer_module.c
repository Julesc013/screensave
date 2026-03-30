#include "explorer_internal.h"

static void explorer_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(explorer_config)) {
        return;
    }

    explorer_apply_preset_to_config(preset_key, common_config, (explorer_config *)product_config);
}

static const screensave_saver_config_hooks g_explorer_config_hooks = {
    sizeof(explorer_config),
    explorer_config_set_defaults,
    explorer_config_clamp,
    explorer_config_load,
    explorer_config_save,
    explorer_config_show_dialog,
    explorer_apply_shared_preset,
    explorer_config_export_settings_entries,
    explorer_config_import_settings_entry,
    explorer_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_explorer_callbacks = {
    explorer_create_session,
    explorer_destroy_session,
    explorer_resize_session,
    explorer_step_session,
    explorer_render_session
};

static const screensave_saver_module g_explorer_module = {
    {
        "explorer",
        "Explorer",
        "Raycast-and-autopilot exploration saver with corridor, industrial, and canyon route studies."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_explorer_presets,
    EXPLORER_PRESET_COUNT,
    g_explorer_themes,
    EXPLORER_THEME_COUNT,
    &g_explorer_config_hooks,
    &g_explorer_callbacks
};

const screensave_saver_module *explorer_get_module(void)
{
    return &g_explorer_module;
}
