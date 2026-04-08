#include "plasma_internal.h"

int plasma_output_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->output_family != PLASMA_OUTPUT_FAMILY_RASTER) {
        return 0;
    }

    return plan->output_mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER;
}

int plasma_output_build(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    plasma_output_frame *frame_out
)
{
    if (
        frame_out == NULL ||
        state == NULL ||
        !plasma_output_validate_plan(plan) ||
        state->field_primary == NULL ||
        state->field_size.width <= 0 ||
        state->field_size.height <= 0
    ) {
        return 0;
    }

    frame_out->family = plan->output_family;
    frame_out->mode = plan->output_mode;
    frame_out->size = state->field_size;
    frame_out->scalar_values = state->field_primary;
    return 1;
}
