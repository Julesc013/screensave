#ifndef PLASMA_V2_REALIZATION_H
#define PLASMA_V2_REALIZATION_H

#include "screensave/v2.h"
#include "plasma_v2_core.h"

#define PLASMA_V2_REALIZATION_SOFTWARE_REFERENCE ((ss_u32)1U)
#define PLASMA_V2_REALIZATION_GL11_CANDIDATE ((ss_u32)2U)

#define PLASMA_V2_REALIZATION_STATUS_RENDERED ((ss_u32)1U)
#define PLASMA_V2_REALIZATION_STATUS_FALLBACK_REFERENCE ((ss_u32)2U)
#define PLASMA_V2_REALIZATION_STATUS_UNAVAILABLE ((ss_u32)3U)

#define PLASMA_V2_COMPARISON_EXACT ((ss_u32)1U)
#define PLASMA_V2_COMPARISON_OBSERVATIONAL ((ss_u32)2U)

typedef struct plasma_v2_realization_result_tag {
    ss_u32 struct_size;
    ss_u32 realization_kind;
    ss_u32 status;
    ss_u32 comparison_class;
    const char *fallback_reason;
    const char *claim_boundary;
} plasma_v2_realization_result;

void plasma_v2_realization_result_init(
    plasma_v2_realization_result *result,
    ss_u32 realization_kind
);
ss_u32 plasma_v2_realization_render_software_reference(
    plasma_v2_core_session *session,
    ss_v2_draw_target *target,
    plasma_v2_realization_result *result
);

#endif /* PLASMA_V2_REALIZATION_H */
