#ifndef SCREENSAVE_PRIVATE_SERVICE_SEAMS_H
#define SCREENSAVE_PRIVATE_SERVICE_SEAMS_H

#include "screensave/private/backend_caps.h"
#include "screensave/private/backend_family_service.h"
#include "screensave/private/image_service.h"
#include "screensave/private/present_path.h"
#include "screensave/private/present_service.h"
#include "screensave/private/text_service.h"

typedef struct screensave_service_seams_tag {
    screensave_render_band active_band;
    screensave_image_service image;
    screensave_text_service text;
    screensave_present_service present;
    screensave_backend_family_service backend_family;
} screensave_service_seams;

void screensave_service_seams_init(screensave_service_seams *seams);
void screensave_service_seams_bind_private_defaults(
    screensave_service_seams *seams,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    const screensave_backend_caps *backend_caps,
    const screensave_present_path *present_path
);
int screensave_service_seams_build_summary(
    const screensave_service_seams *seams,
    char *buffer,
    int buffer_size
);

#endif /* SCREENSAVE_PRIVATE_SERVICE_SEAMS_H */
