#include "ecosystems_internal.h"

static void ecosystems_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(ecosystems_config)) {
        return;
    }

    ecosystems_apply_preset_to_config(preset_key, common_config, (ecosystems_config *)product_config);
}

static const screensave_saver_config_hooks g_ecosystems_config_hooks = {
    sizeof(ecosystems_config),
    ecosystems_config_set_defaults,
    ecosystems_config_clamp,
    ecosystems_config_load,
    ecosystems_config_save,
    ecosystems_config_show_dialog,
    ecosystems_apply_shared_preset,
    ecosystems_config_export_settings_entries,
    ecosystems_config_import_settings_entry,
    ecosystems_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_ecosystems_callbacks = {
    ecosystems_create_session,
    ecosystems_destroy_session,
    ecosystems_resize_session,
    ecosystems_step_session,
    ecosystems_render_session
};

static const screensave_saver_module g_ecosystems_module = {
    {
        "ecosystems",
        "Ecosystems",
        "Ambient habitat saver with calm grouping, drift, and restrained light-driven behavior."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_ecosystems_presets,
    ECOSYSTEMS_PRESET_COUNT,
    g_ecosystems_themes,
    ECOSYSTEMS_THEME_COUNT,
    &g_ecosystems_config_hooks,
    &g_ecosystems_callbacks
};

const screensave_saver_module *ecosystems_get_module(void)
{
    return &g_ecosystems_module;
}
