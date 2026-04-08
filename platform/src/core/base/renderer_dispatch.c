#include <string.h>

#include "screensave/private/backend_loader.h"
#include "screensave/private/renderer_runtime.h"

static void screensave_renderer_zero_info(screensave_renderer_info *info)
{
    memset(info, 0, sizeof(*info));
    info->requested_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
    info->active_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
}

void screensave_renderer_init_dispatch(
    screensave_renderer *renderer,
    const screensave_renderer_vtable *vtable,
    void *backend_state,
    const screensave_renderer_info *info
)
{
    if (renderer == NULL) {
        return;
    }

    renderer->vtable = vtable;
    renderer->backend_state = backend_state;
    if (info != NULL) {
        renderer->info = *info;
    } else {
        screensave_renderer_zero_info(&renderer->info);
    }
    screensave_renderer_reset_private_state(renderer);
}

void screensave_renderer_reset_private_state(screensave_renderer *renderer)
{
    if (renderer == NULL) {
        return;
    }

    renderer->backend_kind = SCREENSAVE_BACKEND_KIND_UNKNOWN;
    renderer->active_band = SCREENSAVE_RENDER_BAND_UNKNOWN;
    screensave_backend_caps_init(
        &renderer->backend_caps,
        SCREENSAVE_BACKEND_KIND_UNKNOWN,
        SCREENSAVE_RENDER_BAND_UNKNOWN
    );
    screensave_present_path_init(
        &renderer->present_path,
        SCREENSAVE_BACKEND_KIND_UNKNOWN,
        SCREENSAVE_RENDER_BAND_UNKNOWN,
        0UL,
        NULL
    );
    screensave_service_seams_init(&renderer->service_seams);
}

void screensave_renderer_set_backend_identity(
    screensave_renderer *renderer,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band
)
{
    if (renderer == NULL) {
        return;
    }

    renderer->backend_kind = backend_kind;
    renderer->active_band = active_band;
}

void screensave_renderer_set_backend_caps(
    screensave_renderer *renderer,
    const screensave_backend_caps *caps
)
{
    if (renderer == NULL || caps == NULL) {
        return;
    }

    renderer->backend_caps = *caps;
}

void screensave_renderer_set_present_path(
    screensave_renderer *renderer,
    const screensave_present_path *present_path
)
{
    if (renderer == NULL || present_path == NULL) {
        return;
    }

    renderer->present_path = *present_path;
}

void screensave_renderer_set_service_seams(
    screensave_renderer *renderer,
    const screensave_service_seams *service_seams
)
{
    if (renderer == NULL || service_seams == NULL) {
        return;
    }

    renderer->service_seams = *service_seams;
}

int screensave_renderer_get_private_service_seams(
    const screensave_renderer *renderer,
    screensave_service_seams *service_seams_out
)
{
    if (renderer == NULL || service_seams_out == NULL) {
        return 0;
    }

    *service_seams_out = renderer->service_seams;
    return 1;
}

const char *screensave_renderer_kind_name(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "gdi";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "gl11";

    case SCREENSAVE_RENDERER_KIND_GL21:
        return "gl21";

    case SCREENSAVE_RENDERER_KIND_GL33:
        return "gl33";

    case SCREENSAVE_RENDERER_KIND_GL46:
        return "gl46";

    case SCREENSAVE_RENDERER_KIND_NULL:
        return "null";

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "unknown";
    }
}

int screensave_renderer_has_capability(unsigned long capability_flags, unsigned long capability_flag)
{
    return (capability_flags & capability_flag) == capability_flag;
}

int screensave_renderer_create_for_window(
    screensave_renderer_kind requested_kind,
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out
)
{
    screensave_backend_request request;

    if (renderer_out == NULL) {
        return 0;
    }

    memset(&request, 0, sizeof(request));
    request.requested_kind = requested_kind;
    request.target_window = target_window;
    if (drawable_size != NULL) {
        request.drawable_size = *drawable_size;
    } else {
        request.drawable_size.width = 0;
        request.drawable_size.height = 0;
    }
    request.diagnostics = diagnostics;

    return screensave_backend_loader_select_and_create(
        &request,
        renderer_out,
        NULL
    );
}

int screensave_renderer_resize_for_window(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    return screensave_present_path_resize_renderer(renderer, drawable_size);
}

void screensave_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    screensave_present_path_set_present_dc(renderer, present_dc);
}

void screensave_renderer_clear_present_dc(screensave_renderer *renderer)
{
    screensave_present_path_clear_present_dc(renderer);
}

void screensave_renderer_get_info(const screensave_renderer *renderer, screensave_renderer_info *info)
{
    if (info == NULL) {
        return;
    }

    if (renderer == NULL) {
        screensave_renderer_zero_info(info);
        return;
    }

    *info = renderer->info;
}

int screensave_renderer_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->begin_frame == NULL) {
        return 0;
    }

    return renderer->vtable->begin_frame(renderer, frame_info);
}

void screensave_renderer_clear(screensave_renderer *renderer, screensave_color color)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->clear == NULL) {
        return;
    }

    renderer->vtable->clear(renderer, color);
}

void screensave_renderer_fill_rect(screensave_renderer *renderer, const screensave_recti *rect, screensave_color color)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->fill_rect == NULL) {
        return;
    }

    renderer->vtable->fill_rect(renderer, rect, color);
}

void screensave_renderer_draw_frame_rect(screensave_renderer *renderer, const screensave_recti *rect, screensave_color color)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->draw_frame_rect == NULL) {
        return;
    }

    renderer->vtable->draw_frame_rect(renderer, rect, color);
}

void screensave_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->draw_line == NULL) {
        return;
    }

    renderer->vtable->draw_line(renderer, start_point, end_point, color);
}

void screensave_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->draw_polyline == NULL) {
        return;
    }

    renderer->vtable->draw_polyline(renderer, points, point_count, color);
}

int screensave_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->blit_bitmap == NULL) {
        return 0;
    }

    return renderer->vtable->blit_bitmap(renderer, bitmap, destination_rect);
}

int screensave_renderer_end_frame(screensave_renderer *renderer)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->end_frame == NULL) {
        return 0;
    }

    return renderer->vtable->end_frame(renderer);
}

void screensave_renderer_shutdown(screensave_renderer *renderer)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->shutdown == NULL) {
        return;
    }

    renderer->vtable->shutdown(renderer);
}
