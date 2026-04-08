#ifndef SCREENSAVE_PRIVATE_RENDERER_RUNTIME_H
#define SCREENSAVE_PRIVATE_RENDERER_RUNTIME_H

#include "screensave/renderer_api.h"
#include "screensave/private/backend_caps.h"
#include "screensave/private/present_path.h"
#include "screensave/private/service_seams.h"

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
    screensave_backend_kind backend_kind;
    screensave_render_band active_band;
    screensave_backend_caps backend_caps;
    screensave_present_path present_path;
    screensave_service_seams service_seams;
};

void screensave_renderer_init_dispatch(
    screensave_renderer *renderer,
    const screensave_renderer_vtable *vtable,
    void *backend_state,
    const screensave_renderer_info *info
);
void screensave_renderer_reset_private_state(screensave_renderer *renderer);
void screensave_renderer_set_backend_identity(
    screensave_renderer *renderer,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band
);
void screensave_renderer_set_backend_caps(
    screensave_renderer *renderer,
    const screensave_backend_caps *caps
);
void screensave_renderer_set_present_path(
    screensave_renderer *renderer,
    const screensave_present_path *present_path
);
void screensave_renderer_set_service_seams(
    screensave_renderer *renderer,
    const screensave_service_seams *service_seams
);
int screensave_renderer_get_private_service_seams(
    const screensave_renderer *renderer,
    screensave_service_seams *service_seams_out
);

#endif /* SCREENSAVE_PRIVATE_RENDERER_RUNTIME_H */
