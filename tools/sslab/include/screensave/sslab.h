#ifndef SCREENSAVE_SSLAB_H
#define SCREENSAVE_SSLAB_H

/*
 * Private ScreenSave lab/proof ABI v0.
 *
 * This header is intentionally private to tools/sslab. It is not the public
 * saver API and not the portable v2 semantic seam.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define SSLAB_ABI_VERSION 0ul
#define SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES 4ul
#define SSLAB_MAX_CAPTURE_FRAMES 16ul

typedef unsigned char sslab_u8;
typedef unsigned long sslab_u32;
typedef long sslab_i32;

typedef struct sslab_context sslab_context;
typedef struct sslab_product sslab_product;
typedef struct sslab_session sslab_session;
typedef struct sslab_capture sslab_capture;
typedef struct sslab_proof_profile sslab_proof_profile;

typedef enum sslab_status {
    SSLAB_STATUS_OK = 0,
    SSLAB_STATUS_FAIL = 1,
    SSLAB_STATUS_BLOCKED = 2,
    SSLAB_STATUS_UNSUPPORTED_HOST = 3,
    SSLAB_STATUS_INVALID_ARGUMENT = 4,
    SSLAB_STATUS_NOT_FOUND = 5,
    SSLAB_STATUS_OUT_OF_MEMORY = 6
} sslab_status;

typedef enum sslab_comparison_class {
    SSLAB_COMPARISON_EXACT = 0,
    SSLAB_COMPARISON_TOLERANT = 1,
    SSLAB_COMPARISON_PERCEPTUAL = 2,
    SSLAB_COMPARISON_OBSERVATIONAL = 3
} sslab_comparison_class;

typedef struct sslab_abi_info {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_u32 required_unsigned_long_bytes;
    sslab_u32 observed_unsigned_long_bytes;
    sslab_u32 max_capture_frames;
} sslab_abi_info;

typedef struct sslab_context_desc {
    sslab_u32 size;
    sslab_u32 abi_version;
    const char *output_root;
    const char *catalog_root;
} sslab_context_desc;

typedef struct sslab_surface_desc {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_u32 width;
    sslab_u32 height;
    sslab_u32 stride;
    sslab_u8 *rgba;
} sslab_surface_desc;

typedef struct sslab_run_desc {
    sslab_u32 size;
    sslab_u32 abi_version;
    const char *product_key;
    const char *preset_key;
    sslab_u32 width;
    sslab_u32 height;
    sslab_u32 seed;
    sslab_u32 delta_ms;
    sslab_u32 frame_count;
} sslab_run_desc;

typedef struct sslab_lifecycle_desc {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_run_desc run;
    sslab_u32 resize_width;
    sslab_u32 resize_height;
    sslab_u32 create_destroy_cycles;
} sslab_lifecycle_desc;

typedef struct sslab_profile_desc {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_run_desc run;
    sslab_u32 warmup_frames;
    sslab_u32 measured_frames;
    sslab_u32 short_soak_frames;
} sslab_profile_desc;

typedef struct sslab_proof_profile_desc {
    sslab_u32 size;
    sslab_u32 abi_version;
    const char *profile_key;
    const char *product_key;
    const char *preset_key;
    sslab_u32 width;
    sslab_u32 height;
    sslab_u32 resize_width;
    sslab_u32 resize_height;
    sslab_u32 seed;
    sslab_u32 delta_ms;
    sslab_u32 capture_frame_count;
    sslab_u32 capture_frames[SSLAB_MAX_CAPTURE_FRAMES];
    sslab_comparison_class comparison_class;
} sslab_proof_profile_desc;

typedef struct sslab_capture_receipt {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_status status;
    sslab_u32 frame_index;
    sslab_u32 width;
    sslab_u32 height;
    char rgba_sha256[65];
    char raw_rgba_path[260];
    char review_ppm_path[260];
} sslab_capture_receipt;

typedef struct sslab_lifecycle_receipt {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_status status;
    sslab_u32 create_session;
    sslab_u32 resize_session;
    sslab_u32 step_count;
    sslab_u32 render_session;
    sslab_u32 destroy_session;
    sslab_u32 create_destroy_cycles;
    sslab_u32 checksum;
} sslab_lifecycle_receipt;

typedef struct sslab_profile_receipt {
    sslab_u32 size;
    sslab_u32 abi_version;
    sslab_status status;
    sslab_u32 measured_frames;
    sslab_u32 short_soak_frames;
    double frame_time_ms_p50;
    double frame_time_ms_p95;
    double frame_time_ms_p99;
} sslab_profile_receipt;

sslab_status sslab_get_abi_info(sslab_abi_info *info);
sslab_status sslab_check_host_abi(void);

sslab_status sslab_create_context(const sslab_context_desc *desc, sslab_context **context);
void sslab_destroy_context(sslab_context *context);

sslab_status sslab_open_product(sslab_context *context, const char *product_key, sslab_product **product);
void sslab_close_product(sslab_product *product);

sslab_status sslab_create_session(sslab_product *product, const sslab_run_desc *desc, sslab_session **session);
sslab_status sslab_resize_session(sslab_session *session, sslab_u32 width, sslab_u32 height);
sslab_status sslab_step_session(sslab_session *session, sslab_u32 delta_ms);
sslab_status sslab_render_session(sslab_session *session, sslab_surface_desc *surface);
void sslab_destroy_session(sslab_session *session);

sslab_status sslab_run_capture(
    sslab_context *context,
    const sslab_proof_profile_desc *profile,
    sslab_u32 frame_index,
    sslab_capture_receipt *receipt);

sslab_status sslab_run_lifecycle(
    sslab_context *context,
    const sslab_lifecycle_desc *desc,
    sslab_lifecycle_receipt *receipt);

sslab_status sslab_run_profile(
    sslab_context *context,
    const sslab_profile_desc *desc,
    sslab_profile_receipt *receipt);

#ifdef __cplusplus
}
#endif

#endif
