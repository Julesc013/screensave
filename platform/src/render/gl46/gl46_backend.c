#include <stdlib.h>

#include "gl46_internal.h"

static void screensave_gl46_prepare_frame_state(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    glViewport(0, 0, state->drawable_size.width, state->drawable_size.height);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static int screensave_gl46_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    screensave_gl46_state *state;

    if (!screensave_gl46_state_from_renderer(renderer, &state) || frame_info == NULL) {
        return 0;
    }

    if (!screensave_gl46_context_make_current(state, "gl46_backend", 7061UL)) {
        screensave_gl46_update_renderer_info(renderer, NULL, "make-current-failed");
        return 0;
    }

    if (frame_info->drawable_size.width > 0 && frame_info->drawable_size.height > 0) {
        state->drawable_size = frame_info->drawable_size;
    }

    screensave_gl46_prepare_frame_state(state);
    state->frame_open = 1;
    screensave_gl46_update_renderer_info(
        renderer,
        &state->drawable_size,
        state->caps.double_buffered
            ? "frame-open-double-buffered"
            : "frame-open-single-buffered"
    );
    return 1;
}

static int screensave_gl46_end_frame(screensave_renderer *renderer)
{
    screensave_gl46_state *state;
    int result;

    if (!screensave_gl46_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!state->frame_open) {
        return 0;
    }

    state->frame_open = 0;
    result = screensave_gl46_present(renderer);
    screensave_gl46_context_release_current(state);
    return result;
}

static void screensave_gl46_shutdown_impl(screensave_renderer *renderer)
{
    screensave_gl46_state *state;

    if (renderer == NULL) {
        return;
    }

    state = (screensave_gl46_state *)renderer->backend_state;
    if (state != NULL) {
        screensave_gl46_context_destroy(state);
        free(state);
    }

    free(renderer);
}

static const screensave_renderer_vtable g_screensave_gl46_vtable = {
    screensave_gl46_begin_frame,
    screensave_gl46_clear_impl,
    screensave_gl46_fill_rect_impl,
    screensave_gl46_draw_frame_rect_impl,
    screensave_gl46_draw_line_impl,
    screensave_gl46_draw_polyline_impl,
    screensave_gl46_blit_bitmap_impl,
    screensave_gl46_end_frame,
    screensave_gl46_shutdown_impl
};

int screensave_gl46_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    screensave_renderer *renderer;
    screensave_gl46_state *state;
    screensave_renderer_info info;

    if (renderer_out == NULL) {
        return 0;
    }

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    *renderer_out = NULL;
    renderer = (screensave_renderer *)malloc(sizeof(*renderer));
    state = (screensave_gl46_state *)malloc(sizeof(*state));
    if (renderer == NULL || state == NULL) {
        if (renderer != NULL) {
            free(renderer);
        }
        if (state != NULL) {
            free(state);
        }
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-out-of-memory";
        }
        return 0;
    }

    ZeroMemory(state, sizeof(*state));
    state->target_window = target_window;
    state->diagnostics = diagnostics;
    if (!screensave_gl46_context_create(state, drawable_size, failure_reason_out)) {
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_gl46_context_make_current(state, "gl46_backend", 7062UL)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-make-current-failed";
        }
        screensave_gl46_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_gl46_capture_caps(state, failure_reason_out)) {
        screensave_gl46_context_release_current(state);
        screensave_gl46_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_gl46_pipeline_create(state, failure_reason_out)) {
        screensave_gl46_context_release_current(state);
        screensave_gl46_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    screensave_gl46_context_release_current(state);
    screensave_gl46_capture_refresh(state);

    ZeroMemory(&info, sizeof(info));
    info.requested_kind = SCREENSAVE_RENDERER_KIND_GL46;
    info.active_kind = SCREENSAVE_RENDERER_KIND_GL46;
    info.capability_flags = SCREENSAVE_GL46_CAPABILITIES;
    info.drawable_size = state->drawable_size;
    info.backend_name = "gl46";
    info.status_text =
        state->caps.double_buffered
            ? "premium-context-double-buffered"
            : "premium-context-single-buffered";
    info.selection_reason = "force-gl46";
    info.vendor_name = state->caps.vendor;
    info.renderer_name = state->caps.renderer;
    info.version_name =
        state->detail_text[0] != '\0'
            ? state->detail_text
            : state->caps.version;

    screensave_renderer_init_dispatch(renderer, &g_screensave_gl46_vtable, state, &info);
    *renderer_out = renderer;

    screensave_gl46_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        7063UL,
        "gl46_backend",
        state->caps.double_buffered
            ? "The GL46 premium renderer backend is active."
            : "The GL46 premium renderer backend is active without double buffering."
    );
    return 1;
}

int screensave_gl46_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    screensave_gl46_state *state;

    if (!screensave_gl46_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (drawable_size == NULL || drawable_size->width <= 0 || drawable_size->height <= 0) {
        return 0;
    }

    state->drawable_size = *drawable_size;
    screensave_gl46_update_renderer_info(
        renderer,
        drawable_size,
        state->caps.double_buffered
            ? "resized-double-buffered"
            : "resized-single-buffered"
    );
    return 1;
}
