#include "city_internal.h"

static void city_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(city_config)) {
        return;
    }

    city_apply_preset_to_config(preset_key, common_config, (city_config *)product_config);
}

static const screensave_saver_config_hooks g_city_config_hooks = {
    sizeof(city_config),
    city_config_set_defaults,
    city_config_clamp,
    city_config_load,
    city_config_save,
    city_config_show_dialog,
    city_apply_shared_preset,
    city_config_export_settings_entries,
    city_config_import_settings_entry,
    city_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_city_callbacks = {
    city_create_session,
    city_destroy_session,
    city_resize_session,
    city_step_session,
    city_render_session
};

static const screensave_saver_module g_city_module = {
    {
        "city",
        "City",
        "Urban night-world saver with skyline, harbor, and rooftop/window-river studies."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_city_presets,
    CITY_PRESET_COUNT,
    g_city_themes,
    CITY_THEME_COUNT,
    &g_city_config_hooks,
    &g_city_callbacks
};

const screensave_saver_module *city_get_module(void)
{
    return &g_city_module;
}
