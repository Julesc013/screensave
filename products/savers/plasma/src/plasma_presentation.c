#include "plasma_internal.h"

int plasma_presentation_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->presentation_mode == PLASMA_PRESENTATION_MODE_FLAT) {
        return 1;
    }

    if (!plan->premium_enabled) {
        return 0;
    }

    return
        plan->presentation_mode == PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        plan->presentation_mode == PLASMA_PRESENTATION_MODE_CURTAIN ||
        plan->presentation_mode == PLASMA_PRESENTATION_MODE_RIBBON ||
        plan->presentation_mode == PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION ||
        plan->presentation_mode == PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE;
}

int plasma_presentation_prepare(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
)
{
    if (
        target_out == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        !plasma_presentation_validate_plan(plan)
    ) {
        return 0;
    }

    if (plan->premium_enabled) {
        return plasma_premium_prepare_presentation(
            plan,
            (struct plasma_execution_state_tag *)state,
            treated_frame,
            target_out
        );
    }

    if (plan->modern_enabled) {
        return plasma_modern_prepare_presentation(
            plan,
            (struct plasma_execution_state_tag *)state,
            treated_frame,
            target_out
        );
    }

    screensave_visual_buffer_get_bitmap_view(treated_frame->visual_buffer, &target_out->bitmap_view);
    target_out->destination_rect.x = 0;
    target_out->destination_rect.y = 0;
    target_out->destination_rect.width = state->drawable_size.width;
    target_out->destination_rect.height = state->drawable_size.height;
    return 1;
}
