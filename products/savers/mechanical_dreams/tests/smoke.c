#include "../src/mechanical_dreams_internal.h"

int main(void)
{
    screensave_common_config common_config;
    mechanical_dreams_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = mechanical_dreams_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    mechanical_dreams_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    mechanical_dreams_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (mechanical_dreams_find_preset_values("brass_gear_train") == NULL) {
        return 3;
    }

    if (mechanical_dreams_find_theme_descriptor("brass_workshop") == NULL) {
        return 4;
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
    if (!mechanical_dreams_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 96UL;
    mechanical_dreams_step_session(session, &environment);
    if (session->theme == NULL) {
        mechanical_dreams_destroy_session(session);
        return 6;
    }
    if (session->master_phase == 0U) {
        mechanical_dreams_destroy_session(session);
        return 7;
    }

    mechanical_dreams_destroy_session(session);
    return 0;
}
