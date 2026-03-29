#include "glp_internal.h"

static int screensave_glp_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    screensave_glp_state *state;

    if (!screensave_glp_state_from_renderer(renderer, &state) || frame_info == NULL) {
        return 0;
    }

    if (!screensave_glp_context_make_current(state, "glp_backend", 6801UL)) {
        screensave_glp_update_renderer_info(renderer, NULL, "make-current-failed");
        return 0;
    }

    if (frame_info->drawable_size.width > 0 && frame_info->drawable_size.height > 0) {
        state->drawable_size = frame_info->drawable_size;
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
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
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

    state->frame_open = 1;
    screensave_glp_update_renderer_info(renderer, &state->drawable_size, "frame-open");
    return 1;
}

static int screensave_glp_end_frame(screensave_renderer *renderer)
{
    screensave_glp_state *state;
    int result;

    if (!screensave_glp_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!state->frame_open) {
        return 0;
    }

    state->frame_open = 0;
    result = screensave_glp_present(renderer);
    screensave_glp_context_release_current(state);
    return result;
}

static void screensave_glp_shutdown_impl(screensave_renderer *renderer)
{
    screensave_glp_state *state;

    if (renderer == NULL) {
        return;
    }

    state = (screensave_glp_state *)renderer->backend_state;
    if (state != NULL) {
        screensave_glp_context_destroy(state);
        free(state);
    }

    free(renderer);
}

static const screensave_renderer_vtable g_screensave_glp_vtable = {
    screensave_glp_begin_frame,
    screensave_glp_clear_impl,
    screensave_glp_fill_rect_impl,
    screensave_glp_draw_frame_rect_impl,
    screensave_glp_draw_line_impl,
    screensave_glp_draw_polyline_impl,
    screensave_glp_blit_bitmap_impl,
    screensave_glp_end_frame,
    screensave_glp_shutdown_impl
};

int screensave_glp_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    screensave_renderer *renderer;
    screensave_glp_state *state;
    screensave_renderer_info info;

    if (renderer_out == NULL) {
        return 0;
    }

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    *renderer_out = NULL;
    renderer = (screensave_renderer *)malloc(sizeof(*renderer));
    state = (screensave_glp_state *)malloc(sizeof(*state));
    if (renderer == NULL || state == NULL) {
        if (renderer != NULL) {
            free(renderer);
        }
        if (state != NULL) {
            free(state);
        }
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-out-of-memory";
        }
        return 0;
    }

    ZeroMemory(state, sizeof(*state));
    state->target_window = target_window;
    state->diagnostics = diagnostics;
    if (!screensave_glp_context_create(state, drawable_size, failure_reason_out)) {
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_glp_context_make_current(state, "glp_backend", 6802UL)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-make-current-failed";
        }
        screensave_glp_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    if (!screensave_glp_capture_caps(state, failure_reason_out)) {
        screensave_glp_context_release_current(state);
        screensave_glp_context_destroy(state);
        free(state);
        free(renderer);
        return 0;
    }

    screensave_glp_context_release_current(state);

    ZeroMemory(&info, sizeof(info));
    info.requested_kind = SCREENSAVE_RENDERER_KIND_GL_PLUS;
    info.active_kind = SCREENSAVE_RENDERER_KIND_GL_PLUS;
    info.capability_flags = SCREENSAVE_GLP_CAPABILITIES | state->caps.private_flags;
    info.drawable_size = state->drawable_size;
    info.backend_name = "gl_plus";
    info.status_text = state->caps.double_buffered ? "advanced-context-double-buffered" : "advanced-context-single-buffered";
    info.selection_reason = "force-gl-plus";
    info.vendor_name = state->caps.vendor;
    info.renderer_name = state->caps.renderer;
    info.version_name = state->caps.version;

    screensave_renderer_init_dispatch(renderer, &g_screensave_glp_vtable, state, &info);
    *renderer_out = renderer;

    screensave_glp_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        6803UL,
        "glp_backend",
        "The advanced GL renderer backend is active."
    );
    return 1;
}

int screensave_glp_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    screensave_glp_state *state;

    if (!screensave_glp_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (drawable_size == NULL || drawable_size->width <= 0 || drawable_size->height <= 0) {
        return 0;
    }

    state->drawable_size = *drawable_size;
    screensave_glp_update_renderer_info(renderer, drawable_size, "resized");
    return 1;
}
