#include "gl21_internal.h"

static int screensave_gl21_string_contains(const char *haystack, const char *needle)
{
    if (haystack == NULL || needle == NULL || needle[0] == '\0') {
        return 0;
    }

    return strstr(haystack, needle) != NULL;
}

int screensave_gl21_capture_caps(
    screensave_gl21_state *state,
    const char **failure_reason_out
)
{
    const GLubyte *vendor;
    const GLubyte *renderer;
    const GLubyte *version;
    const GLubyte *extensions;
    GLint major_version;
    GLint minor_version;
    int double_buffered;
    int support_gdi;
    int generic_format;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl21-capture-invalid-state";
        }
        return 0;
    }

    double_buffered = state->caps.double_buffered;
    support_gdi = state->caps.support_gdi;
    generic_format = state->caps.generic_format;

    vendor = glGetString(GL_VENDOR);
    renderer = glGetString(GL_RENDERER);
    version = glGetString(GL_VERSION);
    extensions = glGetString(GL_EXTENSIONS);
    if (vendor == NULL || renderer == NULL || version == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl21-capture-string-failed";
        }
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6701UL,
            "gl21_caps",
            "The advanced GL path could not query core renderer strings."
        );
        return 0;
    }

    ZeroMemory(&state->caps, sizeof(state->caps));
    state->caps.double_buffered = double_buffered;
    state->caps.support_gdi = support_gdi;
    state->caps.generic_format = generic_format;
    state->caps.advanced_context = 1;
    state->caps.compatibility_profile = 1;
    state->caps.private_flags =
        SCREENSAVE_GL21_PRIVATE_CAP_ADVANCED_CONTEXT |
        SCREENSAVE_GL21_PRIVATE_CAP_COMPAT_PROFILE;
    lstrcpynA(state->caps.vendor, (const char *)vendor, sizeof(state->caps.vendor));
    lstrcpynA(state->caps.renderer, (const char *)renderer, sizeof(state->caps.renderer));
    lstrcpynA(state->caps.version, (const char *)version, sizeof(state->caps.version));

    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    glGetIntegerv(GL_MINOR_VERSION, &minor_version);
    state->caps.major_version = (int)major_version;
    state->caps.minor_version = (int)minor_version;

    if (extensions != NULL) {
        if (screensave_gl21_string_contains((const char *)extensions, "GL_ARB_vertex_buffer_object")) {
            state->caps.has_vbo = 1;
            state->caps.private_flags |= SCREENSAVE_GL21_PRIVATE_CAP_VBO;
        }
        if (
            screensave_gl21_string_contains((const char *)extensions, "GL_ARB_framebuffer_object") ||
            screensave_gl21_string_contains((const char *)extensions, "GL_EXT_framebuffer_object")
        ) {
            state->caps.has_fbo = 1;
            state->caps.private_flags |= SCREENSAVE_GL21_PRIVATE_CAP_FBO;
        }
        if (
            screensave_gl21_string_contains((const char *)extensions, "GL_ARB_texture_rectangle") ||
            screensave_gl21_string_contains((const char *)extensions, "GL_EXT_texture_rectangle")
        ) {
            state->caps.has_texture_rectangle = 1;
            state->caps.private_flags |= SCREENSAVE_GL21_PRIVATE_CAP_TEXTURE_RECT;
        }
    }

    screensave_gl21_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        6702UL,
        "gl21_caps",
        "The GL21 path captured renderer capability details successfully."
    );
    return 1;
}


