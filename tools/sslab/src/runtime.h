#ifndef SCREENSAVE_SSLAB_RUNTIME_H
#define SCREENSAVE_SSLAB_RUNTIME_H

#include "screensave/sslab.h"

#include "product_adapter.h"
#include "renderer_rgba8.h"

struct sslab_context {
    char output_root[260];
    char catalog_root[260];
};

struct sslab_product {
    sslab_context *context;
    const sslab_product_adapter_v0 *adapter;
};

struct sslab_session {
    sslab_product *product;
    screensave_common_config common_config;
    void *product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
    screensave_renderer renderer;
    screensave_saver_session *product_session;
    sslab_u32 width;
    sslab_u32 height;
    sslab_u32 frame_index;
    sslab_u32 elapsed_ms;
};

int sslab_copy_text(char *target, unsigned int target_size, const char *source);
sslab_status sslab_validate_desc(unsigned long size, unsigned long expected_size, unsigned long abi_version);
const sslab_product_adapter_v0 *sslab_find_product_adapter(const char *product_key);

#endif
