#include "screensave/private/service_registry.h"

void screensave_service_registry_bind_private_defaults(
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

    seams->active_band = active_band;
    screensave_image_service_bind_private_default(
        &seams->image,
        backend_kind,
        active_band,
        backend_caps
    );
    screensave_text_service_bind_private_default(
        &seams->text,
        backend_kind,
        active_band,
        backend_caps
    );
    screensave_present_service_bind_private_default(
        &seams->present,
        backend_kind,
        active_band,
        backend_caps,
        present_path
    );
    screensave_backend_family_service_bind_private_default(
        &seams->backend_family,
        backend_kind,
        active_band,
        backend_caps
    );
}
