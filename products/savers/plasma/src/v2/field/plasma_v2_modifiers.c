#include "plasma_v2_modifiers.h"
#include "plasma_v2_feedback.h"

static ss_u32 plasma_v2_wrap_add(ss_u32 value, ss_u32 amount, ss_u32 limit)
{
    if (limit == 0U) {
        return 0U;
    }
    return (value + amount) % limit;
}

ss_u32 plasma_v2_apply_domain_warp(plasma_v2_runtime *runtime)
{
    ss_u32 x;
    ss_u32 y;
    ss_u32 width;
    ss_u32 height;
    ss_u32 offset;
    ss_u32 source_x;
    ss_u32 source_y;
    ss_u32 source_offset;
    ss_u32 warp;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    width = runtime->resolved_plan.field_size.width;
    height = runtime->resolved_plan.field_size.height;
    warp = runtime->resolved_plan.resolved_spec.warp / 125U;
    for (y = 0U; y < height; ++y) {
        for (x = 0U; x < width; ++x) {
            offset = (y * width) + x;
            source_x = plasma_v2_wrap_add(x, ((runtime->field_a[offset] + runtime->phase1) & 7U) * warp, width);
            source_y = plasma_v2_wrap_add(y, ((runtime->field_a[offset] + runtime->phase2) & 7U) * warp, height);
            source_offset = (source_y * width) + source_x;
            if (offset >= runtime->field_cell_count || source_offset >= runtime->field_cell_count) {
                return SS_V2_STATUS_BAD_SIZE;
            }
            runtime->field_b[offset] = runtime->field_a[source_offset];
        }
    }
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_apply_modifiers(plasma_v2_runtime *runtime)
{
    ss_u32 status;

    status = plasma_v2_apply_domain_warp(runtime);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return plasma_v2_feedback_apply(runtime);
}
