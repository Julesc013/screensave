#include "template_saver_internal.h"

static void template_saver_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (
        common_config == NULL ||
        product_config == NULL ||
        product_config_size != sizeof(template_saver_config)
    ) {
        return;
    }

    template_saver_apply_preset_to_config(
        preset_key,
        common_config,
        (template_saver_config *)product_config
    );
}

static const screensave_saver_config_hooks g_template_saver_config_hooks = {
    sizeof(template_saver_config),
    template_saver_config_set_defaults,
    template_saver_config_clamp,
    template_saver_config_load,
    template_saver_config_save,
    template_saver_config_show_dialog,
    template_saver_apply_shared_preset,
    template_saver_config_export_settings_entries,
    template_saver_config_import_settings_entry,
    template_saver_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_template_saver_callbacks = {
    template_saver_create_session,
    template_saver_destroy_session,
    template_saver_resize_session,
    template_saver_step_session,
    template_saver_render_session
};

static const screensave_saver_module g_template_saver_module = {
    {
        "template_saver",
        "Template Saver",
        "Contributor starter saver with bounded config, presets, themes, and a conservative renderer path."
    },
    SCREENSAVE_SAVER_CAP_GDI | SCREENSAVE_SAVER_CAP_GL11 | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    g_template_saver_presets,
    TEMPLATE_SAVER_PRESET_COUNT,
    g_template_saver_themes,
    TEMPLATE_SAVER_THEME_COUNT,
    &g_template_saver_config_hooks,
    &g_template_saver_callbacks
};

const screensave_saver_module *template_saver_get_module(void)
{
    return &g_template_saver_module;
}
