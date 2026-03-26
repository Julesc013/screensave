#include "gdi_internal.h"

static void screensave_gdi_surface_zero(screensave_gdi_surface *surface)
{
    if (surface == NULL) {
        return;
    }

    ZeroMemory(surface, sizeof(*surface));
}

static void screensave_gdi_normalize_size(const screensave_sizei *source, screensave_sizei *target)
{
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

void screensave_gdi_surface_release(screensave_gdi_surface *surface)
{
    if (surface == NULL) {
        return;
    }

    if (surface->memory_dc != NULL && surface->old_bitmap != NULL) {
        SelectObject(surface->memory_dc, surface->old_bitmap);
        surface->old_bitmap = NULL;
    }

    if (surface->bitmap != NULL) {
        DeleteObject(surface->bitmap);
        surface->bitmap = NULL;
    }

    if (surface->memory_dc != NULL) {
        DeleteDC(surface->memory_dc);
        surface->memory_dc = NULL;
    }

    surface->bits = NULL;
    surface->size.width = 0;
    surface->size.height = 0;
    surface->stride_bytes = 0;
}

int screensave_gdi_surface_reset(screensave_gdi_state *state, const screensave_sizei *drawable_size)
{
    screensave_sizei normalized_size;
    BITMAPINFO bitmap_info;
    HDC window_dc;
    HDC memory_dc;
    HBITMAP bitmap;
    HBITMAP previous_bitmap;
    void *bits;

    if (state == NULL) {
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

    screensave_gdi_surface_release(&state->surface);
    screensave_gdi_surface_zero(&state->surface);

    window_dc = GetDC(state->target_window);
    if (window_dc == NULL) {
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5101UL,
            "gdi_surface",
            "GetDC failed while creating the GDI backbuffer."
        );
        return 0;
    }

    memory_dc = CreateCompatibleDC(window_dc);
    if (memory_dc == NULL) {
        ReleaseDC(state->target_window, window_dc);
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5102UL,
            "gdi_surface",
            "CreateCompatibleDC failed for the GDI backbuffer."
        );
        return 0;
    }

    ZeroMemory(&bitmap_info, sizeof(bitmap_info));
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = normalized_size.width;
    bitmap_info.bmiHeader.biHeight = normalized_size.height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    bits = NULL;
    bitmap = CreateDIBSection(window_dc, &bitmap_info, DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(state->target_window, window_dc);

    if (bitmap == NULL || bits == NULL) {
        DeleteDC(memory_dc);
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5103UL,
            "gdi_surface",
            "CreateDIBSection failed for the GDI backbuffer."
        );
        return 0;
    }

    previous_bitmap = (HBITMAP)SelectObject(memory_dc, bitmap);
    if (previous_bitmap == NULL || previous_bitmap == HGDI_ERROR) {
        DeleteObject(bitmap);
        DeleteDC(memory_dc);
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            5104UL,
            "gdi_surface",
            "SelectObject failed while binding the GDI backbuffer."
        );
        return 0;
    }

    state->surface.memory_dc = memory_dc;
    state->surface.bitmap = bitmap;
    state->surface.old_bitmap = previous_bitmap;
    state->surface.bits = bits;
    state->surface.size = normalized_size;
    state->surface.stride_bytes = ((normalized_size.width * 32 + 31) / 32) * 4;

    screensave_gdi_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        5105UL,
        "gdi_surface",
        "GDI backbuffer created."
    );
    return 1;
}
