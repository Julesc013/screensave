#include "ricochet_v2_adapter.h"

#include "ricochet_internal.h"

#include <stdlib.h>
#include <string.h>

typedef struct ricochet_v2_session_tag {
    screensave_saver_session *v1_session;
    screensave_common_config common_config;
    ricochet_config product_config;
    screensave_config_binding binding;
    screensave_saver_environment environment;
} ricochet_v2_session;

static unsigned long ricochet_v2_clock_low(const ss_v2_u64_parts *value)
{
    if (value == NULL) {
        return 0UL;
    }
    return (unsigned long)value->low;
}

static screensave_session_mode ricochet_v2_session_mode(ss_u32 mode)
{
    switch (mode) {
    case SS_V2_SESSION_MODE_PREVIEW:
        return SCREENSAVE_SESSION_MODE_PREVIEW;
    case SS_V2_SESSION_MODE_SCREEN:
        return SCREENSAVE_SESSION_MODE_SCREEN;
    case SS_V2_SESSION_MODE_WINDOWED:
    case SS_V2_SESSION_MODE_PROOF:
    default:
        return SCREENSAVE_SESSION_MODE_WINDOWED;
    }
}

static void ricochet_v2_prepare_environment(
    ricochet_v2_session *session,
    ss_u32 mode,
    ss_u32 width,
    ss_u32 height,
    const ss_v2_clock *clock,
    const ss_v2_seed *seed,
    screensave_renderer *renderer)
{
    memset(&session->environment, 0, sizeof(session->environment));
    session->environment.mode = ricochet_v2_session_mode(mode);
    session->environment.drawable_size.width = (int)width;
    session->environment.drawable_size.height = (int)height;
    if (clock != NULL) {
        session->environment.clock.session_start_millis = 0UL;
        session->environment.clock.elapsed_millis = ricochet_v2_clock_low(&clock->elapsed_ms);
        session->environment.clock.delta_millis = (unsigned long)clock->delta_ms;
        session->environment.clock.frame_index = ricochet_v2_clock_low(&clock->frame_index);
    }
    if (seed != NULL) {
        session->environment.seed.base_seed = (unsigned long)seed->base_seed;
        session->environment.seed.stream_seed = (unsigned long)seed->stream_seed;
        session->environment.seed.deterministic = seed->deterministic != 0U ? 1 : 0;
    }
    session->environment.config_binding = &session->binding;
    session->environment.renderer = renderer;
    session->environment.diagnostics = 0;
}

static ss_u32 ricochet_v2_create(const ss_v2_session_desc *desc, ss_v2_session **session_out)
{
    ricochet_v2_session *session;
    ss_u32 status;

    if (session_out == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    *session_out = NULL;
    status = ss_v2_session_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (
        desc->product_config.bytes == NULL ||
        desc->product_config.byte_count < (ss_u32)sizeof(ricochet_config)
    ) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    session = (ricochet_v2_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return SS_V2_STATUS_FAIL;
    }

    screensave_common_config_set_defaults(&session->common_config);
    ricochet_apply_preset_to_config(
        RICOCHET_DEFAULT_PRESET_KEY,
        &session->common_config,
        &session->product_config);
    memcpy(&session->product_config, desc->product_config.bytes, sizeof(session->product_config));
    screensave_config_binding_init(
        &session->binding,
        &session->common_config,
        &session->product_config,
        sizeof(session->product_config));
    ricochet_v2_prepare_environment(
        session,
        desc->mode,
        desc->dimensions.width,
        desc->dimensions.height,
        &desc->clock,
        &desc->seed,
        NULL);
    if (!ricochet_create_session(NULL, &session->v1_session, &session->environment)) {
        free(session);
        return SS_V2_STATUS_FAIL;
    }

    *session_out = (ss_v2_session *)session;
    return SS_V2_STATUS_OK;
}

static void ricochet_v2_destroy(ss_v2_session *opaque_session)
{
    ricochet_v2_session *session;

    session = (ricochet_v2_session *)opaque_session;
    if (session == NULL) {
        return;
    }
    ricochet_destroy_session(session->v1_session);
    free(session);
}

static ss_u32 ricochet_v2_resize(ss_v2_session *opaque_session, const ss_v2_resize_desc *desc)
{
    ricochet_v2_session *session;
    ss_u32 status;

    session = (ricochet_v2_session *)opaque_session;
    if (session == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_resize_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    ricochet_v2_prepare_environment(
        session,
        SS_V2_SESSION_MODE_WINDOWED,
        desc->dimensions.width,
        desc->dimensions.height,
        NULL,
        NULL,
        NULL);
    ricochet_resize_session(session->v1_session, &session->environment);
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_advance(ss_v2_session *opaque_session, const ss_v2_advance_desc *desc)
{
    ricochet_v2_session *session;
    ss_u32 status;

    session = (ricochet_v2_session *)opaque_session;
    if (session == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_advance_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    ricochet_v2_prepare_environment(
        session,
        SS_V2_SESSION_MODE_WINDOWED,
        (ss_u32)session->environment.drawable_size.width,
        (ss_u32)session->environment.drawable_size.height,
        &desc->clock,
        NULL,
        NULL);
    ricochet_step_session(session->v1_session, &session->environment);
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_render(ss_v2_session *opaque_session, const ss_v2_render_desc *desc)
{
    ricochet_v2_session *session;
    screensave_renderer *renderer;
    ss_u32 status;

    session = (ricochet_v2_session *)opaque_session;
    if (session == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_render_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (desc->draw_target == NULL || desc->draw_target->user_data == NULL || desc->surface == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    renderer = (screensave_renderer *)desc->draw_target->user_data;
    ricochet_v2_prepare_environment(
        session,
        SS_V2_SESSION_MODE_WINDOWED,
        desc->surface->width,
        desc->surface->height,
        &desc->clock,
        NULL,
        renderer);
    ricochet_render_session(session->v1_session, &session->environment);
    return SS_V2_STATUS_OK;
}

static const ss_v2_session_ops g_ricochet_v2_session_ops = {
    (ss_u32)sizeof(ss_v2_session_ops),
    SS_V2_ABI_VERSION,
    ricochet_v2_create,
    ricochet_v2_destroy,
    ricochet_v2_resize,
    ricochet_v2_advance,
    ricochet_v2_render
};

static const ss_v2_product_descriptor g_ricochet_v2_product = {
    (ss_u32)sizeof(ss_v2_product_descriptor),
    SS_V2_ABI_VERSION,
    "ricochet",
    "Ricochet",
    "v1",
    "screensave.product.ricochet.config",
    1U,
    SS_V2_PRODUCT_CAP_REFERENCE_CPU |
        SS_V2_PRODUCT_CAP_RGBA8_SURFACE |
        SS_V2_PRODUCT_CAP_FIXED_STEP |
        SS_V2_PRODUCT_CAP_DETERMINISTIC_SEED,
    &g_ricochet_v2_session_ops
};

const ss_v2_product_descriptor *ricochet_v2_product_descriptor(void)
{
    return &g_ricochet_v2_product;
}
