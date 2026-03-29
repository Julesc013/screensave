#include "../src/anthology_internal.h"

int main(void)
{
    screensave_common_config common_config;
    anthology_config product_config;
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

    return 0;
}
