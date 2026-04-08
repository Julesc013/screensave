#include "observatory_internal.h"

static void observatory_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(observatory_config)) {
        return;
    }

    observatory_apply_preset_to_config(preset_key, common_config, (observatory_config *)product_config);
}

static const screensave_saver_config_hooks g_observatory_config_hooks = {
    sizeof(observatory_config),
    observatory_config_set_defaults,
    observatory_config_clamp,
    observatory_config_load,
    observatory_config_save,
    observatory_config_show_dialog,
    observatory_apply_shared_preset,
    observatory_config_export_settings_entries,
    observatory_config_import_settings_entry,
    observatory_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_observatory_callbacks = {
    observatory_create_session,
    observatory_destroy_session,
    observatory_resize_session,
    observatory_step_session,
    observatory_render_session
};

static const screensave_saver_module g_observatory_module = {
    {
        "observatory",
        "Observatory",
        "Celestial exhibit saver with calm orrery motion, chart framing, and dome-watch atmosphere."
    },
    SCREENSAVE_SAVER_CAP_GDI |
        SCREENSAVE_SAVER_CAP_GL11 |
        SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE |
        SCREENSAVE_SAVER_CAP_PREVIEW_SAFE,
    { SCREENSAVE_RENDERER_KIND_GDI, SCREENSAVE_RENDERER_KIND_GL11, SCREENSAVE_CAPABILITY_QUALITY_SAFE },
    g_observatory_presets,
    OBSERVATORY_PRESET_COUNT,
    g_observatory_themes,
    OBSERVATORY_THEME_COUNT,
    &g_observatory_config_hooks,
    &g_observatory_callbacks
};

const screensave_saver_module *observatory_get_module(void)
{
    return &g_observatory_module;
}
