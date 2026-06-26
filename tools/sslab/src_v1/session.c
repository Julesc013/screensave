#include "runtime_v1.h"

#include <stdlib.h>
#include <string.h>

static void sslab_v1_u64_low(ss_v2_u64_parts *value, ss_u32 low)
{
    value->struct_size = (ss_u32)sizeof(*value);
    value->abi_version = SS_V2_ABI_VERSION;
    value->low = low;
    value->high = 0U;
}

void sslab_v1_clock_from_values(ss_u32 frame_index, ss_u32 elapsed_ms, ss_u32 delta_ms, ss_v2_clock *clock)
{
    clock->struct_size = (ss_u32)sizeof(*clock);
    clock->abi_version = SS_V2_ABI_VERSION;
    sslab_v1_u64_low(&clock->frame_index, frame_index);
    sslab_v1_u64_low(&clock->elapsed_ms, elapsed_ms);
    clock->delta_ms = delta_ms;
    clock->fixed_step_ms = delta_ms;
}

static void sslab_v1_size_init(ss_v2_size *size, ss_u32 width, ss_u32 height)
{
    size->struct_size = (ss_u32)sizeof(*size);
    size->abi_version = SS_V2_ABI_VERSION;
    size->width = width;
    size->height = height;
}

sslab_v1_status sslab_v1_create_session(
    sslab_v1_product *product,
    const sslab_v1_run_desc *desc,
    sslab_v1_session **session_out)
{
    sslab_v1_session *session;
    const ss_v2_product_descriptor *descriptor;
    ss_v2_session_desc v2_desc;
    sslab_v1_status status;
    ss_u32 v2_status;

    if (product == 0 || product->definition == 0 || desc == 0 || session_out == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    *session_out = 0;
    status = sslab_v1_check_prefix(desc->struct_size, (ss_u32)sizeof(*desc), desc->abi_version);
    if (status != SSLAB_V1_STATUS_OK) {
        return status;
    }
    if (desc->dimensions.width == 0U || desc->dimensions.height == 0U) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    if (desc->product_key != 0 && strcmp(desc->product_key, product->definition->product_key) != 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }

    session = (sslab_v1_session *)calloc(1U, sizeof(*session));
    if (session == 0) {
        return SSLAB_V1_STATUS_OUT_OF_MEMORY;
    }
    session->product = product;
    session->width = desc->dimensions.width;
    session->height = desc->dimensions.height;
    status = sslab_v1_configure_product(
        product->definition,
        desc->preset_key != 0 ? desc->preset_key : product->definition->default_preset_key,
        session->product_config,
        SSLAB_V1_MAX_PRODUCT_CONFIG_BYTES,
        &session->product_config_size);
    if (status != SSLAB_V1_STATUS_OK) {
        free(session);
        return status;
    }

    descriptor = product->definition->descriptor();
    v2_status = ss_v2_product_descriptor_is_valid(descriptor);
    if (v2_status != SS_V2_STATUS_OK) {
        free(session);
        return sslab_v1_status_from_v2(v2_status);
    }

    memset(&v2_desc, 0, sizeof(v2_desc));
    v2_desc.struct_size = (ss_u32)sizeof(v2_desc);
    v2_desc.abi_version = SS_V2_ABI_VERSION;
    v2_desc.mode = product->definition->mode;
    sslab_v1_size_init(&v2_desc.dimensions, session->width, session->height);
    v2_desc.seed.struct_size = (ss_u32)sizeof(v2_desc.seed);
    v2_desc.seed.abi_version = SS_V2_ABI_VERSION;
    v2_desc.seed.base_seed = desc->seed.base_seed;
    v2_desc.seed.stream_seed = product->definition->stream_seed(desc->seed.base_seed);
    v2_desc.seed.deterministic = SS_V2_TRUE;
    sslab_v1_clock_from_values(0U, 0U, desc->clock.delta_ms, &v2_desc.clock);
    v2_desc.product_config.struct_size = (ss_u32)sizeof(v2_desc.product_config);
    v2_desc.product_config.abi_version = SS_V2_ABI_VERSION;
    v2_desc.product_config.product_schema_id = descriptor->config_schema_id;
    v2_desc.product_config.schema_version = descriptor->config_schema_version;
    v2_desc.product_config.bytes = session->product_config;
    v2_desc.product_config.byte_count = session->product_config_size;
    v2_desc.diagnostics = 0;

    v2_status = descriptor->session_ops->create(&v2_desc, &session->v2_session);
    if (v2_status != SS_V2_STATUS_OK) {
        free(session);
        return sslab_v1_status_from_v2(v2_status);
    }
    *session_out = session;
    return SSLAB_V1_STATUS_OK;
}

sslab_v1_status sslab_v1_resize_session(sslab_v1_session *session, const ss_v2_size *dimensions)
{
    const ss_v2_product_descriptor *descriptor;
    ss_v2_resize_desc resize_desc;
    ss_u32 v2_status;

    if (session == 0 || dimensions == 0 || dimensions->width == 0U || dimensions->height == 0U) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    descriptor = session->product->definition->descriptor();
    session->width = dimensions->width;
    session->height = dimensions->height;
    resize_desc.struct_size = (ss_u32)sizeof(resize_desc);
    resize_desc.abi_version = SS_V2_ABI_VERSION;
    sslab_v1_size_init(&resize_desc.dimensions, dimensions->width, dimensions->height);
    resize_desc.diagnostics = 0;
    v2_status = descriptor->session_ops->resize(session->v2_session, &resize_desc);
    return sslab_v1_status_from_v2(v2_status);
}

sslab_v1_status sslab_v1_advance_session(sslab_v1_session *session, const ss_v2_clock *clock)
{
    const ss_v2_product_descriptor *descriptor;
    ss_v2_advance_desc advance_desc;
    ss_u32 delta_ms;
    ss_u32 v2_status;

    if (session == 0 || clock == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    descriptor = session->product->definition->descriptor();
    delta_ms = clock->delta_ms;
    session->elapsed_ms += delta_ms;
    advance_desc.struct_size = (ss_u32)sizeof(advance_desc);
    advance_desc.abi_version = SS_V2_ABI_VERSION;
    sslab_v1_clock_from_values(session->frame_index, session->elapsed_ms, delta_ms, &advance_desc.clock);
    advance_desc.diagnostics = 0;
    v2_status = descriptor->session_ops->advance(session->v2_session, &advance_desc);
    if (v2_status == SS_V2_STATUS_OK) {
        ++session->frame_index;
    }
    return sslab_v1_status_from_v2(v2_status);
}

sslab_v1_status sslab_v1_render_session(sslab_v1_session *session, sslab_v1_surface_desc *surface)
{
    const ss_v2_product_descriptor *descriptor;
    screensave_rgba8_surface rgba_surface;
    ss_v2_draw_target draw_target;
    ss_v2_render_desc render_desc;
    ss_u32 v2_status;

    if (session == 0 || surface == 0 || surface->surface.pixels == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    if (sslab_v1_check_prefix(surface->struct_size, (ss_u32)sizeof(*surface), surface->abi_version) != SSLAB_V1_STATUS_OK) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    descriptor = session->product->definition->descriptor();
    rgba_surface.width = (int)surface->surface.width;
    rgba_surface.height = (int)surface->surface.height;
    rgba_surface.stride_bytes = surface->surface.stride_bytes;
    rgba_surface.pixels = (ss_u8 *)surface->surface.pixels;
    sslab_rgba8_renderer_init(&session->renderer, &rgba_surface);

    draw_target.struct_size = (ss_u32)sizeof(draw_target);
    draw_target.abi_version = SS_V2_ABI_VERSION;
    draw_target.user_data = &session->renderer;
    draw_target.ops = 0;
    draw_target.surface = &surface->surface;

    render_desc.struct_size = (ss_u32)sizeof(render_desc);
    render_desc.abi_version = SS_V2_ABI_VERSION;
    sslab_v1_clock_from_values(session->frame_index, session->elapsed_ms, 0U, &render_desc.clock);
    render_desc.draw_target = &draw_target;
    render_desc.surface = &surface->surface;
    render_desc.diagnostics = 0;
    v2_status = descriptor->session_ops->render(session->v2_session, &render_desc);
    sslab_rgba8_renderer_init(&session->renderer, 0);
    return sslab_v1_status_from_v2(v2_status);
}

void sslab_v1_destroy_session(sslab_v1_session *session)
{
    const ss_v2_product_descriptor *descriptor;

    if (session == 0) {
        return;
    }
    if (session->v2_session != 0 && session->product != 0 && session->product->definition != 0) {
        descriptor = session->product->definition->descriptor();
        if (descriptor != 0 && descriptor->session_ops != 0 && descriptor->session_ops->destroy != 0) {
            descriptor->session_ops->destroy(session->v2_session);
        }
    }
    free(session);
}
