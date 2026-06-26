#include "plasma_v2_adapter.h"

#include <stdlib.h>
#include <string.h>

#include "plasma_spec_v2.h"
#include "plasma_v2_core.h"

typedef struct plasma_v2_session_tag {
    plasma_v2_core_session *core;
    ss_u32 width;
    ss_u32 height;
} plasma_v2_session;

static ss_u32 plasma_v2_load_spec(const ss_v2_config_view *view, plasma_spec_v2 *spec)
{
    if (spec == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plasma_spec_v2_set_defaults(spec);
    if (view == 0 || view->byte_count == 0U) {
        return SS_V2_STATUS_OK;
    }
    if (view->bytes == 0 || view->byte_count < (ss_u32)sizeof(*spec)) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    memcpy(spec, view->bytes, sizeof(*spec));
    plasma_spec_v2_clamp(spec);
    return plasma_spec_v2_is_valid(spec) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_BAD_ARGUMENT;
}

static ss_u32 plasma_v2_create(const ss_v2_session_desc *desc, ss_v2_session **session_out)
{
    plasma_v2_session *session;
    plasma_spec_v2 spec;
    plasma_v2_core_desc core_desc;
    ss_u32 status;

    if (session_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    *session_out = 0;
    status = ss_v2_session_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (desc->product_config.schema_version != PLASMA_SPEC_V2_SCHEMA_VERSION) {
        return SS_V2_STATUS_BAD_VERSION;
    }

    status = plasma_v2_load_spec(&desc->product_config, &spec);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }

    session = (plasma_v2_session *)calloc(1U, sizeof(*session));
    if (session == 0) {
        return SS_V2_STATUS_FAIL;
    }

    core_desc.spec = &spec;
    core_desc.width = desc->dimensions.width;
    core_desc.height = desc->dimensions.height;
    core_desc.base_seed = desc->seed.base_seed;
    core_desc.stream_seed = desc->seed.stream_seed;
    status = plasma_v2_core_create(&core_desc, &session->core);
    if (status != SS_V2_STATUS_OK) {
        free(session);
        return status;
    }
    session->width = desc->dimensions.width;
    session->height = desc->dimensions.height;

    *session_out = (ss_v2_session *)session;
    return SS_V2_STATUS_OK;
}

static void plasma_v2_destroy(ss_v2_session *opaque_session)
{
    plasma_v2_session *session;

    session = (plasma_v2_session *)opaque_session;
    if (session == 0) {
        return;
    }
    plasma_v2_core_destroy(session->core);
    free(session);
}

static ss_u32 plasma_v2_resize(ss_v2_session *opaque_session, const ss_v2_resize_desc *desc)
{
    plasma_v2_session *session;
    ss_u32 status;

    session = (plasma_v2_session *)opaque_session;
    if (session == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_resize_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = plasma_v2_core_resize(session->core, desc->dimensions.width, desc->dimensions.height);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    session->width = desc->dimensions.width;
    session->height = desc->dimensions.height;
    return SS_V2_STATUS_OK;
}

static ss_u32 plasma_v2_advance(ss_v2_session *opaque_session, const ss_v2_advance_desc *desc)
{
    plasma_v2_session *session;
    ss_u32 status;

    session = (plasma_v2_session *)opaque_session;
    if (session == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_advance_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return plasma_v2_core_advance(session->core, desc->clock.delta_ms);
}

static ss_u32 plasma_v2_render(ss_v2_session *opaque_session, const ss_v2_render_desc *desc)
{
    plasma_v2_session *session;
    ss_v2_draw_target surface_target;
    ss_v2_draw_target *target;
    ss_u32 status;

    session = (plasma_v2_session *)opaque_session;
    if (session == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_render_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }

    target = desc->draw_target;
    if (target == 0 && desc->surface != 0) {
        surface_target.struct_size = (ss_u32)sizeof(surface_target);
        surface_target.abi_version = SS_V2_ABI_VERSION;
        surface_target.user_data = 0;
        surface_target.ops = 0;
        surface_target.surface = desc->surface;
        target = &surface_target;
    }

    return plasma_v2_core_render(session->core, target);
}

static const ss_v2_session_ops g_plasma_v2_session_ops = {
    (ss_u32)sizeof(ss_v2_session_ops),
    SS_V2_ABI_VERSION,
    plasma_v2_create,
    plasma_v2_destroy,
    plasma_v2_resize,
    plasma_v2_advance,
    plasma_v2_render
};

static const ss_v2_product_descriptor g_plasma_v2_product = {
    (ss_u32)sizeof(ss_v2_product_descriptor),
    SS_V2_ABI_VERSION,
    "plasma",
    "Plasma",
    "v2-reference-preview",
    PLASMA_SPEC_V2_SCHEMA_ID,
    PLASMA_SPEC_V2_SCHEMA_VERSION,
    SS_V2_PRODUCT_CAP_REFERENCE_CPU |
        SS_V2_PRODUCT_CAP_RGBA8_SURFACE |
        SS_V2_PRODUCT_CAP_FIXED_STEP |
        SS_V2_PRODUCT_CAP_DETERMINISTIC_SEED,
    &g_plasma_v2_session_ops
};

const ss_v2_product_descriptor *plasma_v2_product_descriptor(void)
{
    return &g_plasma_v2_product;
}
