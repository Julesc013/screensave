#include "runtime.h"

#include <stdlib.h>
#include <string.h>

static void sslab_prepare_environment(sslab_session *session, sslab_u32 width, sslab_u32 height)
{
    memset(&session->environment, 0, sizeof(session->environment));
    session->environment.mode = session->product->adapter->session_mode;
    session->environment.drawable_size.width = (int)width;
    session->environment.drawable_size.height = (int)height;
    session->environment.clock.session_start_millis = 0UL;
    session->environment.clock.elapsed_millis = session->elapsed_ms;
    session->environment.clock.delta_millis = 0UL;
    session->environment.clock.frame_index = session->frame_index;
    session->environment.seed.base_seed = 0UL;
    session->environment.seed.stream_seed = 0UL;
    session->environment.seed.deterministic = 1;
    session->environment.config_binding = &session->binding;
    session->environment.renderer = &session->renderer;
    session->environment.diagnostics = 0;
}

static sslab_status sslab_status_from_v2(ss_u32 status)
{
    switch (status) {
    case SS_V2_STATUS_OK:
        return SSLAB_STATUS_OK;
    case SS_V2_STATUS_BAD_ARGUMENT:
    case SS_V2_STATUS_BAD_SIZE:
    case SS_V2_STATUS_BAD_VERSION:
        return SSLAB_STATUS_INVALID_ARGUMENT;
    case SS_V2_STATUS_UNSUPPORTED:
        return SSLAB_STATUS_UNSUPPORTED_HOST;
    case SS_V2_STATUS_FAIL:
    default:
        return SSLAB_STATUS_FAIL;
    }
}

static void sslab_v2_u64_low(ss_v2_u64_parts *value, sslab_u32 low)
{
    value->struct_size = (ss_u32)sizeof(*value);
    value->abi_version = SS_V2_ABI_VERSION;
    value->low = low;
    value->high = 0U;
}

static void sslab_v2_clock_from_session(sslab_session *session, ss_v2_clock *clock, sslab_u32 delta_ms)
{
    clock->struct_size = (ss_u32)sizeof(*clock);
    clock->abi_version = SS_V2_ABI_VERSION;
    sslab_v2_u64_low(&clock->frame_index, session->frame_index);
    sslab_v2_u64_low(&clock->elapsed_ms, session->elapsed_ms);
    clock->delta_ms = delta_ms;
    clock->fixed_step_ms = delta_ms;
}

static ss_u32 sslab_v2_mode_from_adapter(const sslab_product_adapter_v0 *adapter)
{
    if (adapter == 0) {
        return SS_V2_SESSION_MODE_PROOF;
    }
    switch (adapter->session_mode) {
    case SCREENSAVE_SESSION_MODE_SCREEN:
        return SS_V2_SESSION_MODE_SCREEN;
    case SCREENSAVE_SESSION_MODE_PREVIEW:
        return SS_V2_SESSION_MODE_PREVIEW;
    case SCREENSAVE_SESSION_MODE_WINDOWED:
        return SS_V2_SESSION_MODE_WINDOWED;
    default:
        return SS_V2_SESSION_MODE_PROOF;
    }
}

