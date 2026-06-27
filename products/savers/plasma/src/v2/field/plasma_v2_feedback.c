#include "plasma_v2_feedback.h"

ss_u32 plasma_v2_feedback_apply(plasma_v2_runtime *runtime)
{
    ss_u32 index;
    ss_u32 amount;
    ss_u32 current;
    ss_u32 previous;
    ss_u32 blended;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    amount = runtime->resolved_plan.use_feedback_buffer == SS_V2_TRUE
        ? runtime->resolved_plan.resolved_spec.feedback
        : 0U;
    if (amount > 1000U) {
        amount = 1000U;
    }

    for (index = 0U; index < runtime->field_cell_count; ++index) {
        current = runtime->field_b[index];
        previous = runtime->field_history[index];
        blended = ((current * (1000U - amount)) + (previous * amount)) / 1000U;
        runtime->field_b[index] = blended;
        runtime->field_history[index] = blended;
    }

    return SS_V2_STATUS_OK;
}
