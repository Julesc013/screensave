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

    status = product->adapter->create(product->adapter, &session->product_session, &session->environment);
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
    if (session == 0 || width == 0UL || height == 0UL) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    session->width = width;
    session->height = height;
    session->environment.drawable_size.width = (int)width;
    session->environment.drawable_size.height = (int)height;
    session->product->adapter->resize(session->product_session, &session->environment);
    return SSLAB_STATUS_OK;
}

sslab_status sslab_step_session(sslab_session *session, sslab_u32 delta_ms)
{
    if (session == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    session->elapsed_ms += delta_ms;
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
    sslab_status status;

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
    session->product->adapter->render(session->product_session, &session->environment);
    sslab_rgba8_renderer_init(&session->renderer, 0);
    return SSLAB_STATUS_OK;
}

void sslab_destroy_session(sslab_session *session)
{
    if (session == 0) {
        return;
    }
    if (session->product_session != 0 && session->product != 0 && session->product->adapter != 0) {
        session->product->adapter->destroy(session->product_session);
    }
    free(session->product_config);
    free(session);
}
