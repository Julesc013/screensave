#ifndef SCREENSAVE_GDI_INTERNAL_H
#define SCREENSAVE_GDI_INTERNAL_H

#include <windows.h>

#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "../../core/base/renderer_private.h"

#define SCREENSAVE_GDI_CAPABILITIES \
    (SCREENSAVE_RENDERER_CAP_CLEAR | \
     SCREENSAVE_RENDERER_CAP_FILL_RECT | \
     SCREENSAVE_RENDERER_CAP_FRAME_RECT | \
     SCREENSAVE_RENDERER_CAP_LINE | \
     SCREENSAVE_RENDERER_CAP_POLYLINE | \
     SCREENSAVE_RENDERER_CAP_BITMAP)

typedef struct screensave_gdi_surface_tag {
    HDC memory_dc;
    HBITMAP bitmap;
    HBITMAP old_bitmap;
    void *bits;
    screensave_sizei size;
    int stride_bytes;
} screensave_gdi_surface;

typedef struct screensave_gdi_state_tag {
    HWND target_window;
    HDC present_dc;
    screensave_diag_context *diagnostics;
    screensave_gdi_surface surface;
    int frame_open;
} screensave_gdi_state;

int screensave_gdi_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out
);
int screensave_gdi_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size);
void screensave_gdi_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc);
void screensave_gdi_renderer_clear_present_dc(screensave_renderer *renderer);

int screensave_gdi_state_from_renderer(screensave_renderer *renderer, screensave_gdi_state **state_out);
void screensave_gdi_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
);
void screensave_gdi_emit_diag(
    screensave_gdi_state *state,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
);
COLORREF screensave_gdi_colorref(screensave_color color);

int screensave_gdi_surface_reset(screensave_gdi_state *state, const screensave_sizei *drawable_size);
void screensave_gdi_surface_release(screensave_gdi_surface *surface);
int screensave_gdi_present(screensave_renderer *renderer);

void screensave_gdi_clear_impl(screensave_renderer *renderer, screensave_color color);
void screensave_gdi_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gdi_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gdi_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
);
void screensave_gdi_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
);
int screensave_gdi_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
);

#endif /* SCREENSAVE_GDI_INTERNAL_H */
