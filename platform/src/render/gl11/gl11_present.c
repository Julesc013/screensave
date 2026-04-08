#include "gl11_internal.h"

void screensave_gl11_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    (void)renderer;
    (void)present_dc;
}

void screensave_gl11_renderer_clear_present_dc(screensave_renderer *renderer)
{
    (void)renderer;
}

int screensave_gl11_present(screensave_renderer *renderer)
{
    screensave_gl11_state *state;
    const char *status_text;

    if (!screensave_gl11_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (state->window_dc == NULL) {
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6303UL,
            "gl11_present",
            "No window device context is available for GL11 present."
        );
        screensave_gl11_update_renderer_info(renderer, &state->drawable_size, "present-failed");
        return 0;
    }

    status_text = "presented-flush";
    if (state->caps.double_buffered) {
        if (!SwapBuffers(state->window_dc)) {
            screensave_gl11_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6301UL,
                "gl11_present",
                "SwapBuffers failed while presenting the GL11 frame."
            );
            screensave_gl11_update_renderer_info(renderer, &state->drawable_size, "present-failed");
            return 0;
        }
        state->swap_count += 1UL;
        status_text = "presented-swap";
    } else {
        glFlush();
        state->flush_count += 1UL;
    }

    state->present_count += 1UL;
    screensave_gl11_capture_refresh(state);
    if (
        state->diagnostics != NULL &&
        screensave_diag_should_emit(state->diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG)
    ) {
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_DEBUG,
            6304UL,
            "gl11_present",
            state->detail_text
        );
    }
    screensave_gl11_update_renderer_info(renderer, &state->drawable_size, status_text);
    return 1;
}
