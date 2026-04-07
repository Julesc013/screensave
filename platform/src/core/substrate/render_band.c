#include "screensave/private/render_band.h"

const char *screensave_render_band_name(screensave_render_band band)
{
    switch (band) {
    case SCREENSAVE_RENDER_BAND_UNIVERSAL:
        return "universal";

    case SCREENSAVE_RENDER_BAND_COMPAT:
        return "compat";

    case SCREENSAVE_RENDER_BAND_ADVANCED:
        return "advanced";

    case SCREENSAVE_RENDER_BAND_MODERN:
        return "modern";

    case SCREENSAVE_RENDER_BAND_PREMIUM:
        return "premium";

    case SCREENSAVE_RENDER_BAND_UNKNOWN:
    default:
        return "unknown";
    }
}

screensave_render_band screensave_render_band_from_renderer_kind(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return SCREENSAVE_RENDER_BAND_UNIVERSAL;

    case SCREENSAVE_RENDERER_KIND_GL11:
        return SCREENSAVE_RENDER_BAND_COMPAT;

    case SCREENSAVE_RENDERER_KIND_GL21:
        return SCREENSAVE_RENDER_BAND_ADVANCED;

    case SCREENSAVE_RENDERER_KIND_GL33:
        return SCREENSAVE_RENDER_BAND_MODERN;

    case SCREENSAVE_RENDERER_KIND_GL46:
        return SCREENSAVE_RENDER_BAND_PREMIUM;

    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return SCREENSAVE_RENDER_BAND_UNKNOWN;
    }
}

screensave_render_band screensave_render_band_ceiling_for_request(screensave_renderer_kind requested_kind)
{
    if (requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN) {
        return SCREENSAVE_RENDER_BAND_PREMIUM;
    }

    return screensave_render_band_from_renderer_kind(requested_kind);
}
