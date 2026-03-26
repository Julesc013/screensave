#include <string.h>

#include "renderer_private.h"
#include "../../render/gdi/gdi_internal.h"
#include "../../render/gl11/gl11_internal.h"

static void screensave_renderer_emit_create_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_RENDERER,
        code,
        "renderer_dispatch",
        text
    );
}

static void screensave_renderer_zero_info(screensave_renderer_info *info)
{
    memset(info, 0, sizeof(*info));
    info->requested_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
    info->active_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
}

static void screensave_renderer_apply_selection_info(
    screensave_renderer *renderer,
    screensave_renderer_kind requested_kind,
    const char *selection_reason,
    const char *fallback_reason,
    const char *status_text
)
{
    if (renderer == NULL) {
        return;
    }

    renderer->info.requested_kind = requested_kind;
    renderer->info.selection_reason = selection_reason;
    renderer->info.fallback_reason = fallback_reason;
    if (status_text != NULL) {
        renderer->info.status_text = status_text;
    }
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
}

const char *screensave_renderer_kind_name(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "gdi";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "gl11";

    case SCREENSAVE_RENDERER_KIND_GL_PLUS:
        return "gl_plus";

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
    const char *failure_reason;

    if (renderer_out == NULL) {
        return 0;
    }

    *renderer_out = NULL;
    failure_reason = NULL;

    if (requested_kind == SCREENSAVE_RENDERER_KIND_GDI) {
        if (!screensave_gdi_renderer_create(target_window, drawable_size, diagnostics, renderer_out)) {
            return 0;
        }
        screensave_renderer_apply_selection_info(
            *renderer_out,
            SCREENSAVE_RENDERER_KIND_GDI,
            "force-gdi",
            NULL,
            NULL
        );
        return 1;
    }

    if (
        requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN ||
        requested_kind == SCREENSAVE_RENDERER_KIND_GL11
    ) {
        if (screensave_gl11_renderer_create(target_window, drawable_size, diagnostics, renderer_out, &failure_reason)) {
            screensave_renderer_apply_selection_info(
                *renderer_out,
                requested_kind,
                requested_kind == SCREENSAVE_RENDERER_KIND_GL11 ? "force-gl11" : "auto-prefer-gl11",
                NULL,
                NULL
            );
            return 1;
        }

        screensave_renderer_emit_create_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            4101UL,
            requested_kind == SCREENSAVE_RENDERER_KIND_GL11
                ? "The requested GL11 renderer could not be initialized; falling back to GDI."
                : "GL11 could not be initialized during automatic selection; falling back to GDI."
        );
    } else if (requested_kind == SCREENSAVE_RENDERER_KIND_GL_PLUS) {
        failure_reason = "gl-plus-not-implemented";
        screensave_renderer_emit_create_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            4102UL,
            "The requested GL-plus renderer is not implemented yet; falling back to GDI."
        );
    }

    if (!screensave_gdi_renderer_create(target_window, drawable_size, diagnostics, renderer_out)) {
        return 0;
    }

    screensave_renderer_apply_selection_info(
        *renderer_out,
        requested_kind,
        requested_kind == SCREENSAVE_RENDERER_KIND_GL11
            ? "force-gl11-fallback-gdi"
            : (requested_kind == SCREENSAVE_RENDERER_KIND_GL_PLUS
                ? "force-gl-plus-fallback-gdi"
                : "auto-fallback-gdi"),
        failure_reason,
        "fallback-gdi"
    );
    return 1;
}

int screensave_renderer_resize_for_window(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    if (renderer == NULL) {
        return 0;
    }

    switch (renderer->info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return screensave_gdi_renderer_resize(renderer, drawable_size);

    case SCREENSAVE_RENDERER_KIND_GL11:
        return screensave_gl11_renderer_resize(renderer, drawable_size);

    case SCREENSAVE_RENDERER_KIND_GL_PLUS:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return 0;
    }
}

void screensave_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    if (renderer == NULL) {
        return;
    }

    switch (renderer->info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        screensave_gdi_renderer_set_present_dc(renderer, present_dc);
        break;

    case SCREENSAVE_RENDERER_KIND_GL11:
        screensave_gl11_renderer_set_present_dc(renderer, present_dc);
        break;

    case SCREENSAVE_RENDERER_KIND_GL_PLUS:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        break;
    }
}

void screensave_renderer_clear_present_dc(screensave_renderer *renderer)
{
    if (renderer == NULL) {
        return;
    }

    switch (renderer->info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        screensave_gdi_renderer_clear_present_dc(renderer);
        break;

    case SCREENSAVE_RENDERER_KIND_GL11:
        screensave_gl11_renderer_clear_present_dc(renderer);
        break;

    case SCREENSAVE_RENDERER_KIND_GL_PLUS:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        break;
    }
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
