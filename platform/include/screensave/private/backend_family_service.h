#ifndef SCREENSAVE_PRIVATE_BACKEND_FAMILY_SERVICE_H
#define SCREENSAVE_PRIVATE_BACKEND_FAMILY_SERVICE_H

#include "screensave/private/backend_caps.h"
#include "screensave/private/service_common.h"

typedef enum screensave_backend_family_service_provider_tag {
    SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_NONE = 0,
    SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_SOFTWARE_CORE = 1,
    SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_OPENGL_CORE = 2,
    SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_NULL_SAFETY = 3,
    SCREENSAVE_BACKEND_FAMILY_SERVICE_PROVIDER_FUTURE_ADAPTER = 4
} screensave_backend_family_service_provider;

#define SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_CURRENT_FAMILY   0x00000001UL
#define SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_OPTIONAL_ADAPTER 0x00000002UL
#define SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_DYNAMIC_DISCOVER 0x00000004UL
#define SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_FALLBACK_CHAIN   0x00000008UL
#define SCREENSAVE_BACKEND_FAMILY_SERVICE_CAP_NON_GL_SLOT      0x00000010UL

#define SCREENSAVE_BACKEND_FAMILY_SERVICE_STATE_SOFTWARE 0x00000001UL
#define SCREENSAVE_BACKEND_FAMILY_SERVICE_STATE_OPENGL   0x00000002UL
#define SCREENSAVE_BACKEND_FAMILY_SERVICE_STATE_NULL     0x00000004UL

typedef struct screensave_backend_family_service_tag {
    screensave_service_binding binding;
    screensave_backend_family_service_provider provider;
} screensave_backend_family_service;

void screensave_backend_family_service_init(screensave_backend_family_service *service);
void screensave_backend_family_service_bind_private_default(
    screensave_backend_family_service *service,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps
);

#endif /* SCREENSAVE_PRIVATE_BACKEND_FAMILY_SERVICE_H */
