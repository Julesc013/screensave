#ifndef SCREENSAVE_SSLAB_RUNTIME_V1_H
#define SCREENSAVE_SSLAB_RUNTIME_V1_H

#include "screensave/sslab_v1.h"

#include "renderer_rgba8.h"

#define SSLAB_V1_MAX_PRODUCT_CONFIG_BYTES 64U

struct sslab_v1_context {
    char output_root[260];
    char catalog_root[260];
};

typedef struct sslab_v1_product_def_tag {
    const char *product_key;
    const char *default_preset_key;
    ss_u32 mode;
    ss_u32 (*stream_seed)(ss_u32 seed);
    ss_u32 config_size;
    const ss_v2_product_descriptor *(*descriptor)(void);
} sslab_v1_product_def;

struct sslab_v1_product {
    sslab_v1_context *context;
    const sslab_v1_product_def *definition;
};

struct sslab_v1_session {
    sslab_v1_product *product;
    ss_v2_session *v2_session;
    ss_u32 width;
    ss_u32 height;
    ss_u32 frame_index;
    ss_u32 elapsed_ms;
    ss_u8 product_config[SSLAB_V1_MAX_PRODUCT_CONFIG_BYTES];
    ss_u32 product_config_size;
    screensave_renderer renderer;
};

int sslab_v1_copy_text(char *target, unsigned int target_size, const char *source);
sslab_v1_status sslab_v1_check_prefix(ss_u32 struct_size, ss_u32 minimum_size, ss_u32 abi_version);
sslab_v1_status sslab_v1_status_from_v2(ss_u32 status);
const sslab_v1_product_def *sslab_v1_find_product_def(const char *product_key);
sslab_v1_status sslab_v1_configure_product(
    const sslab_v1_product_def *definition,
    const char *preset_key,
    ss_u8 *config_bytes,
    ss_u32 config_capacity,
    ss_u32 *config_size);
void sslab_v1_clock_from_values(ss_u32 frame_index, ss_u32 elapsed_ms, ss_u32 delta_ms, ss_v2_clock *clock);

#endif /* SCREENSAVE_SSLAB_RUNTIME_V1_H */
