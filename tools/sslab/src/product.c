#include "runtime.h"

#include <stdlib.h>
#include <string.h>

const sslab_product_adapter_v0 *sslab_find_product_adapter(const char *product_key)
{
    const sslab_product_adapter_v0 *adapter;

    if (product_key == 0) {
        return 0;
    }

    adapter = sslab_nocturne_adapter();
    if (adapter != 0 && adapter->product_key != 0 && strcmp(adapter->product_key, product_key) == 0) {
        return adapter;
    }
    adapter = sslab_ricochet_adapter();
    if (adapter != 0 && adapter->product_key != 0 && strcmp(adapter->product_key, product_key) == 0) {
        return adapter;
    }

    return 0;
}

sslab_status sslab_open_product(sslab_context *context, const char *product_key, sslab_product **product)
{
    sslab_product *opened;
    const sslab_product_adapter_v0 *adapter;

    if (context == 0 || product_key == 0 || product == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    *product = 0;
    adapter = sslab_find_product_adapter(product_key);
    if (adapter == 0) {
        return SSLAB_STATUS_NOT_FOUND;
    }

    opened = (sslab_product *)calloc(1U, sizeof(*opened));
    if (opened == 0) {
        return SSLAB_STATUS_OUT_OF_MEMORY;
    }
    opened->context = context;
    opened->adapter = adapter;
    *product = opened;
    return SSLAB_STATUS_OK;
}

void sslab_close_product(sslab_product *product)
{
    free(product);
}
