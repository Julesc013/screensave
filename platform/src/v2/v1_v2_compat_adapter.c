#include "screensave/private/v1_v2_compat_adapter.h"

#include <stdlib.h>
#include <string.h>

#include "screensave/private/v2_draw_renderer_bridge.h"

struct ss_v1_v2_compat_session_tag {
    const ss_v2_product_descriptor *product;
    ss_v2_session *session;
    ss_u32 mode;
    ss_u32 width;
    ss_u32 height;
};

static ss_u32 ss_v1_v2_mode_from_environment(const screensave_saver_environment *environment)
{
    if (environment == 0) {
        return SS_V2_SESSION_MODE_SCREEN;
    }
    switch (environment->mode) {
    case SCREENSAVE_SESSION_MODE_PREVIEW:
        return SS_V2_SESSION_MODE_PREVIEW;
    case SCREENSAVE_SESSION_MODE_WINDOWED:
        return SS_V2_SESSION_MODE_WINDOWED;
    case SCREENSAVE_SESSION_MODE_SCREEN:
    default:
        return SS_V2_SESSION_MODE_SCREEN;
    }
}

static void ss_v1_v2_u64_from_low(ss_v2_u64_parts *value, unsigned long low)
{
    value->struct_size = (ss_u32)sizeof(*value);
    value->abi_version = SS_V2_ABI_VERSION;
    value->low = (ss_u32)low;
    value->high = 0U;
}

static void ss_v1_v2_clock_from_environment(const screensave_saver_environment *environment, ss_v2_clock *clock)
{
    clock->struct_size = (ss_u32)sizeof(*clock);
    clock->abi_version = SS_V2_ABI_VERSION;
    if (environment != 0) {
        ss_v1_v2_u64_from_low(&clock->frame_index, environment->clock.frame_index);
        ss_v1_v2_u64_from_low(&clock->elapsed_ms, environment->clock.elapsed_millis);
        clock->delta_ms = (ss_u32)environment->clock.delta_millis;
        clock->fixed_step_ms = (ss_u32)environment->clock.delta_millis;
    } else {
        ss_v1_v2_u64_from_low(&clock->frame_index, 0UL);
        ss_v1_v2_u64_from_low(&clock->elapsed_ms, 0UL);
        clock->delta_ms = 0U;
        clock->fixed_step_ms = 0U;
    }
}

static void ss_v1_v2_seed_from_environment(const screensave_saver_environment *environment, ss_v2_seed *seed)
{
    seed->struct_size = (ss_u32)sizeof(*seed);
    seed->abi_version = SS_V2_ABI_VERSION;
    if (environment != 0) {
        seed->base_seed = (ss_u32)environment->seed.base_seed;
        seed->stream_seed = (ss_u32)environment->seed.stream_seed;
        seed->deterministic = environment->seed.deterministic ? SS_V2_TRUE : SS_V2_FALSE;
    } else {
        seed->base_seed = 0U;
        seed->stream_seed = 0U;
        seed->deterministic = SS_V2_FALSE;
    }
}

static void ss_v1_v2_size_from_environment(const screensave_saver_environment *environment, ss_v2_size *size)
{
    size->struct_size = (ss_u32)sizeof(*size);
    size->abi_version = SS_V2_ABI_VERSION;
    if (environment != 0) {
        size->width = (ss_u32)environment->drawable_size.width;
        size->height = (ss_u32)environment->drawable_size.height;
    } else {
        size->width = 0U;
        size->height = 0U;
    }
}

