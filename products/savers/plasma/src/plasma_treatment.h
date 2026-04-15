#ifndef PLASMA_TREATMENT_H
#define PLASMA_TREATMENT_H

#include "screensave/visual_buffer_api.h"
#include "plasma_output.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;

typedef enum plasma_sampling_treatment_tag {
    PLASMA_SAMPLING_TREATMENT_NONE = 0,
    PLASMA_SAMPLING_TREATMENT_NEAREST = 1,
    PLASMA_SAMPLING_TREATMENT_SOFT = 2,
    PLASMA_SAMPLING_TREATMENT_DITHER = 3
} plasma_sampling_treatment;

typedef enum plasma_filter_treatment_tag {
    PLASMA_FILTER_TREATMENT_NONE = 0,
    PLASMA_FILTER_TREATMENT_BLUR = 1,
    PLASMA_FILTER_TREATMENT_GLOW_EDGE = 2,
    PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE = 3,
    PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR = 4,
    PLASMA_FILTER_TREATMENT_RESTRAINED_GLITCH = 5,
    PLASMA_FILTER_TREATMENT_EMBOSS_EDGE = 6
} plasma_filter_treatment;

typedef enum plasma_emulation_treatment_tag {
    PLASMA_EMULATION_TREATMENT_NONE = 0,
    PLASMA_EMULATION_TREATMENT_PHOSPHOR = 1,
    PLASMA_EMULATION_TREATMENT_CRT = 2
} plasma_emulation_treatment;

typedef enum plasma_accent_treatment_tag {
    PLASMA_ACCENT_TREATMENT_NONE = 0,
    PLASMA_ACCENT_TREATMENT_OVERLAY_PASS = 1,
    PLASMA_ACCENT_TREATMENT_ACCENT_PASS = 2
} plasma_accent_treatment;

typedef struct plasma_treated_frame_tag {
    plasma_output_family family;
    plasma_output_mode mode;
    screensave_sizei size;
    const screensave_visual_buffer *visual_buffer;
} plasma_treated_frame;

int plasma_sampling_treatment_is_supported(plasma_sampling_treatment treatment);
int plasma_filter_treatment_is_supported(plasma_filter_treatment treatment);
int plasma_filter_treatment_is_primary_visible(plasma_filter_treatment treatment);
int plasma_filter_treatment_requires_advanced(plasma_filter_treatment treatment);
int plasma_emulation_treatment_is_supported(plasma_emulation_treatment treatment);
int plasma_accent_treatment_is_supported(plasma_accent_treatment treatment);
int plasma_accent_treatment_is_primary_visible(plasma_accent_treatment treatment);
int plasma_accent_treatment_requires_advanced(plasma_accent_treatment treatment);
const char *plasma_sampling_treatment_token(plasma_sampling_treatment treatment);
const char *plasma_filter_treatment_token(plasma_filter_treatment treatment);
const char *plasma_emulation_treatment_token(plasma_emulation_treatment treatment);
const char *plasma_accent_treatment_token(plasma_accent_treatment treatment);
int plasma_treatment_validate_plan(const struct plasma_plan_tag *plan);
int plasma_treatment_apply(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer,
    plasma_treated_frame *frame_out
);

#endif /* PLASMA_TREATMENT_H */
