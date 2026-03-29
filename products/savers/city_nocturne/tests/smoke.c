#include "../src/city_nocturne_internal.h"

int main(void)
{
    screensave_common_config common_config;
    city_nocturne_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = city_nocturne_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    city_nocturne_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    city_nocturne_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (city_nocturne_find_preset_values("skyline_vista") == NULL) {
        return 3;
    }

    if (city_nocturne_find_theme_descriptor("quiet_city_sleep") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x000014C0UL;
    environment.seed.stream_seed = 0x000014C1UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!city_nocturne_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 96UL;
    city_nocturne_step_session(session, &environment);
    if (session->theme == NULL) {
        city_nocturne_destroy_session(session);
        return 6;
    }
    if (session->structure_count == 0U || session->light_count == 0U || session->fog_count == 0U) {
        city_nocturne_destroy_session(session);
        return 7;
    }

    city_nocturne_destroy_session(session);
    return 0;
}
