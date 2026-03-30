#include "../src/stormglass_internal.h"

int main(void)
{
    screensave_common_config common_config;
    stormglass_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = stormglass_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    stormglass_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    stormglass_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (stormglass_find_preset_values("quiet_midnight_rain") == NULL) {
        return 3;
    }
    if (stormglass_find_preset_values("winter_streetlamp") == NULL) {
        return 4;
    }

    if (stormglass_find_theme_descriptor("blue_storm") == NULL) {
        return 5;
    }
    if (stormglass_find_theme_descriptor("monochrome_cold_pane") == NULL) {
        return 6;
    }

    if (
        module->callbacks == NULL ||
        module->callbacks->export_settings_entries == NULL ||
        module->callbacks->import_settings_entry == NULL ||
        module->callbacks->randomize_settings == NULL
    ) {
        return 7;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000D30UL;
    environment.seed.stream_seed = 0x00000D31UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!stormglass_create_session(module, &session, &environment) || session == NULL) {
        return 8;
    }

    environment.clock.delta_millis = 96UL;
    stormglass_step_session(session, &environment);
    if (session->theme == NULL) {
        stormglass_destroy_session(session);
        return 9;
    }
    if (session->droplet_count == 0U || session->light_count == 0U) {
        stormglass_destroy_session(session);
        return 10;
    }

    stormglass_destroy_session(session);
    return 0;
}
