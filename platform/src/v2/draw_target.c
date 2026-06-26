#include "screensave/v2/draw.h"

#include "screensave/v2/internal/validate.h"

ss_u32 ss_v2_draw_ops_is_valid(const ss_v2_draw_ops *ops)
{
    ss_u32 status;

    status = ss_v2_check_prefix(ops, (ss_u32)sizeof(*ops), (ss_u32)sizeof(*ops), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if ((ops->capability_flags & SS_V2_DRAW_CAP_CLEAR) != 0U && ops->clear == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if ((ops->capability_flags & SS_V2_DRAW_CAP_FILL_RECT) != 0U && ops->fill_rect == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if ((ops->capability_flags & SS_V2_DRAW_CAP_FRAME_RECT) != 0U && ops->frame_rect == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if ((ops->capability_flags & SS_V2_DRAW_CAP_LINE) != 0U && ops->line == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if ((ops->capability_flags & SS_V2_DRAW_CAP_POLYLINE) != 0U && ops->polyline == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return SS_V2_STATUS_OK;
}

ss_u32 ss_v2_draw_target_is_valid(const ss_v2_draw_target *target)
{
    ss_u32 status;

    status = ss_v2_check_prefix(target, (ss_u32)sizeof(*target), (ss_u32)sizeof(*target), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (target->ops == 0 && target->surface == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (target->ops != 0) {
        status = ss_v2_draw_ops_is_valid(target->ops);
        if (status != SS_V2_STATUS_OK) {
            return status;
        }
    }
    if (target->surface != 0) {
        status = ss_v2_surface_desc_is_valid(target->surface);
        if (status != SS_V2_STATUS_OK) {
            return status;
        }
    }
    return SS_V2_STATUS_OK;
}
