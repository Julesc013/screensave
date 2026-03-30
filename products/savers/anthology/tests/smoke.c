#include "../src/anthology_internal.h"

int main(void)
{
    screensave_common_config common_config;
    anthology_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_saver_session *session;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = anthology_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    anthology_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    anthology_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (anthology_catalog_count() != ANTHOLOGY_INNER_SAVER_COUNT) {
        return 3;
    }

    if (anthology_catalog_find_index("anthology") >= 0) {
        return 4;
    }

    if (anthology_catalog_find_index("gallery") < 0) {
        return 5;
    }

    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GDI) == 0UL) {
        return 6;
    }
    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GL11) == 0UL) {
        return 7;
    }
    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GL21) == 0UL) {
        return 8;
    }

    if (anthology_find_theme_descriptor(common_config.theme_key) == NULL) {
        return 9;
    }

    if (anthology_find_preset_values(common_config.preset_key) == NULL) {
        return 10;
    }
    if (anthology_find_preset_values("scenic_grand_tour") == NULL) {
        return 11;
    }
    if (anthology_find_theme_descriptor("anthology_midnight") == NULL) {
        return 12;
    }

    screensave_config_binding_init(&binding, &common_config, &product_config, sizeof(product_config));
    ZeroMemory(&environment, sizeof(environment));
    environment.mode = SCREENSAVE_SESSION_MODE_PREVIEW;
    environment.drawable_size.width = 320;
    environment.drawable_size.height = 240;
    environment.seed.base_seed = 0x00001550UL;
    environment.seed.stream_seed = 0x00001551UL;
    environment.config_binding = &binding;
    product_config.interval_seconds = 120UL;

    session = NULL;
    if (!anthology_create_session(module, &session, &environment) || session == NULL) {
        return 13;
    }
    if (session->interval_millis > 45000UL) {
        anthology_destroy_session(session);
        return 14;
    }
    anthology_destroy_session(session);

    return 0;
}
