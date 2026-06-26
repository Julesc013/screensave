#ifndef SCREENSAVE_V2_SESSION_H
#define SCREENSAVE_V2_SESSION_H

#include "screensave/v2/clock.h"
#include "screensave/v2/config.h"
#include "screensave/v2/diagnostics.h"
#include "screensave/v2/draw.h"
#include "screensave/v2/seed.h"

#define SS_V2_SESSION_MODE_SCREEN ((ss_u32)0U)
#define SS_V2_SESSION_MODE_PREVIEW ((ss_u32)1U)
#define SS_V2_SESSION_MODE_WINDOWED ((ss_u32)2U)
#define SS_V2_SESSION_MODE_PROOF ((ss_u32)3U)

typedef struct ss_v2_session_tag ss_v2_session;

typedef struct ss_v2_session_desc_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 mode;
    ss_v2_size dimensions;
    ss_v2_seed seed;
    ss_v2_clock clock;
    ss_v2_config_view product_config;
    ss_v2_diag_context *diagnostics;
} ss_v2_session_desc;

typedef struct ss_v2_resize_desc_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_size dimensions;
    ss_v2_diag_context *diagnostics;
} ss_v2_resize_desc;

typedef struct ss_v2_advance_desc_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_clock clock;
    ss_v2_diag_context *diagnostics;
} ss_v2_advance_desc;

typedef struct ss_v2_render_desc_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_clock clock;
    ss_v2_draw_target *draw_target;
    ss_v2_surface_desc *surface;
    ss_v2_diag_context *diagnostics;
} ss_v2_render_desc;

typedef ss_u32 (*ss_v2_session_create_fn)(const ss_v2_session_desc *desc, ss_v2_session **session_out);
typedef void (*ss_v2_session_destroy_fn)(ss_v2_session *session);
typedef ss_u32 (*ss_v2_session_resize_fn)(ss_v2_session *session, const ss_v2_resize_desc *desc);
typedef ss_u32 (*ss_v2_session_advance_fn)(ss_v2_session *session, const ss_v2_advance_desc *desc);
typedef ss_u32 (*ss_v2_session_render_fn)(ss_v2_session *session, const ss_v2_render_desc *desc);

typedef struct ss_v2_session_ops_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_session_create_fn create;
    ss_v2_session_destroy_fn destroy;
    ss_v2_session_resize_fn resize;
    ss_v2_session_advance_fn advance;
    ss_v2_session_render_fn render;
} ss_v2_session_ops;

#endif /* SCREENSAVE_V2_SESSION_H */
