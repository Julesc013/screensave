#include "screensave/private/soft_renderer.h"

#include <stddef.h>

static int screensave_soft_min_int(int a, int b)
{
    return a < b ? a : b;
}

static int screensave_soft_max_int(int a, int b)
{
    return a > b ? a : b;
}

static int screensave_soft_abs_int(int value)
{
    return value < 0 ? -value : value;
}

void screensave_soft_fill_rect(
    screensave_rgba8_surface *surface,
    const screensave_recti *rect,
    screensave_color color
)
{
    int x;
    int y;
    int x0;
    int y0;
    int x1;
    int y1;

    if (surface == NULL || surface->pixels == NULL || rect == NULL || rect->width <= 0 || rect->height <= 0) {
        return;
    }

    x0 = screensave_soft_max_int(rect->x, 0);
    y0 = screensave_soft_max_int(rect->y, 0);
    x1 = screensave_soft_min_int(rect->x + rect->width, surface->width);
    y1 = screensave_soft_min_int(rect->y + rect->height, surface->height);

    for (y = y0; y < y1; ++y) {
        for (x = x0; x < x1; ++x) {
            screensave_rgba8_surface_set_pixel(surface, x, y, color);
        }
    }
}

void screensave_soft_draw_frame_rect(
    screensave_rgba8_surface *surface,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_recti edge;

    if (rect == NULL || rect->width <= 0 || rect->height <= 0) {
        return;
    }

    edge.x = rect->x;
    edge.y = rect->y;
    edge.width = rect->width;
    edge.height = 1;
    screensave_soft_fill_rect(surface, &edge, color);

    edge.y = rect->y + rect->height - 1;
    screensave_soft_fill_rect(surface, &edge, color);

    edge.x = rect->x;
    edge.y = rect->y;
    edge.width = 1;
    edge.height = rect->height;
    screensave_soft_fill_rect(surface, &edge, color);

    edge.x = rect->x + rect->width - 1;
    screensave_soft_fill_rect(surface, &edge, color);
}

void screensave_soft_draw_line(
    screensave_rgba8_surface *surface,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    int x0;
    int y0;
    int x1;
    int y1;
    int dx;
    int dy;
    int sx;
    int sy;
    int err;
    int e2;

    if (surface == NULL || start_point == NULL || end_point == NULL) {
        return;
    }

    x0 = start_point->x;
    y0 = start_point->y;
    x1 = end_point->x;
    y1 = end_point->y;
    dx = screensave_soft_abs_int(x1 - x0);
    dy = -screensave_soft_abs_int(y1 - y0);
    sx = x0 < x1 ? 1 : -1;
    sy = y0 < y1 ? 1 : -1;
    err = dx + dy;

    for (;;) {
        screensave_rgba8_surface_set_pixel(surface, x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = err * 2;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void screensave_soft_draw_polyline(
    screensave_rgba8_surface *surface,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    unsigned int index;

    if (surface == NULL || points == NULL || point_count < 2U) {
        return;
    }

    for (index = 1U; index < point_count; ++index) {
        screensave_soft_draw_line(surface, &points[index - 1U], &points[index], color);
    }
}
