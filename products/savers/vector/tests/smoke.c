#include "../src/vector_internal.h"

int main(void)
{
    screensave_common_config common_config;
    vector_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = vector_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    vector_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    vector_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (vector_find_preset_values("sgi_neon_tunnel") == NULL) {
        return 3;
    }
    if (vector_find_preset_values("amber_wire_tunnel") == NULL) {
        return 4;
    }

    if (vector_find_theme_descriptor("quiet_midnight_grid") == NULL) {
        return 5;
    }
    if (vector_find_theme_descriptor("terrain_museum_glide") == NULL) {
        return 6;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00001430UL;
    environment.seed.stream_seed = 0x00001431UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!vector_create_session(module, &session, &environment) || session == NULL) {
        return 7;
    }

    environment.clock.delta_millis = 96UL;
    vector_step_session(session, &environment);
    if (session->theme == NULL) {
        vector_destroy_session(session);
        return 8;
    }
    if (session->object_count == 0U) {
        vector_destroy_session(session);
        return 9;
    }

    vector_destroy_session(session);
    return 0;
}
