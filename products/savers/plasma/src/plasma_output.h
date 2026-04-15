#ifndef PLASMA_OUTPUT_H
#define PLASMA_OUTPUT_H

#include "screensave/types.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;

typedef enum plasma_output_family_tag {
    PLASMA_OUTPUT_FAMILY_RASTER = 0,
    PLASMA_OUTPUT_FAMILY_BANDED = 1,
    PLASMA_OUTPUT_FAMILY_CONTOUR = 2,
    PLASMA_OUTPUT_FAMILY_GLYPH = 3,
    PLASMA_OUTPUT_FAMILY_SURFACE = 4
} plasma_output_family;

typedef enum plasma_output_mode_tag {
    PLASMA_OUTPUT_MODE_NATIVE_RASTER = 0,
    PLASMA_OUTPUT_MODE_DITHERED_RASTER = 1,
    PLASMA_OUTPUT_MODE_CONTOUR_ONLY = 2,
    PLASMA_OUTPUT_MODE_CONTOUR_BANDS = 3,
    PLASMA_OUTPUT_MODE_ASCII_GLYPH = 4,
    PLASMA_OUTPUT_MODE_MATRIX_GLYPH = 5,
    PLASMA_OUTPUT_MODE_HEIGHTFIELD_SURFACE = 6,
    PLASMA_OUTPUT_MODE_CURTAIN_SURFACE = 7,
    PLASMA_OUTPUT_MODE_RIBBON_SURFACE = 8,
    PLASMA_OUTPUT_MODE_EXTRUDED_CONTOUR_SURFACE = 9,
    PLASMA_OUTPUT_MODE_POSTERIZED_BANDS = 10
} plasma_output_mode;

typedef struct plasma_output_frame_tag {
    plasma_output_family family;
    plasma_output_mode mode;
    screensave_sizei size;
    const unsigned char *scalar_values;
} plasma_output_frame;

int plasma_output_family_is_supported(plasma_output_family family);
int plasma_output_mode_is_supported(plasma_output_mode mode);
int plasma_output_family_supports_mode(
    plasma_output_family family,
    plasma_output_mode mode
);
plasma_output_mode plasma_output_default_mode_for_family(plasma_output_family family);
const char *plasma_output_family_token(plasma_output_family family);
const char *plasma_output_mode_token(plasma_output_mode mode);
int plasma_output_validate_plan(const struct plasma_plan_tag *plan);
int plasma_output_build(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    plasma_output_frame *frame_out
);

#endif /* PLASMA_OUTPUT_H */
