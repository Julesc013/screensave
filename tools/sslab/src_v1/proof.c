#include "runtime_v1.h"

#include <stdlib.h>
#include <string.h>

static ss_u32 sslab_v1_checksum_rgba(const ss_u8 *rgba, ss_u32 byte_count)
{
    ss_u32 checksum;
    ss_u32 index;

    checksum = (ss_u32)2166136261U;
    for (index = 0U; index < byte_count; ++index) {
        checksum ^= (ss_u32)rgba[index];
        checksum *= (ss_u32)16777619U;
    }
    return checksum;
}

sslab_v1_status sslab_v1_run_smoke_proof(
    sslab_v1_context *context,
    const sslab_v1_run_desc *desc,
    sslab_v1_smoke_receipt *receipt)
{
    sslab_v1_product *product;
    sslab_v1_session *session;
    sslab_v1_surface_desc surface;
    ss_v2_clock clock;
    ss_u8 *rgba;
    ss_u32 stride;
    ss_u32 byte_count;
    sslab_v1_status status;

    if (context == 0 || desc == 0 || receipt == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    memset(receipt, 0, sizeof(*receipt));
    receipt->struct_size = (ss_u32)sizeof(*receipt);
    receipt->abi_version = SSLAB_V1_ABI_VERSION;
    receipt->product_key = desc->product_key;
    receipt->width = desc->dimensions.width;
    receipt->height = desc->dimensions.height;

    status = sslab_v1_open_product(context, desc->product_key, &product);
    if (status != SSLAB_V1_STATUS_OK) {
        receipt->status = status;
        return status;
    }
    status = sslab_v1_create_session(product, desc, &session);
    if (status != SSLAB_V1_STATUS_OK) {
        receipt->status = status;
        sslab_v1_close_product(product);
        return status;
    }

    clock = desc->clock;
    if (clock.delta_ms == 0U) {
        clock.delta_ms = 100U;
    }
    status = sslab_v1_advance_session(session, &clock);
    if (status == SSLAB_V1_STATUS_OK) {
        stride = desc->dimensions.width * 4U;
        byte_count = stride * desc->dimensions.height;
        rgba = (ss_u8 *)calloc(1U, (size_t)byte_count);
        if (rgba == 0) {
            status = SSLAB_V1_STATUS_OUT_OF_MEMORY;
        } else {
            surface.struct_size = (ss_u32)sizeof(surface);
            surface.abi_version = SSLAB_V1_ABI_VERSION;
            surface.surface.struct_size = (ss_u32)sizeof(surface.surface);
            surface.surface.abi_version = SS_V2_ABI_VERSION;
            surface.surface.width = desc->dimensions.width;
            surface.surface.height = desc->dimensions.height;
            surface.surface.stride_bytes = (ss_i32)stride;
            surface.surface.format = SS_V2_SURFACE_FORMAT_RGBA8;
            surface.surface.origin = SS_V2_SURFACE_ORIGIN_TOP_LEFT;
            surface.surface.pixels = rgba;
            status = sslab_v1_render_session(session, &surface);
            if (status == SSLAB_V1_STATUS_OK) {
                receipt->checksum = sslab_v1_checksum_rgba(rgba, byte_count);
            }
            free(rgba);
        }
    }

    sslab_v1_destroy_session(session);
    sslab_v1_close_product(product);
    receipt->status = status;
    return status;
}
