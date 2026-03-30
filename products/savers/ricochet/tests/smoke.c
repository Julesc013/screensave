#include "../src/ricochet_internal.h"

int main(void)
{
    screensave_common_config common_config;
    ricochet_config product_config;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = ricochet_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    ricochet_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    ricochet_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (ricochet_find_preset_values("classic_clean") == NULL) {
        return 3;
    }

    if (ricochet_find_theme_descriptor("phosphor_green") == NULL) {
        return 4;
    }

    if (ricochet_find_theme_descriptor("quiet_modern") == NULL) {
        return 5;
    }

    if (ricochet_find_preset_values("amber_arc") == NULL) {
        return 6;
    }

    if (module->config_hooks == NULL || module->config_hooks->randomize_settings == NULL) {
        return 7;
    }

    return 0;
}
