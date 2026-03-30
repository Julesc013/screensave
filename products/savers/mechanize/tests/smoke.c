#include "../src/mechanize_internal.h"

int main(void)
{
    screensave_common_config common_config;
    mechanize_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = mechanize_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    mechanize_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    mechanize_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (mechanize_find_preset_values("copper_counterworks") == NULL) {
        return 3;
    }

    if (mechanize_find_theme_descriptor("ivory_gallery") == NULL) {
        return 4;
    }
    if (
        module->config_hooks == NULL ||
        module->config_hooks->export_settings_entries == NULL ||
        module->config_hooks->import_settings_entry == NULL ||
        module->config_hooks->randomize_settings == NULL
    ) {
        return 5;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000C41UL;
    environment.seed.stream_seed = 0x00000C51UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!mechanize_create_session(module, &session, &environment) || session == NULL) {
        return 6;
    }

    environment.clock.delta_millis = 96UL;
    mechanize_step_session(session, &environment);
    if (session->theme == NULL) {
        mechanize_destroy_session(session);
        return 7;
    }
    if (session->master_phase == 0U) {
        mechanize_destroy_session(session);
        return 8;
    }

    mechanize_destroy_session(session);
    return 0;
}
