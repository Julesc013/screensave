#include "../src/benchlab_internal.h"

int main(void)
{
    benchlab_app_config config;
    const screensave_saver_module *module;
    unsigned int module_count;

    benchlab_app_config_set_defaults(&config);
    benchlab_app_config_clamp(&config);
    if (
        config.client_width < 240 ||
        config.client_height < 180 ||
        config.fixed_seed == 0UL
    ) {
        return 1;
    }

    module = benchlab_get_target_module();
    if (!screensave_saver_module_is_valid(module)) {
        return 2;
    }

    module_count = benchlab_get_available_module_count();
    if (module_count < 7U) {
        return 3;
    }

    if (module->identity.product_key == NULL || lstrcmpA(module->identity.product_key, "nocturne") != 0) {
        return 4;
    }

    if (benchlab_find_target_module("ricochet") == NULL) {
        return 5;
    }

    if (benchlab_find_target_module("deepfield") == NULL) {
        return 6;
    }

    if (benchlab_find_target_module("ember") == NULL) {
        return 7;
    }

    if (benchlab_find_target_module("oscilloscope_dreams") == NULL) {
        return 8;
    }

    if (benchlab_find_target_module("pipeworks") == NULL) {
        return 9;
    }

    if (benchlab_find_target_module("lifeforms") == NULL) {
        return 10;
    }

    return 0;
}
