#include "../src/plasma_internal.h"

int main(void)
{
    screensave_common_config common_config;
    plasma_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = plasma_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (plasma_find_preset_values("plasma_lava") == NULL) {
        return 3;
    }
    if (plasma_find_preset_values("ember_lava") == NULL) {
        return 4;
    }

    if (plasma_find_theme_descriptor("aurora_cool") == NULL) {
        return 5;
    }
    if (plasma_find_theme_descriptor("ember_lava") == NULL) {
        return 6;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000A55UL;
    environment.seed.stream_seed = 0x00000A77UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!plasma_create_session(module, &session, &environment) || session == NULL) {
        return 7;
    }

    environment.clock.delta_millis = 33UL;
    plasma_step_session(session, &environment);
    if (session->visual_buffer.pixels == NULL || session->field_primary == NULL) {
        plasma_destroy_session(session);
        return 8;
    }

    plasma_destroy_session(session);
    return 0;
}
