#include <stdlib.h>
#include <string.h>

#include "null_internal.h"
#include "../../core/base/renderer_private.h"

typedef struct screensave_null_state_tag {
    screensave_sizei drawable_size;
    HDC present_dc;
} screensave_null_state;

static void screensave_null_normalize_size(const screensave_sizei *source, screensave_sizei *target)
{
    target->width = 1;
    target->height = 1;

    if (source != NULL) {
        if (source->width > 0) {
            target->width = source->width;
        }
        if (source->height > 0) {
            target->height = source->height;
        }
    }
}

static int screensave_null_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    screensave_null_state *state;

    if (renderer == NULL || frame_info == NULL) {
        return 0;
    }

    state = (screensave_null_state *)renderer->backend_state;
    if (state == NULL) {
        return 0;
    }

    screensave_null_normalize_size(&frame_info->drawable_size, &state->drawable_size);
    renderer->info.drawable_size = state->drawable_size;
    renderer->info.status_text = "render-suppressed";
    return 1;
}

static void screensave_null_noop_color(screensave_renderer *renderer, screensave_color color)
{
    (void)renderer;
    (void)color;
}

static void screensave_null_noop_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    (void)renderer;
    (void)rect;
    (void)color;
}

static void screensave_null_noop_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    (void)renderer;
    (void)rect;
    (void)color;
}

static void screensave_null_noop_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    (void)renderer;
    (void)start_point;
    (void)end_point;
    (void)color;
}

static void screensave_null_noop_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    (void)renderer;
    (void)points;
    (void)point_count;
    (void)color;
}

static int screensave_null_noop_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    (void)renderer;
    (void)bitmap;
    (void)destination_rect;
    return 1;
}

static int screensave_null_end_frame(screensave_renderer *renderer)
{
    if (renderer != NULL) {
        renderer->info.status_text = "render-suppressed";
    }
    return 1;
}

static void screensave_null_shutdown(screensave_renderer *renderer)
{
    if (renderer != NULL) {
        if (renderer->backend_state != NULL) {
            free(renderer->backend_state);
        }
        free(renderer);
    }
}

static const screensave_renderer_vtable g_screensave_null_vtable = {
    screensave_null_begin_frame,
    screensave_null_noop_color,
    screensave_null_noop_rect,
    screensave_null_noop_frame_rect,
    screensave_null_noop_line,
    screensave_null_noop_polyline,
    screensave_null_noop_bitmap,
    screensave_null_end_frame,
    screensave_null_shutdown
};

int screensave_null_renderer_create(
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    screensave_renderer *renderer;
    screensave_null_state *state;
    screensave_renderer_info info;

    if (renderer_out == NULL) {
        return 0;
    }

    *renderer_out = NULL;
    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    renderer = (screensave_renderer *)malloc(sizeof(*renderer));
    state = (screensave_null_state *)malloc(sizeof(*state));
    if (renderer == NULL || state == NULL) {
        if (renderer != NULL) {
            free(renderer);
        }
        if (state != NULL) {
            free(state);
        }
        if (failure_reason_out != NULL) {
            *failure_reason_out = "null-out-of-memory";
        }
        return 0;
    }

    ZeroMemory(state, sizeof(*state));
    screensave_null_normalize_size(drawable_size, &state->drawable_size);

    ZeroMemory(&info, sizeof(info));
    info.active_kind = SCREENSAVE_RENDERER_KIND_NULL;
    info.capability_flags = 0UL;
    info.drawable_size = state->drawable_size;
    info.backend_name = "null";
    info.status_text = "render-suppressed";

    screensave_renderer_init_dispatch(renderer, &g_screensave_null_vtable, state, &info);
    *renderer_out = renderer;

    if (diagnostics != NULL) {
        screensave_diag_emit(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            SCREENSAVE_DIAG_DOMAIN_RENDERER,
            6903UL,
            "null_backend",
            "The renderer runtime activated the null safety backend because no drawable renderer could be created."
        );
    }

    return 1;
}

int screensave_null_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    screensave_null_state *state;

    if (renderer == NULL || drawable_size == NULL) {
        return 0;
    }

    state = (screensave_null_state *)renderer->backend_state;
    if (state == NULL) {
        return 0;
    }

    screensave_null_normalize_size(drawable_size, &state->drawable_size);
    renderer->info.drawable_size = state->drawable_size;
    return 1;
}

void screensave_null_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    screensave_null_state *state;

    if (renderer == NULL) {
        return;
    }

    state = (screensave_null_state *)renderer->backend_state;
    if (state != NULL) {
        state->present_dc = present_dc;
    }
}

void screensave_null_renderer_clear_present_dc(screensave_renderer *renderer)
{
    screensave_null_renderer_set_present_dc(renderer, NULL);
}
