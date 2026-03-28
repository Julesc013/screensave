#include "../src/lifeforms_internal.h"

int main(void)
{
    screensave_common_config common_config;
    lifeforms_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = lifeforms_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    lifeforms_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    lifeforms_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (lifeforms_find_preset_values("classic_life") == NULL) {
        return 3;
    }

    if (lifeforms_find_theme_descriptor("green_phosphor") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000C55UL;
    environment.seed.stream_seed = 0x00000C77UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!lifeforms_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 140UL;
    lifeforms_step_session(session, &environment);
    if (
        session->current_cells.cells == NULL ||
        session->next_cells.cells == NULL ||
        session->ages.cells == NULL
    ) {
        lifeforms_destroy_session(session);
        return 6;
    }
    if (session->population == 0U) {
        lifeforms_destroy_session(session);
        return 7;
    }

    lifeforms_destroy_session(session);
    return 0;
}
