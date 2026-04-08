#include "gl46_internal.h"

static void screensave_gl46_normalize_size(const screensave_sizei *source, screensave_sizei *target)
{
    target->width = 1;
    target->height = 1;

    if (source != NULL) {
        if (source->width > 0) {
            target->width = source->width;
        }
        if (source->height > 0) {
            target->height = source->height;
        }
    }
}

static void screensave_gl46_context_reset_runtime(screensave_gl46_state *state)
{
    screensave_diag_context *diagnostics;
    HWND target_window;

    if (state == NULL) {
        return;
    }

    diagnostics = state->diagnostics;
    target_window = state->target_window;
    ZeroMemory(state, sizeof(*state));
    state->diagnostics = diagnostics;
    state->target_window = target_window;
}

static void screensave_gl46_context_release_window_dc(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    if (state->window_dc != NULL) {
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
    }
}

static void screensave_gl46_context_release_bootstrap_context(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    if (state->bootstrap_context != NULL) {
        if (wglGetCurrentContext() == state->bootstrap_context) {
            wglMakeCurrent(NULL, NULL);
        }
        wglDeleteContext(state->bootstrap_context);
        state->bootstrap_context = NULL;
    }
}

static void screensave_gl46_context_release_gl_context(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    if (state->gl_context != NULL) {
        if (wglGetCurrentContext() == state->gl_context) {
            wglMakeCurrent(NULL, NULL);
        }
        wglDeleteContext(state->gl_context);
        state->gl_context = NULL;
    }
}

static void screensave_gl46_context_cleanup_failed_create(screensave_gl46_state *state)
{
    screensave_gl46_context_release_gl_context(state);
    screensave_gl46_context_release_bootstrap_context(state);
    screensave_gl46_context_release_window_dc(state);
    if (state != NULL) {
        state->pixel_format = 0;
    }
}

static void screensave_gl46_context_capture_pixel_format_caps(
    screensave_gl46_state *state,
    const PIXELFORMATDESCRIPTOR *actual_format
)
{
    if (state == NULL || actual_format == NULL) {
        return;
    }

    state->caps.double_buffered = (actual_format->dwFlags & PFD_DOUBLEBUFFER) != 0U;
    state->caps.support_gdi = (actual_format->dwFlags & PFD_SUPPORT_GDI) != 0U;
    state->caps.generic_format = (actual_format->dwFlags & PFD_GENERIC_FORMAT) != 0U;
    state->caps.rgba_bits = (int)actual_format->cColorBits;
    state->caps.depth_bits = (int)actual_format->cDepthBits;
}

static int screensave_gl46_choose_or_describe_pixel_format(
    screensave_gl46_state *state,
    PIXELFORMATDESCRIPTOR *actual_format,
    const char **failure_reason_out
)
{
    PIXELFORMATDESCRIPTOR requested_format;
    int pixel_format;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    pixel_format = GetPixelFormat(state->window_dc);
    if (pixel_format == 0) {
        ZeroMemory(&requested_format, sizeof(requested_format));
        requested_format.nSize = sizeof(requested_format);
        requested_format.nVersion = 1;
        requested_format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        requested_format.iPixelType = PFD_TYPE_RGBA;
        requested_format.cColorBits = 24;
        requested_format.cAlphaBits = 8;
        requested_format.cDepthBits = 24;
        requested_format.iLayerType = PFD_MAIN_PLANE;

        pixel_format = ChoosePixelFormat(state->window_dc, &requested_format);
        if (pixel_format == 0) {
            requested_format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
            pixel_format = ChoosePixelFormat(state->window_dc, &requested_format);
            if (pixel_format == 0) {
                if (failure_reason_out != NULL) {
                    *failure_reason_out = "gl46-choose-pixel-format-failed";
                }
                screensave_gl46_emit_diag(
                    state,
                    SCREENSAVE_DIAG_LEVEL_ERROR,
                    7031UL,
                    "gl46_context",
                    "ChoosePixelFormat failed for the GL46 window."
                );
                return 0;
            }

            screensave_gl46_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_WARNING,
                7032UL,
                "gl46_context",
                "The GL46 premium lane is using a single-buffered pixel format."
            );
        }

        if (!SetPixelFormat(state->window_dc, pixel_format, &requested_format)) {
            if (failure_reason_out != NULL) {
                *failure_reason_out = "gl46-set-pixel-format-failed";
            }
            screensave_gl46_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                7033UL,
                "gl46_context",
                "SetPixelFormat failed for the GL46 window."
            );
            return 0;
        }
    } else {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_INFO,
            7034UL,
            "gl46_context",
            "GL46 is reusing the existing window pixel format."
        );
    }

    state->pixel_format = pixel_format;
    ZeroMemory(actual_format, sizeof(*actual_format));
    actual_format->nSize = sizeof(*actual_format);
    actual_format->nVersion = 1;
    if (!DescribePixelFormat(state->window_dc, pixel_format, sizeof(*actual_format), actual_format)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-describe-pixel-format-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7035UL,
            "gl46_context",
            "DescribePixelFormat failed for the GL46 window."
        );
        return 0;
    }

    if ((actual_format->dwFlags & PFD_SUPPORT_OPENGL) == 0U) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-pixel-format-without-opengl";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7036UL,
            "gl46_context",
            "The window pixel format does not support OpenGL."
        );
        return 0;
    }

    screensave_gl46_context_capture_pixel_format_caps(state, actual_format);
    if (state->caps.generic_format) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7037UL,
            "gl46_context",
            "GL46 is running on a generic pixel format; premium-lane acceleration may be limited."
        );
    }
    return 1;
}

