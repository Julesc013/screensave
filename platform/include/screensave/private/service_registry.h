#ifndef SCREENSAVE_PRIVATE_SERVICE_REGISTRY_H
#define SCREENSAVE_PRIVATE_SERVICE_REGISTRY_H

#include "screensave/private/service_seams.h"

void screensave_service_registry_bind_private_defaults(
    screensave_service_seams *seams,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps,
    const screensave_present_path *present_path
);

#endif /* SCREENSAVE_PRIVATE_SERVICE_REGISTRY_H */
