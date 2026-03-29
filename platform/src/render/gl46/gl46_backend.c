#include "gl46_internal.h"

int screensave_gl46_renderer_create(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    (void)target_window;
    (void)drawable_size;

    if (renderer_out != NULL) {
        *renderer_out = NULL;
    }

    if (failure_reason_out != NULL) {
        *failure_reason_out = "gl46-placeholder-unavailable";
    }

    if (diagnostics != NULL) {
        screensave_diag_emit(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            SCREENSAVE_DIAG_DOMAIN_RENDERER,
            6902UL,
            "gl46_backend",
            "The GL46 renderer tier is only a placeholder in this build."
        );
    }

    return 0;
}
