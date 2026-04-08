#include <string.h>

#include "screensave/private/present_service.h"

static void screensave_present_service_bind(
    screensave_present_service *service,
    screensave_present_service_provider provider,
    const char *provider_name,
    unsigned long capability_flags,
    unsigned long state_flags,
    unsigned long seam_flags
)
{
    if (service == NULL) {
        return;
    }

    service->provider = provider;
    service->binding.provider_name = provider_name;
    service->binding.flags = seam_flags;
    service->binding.capability_flags = capability_flags;
    service->binding.state_flags = state_flags;
}

void screensave_present_service_init(screensave_present_service *service)
{
    if (service == NULL) {
        return;
    }

    memset(service, 0, sizeof(*service));
    service->binding.service_name = "present";
    service->binding.provider_name = "unbound";
    service->binding.flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT;
}

void screensave_present_service_bind_private_default(
    screensave_present_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps,
    const screensave_present_path *present_path
)
{
    unsigned long capability_flags;
    unsigned long state_flags;
    unsigned long seam_flags;

    (void)backend_kind;
    (void)active_band;
    (void)backend_caps;

    screensave_present_service_init(service);
    if (service == NULL || present_path == NULL) {
        return;
    }

    capability_flags =
        SCREENSAVE_PRESENT_SERVICE_CAP_SCALING_POLICY |
        SCREENSAVE_PRESENT_SERVICE_CAP_CAPTURE_TAP |
        SCREENSAVE_PRESENT_SERVICE_CAP_OUTPUT_POLICY_SLOT;
    state_flags = 0UL;
    if ((present_path->flags & SCREENSAVE_PRESENT_PATH_FLAG_USES_WINDOW_DC) != 0UL) {
        capability_flags |= SCREENSAVE_PRESENT_SERVICE_CAP_WINDOW_TARGET;
        state_flags |= SCREENSAVE_PRESENT_SERVICE_STATE_WINDOW_ROUTED;
    }
    if ((present_path->flags & SCREENSAVE_PRESENT_PATH_FLAG_ACCEPTS_EXTERNAL_DC) != 0UL) {
        capability_flags |= SCREENSAVE_PRESENT_SERVICE_CAP_EXTERNAL_DC;
        state_flags |= SCREENSAVE_PRESENT_SERVICE_STATE_EXTERNAL_DC;
    }
    if ((present_path->flags & SCREENSAVE_PRESENT_PATH_FLAG_SWAP_BUFFERS) != 0UL) {
        capability_flags |= SCREENSAVE_PRESENT_SERVICE_CAP_SWAP_PRESENT;
    }

    seam_flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT |
        SCREENSAVE_SERVICE_SEAM_FLAG_CURRENT_PRIMARY |
        SCREENSAVE_SERVICE_SEAM_FLAG_BOUND;

    if ((present_path->flags & SCREENSAVE_PRESENT_PATH_FLAG_NOOP_PRESENT) != 0UL) {
        state_flags |= SCREENSAVE_PRESENT_SERVICE_STATE_NOOP_PRESENT;
        screensave_present_service_bind(
            service,
            SCREENSAVE_PRESENT_SERVICE_PROVIDER_NOOP_OUTPUT_CORE,
            "noop-output-core",
            capability_flags,
            state_flags,
            seam_flags
        );
        return;
    }

    screensave_present_service_bind(
        service,
        SCREENSAVE_PRESENT_SERVICE_PROVIDER_OUTPUT_POLICY_CORE,
        "output-policy-core",
        capability_flags,
        state_flags,
        seam_flags
    );
}
