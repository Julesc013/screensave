#ifndef PLASMA_V2_GL11_H
#define PLASMA_V2_GL11_H

#include "plasma_v2_realization.h"

ss_u32 plasma_v2_gl11_candidate_is_admitted(void);
const char *plasma_v2_gl11_candidate_name(void);
const char *plasma_v2_gl11_candidate_boundary(void);
ss_u32 plasma_v2_gl11_candidate_render(
    plasma_v2_core_session *session,
    ss_v2_draw_target *target,
    plasma_v2_realization_result *result
);

#endif /* PLASMA_V2_GL11_H */
