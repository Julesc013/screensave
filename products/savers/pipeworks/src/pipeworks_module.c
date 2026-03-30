#include "pipeworks_internal.h"

static void pipeworks_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(pipeworks_config)) {
        return;
    }

    pipeworks_apply_preset_to_config(preset_key, common_config, (pipeworks_config *)product_config);
}

static const screensave_saver_config_hooks g_pipeworks_config_hooks = {
    sizeof(pipeworks_config),
    pipeworks_config_set_defaults,
    pipeworks_config_clamp,
    pipeworks_config_load,
    pipeworks_config_save,
    pipeworks_config_show_dialog,
    pipeworks_apply_shared_preset,
    pipeworks_config_export_settings_entries,
    pipeworks_config_import_settings_entry,
    pipeworks_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_pipeworks_callbacks = {
    pipeworks_create_session,
    pipeworks_destroy_session,
    pipeworks_resize_session,
    pipeworks_step_session,
    pipeworks_render_session
};

static const screensave_saver_module g_pipeworks_module = {
    {
        "pipeworks",
        "Pipeworks",
        "Grid-grown network saver with branching build cycles, curated flow pulses, and staged rebuild choreography."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_pipeworks_presets,
    PIPEWORKS_PRESET_COUNT,
    g_pipeworks_themes,
    PIPEWORKS_THEME_COUNT,
    &g_pipeworks_config_hooks,
    &g_pipeworks_callbacks
};

const screensave_saver_module *pipeworks_get_module(void)
{
    return &g_pipeworks_module;
}
