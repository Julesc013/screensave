#define SSLAB_RGBA8_USE_PLATFORM_RENDERER
#include "renderer_rgba8.h"

#include "screensave/private/soft_renderer.h"

#include <stddef.h>
#include <string.h>

static screensave_rgba8_surface *sslab_rgba8_surface_from_renderer(screensave_renderer *renderer)
{
    if (renderer == NULL) {
        return NULL;
    }
    return (screensave_rgba8_surface *)renderer->backend_state;
}

static int sslab_rgba8_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    (void)frame_info;
    return sslab_rgba8_surface_from_renderer(renderer) != NULL;
}

static void sslab_rgba8_clear(screensave_renderer *renderer, screensave_color color)
{
    screensave_rgba8_surface *surface;

    surface = sslab_rgba8_surface_from_renderer(renderer);
    if (surface == NULL) {
        return;
    }
    screensave_rgba8_surface_clear(surface, color);
}

static void sslab_rgba8_fill_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color)
{
    screensave_rgba8_surface *surface;

    surface = sslab_rgba8_surface_from_renderer(renderer);
    if (surface == NULL) {
        return;
    }
    screensave_soft_fill_rect(surface, rect, color);
}

static void sslab_rgba8_draw_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color)
{
    screensave_rgba8_surface *surface;

    surface = sslab_rgba8_surface_from_renderer(renderer);
    if (surface == NULL) {
        return;
    }
    screensave_soft_draw_frame_rect(surface, rect, color);
}

static void sslab_rgba8_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color)
{
    screensave_rgba8_surface *surface;

    surface = sslab_rgba8_surface_from_renderer(renderer);
    if (surface == NULL) {
        return;
    }
    screensave_soft_draw_line(surface, start_point, end_point, color);
}

static void sslab_rgba8_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color)
{
    screensave_rgba8_surface *surface;

    surface = sslab_rgba8_surface_from_renderer(renderer);
    if (surface == NULL) {
        return;
    }
    screensave_soft_draw_polyline(surface, points, point_count, color);
}

static int sslab_rgba8_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect)
{
    (void)renderer;
    (void)bitmap;
    (void)destination_rect;
    return 0;
}

static int sslab_rgba8_end_frame(screensave_renderer *renderer)
{
    return sslab_rgba8_surface_from_renderer(renderer) != NULL;
}

static void sslab_rgba8_shutdown(screensave_renderer *renderer)
{
    (void)renderer;
}

static const screensave_renderer_vtable g_sslab_rgba8_vtable = {
    sslab_rgba8_begin_frame,
    sslab_rgba8_clear,
    sslab_rgba8_fill_rect,
    sslab_rgba8_draw_frame_rect,
    sslab_rgba8_draw_line,
    sslab_rgba8_draw_polyline,
    sslab_rgba8_blit_bitmap,
    sslab_rgba8_end_frame,
    sslab_rgba8_shutdown
};

void sslab_rgba8_renderer_init(screensave_renderer *renderer, screensave_rgba8_surface *surface)
{
    screensave_renderer_info info;

    if (renderer == NULL) {
        return;
    }
    if (surface == NULL) {
        screensave_renderer_init_dispatch(renderer, NULL, NULL, NULL);
        return;
    }

    memset(&info, 0, sizeof(info));
    info.requested_kind = SCREENSAVE_RENDERER_KIND_NULL;
    info.active_kind = SCREENSAVE_RENDERER_KIND_NULL;
    info.capability_flags =
        SCREENSAVE_RENDERER_CAP_CLEAR |
        SCREENSAVE_RENDERER_CAP_FILL_RECT |
        SCREENSAVE_RENDERER_CAP_FRAME_RECT |
        SCREENSAVE_RENDERER_CAP_LINE |
        SCREENSAVE_RENDERER_CAP_POLYLINE;
    info.drawable_size.width = surface->width;
    info.drawable_size.height = surface->height;
    info.backend_name = "sslab-rgba8";
    info.status_text = "offscreen";
    info.selection_reason = "portable proof surface";

    screensave_renderer_init_dispatch(renderer, &g_sslab_rgba8_vtable, surface, &info);
    screensave_renderer_set_backend_identity(
        renderer,
        SCREENSAVE_BACKEND_KIND_NULL,
        SCREENSAVE_RENDER_BAND_UNIVERSAL);
}
