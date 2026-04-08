#include <string.h>

#include "screensave/private/backend_family_service.h"

static void screensave_backend_family_service_bind(
    screensave_backend_family_service *service,
    screensave_backend_family_service_provider provider,
    const char *provider_name,
    unsigned long state_flags
)
{
    if (service == NULL) {
        return;
    }

    service->provider = provider;
    service->binding.provider_name = provider_name;
    service->binding.flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT |
        SCREENSAVE_SERVICE_SEAM_FLAG_CURRENT_PRIMARY |
        SCREENSAVE_SERVICE_SEAM_FLAG_BOUND;
    service->binding.capability_flags =
        SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_CURRENT_FAMILY |
        SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_OPTIONAL_ADAPTER |
        SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_DYNAMIC_DISCOVER |
        SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_FALLBACK_CHAIN |
        SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_NON_GL_SLOT;
    service->binding.state_flags = state_flags;
}

void screensave_backend_family_service_init(screensave_backend_family_service *service)
{
    if (service == NULL) {
        return;
    }

    memset(service, 0, sizeof(*service));
    service->binding.service_name = "backend-family";
    service->binding.provider_name = "unbound";
    service->binding.flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT;
}

void screensave_backend_family_service_bind_private_default(
    screensave_backend_family_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps
)
{
    (void)active_band;
    (void)backend_caps;

    screensave_backend_family_service_init(service);
    if (service == NULL) {
        return;
    }

    switch (backend_kind) {
    case SCREENSAVE_BACKEND_KIND_GDI:
        screensave_backend_family_service_bind(
            service,
            SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_SOFTWARE_CORE,
            "software-floor-core",
            SCREENSAVE_BACKEND_FAMILY_SERVICE_STATE_SOFTWARE
        );
        break;

    case SCREENSAVE_BACKEND_KIND_GL11:
    case SCREENSAVE_BACKEND_KIND_GL21:
    case SCREENSAVE_BACKEND_KIND_GL33:
    case SCREENSAVE_BACKEND_KIND_GL46:
        screensave_backend_family_service_bind(
            service,
            SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_OPENGL_CORE,
            "gl-ladder-core",
            SCREENSAVE_BACKEND_FAMILY_SERVICE_STATE_OPENGL
        );
        break;

    case SCREENSAVE_BACKEND_KIND_NULL:
        screensave_backend_family_service_bind(
            service,
            SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_NULL_SAFETY,
            "null-safety-core",
            SCREENSAVE_BACKEND_FAMILY_SERVICE_STATE_NULL
        );
        break;

    case SCREENSAVE_BACKEND_KIND_UNKNOWN:
    default:
        break;
    }
}