static int screensave_gl46_load_create_context_attribs(screensave_gl46_state *state)
{
    PROC proc;

    proc = wglGetProcAddress("wglCreateContextAttribsARB");
    if (
        proc == NULL ||
        proc == (PROC)0x1 ||
        proc == (PROC)0x2 ||
        proc == (PROC)0x3 ||
        proc == (PROC)-1
    ) {
        state->create_context_attribs = NULL;
        return 0;
    }

    state->create_context_attribs = (PFNWGLCREATECONTEXTATTRIBSARBPROC)proc;
    return 1;
}

static int screensave_gl46_create_premium_context(screensave_gl46_state *state)
{
    static const int context_attributes_46[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        0
    };

    if (state->create_context_attribs == NULL) {
        return 0;
    }

    state->gl_context = state->create_context_attribs(state->window_dc, NULL, context_attributes_46);
    return state->gl_context != NULL;
}

int screensave_gl46_context_create(
    screensave_gl46_state *state,
    const screensave_sizei *drawable_size,
    const char **failure_reason_out
)
{
    PIXELFORMATDESCRIPTOR actual_format;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL || state->target_window == NULL || !IsWindow(state->target_window)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-invalid-window";
        }
        return 0;
    }

    screensave_gl46_context_reset_runtime(state);

    state->window_dc = GetDC(state->target_window);
    if (state->window_dc == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-getdc-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7038UL,
            "gl46_context",
            "GetDC failed for the GL46 window."
        );
        return 0;
    }

    if (
        !screensave_gl46_choose_or_describe_pixel_format(
            state,
            &actual_format,
            failure_reason_out
        )
    ) {
        screensave_gl46_context_cleanup_failed_create(state);
        return 0;
    }

    state->bootstrap_context = wglCreateContext(state->window_dc);
    if (state->bootstrap_context == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-bootstrap-context-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7039UL,
            "gl46_context",
            "wglCreateContext failed while bootstrapping the GL46 premium lane."
        );
        screensave_gl46_context_cleanup_failed_create(state);
        return 0;
    }

    if (!wglMakeCurrent(state->window_dc, state->bootstrap_context)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-bootstrap-make-current-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7040UL,
            "gl46_context",
            "wglMakeCurrent failed while bootstrapping the GL46 premium lane."
        );
        screensave_gl46_context_cleanup_failed_create(state);
        return 0;
    }

    if (!screensave_gl46_load_create_context_attribs(state)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-create-context-attribs-unavailable";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7041UL,
            "gl46_context",
            "wglCreateContextAttribsARB is unavailable; the GL46 premium lane cannot proceed."
        );
        screensave_gl46_context_cleanup_failed_create(state);
        return 0;
    }

    if (!screensave_gl46_create_premium_context(state)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-create-context-attribs-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7042UL,
            "gl46_context",
            "wglCreateContextAttribsARB failed while creating the GL46 compatibility context."
        );
        screensave_gl46_context_cleanup_failed_create(state);
        return 0;
    }

    if (!wglMakeCurrent(state->window_dc, state->gl_context)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl46-make-current-failed";
        }
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7043UL,
            "gl46_context",
            "wglMakeCurrent failed for the GL46 premium context."
        );
        screensave_gl46_context_cleanup_failed_create(state);
        return 0;
    }

    screensave_gl46_context_release_current(state);
    screensave_gl46_context_release_bootstrap_context(state);

    screensave_gl46_normalize_size(drawable_size, &state->drawable_size);
    screensave_gl46_capture_refresh(state);
    return 1;
}

void screensave_gl46_context_destroy(screensave_gl46_state *state)
{
    int context_current;

    if (state == NULL) {
        return;
    }

    context_current = 0;
    if (state->window_dc != NULL && state->gl_context != NULL) {
        if (wglGetCurrentContext() != state->gl_context) {
            context_current = wglMakeCurrent(state->window_dc, state->gl_context) != 0;
            if (!context_current) {
                screensave_gl46_emit_diag(
                    state,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    7044UL,
                    "gl46_context",
                    "wglMakeCurrent failed while preparing GL46 teardown; context-owned objects will rely on context destruction."
                );
            }
        } else {
            context_current = 1;
        }
    }

    if (context_current) {
        screensave_gl46_pipeline_destroy(state);
        screensave_gl46_context_release_current(state);
    }

    screensave_gl46_context_release_gl_context(state);
    screensave_gl46_context_release_bootstrap_context(state);
    screensave_gl46_context_release_window_dc(state);
}

int screensave_gl46_context_make_current(
    screensave_gl46_state *state,
    const char *origin,
    unsigned long code
)
{
    if (state == NULL || state->window_dc == NULL || state->gl_context == NULL) {
        return 0;
    }

    if (wglGetCurrentContext() == state->gl_context) {
        return 1;
    }

    if (!wglMakeCurrent(state->window_dc, state->gl_context)) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            code,
            origin,
            "wglMakeCurrent failed for the GL46 premium renderer."
        );
        return 0;
    }

    return 1;
}

void screensave_gl46_context_release_current(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    if (wglGetCurrentContext() == state->gl_context && !wglMakeCurrent(NULL, NULL)) {
        screensave_gl46_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7045UL,
            "gl46_context",
            "wglMakeCurrent(NULL, NULL) failed while releasing the GL46 context."
        );
    }
}