static ss_u32 ss_v1_v2_config_from_args(
    const ss_v2_product_descriptor *product,
    const screensave_saver_environment *environment,
    const void *product_config,
    ss_u32 product_config_size,
    ss_v2_config_view *view)
{
    const void *bytes;
    ss_u32 byte_count;

    bytes = product_config;
    byte_count = product_config_size;
    if (bytes == 0 && environment != 0 && environment->config_binding != 0) {
        bytes = environment->config_binding->product_config;
        byte_count = (ss_u32)environment->config_binding->product_config_size;
    }
    if (product == 0 || bytes == 0 || byte_count == 0U || view == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    view->struct_size = (ss_u32)sizeof(*view);
    view->abi_version = SS_V2_ABI_VERSION;
    view->product_schema_id = product->config_schema_id;
    view->schema_version = product->config_schema_version;
    view->bytes = bytes;
    view->byte_count = byte_count;
    return SS_V2_STATUS_OK;
}

ss_u32 ss_v1_v2_compat_create(
    const ss_v2_product_descriptor *product,
    const screensave_saver_environment *environment,
    const void *product_config,
    ss_u32 product_config_size,
    ss_v1_v2_compat_session **session_out)
{
    ss_v1_v2_compat_session *created;
    ss_v2_session_desc desc;
    ss_u32 status;

    if (product == 0 || environment == 0 || session_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    *session_out = 0;
    status = ss_v2_product_descriptor_is_valid(product);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    created = (ss_v1_v2_compat_session *)calloc(1U, sizeof(*created));
    if (created == 0) {
        return SS_V2_STATUS_FAIL;
    }

    memset(&desc, 0, sizeof(desc));
    desc.struct_size = (ss_u32)sizeof(desc);
    desc.abi_version = SS_V2_ABI_VERSION;
    desc.mode = ss_v1_v2_mode_from_environment(environment);
    ss_v1_v2_size_from_environment(environment, &desc.dimensions);
    ss_v1_v2_seed_from_environment(environment, &desc.seed);
    ss_v1_v2_clock_from_environment(environment, &desc.clock);
    status = ss_v1_v2_config_from_args(product, environment, product_config, product_config_size, &desc.product_config);
    if (status != SS_V2_STATUS_OK) {
        free(created);
        return status;
    }
    desc.diagnostics = 0;

    status = product->session_ops->create(&desc, &created->session);
    if (status != SS_V2_STATUS_OK) {
        free(created);
        return status;
    }
    created->product = product;
    created->mode = desc.mode;
    created->width = desc.dimensions.width;
    created->height = desc.dimensions.height;
    *session_out = created;
    return SS_V2_STATUS_OK;
}

void ss_v1_v2_compat_destroy(ss_v1_v2_compat_session *session)
{
    if (session == 0) {
        return;
    }
    if (session->product != 0 && session->product->session_ops != 0 && session->product->session_ops->destroy != 0) {
        session->product->session_ops->destroy(session->session);
    }
    free(session);
}

ss_u32 ss_v1_v2_compat_resize(
    ss_v1_v2_compat_session *session,
    const screensave_saver_environment *environment)
{
    ss_v2_resize_desc desc;
    ss_u32 status;

    if (session == 0 || environment == 0 || session->product == 0 || session->product->session_ops == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    desc.struct_size = (ss_u32)sizeof(desc);
    desc.abi_version = SS_V2_ABI_VERSION;
    ss_v1_v2_size_from_environment(environment, &desc.dimensions);
    desc.diagnostics = 0;
    status = session->product->session_ops->resize(session->session, &desc);
    if (status == SS_V2_STATUS_OK) {
        session->width = desc.dimensions.width;
        session->height = desc.dimensions.height;
    }
    return status;
}

ss_u32 ss_v1_v2_compat_advance(
    ss_v1_v2_compat_session *session,
    const screensave_saver_environment *environment)
{
    ss_v2_advance_desc desc;

    if (session == 0 || environment == 0 || session->product == 0 || session->product->session_ops == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    desc.struct_size = (ss_u32)sizeof(desc);
    desc.abi_version = SS_V2_ABI_VERSION;
    ss_v1_v2_clock_from_environment(environment, &desc.clock);
    desc.diagnostics = 0;
    return session->product->session_ops->advance(session->session, &desc);
}

ss_u32 ss_v1_v2_compat_render(
    ss_v1_v2_compat_session *session,
    const screensave_saver_environment *environment)
{
    ss_v2_renderer_draw_bridge bridge;
    ss_v2_draw_target target;
    ss_v2_render_desc desc;
    ss_u32 status;

    if (
        session == 0 ||
        environment == 0 ||
        environment->renderer == 0 ||
        session->product == 0 ||
        session->product->session_ops == 0
    ) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    ss_v2_renderer_draw_bridge_init(&bridge, environment->renderer);
    status = ss_v2_renderer_draw_bridge_make_target(&bridge, &target);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    desc.struct_size = (ss_u32)sizeof(desc);
    desc.abi_version = SS_V2_ABI_VERSION;
    ss_v1_v2_clock_from_environment(environment, &desc.clock);
    desc.draw_target = &target;
    desc.surface = 0;
    desc.diagnostics = 0;
    return session->product->session_ops->render(session->session, &desc);
}
