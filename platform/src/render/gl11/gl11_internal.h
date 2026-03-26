#ifndef SCREENSAVE_GL11_INTERNAL_H
#define SCREENSAVE_GL11_INTERNAL_H

#include <windows.h>
#include <gl/gl.h>

#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "../../core/base/renderer_private.h"

#define SCREENSAVE_GL11_CAPABILITIES \
    (SCREENSAVE_RENDERER_CAP_CLEAR | \
     SCREENSAVE_RENDERER_CAP_FILL_RECT | \
     SCREENSAVE_RENDERER_CAP_FRAME_RECT | \
     SCREENSAVE_RENDERER_CAP_LINE | \
     SCREENSAVE_RENDERER_CAP_POLYLINE | \
     SCREENSAVE_RENDERER_CAP_BITMAP)

typedef struct screensave_gl11_caps_tag {
    int double_buffered;
    int support_gdi;
    int generic_format;
    int rgba_bits;
    int depth_bits;
    char vendor[64];
    char renderer[96];
    char version[64];
} screensave_gl11_caps;

typedef struct screensave_gl11_state_tag {
    HWND target_window;
    HDC window_dc;
    HGLRC gl_context;
    screensave_diag_context *diagnostics;
    screensave_sizei drawable_size;
    int pixel_format;
    int frame_open;
    screensave_gl11_caps caps;
} screensave_gl11_state;

int screensave_gl11_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
);
int screensave_gl11_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size);
void screensave_gl11_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc);
void screensave_gl11_renderer_clear_present_dc(screensave_renderer *renderer);

int screensave_gl11_state_from_renderer(screensave_renderer *renderer, screensave_gl11_state **state_out);
void screensave_gl11_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
);
void screensave_gl11_emit_diag(
    screensave_gl11_state *state,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
);

int screensave_gl11_context_create(
    screensave_gl11_state *state,
    const screensave_sizei *drawable_size,
    const char **failure_reason_out
);
void screensave_gl11_context_destroy(screensave_gl11_state *state);
int screensave_gl11_context_make_current(
    screensave_gl11_state *state,
    const char *origin,
    unsigned long code
);
void screensave_gl11_context_release_current(screensave_gl11_state *state);

int screensave_gl11_capture_caps(
    screensave_gl11_state *state,
    const char **failure_reason_out
);
int screensave_gl11_present(screensave_renderer *renderer);

void screensave_gl11_clear_impl(screensave_renderer *renderer, screensave_color color);
void screensave_gl11_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gl11_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gl11_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
);
void screensave_gl11_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
);
int screensave_gl11_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
);

#endif /* SCREENSAVE_GL11_INTERNAL_H */
