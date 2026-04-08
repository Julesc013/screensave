#ifndef SCREENSAVE_GL46_INTERNAL_H
#define SCREENSAVE_GL46_INTERNAL_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <gl/gl.h>

#include "screensave/diagnostics_api.h"
#include "screensave/private/renderer_runtime.h"
#include "screensave/renderer_api.h"

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

#ifndef GL_STREAM_DRAW
#define GL_STREAM_DRAW 0x88E0
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
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

typedef ptrdiff_t screensave_gl46_sizeiptr;

#ifndef SCREENSAVE_WGL_CREATE_CONTEXT_ATTRIBS_ARB_PROC_DEFINED
#define SCREENSAVE_WGL_CREATE_CONTEXT_ATTRIBS_ARB_PROC_DEFINED
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int *);
#endif

#ifndef SCREENSAVE_GL_SHARED_PIPELINE_PROC_TYPES_DEFINED
#define SCREENSAVE_GL_SHARED_PIPELINE_PROC_TYPES_DEFINED
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei count, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC)(
    GLenum target,
    screensave_gl46_sizeiptr size,
    const GLvoid *data,
    GLenum usage
);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei count, const GLuint *buffers);
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei count, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei count, const GLuint *arrays);
typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC)(GLenum shader_type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC)(
    GLuint shader,
    GLsizei count,
    const char *const *text,
    const GLint *lengths
);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(
    GLuint shader,
    GLsizei buffer_size,
    GLsizei *length_out,
    char *buffer
);
typedef void (APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(
    GLuint program,
    GLsizei buffer_size,
    GLsizei *length_out,
    char *buffer
);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (APIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const char *name);
typedef void (APIENTRY *PFNGLUNIFORM2FPROC)(GLint location, GLfloat value0, GLfloat value1);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint value0);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    const GLvoid *pointer
);
typedef void (APIENTRY *PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY *PFNGLGENFRAMEBUFFERSPROC)(GLsizei count, GLuint *framebuffers);
typedef void (APIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (APIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei count, const GLuint *framebuffers);
typedef GLenum (APIENTRY *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
#endif
typedef void (APIENTRY *PFNGLTEXSTORAGE2DPROC)(
    GLenum target,
    GLsizei levels,
    GLenum internalformat,
    GLsizei width,
    GLsizei height
);
typedef void (APIENTRY *PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint *samplers);
typedef void (APIENTRY *PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
typedef void (APIENTRY *PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint *samplers);
typedef void (APIENTRY *PFNGLBUFFERSTORAGEPROC)(
    GLenum target,
    screensave_gl46_sizeiptr size,
    const void *data,
    GLbitfield flags
);

#define SCREENSAVE_GL46_CAPABILITIES \
    (SCREENSAVE_RENDERER_CAP_CLEAR | \
     SCREENSAVE_RENDERER_CAP_FILL_RECT | \
     SCREENSAVE_RENDERER_CAP_FRAME_RECT | \
     SCREENSAVE_RENDERER_CAP_LINE | \
     SCREENSAVE_RENDERER_CAP_POLYLINE | \
     SCREENSAVE_RENDERER_CAP_BITMAP)

#define SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_ATTRIBS        0x00010000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_PREMIUM_CONTEXT        0x00020000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_COMPAT_PROFILE         0x00040000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_CONTEXT_46_PLUS        0x00080000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_VBO                    0x00100000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_VAO                    0x00200000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_SHADER_PIPELINE        0x00400000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_TEXTURE_STORAGE        0x00800000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_SAMPLER_OBJECTS        0x01000000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_FBO                    0x02000000UL
#define SCREENSAVE_GL46_PRIVATE_CAP_BUFFER_STORAGE         0x04000000UL

typedef struct screensave_gl46_capability_bundle_tag {
    unsigned long required_flags;
    unsigned long preferred_flags;
    unsigned long available_flags;
    unsigned long missing_required_flags;
    int satisfied;
} screensave_gl46_capability_bundle;

typedef struct screensave_gl46_caps_tag {
    unsigned long private_flags;
    int premium_context;
    int compatibility_profile;
    int double_buffered;
    int support_gdi;
    int generic_format;
    int rgba_bits;
    int depth_bits;
    int has_vbo;
    int has_vao;
    int has_shader_pipeline;
    int has_texture_storage;
    int has_sampler_objects;
    int has_fbo;
    int has_buffer_storage;
    int major_version;
    int minor_version;
    char vendor[64];
    char renderer[96];
    char version[64];
    screensave_gl46_capability_bundle bundle;
} screensave_gl46_caps;

typedef struct screensave_gl46_vertex_tag {
    GLfloat x;
    GLfloat y;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    GLfloat u;
    GLfloat v;
} screensave_gl46_vertex;

typedef struct screensave_gl46_state_tag {
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
    char detail_text[192];
    PFNWGLCREATECONTEXTATTRIBSARBPROC create_context_attribs;
    PFNGLACTIVETEXTUREPROC active_texture_fn;
    PFNGLGENBUFFERSPROC gen_buffers_fn;
    PFNGLBINDBUFFERPROC bind_buffer_fn;
    PFNGLBUFFERDATAPROC buffer_data_fn;
    PFNGLDELETEBUFFERSPROC delete_buffers_fn;
    PFNGLGENVERTEXARRAYSPROC gen_vertex_arrays_fn;
    PFNGLBINDVERTEXARRAYPROC bind_vertex_array_fn;
    PFNGLDELETEVERTEXARRAYSPROC delete_vertex_arrays_fn;
    PFNGLCREATESHADERPROC create_shader_fn;
    PFNGLSHADERSOURCEPROC shader_source_fn;
    PFNGLCOMPILESHADERPROC compile_shader_fn;
    PFNGLGETSHADERIVPROC get_shader_iv_fn;
    PFNGLGETSHADERINFOLOGPROC get_shader_info_log_fn;
    PFNGLDELETESHADERPROC delete_shader_fn;
    PFNGLCREATEPROGRAMPROC create_program_fn;
    PFNGLATTACHSHADERPROC attach_shader_fn;
    PFNGLLINKPROGRAMPROC link_program_fn;
    PFNGLGETPROGRAMIVPROC get_program_iv_fn;
    PFNGLGETPROGRAMINFOLOGPROC get_program_info_log_fn;
    PFNGLUSEPROGRAMPROC use_program_fn;
    PFNGLDELETEPROGRAMPROC delete_program_fn;
    PFNGLGETUNIFORMLOCATIONPROC get_uniform_location_fn;
    PFNGLUNIFORM2FPROC uniform_2f_fn;
    PFNGLUNIFORM1IPROC uniform_1i_fn;
    PFNGLENABLEVERTEXATTRIBARRAYPROC enable_vertex_attrib_array_fn;
    PFNGLVERTEXATTRIBPOINTERPROC vertex_attrib_pointer_fn;
    PFNGLDRAWARRAYSPROC draw_arrays_fn;
    PFNGLGENFRAMEBUFFERSPROC gen_framebuffers_fn;
    PFNGLBINDFRAMEBUFFERPROC bind_framebuffer_fn;
    PFNGLDELETEFRAMEBUFFERSPROC delete_framebuffers_fn;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC check_framebuffer_status_fn;
    PFNGLTEXSTORAGE2DPROC tex_storage_2d_fn;
    PFNGLGENSAMPLERSPROC gen_samplers_fn;
    PFNGLBINDSAMPLERPROC bind_sampler_fn;
    PFNGLSAMPLERPARAMETERIPROC sampler_parameter_i_fn;
    PFNGLDELETESAMPLERSPROC delete_samplers_fn;
    PFNGLBUFFERSTORAGEPROC buffer_storage_fn;
    GLuint vertex_buffer;
    GLuint vertex_array;
    GLuint color_program;
    GLuint texture_program;
    GLuint texture_sampler;
    GLint color_viewport_location;
    GLint texture_viewport_location;
    GLint texture_sampler_location;
    screensave_gl46_caps caps;
} screensave_gl46_state;

int screensave_gl46_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
);
int screensave_gl46_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size);
void screensave_gl46_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc);
void screensave_gl46_renderer_clear_present_dc(screensave_renderer *renderer);

