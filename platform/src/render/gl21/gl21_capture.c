#include "gl21_internal.h"

static void screensave_gl21_capture_update_detail_text(screensave_gl21_state *state)
{
    if (state == NULL) {
        return;
    }

    wsprintfA(
        state->detail_text,
        "gl=%d.%d pf=%d rgba=%d depth=%d dbl=%d req=%d vbo=%d fbo=%d texr=%d p=%lu s=%lu f=%lu",
        state->caps.major_version,
        state->caps.minor_version,
        state->pixel_format,
        state->caps.rgba_bits,
        state->caps.depth_bits,
        state->caps.double_buffered ? 1 : 0,
        state->caps.bundle.satisfied ? 1 : 0,
        state->caps.has_vbo ? 1 : 0,
        state->caps.has_fbo ? 1 : 0,
        state->caps.has_texture_rectangle ? 1 : 0,
        state->present_count,
        state->swap_count,
        state->flush_count
    );
}

void screensave_gl21_capture_refresh(screensave_gl21_state *state)
{
    if (state == NULL) {
        return;
    }

    screensave_gl21_capture_update_detail_text(state);
}
