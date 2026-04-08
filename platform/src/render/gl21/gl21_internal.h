#ifndef SCREENSAVE_GL21_INTERNAL_H
#define SCREENSAVE_GL21_INTERNAL_H

#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <gl/gl.h>

#include "screensave/diagnostics_api.h"
#include "screensave/private/renderer_runtime.h"
#include "screensave/renderer_api.h"

#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION 0x821B
#endif

#ifndef GL_MINOR_VERSION
#define GL_MINOR_VERSION 0x821C
#endif

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef WGL_CONTEXT_MINOR_VERSION_ARB
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef WGL_CONTEXT_FLAGS_ARB
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#endif

#ifndef WGL_CONTEXT_PROFILE_MASK_ARB
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#endif

#ifndef WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int *);

#define SCREENSAVE_GL21_CAPABILITIES \
    (SCREENSAVE_RENDERER_CAP_CLEAR | \
     SCREENSAVE_RENDERER_CAP_FILL_RECT | \
     SCREENSAVE_RENDERER_CAP_FRAME_RECT | \
     SCREENSAVE_RENDERER_CAP_LINE | \
     SCREENSAVE_RENDERER_CAP_POLYLINE | \
     SCREENSAVE_RENDERER_CAP_BITMAP)

#define SCREENSAVE_GL21_PRIVATE_CAP_ADVANCED_CONTEXT 0x01000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_COMPAT_PROFILE   0x02000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_VBO              0x04000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_FBO              0x08000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_TEXTURE_RECT     0x10000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_ATTRIBS  0x20000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_21_PLUS  0x40000000UL
#define SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_32_PLUS  0x80000000UL

typedef struct screensave_gl21_capability_bundle_tag {
    unsigned long required_flags;
    unsigned long preferred_flags;
    unsigned long available_flags;
    unsigned long missing_required_flags;
    int satisfied;
} screensave_gl21_capability_bundle;

typedef struct screensave_gl21_caps_tag {
    unsigned long private_flags;
    int advanced_context;
    int compatibility_profile;
    int double_buffered;
    int support_gdi;
    int generic_format;
    int rgba_bits;
    int depth_bits;
    int has_vbo;
    int has_fbo;
    int has_texture_rectangle;
    int major_version;
    int minor_version;
    char vendor[64];
    char renderer[96];
    char version[64];
    screensave_gl21_capability_bundle bundle;
} screensave_gl21_caps;

typedef struct screensave_gl21_state_tag {
    HWND target_window;
    HDC window_dc;
    HGLRC bootstrap_context;
    HGLRC gl_context;
    screensave_diag_context *diagnostics;
    screensave_sizei drawable_size;
    int pixel_format;
    int frame_open;
    unsigned long present_count;
    unsigned long swap_count;
    unsigned long flush_count;
    char detail_text[128];
    PFNWGLCREATECONTEXTATTRIBSARBPROC create_context_attribs;
    screensave_gl21_caps caps;
} screensave_gl21_state;

int screensave_gl21_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
);
int screensave_gl21_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size);
void screensave_gl21_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc);
void screensave_gl21_renderer_clear_present_dc(screensave_renderer *renderer);

int screensave_gl21_state_from_renderer(screensave_renderer *renderer, screensave_gl21_state **state_out);
void screensave_gl21_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
);
void screensave_gl21_emit_diag(
    screensave_gl21_state *state,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
);
void screensave_gl21_capture_refresh(screensave_gl21_state *state);

int screensave_gl21_context_create(
    screensave_gl21_state *state,
    const screensave_sizei *drawable_size,
    const char **failure_reason_out
);
void screensave_gl21_context_destroy(screensave_gl21_state *state);
int screensave_gl21_context_make_current(
    screensave_gl21_state *state,
    const char *origin,
    unsigned long code
);
void screensave_gl21_context_release_current(screensave_gl21_state *state);

int screensave_gl21_capture_caps(
    screensave_gl21_state *state,
    const char **failure_reason_out
);
int screensave_gl21_present(screensave_renderer *renderer);

void screensave_gl21_clear_impl(screensave_renderer *renderer, screensave_color color);
void screensave_gl21_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gl21_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gl21_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
);
void screensave_gl21_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
);
int screensave_gl21_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
);

#endif /* SCREENSAVE_GL21_INTERNAL_H */


