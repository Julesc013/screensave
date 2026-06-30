#ifndef PLASMA_V3_PLAN_H
#define PLASMA_V3_PLAN_H

#include "plasma_v3_spec.h"

void plasma_v3_plan_set_defaults(plasma_v3_plan *plan);
ss_u32 plasma_v3_plan_compile(const plasma_v3_plan_request *request, plasma_v3_plan *plan_out);
ss_u32 plasma_v3_plan_is_valid(const plasma_v3_plan *plan);
ss_u32 plasma_v3_plan_required_field_cells(const plasma_v3_plan *plan, ss_u32 *cell_count_out);
ss_u32 plasma_v3_plan_required_pixel_bytes(const plasma_v3_plan *plan, ss_u32 *byte_count_out);

#endif /* PLASMA_V3_PLAN_H */
