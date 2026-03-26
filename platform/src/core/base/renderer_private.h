#ifndef SCREENSAVE_RENDERER_PRIVATE_H
#define SCREENSAVE_RENDERER_PRIVATE_H

#include "screensave/renderer_api.h"

typedef struct screensave_renderer_vtable_tag {
    int (*begin_frame)(screensave_renderer *renderer, const screensave_frame_info *frame_info);
    void (*clear)(screensave_renderer *renderer, screensave_color color);
    void (*fill_rect)(screensave_renderer *renderer, const screensave_recti *rect, screensave_color color);
    void (*draw_frame_rect)(screensave_renderer *renderer, const screensave_recti *rect, screensave_color color);
    void (*draw_line)(
        screensave_renderer *renderer,
        const screensave_pointi *start_point,
        const screensave_pointi *end_point,
        screensave_color color
    );
    void (*draw_polyline)(
        screensave_renderer *renderer,
        const screensave_pointi *points,
        unsigned int point_count,
        screensave_color color
    );
    int (*blit_bitmap)(
        screensave_renderer *renderer,
        const screensave_bitmap_view *bitmap,
        const screensave_recti *destination_rect
    );
    int (*end_frame)(screensave_renderer *renderer);
    void (*shutdown)(screensave_renderer *renderer);
} screensave_renderer_vtable;

struct screensave_renderer_tag {
    const screensave_renderer_vtable *vtable;
    screensave_renderer_info info;
    void *backend_state;
};

void screensave_renderer_init_dispatch(
    screensave_renderer *renderer,
    const screensave_renderer_vtable *vtable,
    void *backend_state,
    const screensave_renderer_info *info
);

#endif /* SCREENSAVE_RENDERER_PRIVATE_H */
