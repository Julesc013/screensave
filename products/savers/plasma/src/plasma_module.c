#include "plasma_internal.h"

static void plasma_apply_shared_preset(
    const screensave_saver_module *module,
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    (void)module;
    if (common_config == NULL || product_config == NULL || product_config_size != sizeof(plasma_config)) {
        return;
    }

    plasma_apply_preset_to_config(preset_key, common_config, (plasma_config *)product_config);
}

static const screensave_saver_config_hooks g_plasma_config_hooks = {
    sizeof(plasma_config),
    plasma_config_set_defaults,
    plasma_config_clamp,
    plasma_config_load,
    plasma_config_save,
    plasma_config_show_dialog,
    plasma_apply_shared_preset,
    plasma_config_export_settings_entries,
    plasma_config_import_settings_entry,
    plasma_config_randomize_settings,
    SCREENSAVE_CONFIG_SCHEMA_VERSION,
    SCREENSAVE_SETTINGS_CAP_PRESET_FILES |
        SCREENSAVE_SETTINGS_CAP_THEME_FILES |
        SCREENSAVE_SETTINGS_CAP_RANDOMIZATION |
        SCREENSAVE_SETTINGS_CAP_PACKS
};

static const screensave_saver_callbacks g_plasma_callbacks = {
    plasma_create_session,
    plasma_destroy_session,
    plasma_resize_session,
    plasma_step_session,
    plasma_render_session
};

static const screensave_saver_module g_plasma_module = {
    {
        "plasma",
        "Plasma",
        "Palette-driven framebuffer saver with curated dark-room palettes, clearer effect families, and calmer long-run composition refresh."
    },
    SCREENSAVE_SAVER_CAP_GDI |
        SCREENSAVE_SAVER_CAP_GL11 |
        SCREENSAVE_SAVER_CAP_GL21 |
        SCREENSAVE_SAVER_CAP_GL33 |
        SCREENSAVE_SAVER_CAP_GL46 |
        SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE |
        SCREENSAVE_SAVER_CAP_PREVIEW_SAFE,
    { SCREENSAVE_RENDERER_KIND_GDI, SCREENSAVE_RENDERER_KIND_GL11, SCREENSAVE_CAPABILITY_QUALITY_SAFE },
    g_plasma_presets,
    PLASMA_PRESET_COUNT,
    g_plasma_themes,
    PLASMA_THEME_COUNT,
    &g_plasma_config_hooks,
    &g_plasma_callbacks
};

const screensave_saver_module *plasma_get_module(void)
{
    return &g_plasma_module;
}
