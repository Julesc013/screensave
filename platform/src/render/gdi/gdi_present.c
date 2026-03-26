#include "gdi_internal.h"

void screensave_gdi_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    screensave_gdi_state *state;

    if (!screensave_gdi_state_from_renderer(renderer, &state)) {
        return;
    }

    state->present_dc = present_dc;
}

void screensave_gdi_renderer_clear_present_dc(screensave_renderer *renderer)
{
    screensave_gdi_state *state;

    if (!screensave_gdi_state_from_renderer(renderer, &state)) {
        return;
    }

    state->present_dc = NULL;
}

int screensave_gdi_present(screensave_renderer *renderer)
{
    screensave_gdi_state *state;
    HDC target_dc;
    int owns_dc;

    if (!screensave_gdi_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (state->surface.memory_dc == NULL) {
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5201UL,
            "gdi_present",
            "The GDI backbuffer is not available for presentation."
        );
        return 0;
    }

    owns_dc = 0;
    target_dc = state->present_dc;
    if (target_dc == NULL) {
        target_dc = GetDC(state->target_window);
        owns_dc = target_dc != NULL;
    }

    if (target_dc == NULL) {
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5202UL,
            "gdi_present",
            "No target device context was available for GDI present."
        );
        return 0;
    }

    if (!BitBlt(
            target_dc,
            0,
            0,
            state->surface.size.width,
            state->surface.size.height,
            state->surface.memory_dc,
            0,
            0,
            SRCCOPY
        )) {
        if (owns_dc) {
            ReleaseDC(state->target_window, target_dc);
        }
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5203UL,
            "gdi_present",
            "BitBlt failed while presenting the GDI backbuffer."
        );
        return 0;
    }

    if (owns_dc) {
        ReleaseDC(state->target_window, target_dc);
    }

    screensave_gdi_update_renderer_info(renderer, &state->surface.size, "presented");
    return 1;
}
