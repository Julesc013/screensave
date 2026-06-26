#ifndef SCREENSAVE_PRIVATE_V2_DRAW_RENDERER_BRIDGE_H
#define SCREENSAVE_PRIVATE_V2_DRAW_RENDERER_BRIDGE_H

#include "screensave/renderer_api.h"
#include "screensave/v2/draw.h"

typedef struct ss_v2_renderer_draw_bridge_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    screensave_renderer *renderer;
} ss_v2_renderer_draw_bridge;

void ss_v2_renderer_draw_bridge_init(
    ss_v2_renderer_draw_bridge *bridge,
    screensave_renderer *renderer
);

ss_u32 ss_v2_renderer_draw_bridge_is_valid(const ss_v2_renderer_draw_bridge *bridge);

ss_u32 ss_v2_renderer_draw_bridge_make_target(
    ss_v2_renderer_draw_bridge *bridge,
    ss_v2_draw_target *target
);

#endif /* SCREENSAVE_PRIVATE_V2_DRAW_RENDERER_BRIDGE_H */
