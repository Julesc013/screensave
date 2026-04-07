#include "gdi_internal.h"

static void screensave_gdi_capture_update_detail_text(
    screensave_gdi_state *state,
    int include_signature
)
{
    if (state == NULL) {
        return;
    }

    if (include_signature) {
        wsprintfA(
            state->detail_text,
            "surface=%lu presents=%lu sig=%08lX",
            state->surface_generation,
            state->present_count,
            state->last_capture_signature
        );
        return;
    }

    wsprintfA(
        state->detail_text,
        "surface=%lu presents=%lu",
        state->surface_generation,
        state->present_count
    );
}

static unsigned long screensave_gdi_capture_signature(const screensave_gdi_surface *surface)
{
    const unsigned char *bytes;
    unsigned long byte_count;
    unsigned long hash;
    unsigned long index;

    if (
        surface == NULL ||
        surface->bits == NULL ||
        surface->stride_bytes <= 0 ||
        surface->size.height <= 0
    ) {
        return 0UL;
    }

    bytes = (const unsigned char *)surface->bits;
    byte_count = (unsigned long)surface->stride_bytes * (unsigned long)surface->size.height;
    hash = 2166136261UL;
    for (index = 0UL; index < byte_count; ++index) {
        hash ^= (unsigned long)bytes[index];
        hash *= 16777619UL;
    }

    return hash;
}

void screensave_gdi_capture_refresh(screensave_gdi_state *state)
{
    int emit_debug_capture;

    if (state == NULL) {
        return;
    }

    emit_debug_capture =
        state->diagnostics != NULL &&
        screensave_diag_should_emit(state->diagnostics, SCREENSAVE_DIAG_LEVEL_DEBUG);

    if (emit_debug_capture && state->surface_presented) {
        state->last_capture_signature = screensave_gdi_capture_signature(&state->surface);
        screensave_gdi_capture_update_detail_text(state, 1);
        screensave_gdi_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_DEBUG,
            5401UL,
            "gdi_capture",
            state->detail_text
        );
        return;
    }

    screensave_gdi_capture_update_detail_text(state, 0);
}
