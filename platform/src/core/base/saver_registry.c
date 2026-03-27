#include "saver_registry.h"

int screensave_saver_registry_is_valid(
    const screensave_saver_module *const *modules,
    unsigned int module_count
)
{
    unsigned int index;

    if (modules == NULL || module_count == 0U) {
        return 0;
    }

    for (index = 0U; index < module_count; ++index) {
        if (!screensave_saver_module_is_valid(modules[index])) {
            return 0;
        }
    }

    return 1;
}

const screensave_saver_module *screensave_saver_registry_find(
    const screensave_saver_module *const *modules,
    unsigned int module_count,
    const char *product_key
)
{
    unsigned int index;

    if (
        modules == NULL ||
        module_count == 0U ||
        product_key == NULL ||
        product_key[0] == '\0'
    ) {
        return NULL;
    }

    for (index = 0U; index < module_count; ++index) {
        if (
            modules[index] != NULL &&
            modules[index]->identity.product_key != NULL &&
            lstrcmpiA(modules[index]->identity.product_key, product_key) == 0
        ) {
            return modules[index];
        }
    }

    return NULL;
}
