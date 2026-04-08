#include "plasma_internal.h"

static screensave_color plasma_background_color(void)
{
    screensave_color color;

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 255;
    return color;
}

void plasma_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer_kind renderer_kind;
    plasma_output_frame output_frame;
    plasma_treated_frame treated_frame;
    plasma_presentation_target presentation_target;

    if (session == NULL || environment == NULL || environment->renderer == NULL) {
        return;
    }

    screensave_renderer_clear(environment->renderer, plasma_background_color());
    renderer_kind = plasma_resolve_renderer_kind(environment);
    session->state.active_renderer_kind = renderer_kind;
    if (!plasma_plan_validate_for_renderer_kind(&session->plan, plasma_get_module(), renderer_kind)) {
        return;
    }

    if (!plasma_output_build(&session->plan, &session->state, &output_frame)) {
        return;
    }
    if (
        !plasma_treatment_apply(
            &session->plan,
            &session->state,
            &output_frame,
            &session->state.visual_buffer,
            &treated_frame
        )
    ) {
        return;
    }
    if (
        !plasma_presentation_prepare(
            &session->plan,
            &session->state,
            &treated_frame,
            &presentation_target
        )
    ) {
        return;
    }

    (void)screensave_renderer_blit_bitmap(
        environment->renderer,
        &presentation_target.bitmap_view,
        &presentation_target.destination_rect
    );
}
