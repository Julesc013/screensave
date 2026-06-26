#include "plasma_v2_gl11.h"

ss_u32 plasma_v2_gl11_candidate_is_admitted(void)
{
    return SS_V2_TRUE;
}

const char *plasma_v2_gl11_candidate_name(void)
{
    return "plasma_v2_realization_gl11_candidate";
}

const char *plasma_v2_gl11_candidate_boundary(void)
{
    return "Optional GL11 realization candidate only; software reference remains canonical and release promotion remains blocked.";
}

ss_u32 plasma_v2_gl11_candidate_render(
    plasma_v2_core_session *session,
    ss_v2_draw_target *target,
    plasma_v2_realization_result *result
)
{
    ss_u32 status;

    if (result != 0) {
        plasma_v2_realization_result_init(result, PLASMA_V2_REALIZATION_GL11_CANDIDATE);
    }

    status = plasma_v2_core_render(session, target);
    if (result != 0 && status == SS_V2_STATUS_OK) {
        result->status = PLASMA_V2_REALIZATION_STATUS_FALLBACK_REFERENCE;
        result->comparison_class = PLASMA_V2_COMPARISON_EXACT;
        result->fallback_reason = "GL11 presenter is not active in the portable proof runner; rendered through the software reference oracle.";
        result->claim_boundary = plasma_v2_gl11_candidate_boundary();
    }
    return status;
}
