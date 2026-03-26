#include "gdi_internal.h"

static void screensave_gdi_fill_rect_internal(HDC dc, const RECT *rect, screensave_color color)
{
    HBRUSH brush;

    brush = CreateSolidBrush(screensave_gdi_colorref(color));
    if (brush == NULL) {
        return;
    }

    FillRect(dc, rect, brush);
    DeleteObject(brush);
}

void screensave_gdi_clear_impl(screensave_renderer *renderer, screensave_color color)
{
    screensave_gdi_state *state;
    RECT rect;

    if (!screensave_gdi_state_from_renderer(renderer, &state) || state->surface.memory_dc == NULL) {
        return;
    }

    SetRect(&rect, 0, 0, state->surface.size.width, state->surface.size.height);
    screensave_gdi_fill_rect_internal(state->surface.memory_dc, &rect, color);
}

void screensave_gdi_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gdi_state *state;
    RECT fill_rect;

    if (!screensave_gdi_state_from_renderer(renderer, &state) || state->surface.memory_dc == NULL || rect == NULL) {
        return;
    }

    SetRect(&fill_rect, rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
    screensave_gdi_fill_rect_internal(state->surface.memory_dc, &fill_rect, color);
}

void screensave_gdi_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gdi_state *state;
    RECT frame_rect;
    HBRUSH brush;

    if (!screensave_gdi_state_from_renderer(renderer, &state) || state->surface.memory_dc == NULL || rect == NULL) {
        return;
    }

    SetRect(&frame_rect, rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
    brush = CreateSolidBrush(screensave_gdi_colorref(color));
    if (brush == NULL) {
        return;
    }

    FrameRect(state->surface.memory_dc, &frame_rect, brush);
    DeleteObject(brush);
}

void screensave_gdi_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    screensave_gdi_state *state;
    HPEN pen;
    HPEN old_pen;

    if (
        !screensave_gdi_state_from_renderer(renderer, &state) ||
        state->surface.memory_dc == NULL ||
        start_point == NULL ||
        end_point == NULL
    ) {
        return;
    }

    pen = CreatePen(PS_SOLID, 1, screensave_gdi_colorref(color));
    if (pen == NULL) {
        return;
    }

    old_pen = (HPEN)SelectObject(state->surface.memory_dc, pen);
    MoveToEx(state->surface.memory_dc, start_point->x, start_point->y, NULL);
    LineTo(state->surface.memory_dc, end_point->x, end_point->y);
    if (old_pen != NULL && old_pen != HGDI_ERROR) {
        SelectObject(state->surface.memory_dc, old_pen);
    }
    DeleteObject(pen);
}

void screensave_gdi_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    screensave_gdi_state *state;
    HPEN pen;
    HPEN old_pen;

    if (
        !screensave_gdi_state_from_renderer(renderer, &state) ||
        state->surface.memory_dc == NULL ||
        points == NULL ||
        point_count < 2U
    ) {
        return;
    }

    pen = CreatePen(PS_SOLID, 1, screensave_gdi_colorref(color));
    if (pen == NULL) {
        return;
    }

    old_pen = (HPEN)SelectObject(state->surface.memory_dc, pen);
    Polyline(state->surface.memory_dc, (const POINT *)points, (int)point_count);
    if (old_pen != NULL && old_pen != HGDI_ERROR) {
        SelectObject(state->surface.memory_dc, old_pen);
    }
    DeleteObject(pen);
}
