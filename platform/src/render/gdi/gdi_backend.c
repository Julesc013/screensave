#include <stdlib.h>

#include "gdi_internal.h"

static int screensave_gdi_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    screensave_gdi_state *state;

    if (!screensave_gdi_state_from_renderer(renderer, &state) || frame_info == NULL) {
        return 0;
    }

    if (!screensave_gdi_surface_reset(state, &frame_info->drawable_size)) {
        screensave_gdi_update_renderer_info(renderer, NULL, "surface-reset-failed");
        return 0;
    }

    state->frame_open = 1;
    screensave_gdi_update_renderer_info(renderer, &state->surface.size, "frame-open");
    return 1;
}

static int screensave_gdi_end_frame(screensave_renderer *renderer)
{
    screensave_gdi_state *state;

    if (!screensave_gdi_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!state->frame_open) {
        return 0;
    }

    state->frame_open = 0;
    return screensave_gdi_present(renderer);
}

static void screensave_gdi_shutdown_impl(screensave_renderer *renderer)
{
    screensave_gdi_state *state;

    if (renderer == NULL) {
        return;
    }

    state = (screensave_gdi_state *)renderer->backend_state;
    if (state != NULL) {
        screensave_gdi_surface_release(&state->surface);
        free(state);
    }

    free(renderer);
}

static const screensave_renderer_vtable g_screensave_gdi_vtable = {
    screensave_gdi_begin_frame,
    screensave_gdi_clear_impl,
    screensave_gdi_fill_rect_impl,
    screensave_gdi_draw_frame_rect_impl,
    screensave_gdi_draw_line_impl,
    screensave_gdi_draw_polyline_impl,
    screensave_gdi_blit_bitmap_impl,
    screensave_gdi_end_frame,
    screensave_gdi_shutdown_impl
};

int screensave_gdi_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out
)
{
    screensave_renderer *renderer;
    screensave_gdi_state *state;
    screensave_renderer_info info;

    if (renderer_out == NULL || target_window == NULL) {
        return 0;
    }

    *renderer_out = NULL;
    renderer = (screensave_renderer *)malloc(sizeof(*renderer));
    state = (screensave_gdi_state *)malloc(sizeof(*state));
    if (renderer == NULL || state == NULL) {
        if (renderer != NULL) {
            free(renderer);
        }
        if (state != NULL) {
            free(state);
        }
        return 0;
    }

    ZeroMemory(state, sizeof(*state));
    state->target_window = target_window;
    state->diagnostics = diagnostics;

    if (!screensave_gdi_surface_reset(state, drawable_size)) {
        free(state);
        free(renderer);
        return 0;
    }

    ZeroMemory(&info, sizeof(info));
    info.requested_kind = SCREENSAVE_RENDERER_KIND_GDI;
    info.active_kind = SCREENSAVE_RENDERER_KIND_GDI;
    info.capability_flags = SCREENSAVE_GDI_CAPABILITIES;
    info.drawable_size = state->surface.size;
    info.backend_name = "gdi";
    info.status_text = "created";

    screensave_renderer_init_dispatch(renderer, &g_screensave_gdi_vtable, state, &info);
    *renderer_out = renderer;

    screensave_gdi_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        5001UL,
        "gdi_backend",
        "The mandatory GDI renderer backend is active."
    );
    return 1;
}

int screensave_gdi_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    screensave_gdi_state *state;

    if (!screensave_gdi_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!screensave_gdi_surface_reset(state, drawable_size)) {
        screensave_gdi_update_renderer_info(renderer, NULL, "resize-failed");
        return 0;
    }

    screensave_gdi_update_renderer_info(renderer, &state->surface.size, "resized");
    return 1;
}
