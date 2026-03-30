#include "stormglass_internal.h"

static void stormglass_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(stormglass_config)) {
        return;
    }

    stormglass_apply_preset_to_config(preset_key, common_config, (stormglass_config *)product_config);
}

static const screensave_saver_config_hooks g_stormglass_config_hooks = {
    sizeof(stormglass_config),
    stormglass_config_set_defaults,
    stormglass_config_clamp,
    stormglass_config_load,
    stormglass_config_save,
    stormglass_config_show_dialog,
    stormglass_apply_shared_preset,
    stormglass_config_export_settings_entries,
    stormglass_config_import_settings_entry,
    stormglass_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_stormglass_callbacks = {
    stormglass_create_session,
    stormglass_destroy_session,
    stormglass_resize_session,
    stormglass_step_session,
    stormglass_render_session
};

static const screensave_saver_module g_stormglass_module = {
    {
        "stormglass",
        "Stormglass",
        "Weather-on-glass saver with restrained pane, droplet, fog, and distant-light atmosphere."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_stormglass_presets,
    STORMGLASS_PRESET_COUNT,
    g_stormglass_themes,
    STORMGLASS_THEME_COUNT,
    &g_stormglass_config_hooks,
    &g_stormglass_callbacks
};

const screensave_saver_module *stormglass_get_module(void)
{
    return &g_stormglass_module;
}
