#ifndef SCREENSAVE_GL33_INTERNAL_H
#define SCREENSAVE_GL33_INTERNAL_H

#include <windows.h>

#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"

int screensave_gl33_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
);

#endif /* SCREENSAVE_GL33_INTERNAL_H */
