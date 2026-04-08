#include <string.h>

#include "screensave/private/image_service.h"

static void screensave_image_service_bind(
    screensave_image_service *service,
    screensave_image_service_provider provider,
    const char *provider_name,
    unsigned long capability_flags,
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
        SCREENSAVE_SERVICE_SEAM_FLAG_BOUND |
        SCREENSAVE_SERVICE_SEAM_FLAG_CURRENT_PRIMARY |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT;
    service->binding.capability_flags = capability_flags;
    service->binding.state_flags = state_flags;
}

void screensave_image_service_init(screensave_image_service *service)
{
    if (service == NULL) {
        return;
    }

    memset(service, 0, sizeof(*service));
    service->binding.service_name = "image";
    service->binding.provider_name = "unbound";
    service->binding.flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_DISCOVERY_READY |
        SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND |
        SCREENSAVE_SERVICE_SEAM_FLAG_ADAPTER_SLOT;
}

void screensave_image_service_bind_private_default(
    screensave_image_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps
)
{
    (void)active_band;
    (void)backend_caps;

    screensave_image_service_init(service);
    if (service == NULL) {
        return;
    }

    switch (backend_kind) {
    case SCREENSAVE_BACKEND_KIND_GDI:
        screensave_image_service_bind(
            service,
            SCREENSAVE_IMAGE_SERVICE_PROVIDER_BITMAP_CORE,
            "bitmap-surface-core",
            SCREENSAVE_IMAGE_SERVICE_CAP_BITMAP_VIEW |
                SCREENSAVE_IMAGE_SERVICE_CAP_SOFTWARE_SURFACE |
                SCREENSAVE_IMAGE_SERVICE_CAP_INGEST_ADAPTER |
                SCREENSAVE_IMAGE_SERVICE_CAP_THUMBNAIL_ADAPTER,
            SCREENSAVE_IMAGE_SERVICE_STATE_SOFTWARE_CORE
        );
        break;

    case SCREENSAVE_BACKEND_KIND_GL11:
    case SCREENSAVE_BACKEND_KIND_GL21:
    case SCREENSAVE_BACKEND_KIND_GL33:
    case SCREENSAVE_BACKEND_KIND_GL46:
        screensave_image_service_bind(
            service,
            SCREENSAVE_IMAGE_SERVICE_PROVIDER_TEXTURE_BRIDGE,
            "bitmap-upload-bridge",
            SCREENSAVE_IMAGE_SERVICE_CAP_BITMAP_VIEW |
                SCREENSAVE_IMAGE_SERVICE_CAP_GPU_UPLOAD_BRIDGE |
                SCREENSAVE_IMAGE_SERVICE_CAP_INGEST_ADAPTER |
                SCREENSAVE_IMAGE_SERVICE_CAP_THUMBNAIL_ADAPTER,
            SCREENSAVE_IMAGE_SERVICE_STATE_GPU_CORE
        );
        break;

    case SCREENSAVE_BACKEND_KIND_NULL:
    case SCREENSAVE_BACKEND_KIND_UNKNOWN:
    default:
        break;
    }
}
