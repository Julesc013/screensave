#include "screensave/v2/surface.h"

#include "screensave/v2/internal/validate.h"

ss_u32 ss_v2_surface_desc_is_valid(const ss_v2_surface_desc *surface)
{
    ss_u32 status;
    ss_u32 minimum_stride;

    status = ss_v2_check_prefix(surface, (ss_u32)sizeof(*surface), (ss_u32)sizeof(*surface), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (surface->width == 0U || surface->height == 0U || surface->pixels == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (surface->format != SS_V2_SURFACE_FORMAT_RGBA8 || surface->origin != SS_V2_SURFACE_ORIGIN_TOP_LEFT) {
        return SS_V2_STATUS_UNSUPPORTED;
    }
    if (surface->width > (0xffffffffU / 4U)) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    minimum_stride = surface->width * 4U;
    if (surface->stride_bytes < minimum_stride) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    return SS_V2_STATUS_OK;
}
