#include "gdi_internal.h"

int screensave_gdi_state_from_renderer(screensave_renderer *renderer, screensave_gdi_state **state_out)
{
    screensave_gdi_state *state;

    if (state_out != NULL) {
        *state_out = NULL;
    }

    if (renderer == NULL || renderer->backend_state == NULL) {
        return 0;
    }

    state = (screensave_gdi_state *)renderer->backend_state;
    if (state_out != NULL) {
        *state_out = state;
    }

    return 1;
}

void screensave_gdi_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
)
{
    screensave_gdi_state *state;

    if (renderer == NULL) {
        return;
    }

    state = (screensave_gdi_state *)renderer->backend_state;
    renderer->info.active_kind = SCREENSAVE_RENDERER_KIND_GDI;
    renderer->info.capability_flags = SCREENSAVE_GDI_CAPABILITIES;
    renderer->info.backend_name = "gdi";
    renderer->info.status_text = status_text != NULL ? status_text : "ready";
    renderer->info.vendor_name = "Win32";
    renderer->info.renderer_name = "GDI DIB32 floor";
    renderer->info.version_name =
        state != NULL && state->detail_text[0] != '\0'
            ? state->detail_text
            : NULL;

    if (drawable_size != NULL) {
        renderer->info.drawable_size = *drawable_size;
    }
}

void screensave_gdi_emit_diag(
    screensave_gdi_state *state,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    if (state == NULL || state->diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        state->diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_RENDERER,
        code,
        origin,
        text
    );
}

COLORREF screensave_gdi_colorref(screensave_color color)
{
    return RGB(color.red, color.green, color.blue);
}

void screensave_gdi_bitmap_info_init(
    BITMAPINFO *bitmap_info,
    int width,
    int height,
    unsigned int bits_per_pixel,
    int origin_top_left
)
{
    LONG bitmap_height;

    if (bitmap_info == NULL) {
        return;
    }

    ZeroMemory(bitmap_info, sizeof(*bitmap_info));
    bitmap_info->bmiHeader.biSize = sizeof(bitmap_info->bmiHeader);
    bitmap_info->bmiHeader.biWidth = width > 0 ? width : 1;
    bitmap_height = height > 0 ? (LONG)height : 1L;
    if (origin_top_left) {
        bitmap_height = -bitmap_height;
    }
    bitmap_info->bmiHeader.biHeight = bitmap_height;
    bitmap_info->bmiHeader.biPlanes = 1;
    bitmap_info->bmiHeader.biBitCount = (WORD)bits_per_pixel;
    bitmap_info->bmiHeader.biCompression = BI_RGB;
}
