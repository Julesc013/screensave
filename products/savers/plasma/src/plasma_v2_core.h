#ifndef PLASMA_V2_CORE_H
#define PLASMA_V2_CORE_H

#include "screensave/v2.h"
#include "plasma_spec_v2.h"

typedef struct plasma_v2_core_desc_tag {
    const plasma_spec_v2 *spec;
    ss_u32 width;
    ss_u32 height;
    ss_u32 base_seed;
    ss_u32 stream_seed;
} plasma_v2_core_desc;

typedef struct plasma_v2_core_session_tag plasma_v2_core_session;

ss_u32 plasma_v2_core_create(const plasma_v2_core_desc *desc, plasma_v2_core_session **session_out);
void plasma_v2_core_destroy(plasma_v2_core_session *session);
ss_u32 plasma_v2_core_resize(plasma_v2_core_session *session, ss_u32 width, ss_u32 height);
ss_u32 plasma_v2_core_advance(plasma_v2_core_session *session, ss_u32 delta_ms);
ss_u32 plasma_v2_core_render(plasma_v2_core_session *session, ss_v2_draw_target *target);

#endif /* PLASMA_V2_CORE_H */
