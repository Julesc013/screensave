#include "../src/observatory_internal.h"

int main(void)
{
    screensave_common_config common_config;
    observatory_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = observatory_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    observatory_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    observatory_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (observatory_find_preset_values("brass_orrery") == NULL) {
        return 3;
    }

    if (observatory_find_theme_descriptor("deep_blue_dome") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000D50UL;
    environment.seed.stream_seed = 0x00000D51UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!observatory_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 96UL;
    observatory_step_session(session, &environment);
    if (session->theme == NULL) {
        observatory_destroy_session(session);
        return 6;
    }
    if (session->body_count == 0U || session->star_count == 0U) {
        observatory_destroy_session(session);
        return 7;
    }

    observatory_destroy_session(session);
    return 0;
}
