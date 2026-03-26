#include "../src/nocturne_internal.h"

int main(void)
{
    screensave_common_config common_config;
    nocturne_config product_config;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = nocturne_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    nocturne_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    nocturne_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (nocturne_find_preset_values("observatory_night") == NULL) {
        return 3;
    }

    if (nocturne_find_theme_descriptor("gray_black") == NULL) {
        return 4;
    }

    if (nocturne_find_theme_descriptor("pure_black") == NULL) {
        return 5;
    }

    return 0;
}
