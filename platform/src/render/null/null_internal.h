#ifndef SCREENSAVE_NULL_INTERNAL_H
#define SCREENSAVE_NULL_INTERNAL_H

#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"

int screensave_null_renderer_create(
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
);
int screensave_null_renderer_resize(screensave_renderer *renderer, const screensave_sizei *drawable_size);
void screensave_null_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc);
void screensave_null_renderer_clear_present_dc(screensave_renderer *renderer);

#endif /* SCREENSAVE_NULL_INTERNAL_H */
