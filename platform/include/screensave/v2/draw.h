#ifndef SCREENSAVE_V2_DRAW_H
#define SCREENSAVE_V2_DRAW_H

#include "screensave/v2/surface.h"

#define SS_V2_DRAW_CAP_CLEAR ((ss_u32)0x00000001U)
#define SS_V2_DRAW_CAP_FILL_RECT ((ss_u32)0x00000002U)
#define SS_V2_DRAW_CAP_FRAME_RECT ((ss_u32)0x00000004U)
#define SS_V2_DRAW_CAP_LINE ((ss_u32)0x00000008U)
#define SS_V2_DRAW_CAP_POLYLINE ((ss_u32)0x00000010U)

typedef ss_u32 (*ss_v2_draw_clear_fn)(void *user_data, const ss_v2_color_rgba8 *color);
typedef ss_u32 (*ss_v2_draw_fill_rect_fn)(
    void *user_data,
    const ss_v2_recti *rect,
    const ss_v2_color_rgba8 *color
);
typedef ss_u32 (*ss_v2_draw_frame_rect_fn)(
    void *user_data,
    const ss_v2_recti *rect,
    const ss_v2_color_rgba8 *color
);
typedef ss_u32 (*ss_v2_draw_line_fn)(
    void *user_data,
    const ss_v2_pointi *start_point,
    const ss_v2_pointi *end_point,
    const ss_v2_color_rgba8 *color
);
typedef ss_u32 (*ss_v2_draw_polyline_fn)(
    void *user_data,
    const ss_v2_pointi *points,
    ss_u32 point_count,
    const ss_v2_color_rgba8 *color
);

typedef struct ss_v2_draw_ops_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 capability_flags;
    ss_v2_draw_clear_fn clear;
    ss_v2_draw_fill_rect_fn fill_rect;
    ss_v2_draw_frame_rect_fn frame_rect;
    ss_v2_draw_line_fn line;
    ss_v2_draw_polyline_fn polyline;
} ss_v2_draw_ops;

typedef struct ss_v2_draw_target_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    void *user_data;
    const ss_v2_draw_ops *ops;
    ss_v2_surface_desc *surface;
} ss_v2_draw_target;

ss_u32 ss_v2_draw_ops_is_valid(const ss_v2_draw_ops *ops);
ss_u32 ss_v2_draw_target_is_valid(const ss_v2_draw_target *target);

#endif /* SCREENSAVE_V2_DRAW_H */
