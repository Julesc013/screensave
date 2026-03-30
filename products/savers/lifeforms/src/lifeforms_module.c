#include "lifeforms_internal.h"

static void lifeforms_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(lifeforms_config)) {
        return;
    }

    lifeforms_apply_preset_to_config(preset_key, common_config, (lifeforms_config *)product_config);
}

static const screensave_saver_config_hooks g_lifeforms_config_hooks = {
    sizeof(lifeforms_config),
    lifeforms_config_set_defaults,
    lifeforms_config_clamp,
    lifeforms_config_load,
    lifeforms_config_save,
    lifeforms_config_show_dialog,
    lifeforms_apply_shared_preset,
    lifeforms_config_export_settings_entries,
    lifeforms_config_import_settings_entry,
    lifeforms_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_lifeforms_callbacks = {
    lifeforms_create_session,
    lifeforms_destroy_session,
    lifeforms_resize_session,
    lifeforms_step_session,
    lifeforms_render_session
};

static const screensave_saver_module g_lifeforms_module = {
    {
        "lifeforms",
        "Lifeforms",
        "Contemplative cellular automata saver with curated rules, reseed choreography, and age-tinted population studies."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_lifeforms_presets,
    LIFEFORMS_PRESET_COUNT,
    g_lifeforms_themes,
    LIFEFORMS_THEME_COUNT,
    &g_lifeforms_config_hooks,
    &g_lifeforms_callbacks
};

const screensave_saver_module *lifeforms_get_module(void)
{
    return &g_lifeforms_module;
}
