#ifndef SCREENSAVE_SSLAB_V1_H
#define SCREENSAVE_SSLAB_V1_H

/*
 * Private ScreenSave lab/proof ABI v1.
 *
 * ABI v1 is fixed-width and v2-backed. It is still private to tools/sslab and
 * is not a public SDK surface or compatibility certification API.
 */

#include "screensave/v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSLAB_V1_ABI_VERSION ((ss_u32)1U)
#define SSLAB_V1_MAX_CAPTURE_FRAMES ((ss_u32)16U)

typedef struct sslab_v1_context sslab_v1_context;
typedef struct sslab_v1_product sslab_v1_product;
typedef struct sslab_v1_session sslab_v1_session;

typedef enum sslab_v1_status {
    SSLAB_V1_STATUS_OK = 0,
    SSLAB_V1_STATUS_FAIL = 1,
    SSLAB_V1_STATUS_BLOCKED = 2,
    SSLAB_V1_STATUS_UNSUPPORTED_HOST = 3,
    SSLAB_V1_STATUS_INVALID_ARGUMENT = 4,
    SSLAB_V1_STATUS_NOT_FOUND = 5,
    SSLAB_V1_STATUS_OUT_OF_MEMORY = 6
} sslab_v1_status;

typedef struct sslab_v1_abi_info {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 max_capture_frames;
    ss_u32 portable_v2_abi_version;
} sslab_v1_abi_info;

typedef struct sslab_v1_context_desc {
    ss_u32 struct_size;
    ss_u32 abi_version;
    const char *output_root;
    const char *catalog_root;
} sslab_v1_context_desc;

typedef struct sslab_v1_run_desc {
    ss_u32 struct_size;
    ss_u32 abi_version;
    const char *product_key;
    const char *preset_key;
    ss_v2_size dimensions;
    ss_v2_seed seed;
    ss_v2_clock clock;
} sslab_v1_run_desc;

typedef struct sslab_v1_surface_desc {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_surface_desc surface;
} sslab_v1_surface_desc;

typedef struct sslab_v1_smoke_receipt {
    ss_u32 struct_size;
    ss_u32 abi_version;
    sslab_v1_status status;
    const char *product_key;
    ss_u32 width;
    ss_u32 height;
    ss_u32 checksum;
} sslab_v1_smoke_receipt;

sslab_v1_status sslab_v1_get_abi_info(sslab_v1_abi_info *info);

sslab_v1_status sslab_v1_create_context(const sslab_v1_context_desc *desc, sslab_v1_context **context);
void sslab_v1_destroy_context(sslab_v1_context *context);

sslab_v1_status sslab_v1_open_product(sslab_v1_context *context, const char *product_key, sslab_v1_product **product);
void sslab_v1_close_product(sslab_v1_product *product);

sslab_v1_status sslab_v1_create_session(
    sslab_v1_product *product,
    const sslab_v1_run_desc *desc,
    sslab_v1_session **session);
sslab_v1_status sslab_v1_resize_session(sslab_v1_session *session, const ss_v2_size *dimensions);
sslab_v1_status sslab_v1_advance_session(sslab_v1_session *session, const ss_v2_clock *clock);
sslab_v1_status sslab_v1_render_session(sslab_v1_session *session, sslab_v1_surface_desc *surface);
void sslab_v1_destroy_session(sslab_v1_session *session);

sslab_v1_status sslab_v1_run_smoke_proof(
    sslab_v1_context *context,
    const sslab_v1_run_desc *desc,
    sslab_v1_smoke_receipt *receipt);

const char *sslab_v1_status_name(sslab_v1_status status);

#ifdef __cplusplus
}
#endif

#endif /* SCREENSAVE_SSLAB_V1_H */
