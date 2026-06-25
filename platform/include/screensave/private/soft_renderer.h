#ifndef SCREENSAVE_PRIVATE_SOFT_RENDERER_H
#define SCREENSAVE_PRIVATE_SOFT_RENDERER_H

#include "screensave/private/surface_rgba8.h"

void screensave_soft_fill_rect(
    screensave_rgba8_surface *surface,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_soft_draw_frame_rect(
    screensave_rgba8_surface *surface,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_soft_draw_line(
    screensave_rgba8_surface *surface,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
);
void screensave_soft_draw_polyline(
    screensave_rgba8_surface *surface,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
);

#endif /* SCREENSAVE_PRIVATE_SOFT_RENDERER_H */
