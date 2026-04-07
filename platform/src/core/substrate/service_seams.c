#include <string.h>

#include "screensave/private/service_seams.h"

static void screensave_service_slot_reset(screensave_service_slot *slot, const char *service_name)
{
    if (slot == NULL) {
        return;
    }

    memset(slot, 0, sizeof(*slot));
    slot->service_name = service_name;
    slot->flags =
        SCREENSAVE_SERVICE_SEAM_FLAG_PRIVATE_ONLY |
        SCREENSAVE_SERVICE_SEAM_FLAG_DYNAMIC_OPTIONAL |
        SCREENSAVE_SERVICE_SEAM_FLAG_UNBOUND;
}

void screensave_service_seams_init(screensave_service_seams *seams)
{
    if (seams == NULL) {
        return;
    }

    memset(seams, 0, sizeof(*seams));
    seams->active_band = SCREENSAVE_RENDER_BAND_UNKNOWN;
    screensave_service_slot_reset(&seams->image, "image");
    screensave_service_slot_reset(&seams->text, "text");
}

void screensave_service_seams_bind_private_defaults(
    screensave_service_seams *seams,
    screensave_render_band active_band
)
{
    if (seams == NULL) {
        return;
    }

    screensave_service_seams_init(seams);
    seams->active_band = active_band;
}
