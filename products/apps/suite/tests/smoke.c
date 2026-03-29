#include "../src/suite_internal.h"

int main(void)
{
    suite_app_config config;
    unsigned int module_count;

    suite_app_config_set_defaults(&config);
    suite_app_config_clamp(&config);

    if (config.client_width < 800 || config.client_height < 520) {
        return 1;
    }

    module_count = suite_get_available_module_count();
    if (module_count < 19U) {
        return 2;
    }

    if (suite_find_target_module("nocturne") == NULL) {
        return 3;
    }

    if (suite_find_target_module("anthology") == NULL) {
        return 4;
    }

    if (suite_find_target_module("suite") != NULL) {
        return 5;
    }

    return 0;
}
