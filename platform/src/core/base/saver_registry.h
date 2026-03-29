#ifndef SCREENSAVE_SAVER_REGISTRY_H
#define SCREENSAVE_SAVER_REGISTRY_H

#include "screensave/saver_api.h"

int screensave_saver_registry_is_valid(
    const screensave_saver_module *const *modules,
    unsigned int module_count
);

const char *screensave_saver_registry_canonical_product_key(const char *product_key);
const char *screensave_saver_registry_legacy_product_key(const char *product_key);
int screensave_saver_registry_build_registry_root(
    const char *product_key,
    char *buffer,
    unsigned int buffer_size
);

const screensave_saver_module *screensave_saver_registry_find(
    const screensave_saver_module *const *modules,
    unsigned int module_count,
    const char *product_key
);

#endif /* SCREENSAVE_SAVER_REGISTRY_H */
