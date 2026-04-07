#ifndef SCREENSAVE_PRIVATE_RENDER_BAND_H
#define SCREENSAVE_PRIVATE_RENDER_BAND_H

#include "screensave/renderer_api.h"

typedef enum screensave_render_band_tag {
    SCREENSAVE_RENDER_BAND_UNKNOWN = 0,
    SCREENSAVE_RENDER_BAND_UNIVERSAL = 1,
    SCREENSAVE_RENDER_BAND_COMPAT = 2,
    SCREENSAVE_RENDER_BAND_ADVANCED = 3,
    SCREENSAVE_RENDER_BAND_MODERN = 4,
    SCREENSAVE_RENDER_BAND_PREMIUM = 5
} screensave_render_band;

const char *screensave_render_band_name(screensave_render_band band);
screensave_render_band screensave_render_band_from_renderer_kind(screensave_renderer_kind kind);
screensave_render_band screensave_render_band_ceiling_for_request(screensave_renderer_kind requested_kind);

#endif /* SCREENSAVE_PRIVATE_RENDER_BAND_H */
