#include "ricochet_v2_adapter.h"

#include "ricochet_internal.h"

#include <stdlib.h>
#include <string.h>

typedef struct ricochet_v2_session_tag {
    ricochet_core_session *core;
    ss_u32 width;
    ss_u32 height;
    ss_u32 preview_mode;
} ricochet_v2_session;

typedef struct ricochet_v2_renderer_bridge_tag {
    screensave_renderer *renderer;
} ricochet_v2_renderer_bridge;

static ss_v2_color_rgba8 ricochet_v2_color(screensave_color color)
{
    ss_v2_color_rgba8 converted;

    converted.struct_size = (ss_u32)sizeof(converted);
    converted.abi_version = SS_V2_ABI_VERSION;
    converted.red = color.red;
    converted.green = color.green;
    converted.blue = color.blue;
    converted.alpha = color.alpha;
    return converted;
}

static screensave_color ricochet_v2_renderer_color(const ss_v2_color_rgba8 *color)
{
    screensave_color converted;

    converted.red = color->red;
    converted.green = color->green;
    converted.blue = color->blue;
    converted.alpha = color->alpha;
    return converted;
}

static ss_u32 ricochet_v2_renderer_clear(void *user_data, const ss_v2_color_rgba8 *color)
{
    ricochet_v2_renderer_bridge *bridge;

    bridge = (ricochet_v2_renderer_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    screensave_renderer_clear(bridge->renderer, ricochet_v2_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_renderer_fill_rect(
    void *user_data,
    const ss_v2_recti *rect,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v2_renderer_bridge *bridge;
    screensave_recti converted;

    bridge = (ricochet_v2_renderer_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || rect == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted.x = (int)rect->x;
    converted.y = (int)rect->y;
    converted.width = (int)rect->width;
    converted.height = (int)rect->height;
    screensave_renderer_fill_rect(bridge->renderer, &converted, ricochet_v2_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_renderer_frame_rect(
    void *user_data,
    const ss_v2_recti *rect,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v2_renderer_bridge *bridge;
    screensave_recti converted;

    bridge = (ricochet_v2_renderer_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || rect == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted.x = (int)rect->x;
    converted.y = (int)rect->y;
    converted.width = (int)rect->width;
    converted.height = (int)rect->height;
    screensave_renderer_draw_frame_rect(bridge->renderer, &converted, ricochet_v2_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_renderer_line(
    void *user_data,
    const ss_v2_pointi *start_point,
    const ss_v2_pointi *end_point,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v2_renderer_bridge *bridge;
    screensave_pointi converted_start;
    screensave_pointi converted_end;

    bridge = (ricochet_v2_renderer_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || start_point == NULL || end_point == NULL || color == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted_start.x = (int)start_point->x;
    converted_start.y = (int)start_point->y;
    converted_end.x = (int)end_point->x;
    converted_end.y = (int)end_point->y;
    screensave_renderer_draw_line(bridge->renderer, &converted_start, &converted_end, ricochet_v2_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_renderer_polyline(
    void *user_data,
    const ss_v2_pointi *points,
    ss_u32 point_count,
    const ss_v2_color_rgba8 *color)
{
    ricochet_v2_renderer_bridge *bridge;
    screensave_pointi converted[8];
    ss_u32 index;

    bridge = (ricochet_v2_renderer_bridge *)user_data;
    if (bridge == NULL || bridge->renderer == NULL || points == NULL || color == NULL || point_count > 8U) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    for (index = 0U; index < point_count; ++index) {
        converted[index].x = (int)points[index].x;
        converted[index].y = (int)points[index].y;
    }
    screensave_renderer_draw_polyline(bridge->renderer, converted, (unsigned int)point_count, ricochet_v2_renderer_color(color));
    return SS_V2_STATUS_OK;
}

static const ss_v2_draw_ops g_ricochet_v2_renderer_ops = {
    (ss_u32)sizeof(ss_v2_draw_ops),
    SS_V2_ABI_VERSION,
    SS_V2_DRAW_CAP_CLEAR |
        SS_V2_DRAW_CAP_FILL_RECT |
        SS_V2_DRAW_CAP_FRAME_RECT |
        SS_V2_DRAW_CAP_LINE |
        SS_V2_DRAW_CAP_POLYLINE,
    ricochet_v2_renderer_clear,
    ricochet_v2_renderer_fill_rect,
    ricochet_v2_renderer_frame_rect,
    ricochet_v2_renderer_line,
    ricochet_v2_renderer_polyline
};

static ss_u32 ricochet_v2_preview_mode(ss_u32 mode)
{
    return mode == SS_V2_SESSION_MODE_PREVIEW ? 1U : 0U;
}

static void ricochet_v2_default_config(ricochet_config *config)
{
    if (config == NULL) {
        return;
    }
    config->object_mode = RICOCHET_OBJECT_BLOCK;
    config->object_count = 1;
    config->speed_mode = RICOCHET_SPEED_STANDARD;
    config->trail_mode = RICOCHET_TRAIL_SHORT;
}

static void ricochet_v2_default_theme(ricochet_core_theme *theme)
{
    const screensave_theme_descriptor *descriptor;
    screensave_color fallback_primary;
    screensave_color fallback_accent;

    if (theme == NULL) {
        return;
    }
    descriptor = ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    if (descriptor != NULL) {
        theme->primary_color = ricochet_v2_color(descriptor->primary_color);
        theme->accent_color = ricochet_v2_color(descriptor->accent_color);
        return;
    }
    fallback_primary.red = 232;
    fallback_primary.green = 232;
    fallback_primary.blue = 228;
    fallback_primary.alpha = 255;
    fallback_accent.red = 80;
    fallback_accent.green = 180;
    fallback_accent.blue = 255;
    fallback_accent.alpha = 255;
    theme->primary_color = ricochet_v2_color(fallback_primary);
    theme->accent_color = ricochet_v2_color(fallback_accent);
}

static ss_u32 ricochet_v2_create(const ss_v2_session_desc *desc, ss_v2_session **session_out)
{
    ricochet_v2_session *session;
    ricochet_config product_config;
    ricochet_core_theme theme;
    ricochet_core_desc core_desc;
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

    ricochet_v2_default_config(&product_config);
    memcpy(&product_config, desc->product_config.bytes, sizeof(product_config));
    ricochet_v2_default_theme(&theme);

    core_desc.config = &product_config;
    core_desc.theme = &theme;
    core_desc.width = desc->dimensions.width;
    core_desc.height = desc->dimensions.height;
    core_desc.detail_level = RICOCHET_DETAIL_STANDARD;
    core_desc.preview_mode = ricochet_v2_preview_mode(desc->mode);
    core_desc.base_seed = desc->seed.base_seed;
    core_desc.stream_seed = desc->seed.stream_seed;
    if (!ricochet_core_create(&core_desc, &session->core)) {
        free(session);
        return SS_V2_STATUS_FAIL;
    }
    session->width = desc->dimensions.width;
    session->height = desc->dimensions.height;
    session->preview_mode = core_desc.preview_mode;

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
    ricochet_core_destroy(session->core);
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
    session->width = desc->dimensions.width;
    session->height = desc->dimensions.height;
    ricochet_core_resize(session->core, session->width, session->height, session->preview_mode);
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
    ricochet_core_advance(session->core, desc->clock.delta_ms);
    return SS_V2_STATUS_OK;
}

static ss_u32 ricochet_v2_render(ss_v2_session *opaque_session, const ss_v2_render_desc *desc)
{
    ricochet_v2_session *session;
    ricochet_v2_renderer_bridge renderer_bridge;
    ss_v2_draw_target renderer_target;
    ss_v2_draw_target *target;
    ss_u32 status;

    session = (ricochet_v2_session *)opaque_session;
    if (session == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_render_desc_is_valid(desc);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (desc->draw_target == NULL || desc->surface == NULL) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    target = desc->draw_target;
    if (target->ops == NULL && target->user_data != NULL) {
        renderer_bridge.renderer = (screensave_renderer *)target->user_data;
        renderer_target.struct_size = (ss_u32)sizeof(renderer_target);
        renderer_target.abi_version = SS_V2_ABI_VERSION;
        renderer_target.user_data = &renderer_bridge;
        renderer_target.ops = &g_ricochet_v2_renderer_ops;
        renderer_target.surface = desc->surface;
        target = &renderer_target;
    }
    if (!ricochet_core_render(session->core, target)) {
        return SS_V2_STATUS_FAIL;
    }
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
