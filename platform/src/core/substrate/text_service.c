#include <string.h>

#include "screensave/private/text_service.h"

void screensave_text_service_init(screensave_text_service *service)
{
    if (service == NULL) {
        return;
    }

    memset(service, 0, sizeof(*service));
    service->binding.service_name = "text";
    service->binding.provider_name = "unbound";
    service->binding.flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT;
}

void screensave_text_service_bind_private_default(
    screensave_text_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps
)
{
    (void)backend_kind;
    (void)active_band;
    (void)backend_caps;

    screensave_text_service_init(service);
    if (service == NULL) {
        return;
    }

    service->provider = SCREENSAVE_TEXT_SERVICE_PROVIDER_DEFERRED_SLOT;
    service->binding.provider_name = "deferred-text-slot";
    service->binding.flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT |
        SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND;
    service->binding.capability_flags =
        SCREENSAVE_TEXT_SERVICE_CAP_OVERLAY_TEXT |
        SCREENSAVE_TEXT_SERVICE_CAP_SURFACE_TEXT |
        SCREENSAVE_TEXT_SERVICE_CAP_LAYOUT_ADAPTER |
        SCREENSAVE_TEXT_SERVICE_CAP_DISCOVERY_ADAPTER;
    service->binding.state_flags = SCREENSAVE_TEXT_SERVICE_STATE_DEFERRED;
}
