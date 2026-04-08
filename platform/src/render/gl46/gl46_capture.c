#include "gl46_internal.h"

static void screensave_gl46_capture_update_detail_text(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    wsprintfA(
        state->detail_text,
        "gl=%d.%d pf=%d rgba=%d depth=%d dbl=%d req=%d tex=%d samp=%d bsto=%d fbo=%d p=%lu s=%lu f=%lu",
        state->caps.major_version,
        state->caps.minor_version,
        state->pixel_format,
        state->caps.rgba_bits,
        state->caps.depth_bits,
        state->caps.double_buffered ? 1 : 0,
        state->caps.bundle.satisfied ? 1 : 0,
        state->caps.has_texture_storage ? 1 : 0,
        state->caps.has_sampler_objects ? 1 : 0,
        state->caps.has_buffer_storage ? 1 : 0,
        state->caps.has_fbo ? 1 : 0,
        state->present_count,
        state->swap_count,
        state->flush_count
    );
}

void screensave_gl46_capture_refresh(screensave_gl46_state *state)
{
    if (state == NULL) {
        return;
    }

    screensave_gl46_capture_update_detail_text(state);
}
