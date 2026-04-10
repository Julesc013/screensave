#include "plasma_internal.h"

static int plasma_output_mode_matches_family(
    plasma_output_family family,
    plasma_output_mode mode
)
{
    switch (family) {
    case PLASMA_OUTPUT_FAMILY_RASTER:
        return mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER;

    case PLASMA_OUTPUT_FAMILY_BANDED:
        return mode == PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;

    case PLASMA_OUTPUT_FAMILY_CONTOUR:
        return
            mode == PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
            mode == PLASMA_OUTPUT_MODE_CONTOUR_BANDS;

    case PLASMA_OUTPUT_FAMILY_GLYPH:
        return
            mode == PLASMA_OUTPUT_MODE_ASCII_GLYPH ||
            mode == PLASMA_OUTPUT_MODE_MATRIX_GLYPH;

    case PLASMA_OUTPUT_FAMILY_SURFACE:
    default:
        return 0;
    }
}

int plasma_output_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    return plasma_output_mode_matches_family(plan->output_family, plan->output_mode);
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
