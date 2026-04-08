#include "gl11_internal.h"

static void screensave_gl11_capture_copy_text(
    char *buffer,
    unsigned int buffer_size,
    const char *text
)
{
    unsigned int index;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return;
    }

    index = 0U;
    while (text[index] != '\0' && index + 1U < buffer_size) {
        buffer[index] = text[index];
        ++index;
    }
    buffer[index] = '\0';
}

static void screensave_gl11_capture_update_detail_text(screensave_gl11_state *state)
{
    char version_text[24];

    if (state == NULL) {
        return;
    }

    version_text[0] = '\0';
    screensave_gl11_capture_copy_text(
        version_text,
        sizeof(version_text),
        state->caps.version[0] != '\0' ? state->caps.version : "unknown"
    );

    wsprintfA(
        state->detail_text,
        "gl=%s pf=%d dbl=%d gdi=%d gen=%d p=%lu s=%lu f=%lu",
        version_text,
        state->pixel_format,
        state->caps.double_buffered ? 1 : 0,
        state->caps.support_gdi ? 1 : 0,
        state->caps.generic_format ? 1 : 0,
        state->present_count,
        state->swap_count,
        state->flush_count
    );
}

void screensave_gl11_capture_refresh(screensave_gl11_state *state)
{
    if (state == NULL) {
        return;
    }

    screensave_gl11_capture_update_detail_text(state);
}
