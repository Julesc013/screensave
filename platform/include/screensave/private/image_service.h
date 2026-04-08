#ifndef SCREENSAVE_PRIVATE_IMAGE_SERVICE_H
#define SCREENSAVE_PRIVATE_IMAGE_SERVICE_H

#include "screensave/private/backend_caps.h"
#include "screensave/private/service_common.h"

typedef enum screensave_image_service_provider_tag {
    SCREENSAVE_IMAGE_SERVICE_PROVIDER_NONE = 0,
    SCREENSAVE_IMAGE_SERVICE_PROVIDER_BITMAP_CORE = 1,
    SCREENSAVE_IMAGE_SERVICE_PROVIDER_TEXTURE_BRIDGE = 2,
    SCREENSAVE_IMAGE_SERVICE_PROVIDER_DEFERRED_ADAPTER = 3
} screensave_image_service_provider;

#define SCREENSAVE_IMAGE_SERVICE_CAP_BITMAP_VIEW        0x00000001UL
#define SCREENSAVE_IMAGE_SERVICE_CAP_SOFTWARE_SURFACE   0x00000002UL
#define SCREENSAVE_IMAGE_SERVICE_CAP_GPU_UPLOAD_BRIDGE  0x00000004UL
#define SCREENSAVE_IMAGE_SERVICE_CAP_INGEST_ADAPTER     0x00000008UL
#define SCREENSAVE_IMAGE_SERVICE_CAP_THUMBNAIL_ADAPTER  0x00000010UL

#define SCREENSAVE_IMAGE_SERVICE_STATE_SOFTWARE_CORE 0x00000001UL
#define SCREENSAVE_IMAGE_SERVICE_STATE_GPU_CORE      0x00000002UL

typedef struct screensave_image_service_tag {
    screensave_service_binding binding;
    screensave_image_service_provider provider;
} screensave_image_service;

void screensave_image_service_init(screensave_image_service *service);
void screensave_image_service_bind_private_default(
    screensave_image_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps
);

#endif /* SCREENSAVE_PRIVATE_IMAGE_SERVICE_H */
