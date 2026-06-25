#include "renderer_rgba8.h"

#include "screensave/private/soft_renderer.h"

#include <stddef.h>

typedef struct screensave_frame_info_tag screensave_frame_info;
typedef struct screensave_bitmap_view_tag screensave_bitmap_view;

void sslab_rgba8_renderer_init(screensave_renderer *renderer, screensave_rgba8_surface *surface)
{
    if (renderer == NULL) {
        return;
    }

    renderer->surface = surface;
}

int screensave_renderer_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    (void)renderer;
    (void)frame_info;
    return 1;
}

void screensave_renderer_clear(screensave_renderer *renderer, screensave_color color)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_rgba8_surface_clear(renderer->surface, color);
}

void screensave_renderer_fill_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_fill_rect(renderer->surface, rect, color);
}

void screensave_renderer_draw_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_draw_frame_rect(renderer->surface, rect, color);
}

void screensave_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_draw_line(renderer->surface, start_point, end_point, color);
}

void screensave_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    if (renderer == NULL || renderer->surface == NULL) {
        return;
    }

    screensave_soft_draw_polyline(renderer->surface, points, point_count, color);
}

int screensave_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    (void)renderer;
    (void)bitmap;
    (void)destination_rect;
    return 0;
}

int screensave_renderer_end_frame(screensave_renderer *renderer)
{
    (void)renderer;
    return 1;
}

void screensave_renderer_shutdown(screensave_renderer *renderer)
{
    (void)renderer;
}
