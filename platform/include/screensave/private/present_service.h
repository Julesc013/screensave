#ifndef SCREENSAVE_PRIVATE_PRESENT_SERVICE_H
#define SCREENSAVE_PRIVATE_PRESENT_SERVICE_H

#include "screensave/private/present_path.h"
#include "screensave/private/service_common.h"

typedef enum screensave_present_service_provider_tag {
    SCREENSAVE_PRESENT_SERVICE_PROVIDER_NONE = 0,
    SCREENSAVE_PRESENT_SERVICE_PROVIDER_OUTPUT_POLICY_CORE = 1,
    SCREENSAVE_PRESENT_SERVICE_PROVIDER_NOOP_OUTPUT_CORE = 2
} screensave_present_service_provider;

#define SCREENSAVE_PRESENT_SERVICE_CAP_WINDOW_TARGET      0x00000001UL
#define SCREENSAVE_PRESENT_SERVICE_CAP_EXTERNAL_DC        0x00000002UL
#define SCREENSAVE_PRESENT_SERVICE_CAP_SWAP_PRESENT       0x00000004UL
#define SCREENSAVE_PRESENT_SERVICE_CAP_SCALING_POLICY     0x00000008UL
#define SCREENSAVE_PRESENT_SERVICE_CAP_CAPTURE_TAP        0x00000010UL
#define SCREENSAVE_PRESENT_SERVICE_CAP_OUTPUT_POLICY_SLOT 0x00000020UL

#define SCREENSAVE_PRESENT_SERVICE_STATE_WINDOW_ROUTED 0x00000001UL
#define SCREENSAVE_PRESENT_SERVICE_STATE_EXTERNAL_DC   0x00000002UL
#define SCREENSAVE_PRESENT_SERVICE_STATE_NOOP_PRESENT  0x00000004UL

typedef struct screensave_present_service_tag {
    screensave_service_binding binding;
    screensave_present_service_provider provider;
} screensave_present_service;

void screensave_present_service_init(screensave_present_service *service);
void screensave_present_service_bind_private_default(
    screensave_present_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps,
    const screensave_present_path *present_path
);

#endif /* SCREENSAVE_PRIVATE_PRESENT_SERVICE_H */
