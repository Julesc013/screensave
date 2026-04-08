#include <string.h>

#include "screensave/private/service_registry.h"
#include "screensave/private/service_seams.h"

static int screensave_service_seams_append_text(char *buffer, int buffer_size, const char *text)
{
    int used;
    int added;

    if (buffer == NULL || text == NULL || buffer_size <= 0) {
        return 0;
    }

    used = lstrlenA(buffer);
    added = lstrlenA(text);
    if (used + added + 1 > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

void screensave_service_seams_init(screensave_service_seams *seams)
{
    if (seams == NULL) {
        return;
    }

    memset(seams, 0, sizeof(*seams));
    seams->active_band = SCREENSAVE_RENDER_BAND_UNKNOWN;
    screensave_image_service_init(&seams->image);
    screensave_text_service_init(&seams->text);
    screensave_present_service_init(&seams->present);
    screensave_backend_family_service_init(&seams->backend_family);
}

void screensave_service_seams_bind_private_defaults(
    screensave_service_seams *seams,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps,
    const screensave_present_path *present_path
)
{
    if (seams == NULL) {
        return;
    }

    screensave_service_seams_init(seams);
    screensave_service_registry_bind_private_defaults(
        seams,
        backend_kind,
        active_band,
        backend_caps,
        present_path
    );
}

int screensave_service_seams_build_summary(
    const screensave_service_seams *seams,
    char *buffer,
    int buffer_size
)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    if (seams == NULL) {
        return 0;
    }

    return
        screensave_service_seams_append_text(buffer, buffer_size, "image=") &&
        screensave_service_seams_append_text(
            buffer,
            buffer_size,
            seams->image.binding.provider_name != NULL
                ? seams->image.binding.provider_name
                : "unbound"
        ) &&
        screensave_service_seams_append_text(buffer, buffer_size, " text=") &&
        screensave_service_seams_append_text(
            buffer,
            buffer_size,
            seams->text.binding.provider_name != NULL
                ? seams->text.binding.provider_name
                : "unbound"
        ) &&
        screensave_service_seams_append_text(buffer, buffer_size, " present=") &&
        screensave_service_seams_append_text(
            buffer,
            buffer_size,
            seams->present.binding.provider_name != NULL
                ? seams->present.binding.provider_name
                : "unbound"
        ) &&
        screensave_service_seams_append_text(buffer, buffer_size, " family=") &&
        screensave_service_seams_append_text(
            buffer,
            buffer_size,
            seams->backend_family.binding.provider_name != NULL
                ? seams->backend_family.binding.provider_name
                : "unbound"
        );
}
