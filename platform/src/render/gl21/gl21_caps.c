#include "gl21_internal.h"

static int screensave_gl21_string_contains(const char *haystack, const char *needle)
{
    if (haystack == NULL || needle == NULL || needle[0] == '\0') {
        return 0;
    }

    return strstr(haystack, needle) != NULL;
}

static int screensave_gl21_parse_version_string(
    const char *version_text,
    int *major_out,
    int *minor_out
)
{
    unsigned int index;
    int minor;

    if (major_out != NULL) {
        *major_out = 0;
    }
    if (minor_out != NULL) {
        *minor_out = 0;
    }

    if (version_text == NULL) {
        return 0;
    }

    index = 0U;
    while (version_text[index] >= '0' && version_text[index] <= '9') {
        if (major_out != NULL) {
            *major_out = (*major_out * 10) + (version_text[index] - '0');
        }
        ++index;
    }
    if (version_text[index] != '.') {
        return 0;
    }

    ++index;
    minor = 0;
    while (version_text[index] >= '0' && version_text[index] <= '9') {
        minor = (minor * 10) + (version_text[index] - '0');
        ++index;
    }

    if (major_out != NULL && *major_out <= 0) {
        return 0;
    }
    if (minor_out != NULL) {
        *minor_out = minor;
    }
    return 1;
}

static void screensave_gl21_init_capability_bundle(screensave_gl21_capability_bundle *bundle)
{
    if (bundle == NULL) {
        return;
    }

    ZeroMemory(bundle, sizeof(*bundle));
    bundle->required_flags =
        SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_ATTRIBS |
        SCREENSAVE_GL21_PRIVATE_CAP_ADVANCED_CONTEXT |
        SCREENSAVE_GL21_PRIVATE_CAP_COMPAT_PROFILE |
        SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_21_PLUS;
    bundle->preferred_flags =
        SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_32_PLUS |
        SCREENSAVE_GL21_PRIVATE_CAP_VBO |
        SCREENSAVE_GL21_PRIVATE_CAP_FBO |
        SCREENSAVE_GL21_PRIVATE_CAP_TEXTURE_RECT;
}

static void screensave_gl21_finalize_capability_bundle(screensave_gl21_state *state)
{
    screensave_gl21_capability_bundle *bundle;

    if (state == NULL) {
        return;
    }

    bundle = &state->caps.bundle;
    bundle->available_flags = state->caps.private_flags;
    bundle->missing_required_flags =
        bundle->required_flags & ~bundle->available_flags;
    bundle->satisfied = bundle->missing_required_flags == 0UL;
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
    int double_buffered;
    int support_gdi;
    int generic_format;
    int rgba_bits;
    int depth_bits;
    int major_version;
    int minor_version;

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
    rgba_bits = state->caps.rgba_bits;
    depth_bits = state->caps.depth_bits;

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
    state->caps.rgba_bits = rgba_bits;
    state->caps.depth_bits = depth_bits;
    state->caps.advanced_context = 1;
    state->caps.compatibility_profile = 1;
    screensave_gl21_init_capability_bundle(&state->caps.bundle);
    state->caps.private_flags =
        SCREENSAVE_GL21_PRIVATE_CAP_ADVANCED_CONTEXT |
        SCREENSAVE_GL21_PRIVATE_CAP_COMPAT_PROFILE;
    if (state->create_context_attribs != NULL) {
        state->caps.private_flags |= SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_ATTRIBS;
    }
    lstrcpynA(state->caps.vendor, (const char *)vendor, sizeof(state->caps.vendor));
    lstrcpynA(state->caps.renderer, (const char *)renderer, sizeof(state->caps.renderer));
    lstrcpynA(state->caps.version, (const char *)version, sizeof(state->caps.version));

    major_version = 0;
    minor_version = 0;
    if (
        !screensave_gl21_parse_version_string(
            state->caps.version,
            &major_version,
            &minor_version
        )
    ) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl21-capture-string-failed";
        }
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6706UL,
            "gl21_caps",
            "The advanced GL path could not parse the renderer version string."
        );
        return 0;
    }

    state->caps.major_version = major_version;
    state->caps.minor_version = minor_version;
    if (major_version > 2 || (major_version == 2 && minor_version >= 1)) {
        state->caps.private_flags |= SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_21_PLUS;
    }
    if (major_version > 3 || (major_version == 3 && minor_version >= 2)) {
        state->caps.private_flags |= SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_32_PLUS;
    }

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

    screensave_gl21_finalize_capability_bundle(state);
    if (!state->caps.bundle.satisfied) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl21-version-too-old";
        }
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6703UL,
            "gl21_caps",
            "The GL21 advanced-lane capability bundle is incomplete and must degrade to a lower tier."
        );
        return 0;
    }

    if ((state->caps.private_flags & SCREENSAVE_GL21_PRIVATE_CAP_CONTEXT_32_PLUS) != 0UL) {
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_INFO,
            6704UL,
            "gl21_caps",
            "The GL21 lane is using a 3.x compatibility context while preserving the bounded advanced-tier contract."
        );
    }
    if (!state->caps.has_fbo || !state->caps.has_texture_rectangle) {
        screensave_gl21_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_INFO,
            6705UL,
            "gl21_caps",
            "The GL21 lane is active with a bounded optional-capability bundle; later advanced work must keep explicit degrade paths."
        );
    }

    screensave_gl21_capture_refresh(state);
    screensave_gl21_emit_diag(
        state,
        SCREENSAVE_DIAG_LEVEL_INFO,
        6702UL,
        "gl21_caps",
        "The GL21 path captured renderer capability details successfully."
    );
    return 1;
}


