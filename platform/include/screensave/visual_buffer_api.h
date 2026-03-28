#ifndef SCREENSAVE_VISUAL_BUFFER_API_H
#define SCREENSAVE_VISUAL_BUFFER_API_H

#include "screensave/renderer_api.h"
#include "screensave/types.h"

typedef struct screensave_visual_buffer_tag {
    screensave_sizei size;
    int stride_bytes;
    unsigned char *pixels;
} screensave_visual_buffer;

int screensave_visual_buffer_init(
    screensave_visual_buffer *buffer,
    const screensave_sizei *size
);
int screensave_visual_buffer_resize(
    screensave_visual_buffer *buffer,
    const screensave_sizei *size
);
void screensave_visual_buffer_dispose(screensave_visual_buffer *buffer);
void screensave_visual_buffer_clear(
    screensave_visual_buffer *buffer,
    screensave_color color
);
void screensave_visual_buffer_decay(
    screensave_visual_buffer *buffer,
    unsigned int keep_scale
);
void screensave_visual_buffer_add_pixel(
    screensave_visual_buffer *buffer,
    int x,
    int y,
    screensave_color color,
    unsigned int intensity
);
void screensave_visual_buffer_draw_line(
    screensave_visual_buffer *buffer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color,
    unsigned int intensity
);
void screensave_visual_buffer_get_bitmap_view(
    const screensave_visual_buffer *buffer,
    screensave_bitmap_view *bitmap_view
);
screensave_color screensave_color_lerp(
    screensave_color start_color,
    screensave_color end_color,
    unsigned int amount
);

#endif /* SCREENSAVE_VISUAL_BUFFER_API_H */
