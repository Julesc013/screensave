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

    if (!screensave_gl11_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (state->caps.double_buffered) {
        if (!SwapBuffers(state->window_dc)) {
            screensave_gl11_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6301UL,
                "gl11_present",
                "SwapBuffers failed while presenting the GL11 frame."
            );
            return 0;
        }
    } else {
        glFlush();
    }

    screensave_gl11_update_renderer_info(renderer, &state->drawable_size, "presented");
    return 1;
}
