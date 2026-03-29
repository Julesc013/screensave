#include "../src/template_saver_internal.h"

int main(void)
{
    screensave_common_config common_config;
    template_saver_config product_config;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = template_saver_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    template_saver_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    template_saver_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if (template_saver_find_preset_values("starter_drift") == NULL) {
        return 3;
    }

    if (template_saver_find_theme_descriptor("starter_blue") == NULL) {
        return 4;
    }

    if (template_saver_find_theme_descriptor("starter_mono") == NULL) {
        return 5;
    }

    return 0;
}
