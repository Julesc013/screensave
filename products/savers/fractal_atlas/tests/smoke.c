#include "../src/fractal_atlas_internal.h"

int main(void)
{
    screensave_common_config common_config;
    fractal_atlas_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = fractal_atlas_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    fractal_atlas_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    fractal_atlas_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (fractal_atlas_find_preset_values("atlas_monochrome") == NULL) {
        return 3;
    }

    if (fractal_atlas_find_theme_descriptor("deep_ultraviolet") == NULL) {
        return 4;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_WINDOWED;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00000E71UL;
    environment.seed.stream_seed = 0x00000E72UL;
    environment.config_binding = &binding;

    session = NULL;
    if (!fractal_atlas_create_session(module, &session, &environment) || session == NULL) {
        return 5;
    }

    environment.clock.delta_millis = 40UL;
    fractal_atlas_step_session(session, &environment);
    if (session->visual_buffer.pixels == NULL || session->escape_values == NULL) {
        fractal_atlas_destroy_session(session);
        return 6;
    }

    fractal_atlas_destroy_session(session);
    return 0;
}