static sslab_status sslab_create_v2_session(sslab_session *session, const sslab_run_desc *desc)
{
    const ss_v2_product_descriptor *descriptor;
    ss_v2_session_desc v2_desc;
    ss_u32 v2_status;

    if (session == 0 || session->product == 0 || session->product->adapter == 0 || desc == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    if (session->product->adapter->v2_descriptor == 0) {
        return SSLAB_STATUS_NOT_FOUND;
    }
    descriptor = session->product->adapter->v2_descriptor();
    v2_status = ss_v2_product_descriptor_is_valid(descriptor);
    if (v2_status != SS_V2_STATUS_OK) {
        return sslab_status_from_v2(v2_status);
    }

    memset(&v2_desc, 0, sizeof(v2_desc));
    v2_desc.struct_size = (ss_u32)sizeof(v2_desc);
    v2_desc.abi_version = SS_V2_ABI_VERSION;
    v2_desc.mode = sslab_v2_mode_from_adapter(session->product->adapter);
    v2_desc.dimensions.struct_size = (ss_u32)sizeof(v2_desc.dimensions);
    v2_desc.dimensions.abi_version = SS_V2_ABI_VERSION;
    v2_desc.dimensions.width = desc->width;
    v2_desc.dimensions.height = desc->height;
    v2_desc.seed.struct_size = (ss_u32)sizeof(v2_desc.seed);
    v2_desc.seed.abi_version = SS_V2_ABI_VERSION;
    v2_desc.seed.base_seed = desc->seed;
    v2_desc.seed.stream_seed = (ss_u32)session->environment.seed.stream_seed;
    v2_desc.seed.deterministic = SS_V2_TRUE;
    sslab_v2_clock_from_session(session, &v2_desc.clock, 0U);
    v2_desc.product_config.struct_size = (ss_u32)sizeof(v2_desc.product_config);
    v2_desc.product_config.abi_version = SS_V2_ABI_VERSION;
    v2_desc.product_config.product_schema_id = descriptor->config_schema_id;
    v2_desc.product_config.schema_version = descriptor->config_schema_version;
    v2_desc.product_config.bytes = session->product_config;
    v2_desc.product_config.byte_count = (ss_u32)session->product->adapter->product_config_size;
    v2_desc.diagnostics = 0;

    v2_status = descriptor->session_ops->create(&v2_desc, &session->v2_product_session);
    return sslab_status_from_v2(v2_status);
}

sslab_status sslab_create_session(sslab_product *product, const sslab_run_desc *desc, sslab_session **session_out)
{
    sslab_session *session;
    const char *preset_key;
    sslab_status status;

    if (product == 0 || product->adapter == 0 || desc == 0 || session_out == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    *session_out = 0;
    status = sslab_validate_desc(desc->size, sizeof(*desc), desc->abi_version);
    if (status != SSLAB_STATUS_OK) {
        return status;
    }
    if (desc->width == 0UL || desc->height == 0UL) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    if (desc->product_key != 0 && strcmp(desc->product_key, product->adapter->product_key) != 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }

    session = (sslab_session *)calloc(1U, sizeof(*session));
    if (session == 0) {
        return SSLAB_STATUS_OUT_OF_MEMORY;
    }
    session->product = product;
    session->width = desc->width;
    session->height = desc->height;
    session->product_config = calloc(1U, product->adapter->product_config_size);
    if (session->product_config == 0) {
        free(session);
        return SSLAB_STATUS_OUT_OF_MEMORY;
    }

    preset_key = desc->preset_key != 0 ? desc->preset_key : product->adapter->default_preset_key;
    status = product->adapter->configure(
        preset_key,
        &session->common_config,
        session->product_config,
        product->adapter->product_config_size);
    if (status != SSLAB_STATUS_OK) {
        free(session->product_config);
        free(session);
        return status;
    }

    screensave_config_binding_init(
        &session->binding,
        &session->common_config,
        session->product_config,
        product->adapter->product_config_size);
    sslab_rgba8_renderer_init(&session->renderer, 0);
    sslab_prepare_environment(session, desc->width, desc->height);
    session->environment.seed.base_seed = desc->seed;
    session->environment.seed.stream_seed = product->adapter->stream_seed(desc->seed);

    if (sslab_get_execution_path(product->context) == SSLAB_EXECUTION_PATH_V2) {
        status = sslab_create_v2_session(session, desc);
    } else {
        status = product->adapter->create(product->adapter, &session->product_session, &session->environment);
    }
    if (status != SSLAB_STATUS_OK) {
        free(session->product_config);
        free(session);
        return status;
    }

    *session_out = session;
    return SSLAB_STATUS_OK;
}

sslab_status sslab_resize_session(sslab_session *session, sslab_u32 width, sslab_u32 height)
{
    const ss_v2_product_descriptor *descriptor;
    ss_v2_resize_desc resize_desc;
    ss_u32 v2_status;

    if (session == 0 || width == 0UL || height == 0UL) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    session->width = width;
    session->height = height;
    if (sslab_get_execution_path(session->product->context) == SSLAB_EXECUTION_PATH_V2) {
        descriptor = session->product->adapter->v2_descriptor();
        resize_desc.struct_size = (ss_u32)sizeof(resize_desc);
        resize_desc.abi_version = SS_V2_ABI_VERSION;
        resize_desc.dimensions.struct_size = (ss_u32)sizeof(resize_desc.dimensions);
        resize_desc.dimensions.abi_version = SS_V2_ABI_VERSION;
        resize_desc.dimensions.width = width;
        resize_desc.dimensions.height = height;
        resize_desc.diagnostics = 0;
        v2_status = descriptor->session_ops->resize(session->v2_product_session, &resize_desc);
        return sslab_status_from_v2(v2_status);
    }
    session->environment.drawable_size.width = (int)width;
    session->environment.drawable_size.height = (int)height;
    session->product->adapter->resize(session->product_session, &session->environment);
    return SSLAB_STATUS_OK;
}

sslab_status sslab_step_session(sslab_session *session, sslab_u32 delta_ms)
{
    const ss_v2_product_descriptor *descriptor;
    ss_v2_advance_desc advance_desc;
    ss_u32 v2_status;

    if (session == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    session->elapsed_ms += delta_ms;
    if (sslab_get_execution_path(session->product->context) == SSLAB_EXECUTION_PATH_V2) {
        descriptor = session->product->adapter->v2_descriptor();
        advance_desc.struct_size = (ss_u32)sizeof(advance_desc);
        advance_desc.abi_version = SS_V2_ABI_VERSION;
        sslab_v2_clock_from_session(session, &advance_desc.clock, delta_ms);
        advance_desc.diagnostics = 0;
        v2_status = descriptor->session_ops->advance(session->v2_product_session, &advance_desc);
        if (v2_status == SS_V2_STATUS_OK) {
            ++session->frame_index;
        }
        return sslab_status_from_v2(v2_status);
    }
    session->environment.clock.delta_millis = delta_ms;
    session->environment.clock.elapsed_millis = session->elapsed_ms;
    session->environment.clock.frame_index = session->frame_index;
    session->product->adapter->step(session->product_session, &session->environment);
    ++session->frame_index;
    return SSLAB_STATUS_OK;
}

sslab_status sslab_render_session(sslab_session *session, sslab_surface_desc *surface_desc)
{
    screensave_rgba8_surface surface;
    const ss_v2_product_descriptor *descriptor;
    ss_v2_surface_desc v2_surface;
    ss_v2_draw_target draw_target;
    ss_v2_render_desc render_desc;
    sslab_status status;
    ss_u32 v2_status;

    if (session == 0 || surface_desc == 0 || surface_desc->rgba == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    status = sslab_validate_desc(surface_desc->size, sizeof(*surface_desc), surface_desc->abi_version);
    if (status != SSLAB_STATUS_OK) {
        return status;
    }
    surface.width = (int)surface_desc->width;
    surface.height = (int)surface_desc->height;
    surface.stride_bytes = (int)surface_desc->stride;
    surface.pixels = surface_desc->rgba;
    sslab_rgba8_renderer_init(&session->renderer, &surface);
    session->environment.renderer = &session->renderer;
    session->environment.drawable_size.width = (int)surface_desc->width;
    session->environment.drawable_size.height = (int)surface_desc->height;
    if (sslab_get_execution_path(session->product->context) == SSLAB_EXECUTION_PATH_V2) {
        descriptor = session->product->adapter->v2_descriptor();
        v2_surface.struct_size = (ss_u32)sizeof(v2_surface);
        v2_surface.abi_version = SS_V2_ABI_VERSION;
        v2_surface.width = surface_desc->width;
        v2_surface.height = surface_desc->height;
        v2_surface.stride_bytes = surface_desc->stride;
        v2_surface.format = SS_V2_SURFACE_FORMAT_RGBA8;
        v2_surface.origin = SS_V2_SURFACE_ORIGIN_TOP_LEFT;
        v2_surface.pixels = surface_desc->rgba;
        draw_target.struct_size = (ss_u32)sizeof(draw_target);
        draw_target.abi_version = SS_V2_ABI_VERSION;
        draw_target.user_data = &session->renderer;
        draw_target.ops = 0;
        draw_target.surface = &v2_surface;
        render_desc.struct_size = (ss_u32)sizeof(render_desc);
        render_desc.abi_version = SS_V2_ABI_VERSION;
        sslab_v2_clock_from_session(session, &render_desc.clock, 0U);
        render_desc.draw_target = &draw_target;
        render_desc.surface = &v2_surface;
        render_desc.diagnostics = 0;
        v2_status = descriptor->session_ops->render(session->v2_product_session, &render_desc);
        sslab_rgba8_renderer_init(&session->renderer, 0);
        return sslab_status_from_v2(v2_status);
    }
    session->product->adapter->render(session->product_session, &session->environment);
    sslab_rgba8_renderer_init(&session->renderer, 0);
    return SSLAB_STATUS_OK;
}

void sslab_destroy_session(sslab_session *session)
{
    if (session == 0) {
        return;
    }
    if (session->v2_product_session != 0 && session->product != 0 && session->product->adapter != 0) {
        const ss_v2_product_descriptor *descriptor;

        descriptor = session->product->adapter->v2_descriptor();
        if (descriptor != 0 && descriptor->session_ops != 0 && descriptor->session_ops->destroy != 0) {
            descriptor->session_ops->destroy(session->v2_product_session);
        }
    }
    if (session->product_session != 0 && session->product != 0 && session->product->adapter != 0) {
        session->product->adapter->destroy(session->product_session);
    }
    free(session->product_config);
    free(session);
}
