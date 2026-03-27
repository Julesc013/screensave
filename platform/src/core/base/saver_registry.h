#ifndef SCREENSAVE_SAVER_REGISTRY_H
#define SCREENSAVE_SAVER_REGISTRY_H

#include "screensave/saver_api.h"

int screensave_saver_registry_is_valid(
    const screensave_saver_module *const *modules,
    unsigned int module_count
);

const screensave_saver_module *screensave_saver_registry_find(
    const screensave_saver_module *const *modules,
    unsigned int module_count,
    const char *product_key
);

#endif /* SCREENSAVE_SAVER_REGISTRY_H */
