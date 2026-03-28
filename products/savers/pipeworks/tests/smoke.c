#include "../src/pipeworks_internal.h"

int main(void)
{
    screensave_common_config common_config;
    pipeworks_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = pipeworks_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    pipeworks_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    pipeworks_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (pipeworks_find_preset_values("industrial_pipes") == NULL) {
        return 3;
    }

    if (pipeworks_find_theme_descriptor("phosphor_grid") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000B55UL;
    environment.seed.stream_seed = 0x00000B77UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!pipeworks_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 48UL;
    pipeworks_step_session(session, &environment);
    pipeworks_step_session(session, &environment);
    if (session->cells.cells == NULL || session->glow.cells == NULL) {
        pipeworks_destroy_session(session);
        return 6;
    }
    if (session->occupied_cells == 0U) {
        pipeworks_destroy_session(session);
        return 7;
    }

    pipeworks_destroy_session(session);
    return 0;
}
