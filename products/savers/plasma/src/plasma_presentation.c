#include "plasma_internal.h"

int plasma_presentation_mode_is_supported(plasma_presentation_mode mode)
{
    return
        mode == PLASMA_PRESENTATION_MODE_FLAT ||
        mode == PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        mode == PLASMA_PRESENTATION_MODE_CURTAIN ||
        mode == PLASMA_PRESENTATION_MODE_RIBBON ||
        mode == PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION ||
        mode == PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE;
}

int plasma_presentation_mode_requires_premium(plasma_presentation_mode mode)
{
    return mode != PLASMA_PRESENTATION_MODE_FLAT;
}

int plasma_presentation_mode_supports_output_family(
    plasma_presentation_mode mode,
    plasma_output_family family
)
{
    if (mode != PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION) {
        return 1;
    }

    return family == PLASMA_OUTPUT_FAMILY_CONTOUR;
}

const char *plasma_presentation_mode_token(plasma_presentation_mode mode)
{
    switch (mode) {
    case PLASMA_PRESENTATION_MODE_FLAT:
        return "flat";

    case PLASMA_PRESENTATION_MODE_HEIGHTFIELD:
        return "heightfield";

    case PLASMA_PRESENTATION_MODE_CURTAIN:
        return "curtain";

    case PLASMA_PRESENTATION_MODE_RIBBON:
        return "ribbon";

    case PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION:
        return "contour_extrusion";

    case PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE:
        return "bounded_surface";

    default:
        return "unsupported";
    }
}

static plasma_presentation_mode plasma_presentation_resolved_mode(
    const struct plasma_plan_tag *plan
)
{
    plasma_presentation_mode mode;

    if (plan == NULL) {
        return PLASMA_PRESENTATION_MODE_FLAT;
    }

    mode = plan->presentation_mode;
    if (!plasma_presentation_mode_is_supported(mode)) {
        return PLASMA_PRESENTATION_MODE_FLAT;
    }
    if (
        plasma_presentation_mode_requires_premium(mode) &&
        !plan->premium_enabled
    ) {
        return PLASMA_PRESENTATION_MODE_FLAT;
    }
    if (!plasma_presentation_mode_supports_output_family(mode, plan->output_family)) {
        return PLASMA_PRESENTATION_MODE_FLAT;
    }

    return mode;
}

void plasma_presentation_bind_plan(struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return;
    }

    plan->presentation_mode = plasma_presentation_resolved_mode(plan);
}

int plasma_presentation_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    return plan->presentation_mode == plasma_presentation_resolved_mode(plan);
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

    if (plan->premium_enabled && plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT) {
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
