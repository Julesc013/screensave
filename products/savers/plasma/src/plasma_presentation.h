#ifndef PLASMA_PRESENTATION_H
#define PLASMA_PRESENTATION_H

#include "screensave/visual_buffer_api.h"
#include "plasma_treatment.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;

typedef enum plasma_presentation_mode_tag {
    PLASMA_PRESENTATION_MODE_FLAT = 0,
    PLASMA_PRESENTATION_MODE_HEIGHTFIELD = 1,
    PLASMA_PRESENTATION_MODE_CURTAIN = 2,
    PLASMA_PRESENTATION_MODE_RIBBON = 3,
    PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION = 4,
    PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE = 5,
    PLASMA_PRESENTATION_MODE_BOUNDED_BILLBOARD_VOLUME = 6
} plasma_presentation_mode;

typedef struct plasma_presentation_target_tag {
    screensave_bitmap_view bitmap_view;
    screensave_recti destination_rect;
} plasma_presentation_target;

int plasma_presentation_validate_plan(const struct plasma_plan_tag *plan);
int plasma_presentation_prepare(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
);

#endif /* PLASMA_PRESENTATION_H */
