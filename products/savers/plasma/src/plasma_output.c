#include "plasma_internal.h"

int plasma_output_family_is_supported(plasma_output_family family)
{
    return
        family == PLASMA_OUTPUT_FAMILY_RASTER ||
        family == PLASMA_OUTPUT_FAMILY_BANDED ||
        family == PLASMA_OUTPUT_FAMILY_CONTOUR ||
        family == PLASMA_OUTPUT_FAMILY_GLYPH;
}

int plasma_output_mode_is_supported(plasma_output_mode mode)
{
    return
        mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        mode == PLASMA_OUTPUT_MODE_POSTERIZED_BANDS ||
        mode == PLASMA_OUTPUT_MODE_CONTOUR_ONLY ||
        mode == PLASMA_OUTPUT_MODE_CONTOUR_BANDS ||
        mode == PLASMA_OUTPUT_MODE_ASCII_GLYPH ||
        mode == PLASMA_OUTPUT_MODE_MATRIX_GLYPH;
}

int plasma_output_family_supports_mode(
    plasma_output_family family,
    plasma_output_mode mode
)
{
    if (!plasma_output_family_is_supported(family) || !plasma_output_mode_is_supported(mode)) {
        return 0;
    }

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

    default:
        return 0;
    }
}

plasma_output_mode plasma_output_default_mode_for_family(plasma_output_family family)
{
    switch (family) {
    case PLASMA_OUTPUT_FAMILY_BANDED:
        return PLASMA_OUTPUT_MODE_POSTERIZED_BANDS;

    case PLASMA_OUTPUT_FAMILY_CONTOUR:
        return PLASMA_OUTPUT_MODE_CONTOUR_ONLY;

    case PLASMA_OUTPUT_FAMILY_GLYPH:
        return PLASMA_OUTPUT_MODE_ASCII_GLYPH;

    case PLASMA_OUTPUT_FAMILY_RASTER:
    default:
        return PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    }
}

const char *plasma_output_family_token(plasma_output_family family)
{
    switch (family) {
    case PLASMA_OUTPUT_FAMILY_RASTER:
        return "raster";

    case PLASMA_OUTPUT_FAMILY_BANDED:
        return "banded";

    case PLASMA_OUTPUT_FAMILY_CONTOUR:
        return "contour";

    case PLASMA_OUTPUT_FAMILY_GLYPH:
        return "glyph";

    default:
        return "unsupported";
    }
}

const char *plasma_output_mode_token(plasma_output_mode mode)
{
    switch (mode) {
    case PLASMA_OUTPUT_MODE_NATIVE_RASTER:
        return "native_raster";

    case PLASMA_OUTPUT_MODE_POSTERIZED_BANDS:
        return "posterized_bands";

    case PLASMA_OUTPUT_MODE_CONTOUR_ONLY:
        return "contour_only";

    case PLASMA_OUTPUT_MODE_CONTOUR_BANDS:
        return "contour_bands";

    case PLASMA_OUTPUT_MODE_ASCII_GLYPH:
        return "ascii_glyph";

    case PLASMA_OUTPUT_MODE_MATRIX_GLYPH:
        return "matrix_glyph";

    default:
        return "unsupported";
    }
}

int plasma_output_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    return plasma_output_family_supports_mode(plan->output_family, plan->output_mode);
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
