#include "vector_internal.h"

static void vector_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(vector_config)) {
        return;
    }

    vector_apply_preset_to_config(preset_key, common_config, (vector_config *)product_config);
}

static const screensave_saver_config_hooks g_vector_config_hooks = {
    sizeof(vector_config),
    vector_config_set_defaults,
    vector_config_clamp,
    vector_config_load,
    vector_config_save,
    vector_config_show_dialog,
    vector_apply_shared_preset,
    vector_config_export_settings_entries,
    vector_config_import_settings_entry,
    vector_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_vector_callbacks = {
    vector_create_session,
    vector_destroy_session,
    vector_resize_session,
    vector_step_session,
    vector_render_session
};

static const screensave_saver_module g_vector_module = {
    {
        "vector",
        "Vector",
        "Software-3D wireframe flyovers with curated structure, tunnel, and terrain scenes."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_vector_presets,
    VECTOR_PRESET_COUNT,
    g_vector_themes,
    VECTOR_THEME_COUNT,
    &g_vector_config_hooks,
    &g_vector_callbacks
};

const screensave_saver_module *vector_get_module(void)
{
    return &g_vector_module;
}
