#include <stdlib.h>

#include "gl11_internal.h"

static void screensave_gl11_prepare_frame_state(screensave_gl11_state *state)
{
    if (state == NULL) {
        return;
    }

    glViewport(0, 0, state->drawable_size.width, state->drawable_size.height);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        0.0,
        (GLdouble)state->drawable_size.width,
        (GLdouble)state->drawable_size.height,
        0.0,
        -1.0,
        1.0
    );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static int screensave_gl11_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    screensave_gl11_state *state;

    if (!screensave_gl11_state_from_renderer(renderer, &state) || frame_info == NULL) {
        return 0;
    }

    if (!screensave_gl11_context_make_current(state, "gl11_backend", 6501UL)) {
        screensave_gl11_update_renderer_info(renderer, NULL, "make-current-failed");
        return 0;
    }

    if (frame_info->drawable_size.width > 0 && frame_info->drawable_size.height > 0) {
        state->drawable_size = frame_info->drawable_size;
    }

    screensave_gl11_prepare_frame_state(state);
    state->frame_open = 1;
    screensave_gl11_update_renderer_info(
        renderer,
        &state->drawable_size,
        state->caps.double_buffered ? "frame-open-double-buffered" : "frame-open-single-buffered"
    );
    return 1;
}

static int screensave_gl11_end_frame(screensave_renderer *renderer)
{
    screensave_gl11_state *state;
    int result;

    if (!screensave_gl11_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!state->frame_open) {
        return 0;
    }

    state->frame_open = 0;
    result = screensave_gl11_present(renderer);
    screensave_gl11_context_release_current(state);
    return result;
}

static void screensave_gl11_shutdown_impl(screensave_renderer *renderer)
{
    screensave_gl11_state *state;

    if (renderer == NULL) {
        return;
    }

    state = (screensave_gl11_state *)renderer->backend_state;
    if (state != NULL) {
        screensave_gl11_context_destroy(state);
        free(state);
    }

    free(renderer);
}

static const screensave_renderer_vtable g_screensave_gl11_vtable = {
    screensave_gl11_begin_frame,
    screensave_gl11_clear_impl,
    screensave_gl11_fill_rect_impl,
    screensave_gl11_draw_frame_rect_impl,
    screensave_gl11_draw_line_impl,
    screensave_gl11_draw_polyline_impl,
    screensave_gl11_blit_bitmap_impl,
    screensave_gl11_end_frame,
    screensave_gl11_shutdown_impl
};

int screensave_gl11_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    screensave_renderer *renderer;
    screensave_gl11_state *state;
    screensave_renderer_info info;

    if (renderer_out == NULL) {
        return 0;
    }

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    *renderer_out = NULL;
    renderer = (screensave_renderer *)malloc(sizeof(*renderer));
    state = (screensave_gl11_state *)malloc(sizeof(*state));
    if (renderer == NULL || state == NULL) {
        if (renderer != NULL) {
            free(renderer);
        }
        if (state != NULL) {
            free(state);
        }
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-out-of-memory";
        }
        return 0;
    }

    ZeroMemory(state, sizeof(*state));
    state->target_window = target_window;
    state->diagnostics = diagnostics;
    if (!screensave_gl11_context_create(state, drawable_size, failure_reason_out)) {
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_gl11_context_make_current(state, "gl11_backend", 6502UL)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-make-current-failed";
        }
        screensave_gl11_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_gl11_capture_caps(state, failure_reason_out)) {
        screensave_gl11_context_release_current(state);
        screensave_gl11_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    screensave_gl11_context_release_current(state);
    screensave_gl11_capture_refresh(state);

    ZeroMemory(&info, sizeof(info));
    info.requested_kind = SCREENSAVE_RENDERER_KIND_GL11;
    info.active_kind = SCREENSAVE_RENDERER_KIND_GL11;
    info.capability_flags = SCREENSAVE_GL11_CAPABILITIES;
    info.drawable_size = state->drawable_size;
    info.backend_name = "gl11";
    info.status_text = state->caps.double_buffered ? "created-double-buffered" : "created-single-buffered";
    info.vendor_name = state->caps.vendor;
    info.renderer_name = state->caps.renderer;
    info.version_name = state->detail_text[0] != '\0' ? state->detail_text : state->caps.version;

    screensave_renderer_init_dispatch(renderer, &g_screensave_gl11_vtable, state, &info);
    *renderer_out = renderer;

    screensave_gl11_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        6503UL,
        "gl11_backend",
        state->caps.double_buffered
            ? "The optional GL11 renderer backend is active."
            : "The optional GL11 renderer backend is active without double buffering."
    );
    return 1;
}

int screensave_gl11_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    screensave_gl11_state *state;

    if (!screensave_gl11_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (drawable_size == NULL || drawable_size->width <= 0 || drawable_size->height <= 0) {
        return 0;
    }

    state->drawable_size = *drawable_size;
    screensave_gl11_update_renderer_info(
        renderer,
        drawable_size,
        state->caps.double_buffered ? "resized-double-buffered" : "resized-single-buffered"
    );
    return 1;
}
