#include "gdi_internal.h"

static void screensave_gdi_surface_zero(screensave_gdi_surface *surface)
{
    if (surface == NULL) {
        return;
    }

    ZeroMemory(surface, sizeof(*surface));
}

static void screensave_gdi_normalize_size(
    const screensave_sizei *source,
    screensave_sizei *target
)
{
    if (target == NULL) {
        return;
    }

    target->width = 1;
    target->height = 1;

    if (source != NULL) {
        if (source->width > 0) {
            target->width = source->width;
        }
        if (source->height > 0) {
            target->height = source->height;
        }
    }
}

static int screensave_gdi_surface_build(
    screensave_gdi_state *state,
    const screensave_sizei *drawable_size,
    screensave_gdi_surface *surface_out,
    const char **failure_reason_out
)
{
    screensave_sizei normalized_size;
    BITMAPINFO bitmap_info;
    HDC window_dc;
    HDC memory_dc;
    HBITMAP bitmap;
    HBITMAP previous_bitmap;
    void *bits;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL || surface_out == NULL || state->target_window == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-invalid-window";
        }
        return 0;
    }

    screensave_gdi_surface_zero(surface_out);
    screensave_gdi_normalize_size(drawable_size, &normalized_size);

    window_dc = GetDC(state->target_window);
    if (window_dc == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-getdc-failed";
        }
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5101UL,
            "gdi_surface",
            "GetDC failed while preparing the GDI backbuffer."
        );
        return 0;
    }

    memory_dc = CreateCompatibleDC(window_dc);
    if (memory_dc == NULL) {
        ReleaseDC(state->target_window, window_dc);
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-create-memory-dc-failed";
        }
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5102UL,
            "gdi_surface",
            "CreateCompatibleDC failed while preparing the GDI backbuffer."
        );
        return 0;
    }

    screensave_gdi_bitmap_info_init(
        &bitmap_info,
        normalized_size.width,
        normalized_size.height,
        32U,
        0
    );

    bits = NULL;
    bitmap = CreateDIBSection(window_dc, &bitmap_info, DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(state->target_window, window_dc);

    if (bitmap == NULL || bits == NULL) {
        DeleteDC(memory_dc);
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-create-dib-failed";
        }
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5103UL,
            "gdi_surface",
            "CreateDIBSection failed while preparing the GDI backbuffer."
        );
        return 0;
    }

    previous_bitmap = (HBITMAP)SelectObject(memory_dc, bitmap);
    if (previous_bitmap == NULL || previous_bitmap == HGDI_ERROR) {
        DeleteObject(bitmap);
        DeleteDC(memory_dc);
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-select-bitmap-failed";
        }
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5104UL,
            "gdi_surface",
            "SelectObject failed while binding the GDI backbuffer."
        );
        return 0;
    }

    surface_out->memory_dc = memory_dc;
    surface_out->bitmap = bitmap;
    surface_out->old_bitmap = previous_bitmap;
    surface_out->bits = bits;
    surface_out->size = normalized_size;
    surface_out->stride_bytes = ((normalized_size.width * 32 + 31) / 32) * 4;
    return 1;
}

void screensave_gdi_surface_release(screensave_gdi_surface *surface)
{
    if (surface == NULL) {
        return;
    }

    if (surface->memory_dc != NULL && surface->old_bitmap != NULL) {
        SelectObject(surface->memory_dc, surface->old_bitmap);
    }

    if (surface->bitmap != NULL) {
        DeleteObject(surface->bitmap);
    }

    if (surface->memory_dc != NULL) {
        DeleteDC(surface->memory_dc);
    }

    screensave_gdi_surface_zero(surface);
}

int screensave_gdi_surface_prepare(
    screensave_gdi_state *state,
    const screensave_sizei *drawable_size,
    int *recreated_out,
    const char **failure_reason_out
)
{
    screensave_sizei normalized_size;
    screensave_gdi_surface pending_surface;
    int rebuilt;

    if (recreated_out != NULL) {
        *recreated_out = 0;
    }
    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-no-surface";
        }
        return 0;
    }

    screensave_gdi_normalize_size(drawable_size, &normalized_size);
    if (
        state->surface.memory_dc != NULL &&
        state->surface.size.width == normalized_size.width &&
        state->surface.size.height == normalized_size.height
    ) {
        return 1;
    }

    rebuilt = screensave_gdi_surface_build(
        state,
        &normalized_size,
        &pending_surface,
        failure_reason_out
    );
    if (!rebuilt) {
        if (state->surface.memory_dc != NULL) {
            screensave_gdi_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_WARNING,
                5106UL,
                "gdi_surface",
                "Retaining the previous GDI backbuffer after a resize allocation failure."
            );
            return 1;
        }
        return 0;
    }

    screensave_gdi_surface_release(&state->surface);
    state->surface = pending_surface;
    state->surface_generation += 1UL;
    state->surface_presented = 0;
    state->last_capture_signature = 0UL;
    screensave_gdi_capture_refresh(state);

    if (recreated_out != NULL) {
        *recreated_out = 1;
    }

    screensave_gdi_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        5105UL,
        "gdi_surface",
        "GDI backbuffer prepared."
    );
    return 1;
}
