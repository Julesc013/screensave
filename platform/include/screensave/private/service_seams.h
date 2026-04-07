#ifndef SCREENSAVE_PRIVATE_SERVICE_SEAMS_H
#define SCREENSAVE_PRIVATE_SERVICE_SEAMS_H

#include "screensave/private/render_band.h"

#define SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY     0x00000001UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL 0x00000002UL
#define SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND          0x00000004UL

typedef struct screensave_service_slot_tag {
    const char *service_name;
    unsigned long flags;
    void *service_state;
} screensave_service_slot;

typedef struct screensave_service_seams_tag {
    screensave_render_band active_band;
    screensave_service_slot image;
    screensave_service_slot text;
} screensave_service_seams;

void screensave_service_seams_init(screensave_service_seams *seams);
void screensave_service_seams_bind_private_defaults(
    screensave_service_seams *seams,
    screensave_render_band active_band
);

#endif /* SCREENSAVE_PRIVATE_SERVICE_SEAMS_H */
