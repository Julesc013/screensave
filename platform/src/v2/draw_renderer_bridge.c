#include "screensave/private/v2_draw_renderer_bridge.h"

#include <limits.h>

#include "screensave/v2/internal/validate.h"

#define SS_V2_RENDERER_BRIDGE_CAPS \
    (SS_V2_DRAW_CAP_CLEAR | SS_V2_DRAW_CAP_FILL_RECT | SS_V2_DRAW_CAP_FRAME_RECT | SS_V2_DRAW_CAP_LINE | SS_V2_DRAW_CAP_POLYLINE)
#define SS_V2_RENDERER_BRIDGE_MAX_POLYLINE_POINTS 64U

static screensave_color ss_v2_bridge_color(const ss_v2_color_rgba8 *color)
{
    screensave_color converted;

    converted.red = color->red;
    converted.green = color->green;
    converted.blue = color->blue;
    converted.alpha = color->alpha;
    return converted;
}

static ss_u32 ss_v2_bridge_rect(const ss_v2_recti *rect, screensave_recti *converted)
{
    if (rect == 0 || converted == 0 || rect->width > (ss_u32)INT_MAX || rect->height > (ss_u32)INT_MAX) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted->x = (int)rect->x;
    converted->y = (int)rect->y;
    converted->width = (int)rect->width;
    converted->height = (int)rect->height;
    return SS_V2_STATUS_OK;
}

static ss_u32 ss_v2_bridge_point(const ss_v2_pointi *point, screensave_pointi *converted)
{
    if (point == 0 || converted == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    converted->x = (int)point->x;
    converted->y = (int)point->y;
    return SS_V2_STATUS_OK;
}

static ss_u32 ss_v2_bridge_clear(void *user_data, const ss_v2_color_rgba8 *color)
{
    ss_v2_renderer_draw_bridge *bridge;

    bridge = (ss_v2_renderer_draw_bridge *)user_data;
    if (ss_v2_renderer_draw_bridge_is_valid(bridge) != SS_V2_STATUS_OK || color == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    screensave_renderer_clear(bridge->renderer, ss_v2_bridge_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ss_v2_bridge_fill_rect(void *user_data, const ss_v2_recti *rect, const ss_v2_color_rgba8 *color)
{
    ss_v2_renderer_draw_bridge *bridge;
    screensave_recti converted_rect;
    ss_u32 status;

    bridge = (ss_v2_renderer_draw_bridge *)user_data;
    if (ss_v2_renderer_draw_bridge_is_valid(bridge) != SS_V2_STATUS_OK || color == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_bridge_rect(rect, &converted_rect);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    screensave_renderer_fill_rect(bridge->renderer, &converted_rect, ss_v2_bridge_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ss_v2_bridge_frame_rect(void *user_data, const ss_v2_recti *rect, const ss_v2_color_rgba8 *color)
{
    ss_v2_renderer_draw_bridge *bridge;
    screensave_recti converted_rect;
    ss_u32 status;

    bridge = (ss_v2_renderer_draw_bridge *)user_data;
    if (ss_v2_renderer_draw_bridge_is_valid(bridge) != SS_V2_STATUS_OK || color == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_bridge_rect(rect, &converted_rect);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    screensave_renderer_draw_frame_rect(bridge->renderer, &converted_rect, ss_v2_bridge_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ss_v2_bridge_line(
    void *user_data,
    const ss_v2_pointi *start_point,
    const ss_v2_pointi *end_point,
    const ss_v2_color_rgba8 *color
)
{
    ss_v2_renderer_draw_bridge *bridge;
    screensave_pointi converted_start;
    screensave_pointi converted_end;
    ss_u32 status;

    bridge = (ss_v2_renderer_draw_bridge *)user_data;
    if (ss_v2_renderer_draw_bridge_is_valid(bridge) != SS_V2_STATUS_OK || color == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_bridge_point(start_point, &converted_start);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = ss_v2_bridge_point(end_point, &converted_end);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    screensave_renderer_draw_line(bridge->renderer, &converted_start, &converted_end, ss_v2_bridge_color(color));
    return SS_V2_STATUS_OK;
}

static ss_u32 ss_v2_bridge_polyline(
    void *user_data,
    const ss_v2_pointi *points,
    ss_u32 point_count,
    const ss_v2_color_rgba8 *color
)
{
    ss_v2_renderer_draw_bridge *bridge;
    screensave_pointi converted[SS_V2_RENDERER_BRIDGE_MAX_POLYLINE_POINTS];
    ss_u32 index;
    ss_u32 status;

    bridge = (ss_v2_renderer_draw_bridge *)user_data;
    if (ss_v2_renderer_draw_bridge_is_valid(bridge) != SS_V2_STATUS_OK || points == 0 || color == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (point_count > SS_V2_RENDERER_BRIDGE_MAX_POLYLINE_POINTS) {
        return SS_V2_STATUS_UNSUPPORTED;
    }
    for (index = 0U; index < point_count; ++index) {
        status = ss_v2_bridge_point(&points[index], &converted[index]);
        if (status != SS_V2_STATUS_OK) {
            return status;
        }
    }
    screensave_renderer_draw_polyline(bridge->renderer, converted, (unsigned int)point_count, ss_v2_bridge_color(color));
    return SS_V2_STATUS_OK;
}

static const ss_v2_draw_ops g_ss_v2_renderer_draw_ops = {
    (ss_u32)sizeof(ss_v2_draw_ops),
    SS_V2_ABI_VERSION,
    SS_V2_RENDERER_BRIDGE_CAPS,
    ss_v2_bridge_clear,
    ss_v2_bridge_fill_rect,
    ss_v2_bridge_frame_rect,
    ss_v2_bridge_line,
    ss_v2_bridge_polyline
};

void ss_v2_renderer_draw_bridge_init(
    ss_v2_renderer_draw_bridge *bridge,
    screensave_renderer *renderer
)
{
    if (bridge == 0) {
        return;
    }
    bridge->struct_size = (ss_u32)sizeof(*bridge);
    bridge->abi_version = SS_V2_ABI_VERSION;
    bridge->renderer = renderer;
}

ss_u32 ss_v2_renderer_draw_bridge_is_valid(const ss_v2_renderer_draw_bridge *bridge)
{
    ss_u32 status;

    status = ss_v2_check_prefix(bridge, (ss_u32)sizeof(*bridge), (ss_u32)sizeof(*bridge), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (bridge->renderer == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return SS_V2_STATUS_OK;
}

ss_u32 ss_v2_renderer_draw_bridge_make_target(
    ss_v2_renderer_draw_bridge *bridge,
    ss_v2_draw_target *target
)
{
    ss_u32 status;

    status = ss_v2_renderer_draw_bridge_is_valid(bridge);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (target == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    target->struct_size = (ss_u32)sizeof(*target);
    target->abi_version = SS_V2_ABI_VERSION;
    target->user_data = bridge;
    target->ops = &g_ss_v2_renderer_draw_ops;
    target->surface = 0;
    return SS_V2_STATUS_OK;
}
