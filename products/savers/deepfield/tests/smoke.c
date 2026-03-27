#include "../src/deepfield_internal.h"

int main(void)
{
    screensave_common_config common_config;
    deepfield_config product_config;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = deepfield_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    deepfield_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    deepfield_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (deepfield_find_preset_values("warp_travel") == NULL) {
        return 3;
    }

    if (deepfield_find_theme_descriptor("phosphor_space") == NULL) {
        return 4;
    }

    if (deepfield_find_theme_descriptor("blue_vault") == NULL) {
        return 5;
    }

    return 0;
}
