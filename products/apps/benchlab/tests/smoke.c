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
    if (module_count < 3U) {
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

    return 0;
}
