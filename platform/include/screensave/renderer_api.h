#ifndef SCREENSAVE_RENDERER_API_H
#define SCREENSAVE_RENDERER_API_H

#include "screensave/types.h"

typedef enum screensave_renderer_kind_tag {
    SCREENSAVE_RENDERER_KIND_UNKNOWN = 0,
    SCREENSAVE_RENDERER_KIND_GDI = 1,
    SCREENSAVE_RENDERER_KIND_GL11 = 2,
    SCREENSAVE_RENDERER_KIND_GL_PLUS = 3
} screensave_renderer_kind;

#define SCREENSAVE_RENDERER_CAP_CLEAR 0x00000001UL
#define SCREENSAVE_RENDERER_CAP_FILL_RECT 0x00000002UL
#define SCREENSAVE_RENDERER_CAP_FRAME_RECT 0x00000004UL
#define SCREENSAVE_RENDERER_CAP_LINE 0x00000008UL
#define SCREENSAVE_RENDERER_CAP_POLYLINE 0x00000010UL
#define SCREENSAVE_RENDERER_CAP_BITMAP 0x00000020UL

typedef struct screensave_frame_info_tag {
    screensave_sizei drawable_size;
    unsigned long frame_index;
    unsigned long elapsed_millis;
    unsigned long delta_millis;
} screensave_frame_info;

typedef struct screensave_bitmap_view_tag {
    const void *pixels;
    screensave_sizei size;
    int stride_bytes;
    unsigned int bits_per_pixel;
} screensave_bitmap_view;

typedef struct screensave_renderer_info_tag {
    screensave_renderer_kind requested_kind;
    screensave_renderer_kind active_kind;
    unsigned long capability_flags;
    screensave_sizei drawable_size;
    const char *backend_name;
    const char *status_text;
} screensave_renderer_info;

typedef struct screensave_renderer_tag screensave_renderer;

const char *screensave_renderer_kind_name(screensave_renderer_kind kind);
int screensave_renderer_has_capability(unsigned long capability_flags, unsigned long capability_flag);
void screensave_renderer_get_info(const screensave_renderer *renderer, screensave_renderer_info *info);
int screensave_renderer_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info);
void screensave_renderer_clear(screensave_renderer *renderer, screensave_color color);
void screensave_renderer_fill_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_renderer_draw_frame_rect(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
);
void screensave_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
);
int screensave_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
);
int screensave_renderer_end_frame(screensave_renderer *renderer);
void screensave_renderer_shutdown(screensave_renderer *renderer);

#endif /* SCREENSAVE_RENDERER_API_H */
