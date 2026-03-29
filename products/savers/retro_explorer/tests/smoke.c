#include "../src/retro_explorer_internal.h"

int main(void)
{
    screensave_common_config common_config;
    retro_explorer_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = retro_explorer_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    retro_explorer_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    retro_explorer_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (retro_explorer_find_preset_values("quiet_night_run") == NULL) {
        return 3;
    }

    if (retro_explorer_find_theme_descriptor("industrial_tunnel") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000E10UL;
    environment.seed.stream_seed = 0x00000E11UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!retro_explorer_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 96UL;
    retro_explorer_step_session(session, &environment);
    if (session->theme == NULL) {
        retro_explorer_destroy_session(session);
        return 6;
    }
    if (session->segment_count == 0U || session->current_segment_index >= session->segment_count) {
        retro_explorer_destroy_session(session);
        return 7;
    }

    retro_explorer_destroy_session(session);
    return 0;
}
