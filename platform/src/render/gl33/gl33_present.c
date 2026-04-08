#include "gl33_internal.h"

void screensave_gl33_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    (void)renderer;
    (void)present_dc;
}

void screensave_gl33_renderer_clear_present_dc(screensave_renderer *renderer)
{
    (void)renderer;
}

int screensave_gl33_present(screensave_renderer *renderer)
{
    screensave_gl33_state *state;
    const char *status_text;

    if (!screensave_gl33_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (state->window_dc == NULL) {
        screensave_gl33_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6971UL,
            "gl33_present",
            "No window device context is available for GL33 present."
        );
        screensave_gl33_update_renderer_info(renderer, &state->drawable_size, "present-failed");
        return 0;
    }

    status_text = "presented-flush";
    if (state->caps.double_buffered) {
        if (!SwapBuffers(state->window_dc)) {
            screensave_gl33_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6972UL,
                "gl33_present",
                "SwapBuffers failed while presenting the GL33 frame."
            );
            screensave_gl33_update_renderer_info(renderer, &state->drawable_size, "present-failed");
            return 0;
        }
        state->swap_count += 1UL;
        status_text = "presented-swap";
    } else {
        glFlush();
        state->flush_count += 1UL;
    }

    state->present_count += 1UL;
    screensave_gl33_capture_refresh(state);
    if (
        state->diagnostics != NULL &&
        screensave_diag_should_emit(state->diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG)
    ) {
        screensave_gl33_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_DEBUG,
            6973UL,
            "gl33_present",
            state->detail_text
        );
    }

    screensave_gl33_update_renderer_info(renderer, &state->drawable_size, status_text);
    return 1;
}
