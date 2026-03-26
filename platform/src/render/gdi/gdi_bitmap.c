#include "gdi_internal.h"

int screensave_gdi_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    screensave_gdi_state *state;
    BITMAPINFO bitmap_info;
    int destination_width;
    int destination_height;
    int destination_x;
    int destination_y;
    int source_height;

    if (
        !screensave_gdi_state_from_renderer(renderer, &state) ||
        state->surface.memory_dc == NULL ||
        bitmap == NULL ||
        bitmap->pixels == NULL ||
        bitmap->size.width <= 0 ||
        bitmap->size.height <= 0
    ) {
        return 0;
    }

    if (bitmap->bits_per_pixel != 24U && bitmap->bits_per_pixel != 32U) {
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            5301UL,
            "gdi_bitmap",
            "Unsupported bitmap format; only 24-bit and 32-bit bitmap views are accepted."
        );
        return 0;
    }

    if (destination_rect != NULL) {
        destination_x = destination_rect->x;
        destination_y = destination_rect->y;
        destination_width = destination_rect->width;
        destination_height = destination_rect->height;
    } else {
        destination_x = 0;
        destination_y = 0;
        destination_width = bitmap->size.width;
        destination_height = bitmap->size.height;
    }

    if (destination_width <= 0 || destination_height <= 0) {
        return 0;
    }

    ZeroMemory(&bitmap_info, sizeof(bitmap_info));
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = bitmap->size.width;
    source_height = bitmap->origin_top_left ? -bitmap->size.height : bitmap->size.height;
    bitmap_info.bmiHeader.biHeight = source_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = (WORD)bitmap->bits_per_pixel;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    SetStretchBltMode(state->surface.memory_dc, COLORONCOLOR);
    if (StretchDIBits(
            state->surface.memory_dc,
            destination_x,
            destination_y,
            destination_width,
            destination_height,
            0,
            0,
            bitmap->size.width,
            bitmap->size.height,
            bitmap->pixels,
            &bitmap_info,
            DIB_RGB_COLORS,
            SRCCOPY
        ) == GDI_ERROR) {
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            5302UL,
            "gdi_bitmap",
            "StretchDIBits failed while drawing a bitmap view."
        );
        return 0;
    }

    return 1;
}
