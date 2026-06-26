#include "plasma_v2_realization.h"

void plasma_v2_realization_result_init(
    plasma_v2_realization_result *result,
    ss_u32 realization_kind
)
{
    if (result == 0) {
        return;
    }
    result->struct_size = (ss_u32)sizeof(*result);
    result->realization_kind = realization_kind;
    result->status = PLASMA_V2_REALIZATION_STATUS_UNAVAILABLE;
    result->comparison_class = PLASMA_V2_COMPARISON_EXACT;
    result->fallback_reason = "";
    result->claim_boundary = "Plasma v2 realization evidence only; not stable release, compatibility certification, or final artistic acceptance.";
}

ss_u32 plasma_v2_realization_render_software_reference(
    plasma_v2_core_session *session,
    ss_v2_draw_target *target,
    plasma_v2_realization_result *result
)
{
    ss_u32 status;

    if (result != 0) {
        plasma_v2_realization_result_init(result, PLASMA_V2_REALIZATION_SOFTWARE_REFERENCE);
    }

    status = plasma_v2_core_render(session, target);
    if (result != 0 && status == SS_V2_STATUS_OK) {
        result->status = PLASMA_V2_REALIZATION_STATUS_RENDERED;
        result->fallback_reason = "";
    }
    return status;
}
