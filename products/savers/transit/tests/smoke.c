#include "../src/transit_internal.h"

int main(void)
{
    screensave_common_config common_config;
    transit_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = transit_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    transit_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    transit_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (transit_find_preset_values("wet_motorway") == NULL) {
        return 3;
    }
    if (transit_find_preset_values("motorway_fog") == NULL) {
        return 4;
    }

    if (transit_find_theme_descriptor("harbor_lights") == NULL) {
        return 5;
    }
    if (transit_find_theme_descriptor("harbor_midnight") == NULL) {
        return 6;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000D40UL;
    environment.seed.stream_seed = 0x00000D41UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!transit_create_session(module, &session, &environment) || session == NULL) {
        return 7;
    }

    environment.clock.delta_millis = 96UL;
    transit_step_session(session, &environment);
    if (session->theme == NULL) {
        transit_destroy_session(session);
        return 8;
    }
    if (session->light_count == 0U) {
        transit_destroy_session(session);
        return 9;
    }

    transit_destroy_session(session);
    return 0;
}
