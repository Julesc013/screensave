#include "saver_registry.h"

typedef struct screensave_product_alias_tag {
    const char *legacy_product_key;
    const char *canonical_product_key;
} screensave_product_alias;

static const char g_saver_registry_root_prefix[] = "Software\\Julesc013\\ScreenSave\\Products\\";

static const screensave_product_alias g_product_aliases[] = {
    { "ember", "plasma" },
    { "oscilloscope_dreams", "phosphor" },
    { "signal_lab", "signals" },
    { "mechanical_dreams", "mechanize" },
    { "night_transit", "transit" },
    { "retro_explorer", "explorer" },
    { "vector_worlds", "vector" },
    { "fractal_atlas", "atlas" },
    { "city_nocturne", "city" },
    { "gl_gallery", "gallery" }
};

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

const char *screensave_saver_registry_canonical_product_key(const char *product_key)
{
    unsigned int index;

    if (product_key == NULL || product_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_product_aliases) / sizeof(g_product_aliases[0])); ++index) {
        if (
            lstrcmpiA(g_product_aliases[index].legacy_product_key, product_key) == 0 ||
            lstrcmpiA(g_product_aliases[index].canonical_product_key, product_key) == 0
        ) {
            return g_product_aliases[index].canonical_product_key;
        }
    }

    return product_key;
}

const char *screensave_saver_registry_legacy_product_key(const char *product_key)
{
    unsigned int index;

    if (product_key == NULL || product_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < (unsigned int)(sizeof(g_product_aliases) / sizeof(g_product_aliases[0])); ++index) {
        if (
            lstrcmpiA(g_product_aliases[index].legacy_product_key, product_key) == 0 ||
            lstrcmpiA(g_product_aliases[index].canonical_product_key, product_key) == 0
        ) {
            return g_product_aliases[index].legacy_product_key;
        }
    }

    return NULL;
}

int screensave_saver_registry_build_registry_root(
    const char *product_key,
    char *buffer,
    unsigned int buffer_size
)
{
    unsigned int prefix_length;
    unsigned int product_length;

    if (product_key == NULL || product_key[0] == '\0' || buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    prefix_length = (unsigned int)lstrlenA(g_saver_registry_root_prefix);
    product_length = (unsigned int)lstrlenA(product_key);
    if (prefix_length + product_length + 1U > buffer_size) {
        buffer[0] = '\0';
        return 0;
    }

    lstrcpyA(buffer, g_saver_registry_root_prefix);
    lstrcatA(buffer, product_key);
    return 1;
}

const screensave_saver_module *screensave_saver_registry_find(
    const screensave_saver_module *const *modules,
    unsigned int module_count,
    const char *product_key
)
{
    unsigned int index;
    const char *canonical_product_key;

    if (
        modules == NULL ||
        module_count == 0U ||
        product_key == NULL ||
        product_key[0] == '\0'
    ) {
        return NULL;
    }

    canonical_product_key = screensave_saver_registry_canonical_product_key(product_key);
    if (canonical_product_key == NULL) {
        return NULL;
    }

    for (index = 0U; index < module_count; ++index) {
        if (
            modules[index] != NULL &&
            modules[index]->identity.product_key != NULL &&
            lstrcmpiA(modules[index]->identity.product_key, canonical_product_key) == 0
        ) {
            return modules[index];
        }
    }

    return NULL;
}
