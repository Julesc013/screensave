#include "../src/signals_internal.h"

int main(void)
{
    screensave_common_config common_config;
    signals_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = signals_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    signals_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    signals_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (signals_find_preset_values("green_terminal") == NULL) {
        return 3;
    }

    if (signals_find_theme_descriptor("green_terminal") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000C21UL;
    environment.seed.stream_seed = 0x00000C31UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!signals_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 96UL;
    signals_step_session(session, &environment);
    if (session->theme == NULL) {
        signals_destroy_session(session);
        return 6;
    }
    if (session->counters[0] == 0U && session->counters[1] == 0U && session->counters[2] == 0U) {
        signals_destroy_session(session);
        return 7;
    }

    signals_destroy_session(session);
    return 0;
}