int screensave_gl46_state_from_renderer(screensave_renderer *renderer, screensave_gl46_state **state_out);
void screensave_gl46_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
);
void screensave_gl46_emit_diag(
    screensave_gl46_state *state,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
);
void screensave_gl46_capture_refresh(screensave_gl46_state *state);

int screensave_gl46_context_create(
    screensave_gl46_state *state,
    const screensave_sizei *drawable_size,
    const char **failure_reason_out
);
void screensave_gl46_context_destroy(screensave_gl46_state *state);
int screensave_gl46_context_make_current(
    screensave_gl46_state *state,
    const char *origin,
    unsigned long code
);
void screensave_gl46_context_release_current(screensave_gl46_state *state);

int screensave_gl46_capture_caps(
    screensave_gl46_state *state,
    const char **failure_reason_out
);
int screensave_gl46_pipeline_create(
    screensave_gl46_state *state,
    const char **failure_reason_out
);
void screensave_gl46_pipeline_destroy(screensave_gl46_state *state);
int screensave_gl46_present(screensave_renderer *renderer);

void screensave_gl46_clear_impl(screensave_renderer *renderer, screensave_color color);
void screensave_gl46_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gl46_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
);
void screensave_gl46_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
);
void screensave_gl46_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
);
int screensave_gl46_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
);

#endif /* SCREENSAVE_GL46_INTERNAL_H */
