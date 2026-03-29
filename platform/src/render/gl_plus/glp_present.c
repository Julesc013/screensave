#include "glp_internal.h"

void screensave_glp_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    (void)renderer;
    (void)present_dc;
}

void screensave_glp_renderer_clear_present_dc(screensave_renderer *renderer)
{
    (void)renderer;
}

int screensave_glp_present(screensave_renderer *renderer)
{
    screensave_glp_state *state;

    if (!screensave_glp_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (state->caps.double_buffered) {
        if (!SwapBuffers(state->window_dc)) {
            screensave_glp_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6712UL,
                "glp_present",
                "SwapBuffers failed while presenting the advanced GL frame."
            );
            return 0;
        }
    } else {
        glFlush();
    }

    screensave_glp_update_renderer_info(
        renderer,
        &state->drawable_size,
        state->caps.double_buffered ? "presented-double-buffered" : "presented-single-buffered"
    );
    return 1;
}
