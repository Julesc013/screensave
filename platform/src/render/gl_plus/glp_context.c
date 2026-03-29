#include "glp_internal.h"

static void screensave_glp_normalize_size(const screensave_sizei *source, screensave_sizei *target)
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

static int screensave_glp_choose_pixel_format(
    screensave_glp_state *state,
    const char **failure_reason_out
)
{
    PIXELFORMATDESCRIPTOR actual_format;
    PIXELFORMATDESCRIPTOR requested_format;
    int pixel_format;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

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
                *failure_reason_out = "glp-choose-pixel-format-failed";
            }
            screensave_glp_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6703UL,
                "glp_context",
                "ChoosePixelFormat failed for the advanced GL window."
            );
            return 0;
        }

        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6704UL,
            "glp_context",
            "The advanced GL path is using a single-buffered pixel format."
        );
    }

    if (!SetPixelFormat(state->window_dc, pixel_format, &requested_format)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-set-pixel-format-failed";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6705UL,
            "glp_context",
            "SetPixelFormat failed for the advanced GL window."
        );
        return 0;
    }

    state->pixel_format = pixel_format;
    ZeroMemory(&actual_format, sizeof(actual_format));
    actual_format.nSize = sizeof(actual_format);
    actual_format.nVersion = 1;
    if (DescribePixelFormat(state->window_dc, pixel_format, sizeof(actual_format), &actual_format)) {
        state->caps.double_buffered = (actual_format.dwFlags & PFD_DOUBLEBUFFER) != 0U;
        state->caps.support_gdi = (actual_format.dwFlags & PFD_SUPPORT_GDI) != 0U;
        state->caps.generic_format = (actual_format.dwFlags & PFD_GENERIC_FORMAT) != 0U;
    }
    return 1;
}

static int screensave_glp_load_create_context_attribs(screensave_glp_state *state)
{
    PROC proc;

    proc = wglGetProcAddress("wglCreateContextAttribsARB");
    if (proc == NULL) {
        state->create_context_attribs = NULL;
        return 0;
    }

    state->create_context_attribs = (PFNWGLCREATECONTEXTATTRIBSARBPROC)proc;
    return 1;
}

static int screensave_glp_create_advanced_context(screensave_glp_state *state)
{
    static const int context_attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        0
    };

    if (state->create_context_attribs == NULL) {
        return 0;
    }

    state->gl_context = state->create_context_attribs(state->window_dc, NULL, context_attributes);
    if (state->gl_context == NULL) {
        return 0;
    }

    return 1;
}

int screensave_glp_context_create(
    screensave_glp_state *state,
    const screensave_sizei *drawable_size,
    const char **failure_reason_out
)
{
    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL || state->target_window == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-invalid-window";
        }
        return 0;
    }

    state->window_dc = GetDC(state->target_window);
    if (state->window_dc == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-getdc-failed";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6706UL,
            "glp_context",
            "GetDC failed for the advanced GL window."
        );
        return 0;
    }

    if (!screensave_glp_choose_pixel_format(state, failure_reason_out)) {
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
        return 0;
    }

    state->bootstrap_context = wglCreateContext(state->window_dc);
    if (state->bootstrap_context == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-bootstrap-context-failed";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6707UL,
            "glp_context",
            "wglCreateContext failed while bootstrapping the advanced GL path."
        );
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
        return 0;
    }

    if (!wglMakeCurrent(state->window_dc, state->bootstrap_context)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-bootstrap-make-current-failed";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6708UL,
            "glp_context",
            "wglMakeCurrent failed while bootstrapping the advanced GL path."
        );
        screensave_glp_context_destroy(state);
        return 0;
    }

    if (!screensave_glp_load_create_context_attribs(state)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-create-context-attribs-unavailable";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6709UL,
            "glp_context",
            "wglCreateContextAttribsARB is unavailable; the advanced GL path cannot proceed."
        );
        screensave_glp_context_destroy(state);
        return 0;
    }

    if (!screensave_glp_create_advanced_context(state)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-create-context-attribs-failed";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6710UL,
            "glp_context",
            "wglCreateContextAttribsARB failed while creating the advanced GL context."
        );
        screensave_glp_context_destroy(state);
        return 0;
    }

    if (!wglMakeCurrent(state->window_dc, state->gl_context)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "glp-make-current-failed";
        }
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6711UL,
            "glp_context",
            "wglMakeCurrent failed for the advanced GL context."
        );
        screensave_glp_context_destroy(state);
        return 0;
    }

    wglMakeCurrent(NULL, NULL);
    if (state->bootstrap_context != NULL) {
        wglDeleteContext(state->bootstrap_context);
        state->bootstrap_context = NULL;
    }

    screensave_glp_normalize_size(drawable_size, &state->drawable_size);
    return 1;
}

void screensave_glp_context_destroy(screensave_glp_state *state)
{
    if (state == NULL) {
        return;
    }

    if (
        wglGetCurrentContext() == state->gl_context ||
        wglGetCurrentContext() == state->bootstrap_context
    ) {
        wglMakeCurrent(NULL, NULL);
    }

    if (state->gl_context != NULL) {
        wglDeleteContext(state->gl_context);
        state->gl_context = NULL;
    }

    if (state->bootstrap_context != NULL) {
        wglDeleteContext(state->bootstrap_context);
        state->bootstrap_context = NULL;
    }

    if (state->window_dc != NULL) {
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
    }
}

int screensave_glp_context_make_current(
    screensave_glp_state *state,
    const char *origin,
    unsigned long code
)
{
    if (state == NULL || state->window_dc == NULL || state->gl_context == NULL) {
        return 0;
    }

    if (!wglMakeCurrent(state->window_dc, state->gl_context)) {
        screensave_glp_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            code,
            origin,
            "wglMakeCurrent failed for the advanced GL renderer."
        );
        return 0;
    }

    return 1;
}

void screensave_glp_context_release_current(screensave_glp_state *state)
{
    if (state == NULL) {
        return;
    }

    if (wglGetCurrentContext() == state->gl_context) {
        wglMakeCurrent(NULL, NULL);
    }
}
