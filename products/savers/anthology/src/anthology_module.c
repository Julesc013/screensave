#include "anthology_internal.h"

static void anthology_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(anthology_config)) {
        return;
    }

    anthology_apply_preset_to_config(preset_key, common_config, (anthology_config *)product_config);
}

static const screensave_saver_config_hooks g_anthology_config_hooks = {
    sizeof(anthology_config),
    anthology_config_set_defaults,
    anthology_config_clamp,
    anthology_config_load,
    anthology_config_save,
    anthology_config_show_dialog,
    anthology_apply_shared_preset,
    anthology_config_export_settings_entries,
    anthology_config_import_settings_entry,
    anthology_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION
};

static const screensave_saver_callbacks g_anthology_callbacks = {
    anthology_create_session,
    anthology_destroy_session,
    anthology_resize_session,
    anthology_step_session,
    anthology_render_session
};

static const screensave_saver_module g_anthology_module = {
    {
        "anthology",
        "Anthology",
        "Suite meta-saver that rotates across the current saver line with bounded cross-saver randomization."
    },
    SCREENSAVE_SAVER_CAP_GDI |
        SCREENSAVE_SAVER_CAP_GL11 |
        SCREENSAVE_SAVER_CAP_GL21 |
        SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_anthology_presets,
    ANTHOLOGY_PRESET_COUNT,
    g_anthology_themes,
    ANTHOLOGY_THEME_COUNT,
    &g_anthology_config_hooks,
    &g_anthology_callbacks
};

const screensave_saver_module *anthology_get_module(void)
{
    return &g_anthology_module;
}
