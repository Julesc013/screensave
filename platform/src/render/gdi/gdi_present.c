#include "gdi_internal.h"

typedef struct screensave_gdi_present_target_tag {
    HDC target_dc;
    screensave_sizei target_size;
    int owns_dc;
} screensave_gdi_present_target;

static void screensave_gdi_present_target_zero(screensave_gdi_present_target *target)
{
    if (target == NULL) {
        return;
    }

    ZeroMemory(target, sizeof(*target));
}

static void screensave_gdi_present_target_release(
    screensave_gdi_state *state,
    screensave_gdi_present_target *target
)
{
    if (state == NULL || target == NULL) {
        return;
    }

    if (target->owns_dc && target->target_dc != NULL) {
        ReleaseDC(state->target_window, target->target_dc);
    }
    screensave_gdi_present_target_zero(target);
}

static int screensave_gdi_present_target_acquire(
    screensave_gdi_state *state,
    screensave_gdi_present_target *target
)
{
    RECT client_rect;
    int width;
    int height;

    if (state == NULL || target == NULL) {
        return 0;
    }

    screensave_gdi_present_target_zero(target);
    target->target_size = state->surface.size;

    target->target_dc = state->present_dc;
    if (target->target_dc == NULL) {
        target->target_dc = GetDC(state->target_window);
        target->owns_dc = target->target_dc != NULL;
    }

    if (target->target_dc == NULL) {
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5202UL,
            "gdi_present",
            "No target device context was available for GDI present."
        );
        return 0;
    }

    if (state->target_window != NULL && GetClientRect(state->target_window, &client_rect)) {
        width = client_rect.right - client_rect.left;
        height = client_rect.bottom - client_rect.top;
        if (width > 0 && height > 0) {
            target->target_size.width = width;
            target->target_size.height = height;
        }
    }

    return 1;
}

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
    screensave_gdi_present_target target;
    int used_stretch;
    int previous_stretch_mode;
    const char *status_text;

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

    if (!screensave_gdi_present_target_acquire(state, &target)) {
        return 0;
    }

    used_stretch =
        target.target_size.width != state->surface.size.width ||
        target.target_size.height != state->surface.size.height;
    previous_stretch_mode = 0;
    status_text = "presented";

    if (used_stretch) {
        previous_stretch_mode = SetStretchBltMode(target.target_dc, COLORONCOLOR);
        if (!StretchBlt(
                target.target_dc,
                0,
                0,
                target.target_size.width,
                target.target_size.height,
                state->surface.memory_dc,
                0,
                0,
                state->surface.size.width,
                state->surface.size.height,
                SRCCOPY
            )) {
            if (previous_stretch_mode != 0) {
                SetStretchBltMode(target.target_dc, previous_stretch_mode);
            }
            screensave_gdi_present_target_release(state, &target);
            screensave_gdi_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                5204UL,
                "gdi_present",
                "StretchBlt failed while presenting the GDI backbuffer."
            );
            return 0;
        }
        if (previous_stretch_mode != 0) {
            SetStretchBltMode(target.target_dc, previous_stretch_mode);
        }
        status_text = "presented-stretched";
    } else if (!BitBlt(
            target.target_dc,
            0,
            0,
            state->surface.size.width,
            state->surface.size.height,
            state->surface.memory_dc,
            0,
            0,
            SRCCOPY
        )) {
        screensave_gdi_present_target_release(state, &target);
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5203UL,
            "gdi_present",
            "BitBlt failed while presenting the GDI backbuffer."
        );
        return 0;
    }

    screensave_gdi_present_target_release(state, &target);

    state->present_count += 1UL;
    state->surface_presented = 1;
    screensave_gdi_capture_refresh(state);
    screensave_gdi_update_renderer_info(renderer, &state->surface.size, status_text);
    return 1;
}
