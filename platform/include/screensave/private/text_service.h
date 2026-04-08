#ifndef SCREENSAVE_PRIVATE_TEXT_SERVICE_H
#define SCREENSAVE_PRIVATE_TEXT_SERVICE_H

#include "screensave/private/backend_caps.h"
#include "screensave/private/service_common.h"

typedef enum screensave_text_service_provider_tag {
    SCREENSAVE_TEXT_SERVICE_PROVIDER_NONE = 0,
    SCREENSAVE_TEXT_SERVICE_PROVIDER_DEFERRED_SLOT = 1
} screensave_text_service_provider;

#define SCREENSAVE_TEXT_SERVICE_CAP_OVERLAY_TEXT      0x00000001UL
#define SCREENSAVE_TEXT_SERVICE_CAP_SURFACE_TEXT      0x00000002UL
#define SCREENSAVE_TEXT_SERVICE_CAP_LAYOUT_ADAPTER    0x00000004UL
#define SCREENSAVE_TEXT_SERVICE_CAP_DISCOVERY_ADAPTER 0x00000008UL

#define SCREENSAVE_TEXT_SERVICE_STATE_DEFERRED 0x00000001UL

typedef struct screensave_text_service_tag {
    screensave_service_binding binding;
    screensave_text_service_provider provider;
} screensave_text_service;

void screensave_text_service_init(screensave_text_service *service);
void screensave_text_service_bind_private_default(
    screensave_text_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps
);

#endif /* SCREENSAVE_PRIVATE_TEXT_SERVICE_H */
