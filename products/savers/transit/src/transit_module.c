#include "transit_internal.h"

static void transit_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(transit_config)) {
        return;
    }

    transit_apply_preset_to_config(preset_key, common_config, (transit_config *)product_config);
}

static const screensave_saver_config_hooks g_transit_config_hooks = {
    sizeof(transit_config),
    transit_config_set_defaults,
    transit_config_clamp,
    transit_config_load,
    transit_config_save,
    transit_config_show_dialog,
    transit_apply_shared_preset,
    transit_config_export_settings_entries,
    transit_config_import_settings_entry,
    transit_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_transit_callbacks = {
    transit_create_session,
    transit_destroy_session,
    transit_resize_session,
    transit_step_session,
    transit_render_session
};

static const screensave_saver_module g_transit_module = {
    {
        "transit",
        "Transit",
        "Night infrastructure saver with route progression, distant lights, and restrained reflections."
    },
    SCREENSAVE_SAVER_CAP_GDI |
        SCREENSAVE_SAVER_CAP_GL11 |
        SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE |
        SCREENSAVE_SAVER_CAP_PREVIEW_SAFE,
    { SCREENSAVE_RENDERER_KIND_GDI, SCREENSAVE_RENDERER_KIND_GL11, SCREENSAVE_CAPABILITY_QUALITY_SAFE },
    g_transit_presets,
    TRANSIT_PRESET_COUNT,
    g_transit_themes,
    TRANSIT_THEME_COUNT,
    &g_transit_config_hooks,
    &g_transit_callbacks
};

const screensave_saver_module *transit_get_module(void)
{
    return &g_transit_module;
}
