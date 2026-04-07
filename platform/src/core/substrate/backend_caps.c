#include <string.h>

#include "screensave/private/backend_caps.h"

static unsigned long screensave_backend_public_capability_mask(void)
{
    return
        SCREENSAVE_RENDERER_CAP_CLEAR |
        SCREENSAVE_RENDERER_CAP_FILL_RECT |
        SCREENSAVE_RENDERER_CAP_FRAME_RECT |
        SCREENSAVE_RENDERER_CAP_LINE |
        SCREENSAVE_RENDERER_CAP_POLYLINE |
        SCREENSAVE_RENDERER_CAP_BITMAP;
}

static void screensave_backend_caps_copy_text(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int text_length;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (text == NULL || text[0] == '\0') {
        return;
    }

    text_length = (unsigned int)strlen(text);
    if (text_length >= buffer_size) {
        text_length = buffer_size - 1U;
    }

    memcpy(buffer, text, text_length);
    buffer[text_length] = '\0';
}

void screensave_backend_caps_init(
    screensave_backend_caps *caps,
    screensave_backend_kind backend_kind,
    screensave_render_band band
)
{
    if (caps == NULL) {
        return;
    }

    memset(caps, 0, sizeof(*caps));
    caps->backend_kind = backend_kind;
    caps->band = band;
}

void screensave_backend_caps_apply_renderer_info(
    screensave_backend_caps *caps,
    const screensave_renderer_info *info
)
{
    if (caps == NULL || info == NULL) {
        return;
    }

    caps->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_AVAILABLE;
    caps->public_capability_flags =
        info->capability_flags & screensave_backend_public_capability_mask();
    screensave_backend_caps_copy_text(caps->vendor, sizeof(caps->vendor), info->vendor_name);
    screensave_backend_caps_copy_text(caps->renderer, sizeof(caps->renderer), info->renderer_name);
    screensave_backend_caps_copy_text(caps->version, sizeof(caps->version), info->version_name);
}

void screensave_backend_caps_mark_failure(
    screensave_backend_caps *caps,
    const char *failure_reason,
    int placeholder_only
)
{
    if (caps == NULL) {
        return;
    }

    if (placeholder_only) {
        caps->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_PLACEHOLDER;
    }
    screensave_backend_caps_copy_text(
        caps->failure_reason,
        sizeof(caps->failure_reason),
        failure_reason
    );
}
