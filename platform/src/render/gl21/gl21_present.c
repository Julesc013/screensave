#include "gl21_internal.h"

void screensave_gl21_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    (void)renderer;
    (void)present_dc;
}

void screensave_gl21_renderer_clear_present_dc(screensave_renderer *renderer)
{
    (void)renderer;
}

int screensave_gl21_present(screensave_renderer *renderer)
{
    screensave_gl21_state *state;
    const char *status_text;

    if (!screensave_gl21_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (state->window_dc == NULL) {
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6712UL,
            "gl21_present",
            "No window device context is available for advanced GL present."
        );
        screensave_gl21_update_renderer_info(renderer, &state->drawable_size, "present-failed");
        return 0;
    }

    status_text = "presented-flush";
    if (state->caps.double_buffered) {
        if (!SwapBuffers(state->window_dc)) {
            screensave_gl21_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6713UL,
                "gl21_present",
                "SwapBuffers failed while presenting the advanced GL frame."
            );
            screensave_gl21_update_renderer_info(renderer, &state->drawable_size, "present-failed");
            return 0;
        }
        state->swap_count += 1UL;
        status_text = "presented-swap";
    } else {
        glFlush();
        state->flush_count += 1UL;
    }

    state->present_count += 1UL;
    screensave_gl21_capture_refresh(state);
    if (
        state->diagnostics != NULL &&
        screensave_diag_should_emit(state->diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG)
    ) {
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_DEBUG,
            6714UL,
            "gl21_present",
            state->detail_text
        );
    }

    screensave_gl21_update_renderer_info(renderer, &state->drawable_size, status_text);
    return 1;
}


