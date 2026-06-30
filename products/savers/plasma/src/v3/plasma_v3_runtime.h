#ifndef PLASMA_V3_RUNTIME_H
#define PLASMA_V3_RUNTIME_H

#include "plasma_v3_plan.h"

ss_u32 plasma_v3_runtime_bind(
    const plasma_v3_plan *plan,
    const plasma_v3_runtime_buffers *buffers,
    plasma_v3_runtime *runtime_out
);
ss_u32 plasma_v3_runtime_reset(plasma_v3_runtime *runtime);
ss_u32 plasma_v3_runtime_advance(plasma_v3_runtime *runtime, ss_u32 delta_millis);
ss_u32 plasma_v3_runtime_render_frame(plasma_v3_runtime *runtime);
ss_u32 plasma_v3_runtime_is_valid(const plasma_v3_runtime *runtime);

#endif /* PLASMA_V3_RUNTIME_H */
