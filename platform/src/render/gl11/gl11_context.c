#include "gl11_internal.h"

static void screensave_gl11_normalize_size(const screensave_sizei *source, screensave_sizei *target)
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

static int screensave_gl11_choose_or_describe_pixel_format(
    screensave_gl11_state *state,
    PIXELFORMATDESCRIPTOR *actual_format,
    const char **failure_reason_out
)
{
    PIXELFORMATDESCRIPTOR requested_format;
    DWORD requested_flags;
    int pixel_format;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    pixel_format = GetPixelFormat(state->window_dc);
    if (pixel_format == 0) {
        ZeroMemory(&requested_format, sizeof(requested_format));
        requested_format.nSize = sizeof(requested_format);
        requested_format.nVersion = 1;
        requested_flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        requested_format.dwFlags = requested_flags;
        requested_format.iPixelType = PFD_TYPE_RGBA;
        requested_format.cColorBits = 24;
        requested_format.cAlphaBits = 8;
        requested_format.iLayerType = PFD_MAIN_PLANE;

        pixel_format = ChoosePixelFormat(state->window_dc, &requested_format);
        if (pixel_format == 0) {
            requested_flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
            requested_format.dwFlags = requested_flags;
            pixel_format = ChoosePixelFormat(state->window_dc, &requested_format);
            if (pixel_format == 0) {
                if (failure_reason_out != NULL) {
                    *failure_reason_out = "gl11-choose-pixel-format-failed";
                }
                screensave_gl11_emit_diag(
                    state,
                    SCREENSAVE_DIAG_LEVEL_ERROR,
                    6201UL,
                    "gl11_context",
                    "ChoosePixelFormat failed for the GL11 window."
                );
                return 0;
            }

            screensave_gl11_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_WARNING,
                6207UL,
                "gl11_context",
                "GL11 is using a single-buffered pixel format because no double-buffered format was available."
            );
        }

        if (!SetPixelFormat(state->window_dc, pixel_format, &requested_format)) {
            if (failure_reason_out != NULL) {
                *failure_reason_out = "gl11-set-pixel-format-failed";
            }
            screensave_gl11_emit_diag(
                state,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                6202UL,
                "gl11_context",
                "SetPixelFormat failed for the GL11 window."
            );
            return 0;
        }
    }

    ZeroMemory(actual_format, sizeof(*actual_format));
    actual_format->nSize = sizeof(*actual_format);
    actual_format->nVersion = 1;
    if (!DescribePixelFormat(state->window_dc, pixel_format, sizeof(*actual_format), actual_format)) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-describe-pixel-format-failed";
        }
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6203UL,
            "gl11_context",
            "DescribePixelFormat failed for the GL11 window."
        );
        return 0;
    }

    if ((actual_format->dwFlags & PFD_SUPPORT_OPENGL) == 0U) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-pixel-format-without-opengl";
        }
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6204UL,
            "gl11_context",
            "The window pixel format does not support OpenGL."
        );
        return 0;
    }

    state->pixel_format = pixel_format;
    state->caps.double_buffered = (actual_format->dwFlags & PFD_DOUBLEBUFFER) != 0U;
    state->caps.support_gdi = (actual_format->dwFlags & PFD_SUPPORT_GDI) != 0U;
    state->caps.generic_format = (actual_format->dwFlags & PFD_GENERIC_FORMAT) != 0U;
    state->caps.rgba_bits = (int)actual_format->cColorBits;
    state->caps.depth_bits = (int)actual_format->cDepthBits;
    return 1;
}

int screensave_gl11_context_create(
    screensave_gl11_state *state,
    const screensave_sizei *drawable_size,
    const char **failure_reason_out
)
{
    PIXELFORMATDESCRIPTOR actual_format;

    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    if (state == NULL || state->target_window == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-invalid-window";
        }
        return 0;
    }

    state->window_dc = GetDC(state->target_window);
    if (state->window_dc == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-getdc-failed";
        }
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6205UL,
            "gl11_context",
            "GetDC failed for the GL11 window."
        );
        return 0;
    }

    if (!screensave_gl11_choose_or_describe_pixel_format(state, &actual_format, failure_reason_out)) {
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
        return 0;
    }

    state->gl_context = wglCreateContext(state->window_dc);
    if (state->gl_context == NULL) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gl11-create-context-failed";
        }
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6206UL,
            "gl11_context",
            "wglCreateContext failed for the GL11 window."
        );
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
        return 0;
    }

    screensave_gl11_normalize_size(drawable_size, &state->drawable_size);
    return 1;
}

void screensave_gl11_context_destroy(screensave_gl11_state *state)
{
    if (state == NULL) {
        return;
    }

    if (wglGetCurrentContext() == state->gl_context) {
        wglMakeCurrent(NULL, NULL);
    }

    if (state->gl_context != NULL) {
        wglDeleteContext(state->gl_context);
        state->gl_context = NULL;
    }

    if (state->window_dc != NULL) {
        ReleaseDC(state->target_window, state->window_dc);
        state->window_dc = NULL;
    }
}

int screensave_gl11_context_make_current(
    screensave_gl11_state *state,
    const char *origin,
    unsigned long code
)
{
    if (state == NULL || state->window_dc == NULL || state->gl_context == NULL) {
        return 0;
    }

    if (!wglMakeCurrent(state->window_dc, state->gl_context)) {
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            code,
            origin,
            "wglMakeCurrent failed for the GL11 renderer."
        );
        return 0;
    }

    return 1;
}

void screensave_gl11_context_release_current(screensave_gl11_state *state)
{
    if (state == NULL) {
        return;
    }

    if (wglGetCurrentContext() == state->gl_context) {
        wglMakeCurrent(NULL, NULL);
    }
}
