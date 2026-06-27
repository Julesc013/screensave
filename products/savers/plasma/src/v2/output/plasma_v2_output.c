#include "plasma_v2_output.h"

ss_u32 plasma_v2_output_transform_field(plasma_v2_runtime *runtime)
{
    ss_u32 index;
    ss_u32 value;
    ss_u32 output_kind;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    output_kind = runtime->resolved_plan.resolved_spec.output_kind;
    for (index = 0U; index < runtime->field_cell_count; ++index) {
        value = runtime->field_b[index];
        if (output_kind == PLASMA_V2_OUTPUT_BANDED) {
            value = ((value / 125U) * 125U) + 62U;
        } else if (output_kind == PLASMA_V2_OUTPUT_CONTOUR) {
            value = (value % 125U) < 24U ? 1000U : 120U;
        }
        runtime->field_b[index] = value > 1000U ? 1000U : value;
    }
    return SS_V2_STATUS_OK;
}
