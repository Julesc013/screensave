#include "../src/gallery_internal.h"

int main(void)
{
    screensave_common_config common_config;
    gallery_config product_config;
    unsigned long issue_flags;
    const screensave_saver_module *module;

    module = gallery_get_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 1;
    }

    gallery_config_set_defaults(&common_config, &product_config, sizeof(product_config));
    gallery_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!screensave_common_config_validate(&common_config, &issue_flags)) {
        return 2;
    }

    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GDI) == 0UL) {
        return 3;
    }
    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GL11) == 0UL) {
        return 4;
    }

    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_GL21) == 0UL) {
        return 5;
    }

    if (gallery_find_preset_values("compatibility_gallery") == NULL) {
        return 6;
    }

    if (gallery_find_theme_descriptor("advanced_showcase") == NULL) {
        return 7;
    }

    if (gallery_find_theme_descriptor(common_config.theme_key) == NULL) {
        return 8;
    }

    return 0;
}
