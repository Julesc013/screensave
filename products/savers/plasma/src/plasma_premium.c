#include "plasma_internal.h"

static int plasma_premium_abs_int(int value)
{
    return value < 0 ? -value : value;
}

static unsigned long plasma_premium_default_degrade_policy(void)
{
    return
        PLASMA_PREMIUM_DEGRADE_DROP_EXTENDED_HISTORY |
        PLASMA_PREMIUM_DEGRADE_DROP_POST_CHAIN |
        PLASMA_PREMIUM_DEGRADE_DROP_HEIGHTFIELD_PRESENTATION;
}

static unsigned char plasma_premium_sample_scalar(
    const unsigned char *field,
    int width,
    int height,
    int x,
    int y
)
{
    if (x < 0) {
        x = 0;
    } else if (x >= width) {
        x = width - 1;
    }
    if (y < 0) {
        y = 0;
    } else if (y >= height) {
        y = height - 1;
    }

    return field[(y * width) + x];
}

static unsigned char plasma_premium_sample_channel(
    const screensave_visual_buffer *buffer,
    int x,
    int y,
    int channel
)
{
    const unsigned char *row;

    if (x < 0) {
        x = 0;
    } else if (x >= buffer->size.width) {
        x = buffer->size.width - 1;
    }
    if (y < 0) {
        y = 0;
    } else if (y >= buffer->size.height) {
        y = buffer->size.height - 1;
    }

    row = buffer->pixels + ((size_t)y * (size_t)buffer->stride_bytes);
    return row[(x * 4) + channel];
}

static unsigned char plasma_premium_clamp_channel(unsigned int value)
{
    if (value > 255U) {
        return 255U;
    }

    return (unsigned char)value;
}

static int plasma_premium_presentation_is_supported(plasma_presentation_mode mode)
{
    return
        mode == PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        mode == PLASMA_PRESENTATION_MODE_CURTAIN ||
        mode == PLASMA_PRESENTATION_MODE_RIBBON ||
        mode == PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION ||
        mode == PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE;
}

static int plasma_premium_plan_supports_presentation(
    const struct plasma_plan_tag *plan
)
{
    if (plan == NULL || !plasma_premium_presentation_is_supported(plan->presentation_mode)) {
        return 0;
    }

    if (
        plan->output_family != PLASMA_OUTPUT_FAMILY_RASTER &&
        plan->output_family != PLASMA_OUTPUT_FAMILY_BANDED &&
        plan->output_family != PLASMA_OUTPUT_FAMILY_CONTOUR
    ) {
        return 0;
    }

    if (plan->presentation_mode == PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION) {
        return plan->output_family == PLASMA_OUTPUT_FAMILY_CONTOUR;
    }

    return 1;
}

static void plasma_premium_plot_pixel(
    screensave_visual_buffer *buffer,
    int x,
    int y,
    unsigned int blue_value,
    unsigned int green_value,
    unsigned int red_value
)
{
    unsigned char *row;

    if (
        buffer == NULL ||
        buffer->pixels == NULL ||
        x < 0 ||
        y < 0 ||
        x >= buffer->size.width ||
        y >= buffer->size.height
    ) {
        return;
    }

    row = buffer->pixels + ((size_t)y * (size_t)buffer->stride_bytes);
    if (row[(x * 4) + 3] == 0U) {
        row[(x * 4) + 0] = plasma_premium_clamp_channel(blue_value);
        row[(x * 4) + 1] = plasma_premium_clamp_channel(green_value);
        row[(x * 4) + 2] = plasma_premium_clamp_channel(red_value);
    } else {
        if (row[(x * 4) + 0] < blue_value) {
            row[(x * 4) + 0] = plasma_premium_clamp_channel(blue_value);
        }
        if (row[(x * 4) + 1] < green_value) {
            row[(x * 4) + 1] = plasma_premium_clamp_channel(green_value);
        }
        if (row[(x * 4) + 2] < red_value) {
            row[(x * 4) + 2] = plasma_premium_clamp_channel(red_value);
        }
    }
    row[(x * 4) + 3] = 255U;
}

static void plasma_premium_plot_scaled_pixel(
    screensave_visual_buffer *buffer,
    int x,
    int y,
    unsigned int blue_value,
    unsigned int green_value,
    unsigned int red_value,
    unsigned int scale
)
{
    if (scale > 255U) {
        scale = 255U;
    }

    plasma_premium_plot_pixel(
        buffer,
        x,
        y,
        (blue_value * scale) / 255U,
        (green_value * scale) / 255U,
        (red_value * scale) / 255U
    );
}

int plasma_is_premium_kind(screensave_renderer_kind renderer_kind)
{
    return renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

void plasma_premium_plan_init(struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return;
    }

    plan->premium_capable = 0;
    plan->premium_requested = 0;
    plan->premium_enabled = 0;
    plan->premium_degraded = 0;
    plan->premium_components = 0UL;
    plan->premium_degrade_policy = 0UL;
}

void plasma_premium_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
)
{
    if (plan == NULL) {
        return;
    }

    plan->premium_capable =
        plan->selection.selected_preset != NULL &&
        plan->selection.selected_preset->premium_capable;
    plan->premium_requested =
        plasma_is_premium_kind(requested_kind) ||
        plasma_is_premium_kind(active_kind) ||
        plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT;
    plan->premium_enabled = 0;
    plan->premium_degraded = 0;
    plan->premium_components = 0UL;
    plan->premium_degrade_policy = plan->premium_capable
        ? plasma_premium_default_degrade_policy()
        : 0UL;

    if (
        module != NULL &&
        plan->premium_capable &&
        plan->modern_enabled &&
        plasma_is_premium_kind(active_kind) &&
        screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GL46)
    ) {
        plan->premium_enabled = 1;
        plan->premium_components =
            PLASMA_PREMIUM_COMPONENT_EXTENDED_HISTORY |
            PLASMA_PREMIUM_COMPONENT_POST_CHAIN |
            PLASMA_PREMIUM_COMPONENT_DIMENSIONAL_PRESENTATION;
        return;
    }

    if (plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT) {
        plan->presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;
    }

    if (
        plan->premium_requested &&
        plan->premium_capable &&
        !plasma_is_premium_kind(active_kind)
    ) {
        plan->premium_degraded = 1;
    }
}

int plasma_premium_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->premium_capable != 0 && plan->premium_capable != 1) {
        return 0;
    }
    if (plan->premium_requested != 0 && plan->premium_requested != 1) {
        return 0;
    }
    if (plan->premium_enabled != 0 && plan->premium_enabled != 1) {
        return 0;
    }
    if (plan->premium_degraded != 0 && plan->premium_degraded != 1) {
        return 0;
    }

    if (plan->premium_enabled) {
        if (
            !plan->premium_capable ||
            !plan->premium_requested ||
            !plan->advanced_enabled ||
            !plan->modern_enabled ||
            plan->advanced_degraded ||
            plan->modern_degraded ||
            !plasma_is_premium_kind(plan->active_renderer_kind) ||
            module == NULL ||
            !screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GL46) ||
            plan->premium_degraded ||
            plan->premium_components != (
                PLASMA_PREMIUM_COMPONENT_EXTENDED_HISTORY |
                PLASMA_PREMIUM_COMPONENT_POST_CHAIN |
                PLASMA_PREMIUM_COMPONENT_DIMENSIONAL_PRESENTATION
            ) ||
            plan->premium_degrade_policy != plasma_premium_default_degrade_policy() ||
            (plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT &&
                !plasma_premium_plan_supports_presentation(plan))
        ) {
            return 0;
        }

        return 1;
    }

    if (plan->premium_components != 0UL) {
        return 0;
    }

    if (plan->premium_degraded) {
        if (
            !plan->premium_capable ||
            !plan->premium_requested ||
            plasma_is_premium_kind(plan->active_renderer_kind)
        ) {
            return 0;
        }

        if (plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL33) {
            return
                plan->advanced_enabled &&
                plan->modern_enabled &&
                !plan->advanced_degraded &&
                !plan->modern_degraded &&
                plan->presentation_mode == PLASMA_PRESENTATION_MODE_FLAT;
        }
        if (plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL21) {
            return
                plan->advanced_enabled &&
                !plan->advanced_degraded &&
                !plan->modern_enabled &&
                plan->presentation_mode == PLASMA_PRESENTATION_MODE_FLAT;
        }

        return
            plasma_is_lower_band_kind(plan->active_renderer_kind) &&
            plan->presentation_mode == PLASMA_PRESENTATION_MODE_FLAT;
    }

    return 1;
}

int plasma_premium_apply_field_refinement(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state
)
{
    int width;
    int height;
    int x;
    int y;

    if (plan == NULL || state == NULL) {
        return 0;
    }
    if (!plan->premium_enabled) {
        return 1;
    }
    if (
        state->field_primary == NULL ||
        state->field_secondary == NULL ||
        state->field_history == NULL
    ) {
        return 0;
    }

    width = state->field_size.width;
    height = state->field_size.height;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int index;
            unsigned int center_value;
            unsigned int history_value;
            unsigned int cross_average;
            unsigned int diagonal_average;
            unsigned int refined_value;

            index = (unsigned int)((y * width) + x);
            center_value = (unsigned int)state->field_primary[index];
            history_value = (unsigned int)state->field_history[index];
            cross_average =
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x - 1, y) +
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x + 1, y) +
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x, y - 1) +
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x, y + 1);
            diagonal_average =
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x - 1, y - 1) +
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x + 1, y - 1) +
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x - 1, y + 1) +
                (unsigned int)plasma_premium_sample_scalar(state->field_primary, width, height, x + 1, y + 1);
            cross_average /= 4U;
            diagonal_average /= 4U;

            refined_value =
                ((center_value * 138U) + (history_value * 48U) +
                    (cross_average * 42U) + (diagonal_average * 27U)) / 255U;
            if (center_value > cross_average) {
                refined_value += (center_value - cross_average) / 3U;
            }

            state->field_secondary[index] = plasma_premium_clamp_channel(refined_value);
        }
    }

    {
        unsigned char *swap_buffer;

        swap_buffer = state->field_primary;
        state->field_primary = state->field_secondary;
        state->field_secondary = swap_buffer;
    }

    return 1;
}

int plasma_premium_apply_post_refinement(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    int x;
    int y;

    if (plan == NULL || state == NULL || visual_buffer == NULL) {
        return 0;
    }
    if (!plan->premium_enabled) {
        return 1;
    }
    if (
        (plan->premium_components & PLASMA_PREMIUM_COMPONENT_POST_CHAIN) == 0UL ||
        plan->theme == NULL ||
        state->premium_treatment_buffer.pixels == NULL ||
        state->premium_treatment_buffer.size.width != visual_buffer->size.width ||
        state->premium_treatment_buffer.size.height != visual_buffer->size.height
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;

        target_row = state->premium_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->premium_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            int x_gradient;
            int y_gradient;
            unsigned int gradient_value;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            x_gradient =
                (int)plasma_premium_sample_channel(visual_buffer, x + 1, y, 2) -
                (int)plasma_premium_sample_channel(visual_buffer, x - 1, y, 2);
            y_gradient =
                (int)plasma_premium_sample_channel(visual_buffer, x, y + 1, 2) -
                (int)plasma_premium_sample_channel(visual_buffer, x, y - 1, 2);
            gradient_value = (unsigned int)(
                plasma_premium_abs_int(x_gradient) + plasma_premium_abs_int(y_gradient)
            );
            if (gradient_value > 255U) {
                gradient_value = 255U;
            }

            blue_value =
                (unsigned int)plasma_premium_sample_channel(visual_buffer, x, y, 0) +
                (((unsigned int)plan->theme->accent_color.blue * gradient_value) / 255U) / 4U;
            green_value =
                (unsigned int)plasma_premium_sample_channel(visual_buffer, x, y, 1) +
                (((unsigned int)plan->theme->accent_color.green * gradient_value) / 255U) / 4U;
            red_value =
                (unsigned int)plasma_premium_sample_channel(visual_buffer, x, y, 2) +
                (((unsigned int)plan->theme->accent_color.red * gradient_value) / 255U) / 4U;

            target_row[(x * 4) + 0] = plasma_premium_clamp_channel(blue_value);
            target_row[(x * 4) + 1] = plasma_premium_clamp_channel(green_value);
            target_row[(x * 4) + 2] = plasma_premium_clamp_channel(red_value);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;
        const unsigned char *source_row;

        target_row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        source_row = state->premium_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->premium_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            blue_value =
                ((unsigned int)target_row[(x * 4) + 0] * 120U) +
                ((unsigned int)source_row[(x * 4) + 0] * 135U);
            green_value =
                ((unsigned int)target_row[(x * 4) + 1] * 120U) +
                ((unsigned int)source_row[(x * 4) + 1] * 135U);
            red_value =
                ((unsigned int)target_row[(x * 4) + 2] * 120U) +
                ((unsigned int)source_row[(x * 4) + 2] * 135U);

            target_row[(x * 4) + 0] = (unsigned char)(blue_value / 255U);
            target_row[(x * 4) + 1] = (unsigned char)(green_value / 255U);
            target_row[(x * 4) + 2] = (unsigned char)(red_value / 255U);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_premium_prepare_heightfield(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame
)
{
    int max_lift;
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL
    ) {
        return 0;
    }
    if (
        state->field_primary == NULL ||
        state->premium_presentation_buffer.pixels == NULL ||
        state->premium_presentation_buffer.size.width != treated_frame->size.width ||
        state->premium_presentation_buffer.size.height != treated_frame->size.height
    ) {
        return 0;
    }

    ZeroMemory(
        state->premium_presentation_buffer.pixels,
        (size_t)state->premium_presentation_buffer.stride_bytes *
            (size_t)state->premium_presentation_buffer.size.height
    );

    max_lift = treated_frame->size.height / 4;
    if (max_lift < 4) {
        max_lift = 4;
    }

    for (x = 0; x < treated_frame->size.width; ++x) {
        for (y = treated_frame->size.height - 1; y >= 0; --y) {
            const unsigned char *source_row;
            unsigned char *target_row;
            unsigned int index;
            unsigned int scalar_value;
            unsigned int slope_value;
            unsigned int highlight_bias;
            int lift;
            int target_y;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            index = (unsigned int)((y * treated_frame->size.width) + x);
            scalar_value = (unsigned int)state->field_primary[index];
            source_row = treated_frame->visual_buffer->pixels +
                ((size_t)y * (size_t)treated_frame->visual_buffer->stride_bytes);
            lift = (int)((scalar_value * (unsigned int)max_lift) / 255U);
            target_y = y - lift;
            if (target_y < 0) {
                target_y = 0;
            }

            slope_value = scalar_value;
            if (y + 1 < treated_frame->size.height) {
                unsigned int below_value;

                below_value = (unsigned int)state->field_primary[((y + 1) * treated_frame->size.width) + x];
                if (scalar_value > below_value) {
                    slope_value = scalar_value - below_value;
                } else {
                    slope_value = 0U;
                }
            }
            highlight_bias = slope_value / 3U;

            blue_value = (unsigned int)source_row[(x * 4) + 0] + highlight_bias;
            green_value = (unsigned int)source_row[(x * 4) + 1] + highlight_bias;
            red_value = (unsigned int)source_row[(x * 4) + 2] + highlight_bias;

            target_row = state->premium_presentation_buffer.pixels +
                ((size_t)target_y * (size_t)state->premium_presentation_buffer.stride_bytes);
            target_row[(x * 4) + 0] = plasma_premium_clamp_channel(blue_value);
            target_row[(x * 4) + 1] = plasma_premium_clamp_channel(green_value);
            target_row[(x * 4) + 2] = plasma_premium_clamp_channel(red_value);
            target_row[(x * 4) + 3] = 255U;

            if (target_y + 1 <= y) {
                int shadow_y;

                for (shadow_y = target_y + 1; shadow_y <= y && shadow_y <= target_y + 2; ++shadow_y) {
                    unsigned char *shadow_row;

                    shadow_row = state->premium_presentation_buffer.pixels +
                        ((size_t)shadow_y * (size_t)state->premium_presentation_buffer.stride_bytes);
                    shadow_row[(x * 4) + 0] = plasma_premium_clamp_channel((unsigned int)source_row[(x * 4) + 0] / 3U);
                    shadow_row[(x * 4) + 1] = plasma_premium_clamp_channel((unsigned int)source_row[(x * 4) + 1] / 3U);
                    shadow_row[(x * 4) + 2] = plasma_premium_clamp_channel((unsigned int)source_row[(x * 4) + 2] / 3U);
                    shadow_row[(x * 4) + 3] = 255U;
                }
            }
        }
    }

    return 1;
}

static int plasma_premium_prepare_curtain(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame
)
{
    int max_sway;
    int width;
    int height;
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        state->field_primary == NULL ||
        state->premium_presentation_buffer.pixels == NULL
    ) {
        return 0;
    }

    width = treated_frame->size.width;
    height = treated_frame->size.height;
    if (
        state->premium_presentation_buffer.size.width != width ||
        state->premium_presentation_buffer.size.height != height
    ) {
        return 0;
    }

    ZeroMemory(
        state->premium_presentation_buffer.pixels,
        (size_t)state->premium_presentation_buffer.stride_bytes *
            (size_t)state->premium_presentation_buffer.size.height
    );

    max_sway = width / 18;
    if (max_sway < 3) {
        max_sway = 3;
    }

    for (y = 0; y < height; ++y) {
        const unsigned char *source_row;
        unsigned int depth_scale;

        source_row = treated_frame->visual_buffer->pixels +
            ((size_t)y * (size_t)treated_frame->visual_buffer->stride_bytes);
        depth_scale = 156U + ((unsigned int)y * 72U) / (unsigned int)(height > 0 ? height : 1);
        if (depth_scale > 255U) {
            depth_scale = 255U;
        }

        for (x = 0; x < width; ++x) {
            unsigned int index;
            unsigned int scalar_value;
            unsigned int ridge_value;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            int sway;
            int target_x;

            index = (unsigned int)((y * width) + x);
            scalar_value = (unsigned int)state->field_primary[index];
            ridge_value = (unsigned int)plasma_premium_abs_int(
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x + 1, y) -
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x - 1, y)
            );
            sway = (((int)scalar_value - 128) * max_sway * (y + 8)) / ((height + 8) * 128);
            target_x = x + sway;

            blue_value = (((unsigned int)source_row[(x * 4) + 0] * depth_scale) / 255U) + (ridge_value / 5U);
            green_value = (((unsigned int)source_row[(x * 4) + 1] * depth_scale) / 255U) + (ridge_value / 5U);
            red_value = (((unsigned int)source_row[(x * 4) + 2] * depth_scale) / 255U) + (ridge_value / 5U);
            plasma_premium_plot_pixel(
                &state->premium_presentation_buffer,
                target_x,
                y,
                blue_value,
                green_value,
                red_value
            );
            plasma_premium_plot_scaled_pixel(
                &state->premium_presentation_buffer,
                target_x + (sway >= 0 ? 1 : -1),
                y,
                blue_value,
                green_value,
                red_value,
                96U
            );
        }
    }

    return 1;
}

static int plasma_premium_prepare_ribbon(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame
)
{
    int max_lift;
    int width;
    int height;
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        state->field_primary == NULL ||
        state->premium_presentation_buffer.pixels == NULL
    ) {
        return 0;
    }

    width = treated_frame->size.width;
    height = treated_frame->size.height;
    if (
        state->premium_presentation_buffer.size.width != width ||
        state->premium_presentation_buffer.size.height != height
    ) {
        return 0;
    }

    ZeroMemory(
        state->premium_presentation_buffer.pixels,
        (size_t)state->premium_presentation_buffer.stride_bytes *
            (size_t)state->premium_presentation_buffer.size.height
    );

    max_lift = height / 9;
    if (max_lift < 3) {
        max_lift = 3;
    }

    for (y = 0; y < height; ++y) {
        const unsigned char *source_row;

        source_row = treated_frame->visual_buffer->pixels +
            ((size_t)y * (size_t)treated_frame->visual_buffer->stride_bytes);
        for (x = 0; x < width; ++x) {
            unsigned int index;
            unsigned int phase_value;
            unsigned int scalar_value;
            unsigned int highlight_bias;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            int lift;
            int target_x;
            int target_y;
            int weave;

            index = (unsigned int)((y * width) + x);
            scalar_value = (unsigned int)state->field_primary[index];
            lift = (int)((scalar_value * (unsigned int)max_lift) / 255U);
            phase_value = (unsigned int)((y * 11) + (unsigned int)(state->palette_phase & 255UL));
            phase_value &= 255U;
            if (phase_value < 64U) {
                weave = 128 + (int)(phase_value * 2U);
            } else if (phase_value < 128U) {
                weave = 255 - (int)((phase_value - 64U) * 2U);
            } else if (phase_value < 192U) {
                weave = 127 - (int)((phase_value - 128U) * 2U);
            } else {
                weave = (int)((phase_value - 192U) * 2U);
            }
            weave -= 128;
            target_x = x + (weave / 48);
            target_y = y - (lift / 2);
            if (target_y < 0) {
                target_y = 0;
            }

            highlight_bias = (unsigned int)plasma_premium_abs_int(
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x, y + 1) -
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x, y - 1)
            ) / 4U;
            blue_value = (unsigned int)source_row[(x * 4) + 0] + highlight_bias;
            green_value = (unsigned int)source_row[(x * 4) + 1] + highlight_bias;
            red_value = (unsigned int)source_row[(x * 4) + 2] + highlight_bias;
            plasma_premium_plot_pixel(
                &state->premium_presentation_buffer,
                target_x,
                target_y,
                blue_value,
                green_value,
                red_value
            );
            plasma_premium_plot_scaled_pixel(
                &state->premium_presentation_buffer,
                target_x,
                target_y + 1,
                blue_value,
                green_value,
                red_value,
                132U
            );
        }
    }

    return 1;
}

static int plasma_premium_prepare_contour_extrusion(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame
)
{
    int max_lift;
    int width;
    int height;
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        state->field_primary == NULL ||
        state->premium_presentation_buffer.pixels == NULL
    ) {
        return 0;
    }

    width = treated_frame->size.width;
    height = treated_frame->size.height;
    if (
        state->premium_presentation_buffer.size.width != width ||
        state->premium_presentation_buffer.size.height != height
    ) {
        return 0;
    }

    ZeroMemory(
        state->premium_presentation_buffer.pixels,
        (size_t)state->premium_presentation_buffer.stride_bytes *
            (size_t)state->premium_presentation_buffer.size.height
    );

    max_lift = height / 5;
    if (max_lift < 4) {
        max_lift = 4;
    }

    for (x = 0; x < width; ++x) {
        for (y = height - 1; y >= 0; --y) {
            const unsigned char *source_row;
            unsigned int index;
            unsigned int scalar_value;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            int lift;
            int target_x;
            int target_y;
            int extrusion_step;

            index = (unsigned int)((y * width) + x);
            scalar_value = (unsigned int)state->field_primary[index];
            source_row = treated_frame->visual_buffer->pixels +
                ((size_t)y * (size_t)treated_frame->visual_buffer->stride_bytes);
            lift = (int)((scalar_value * (unsigned int)max_lift) / 255U);
            target_x = x + (lift / 5);
            target_y = y - lift;
            if (target_y < 0) {
                target_y = 0;
            }

            blue_value = (unsigned int)source_row[(x * 4) + 0] + (scalar_value / 5U);
            green_value = (unsigned int)source_row[(x * 4) + 1] + (scalar_value / 5U);
            red_value = (unsigned int)source_row[(x * 4) + 2] + (scalar_value / 5U);
            plasma_premium_plot_pixel(
                &state->premium_presentation_buffer,
                target_x,
                target_y,
                blue_value,
                green_value,
                red_value
            );

            for (extrusion_step = 1; extrusion_step <= 3 && target_y + extrusion_step <= y; ++extrusion_step) {
                plasma_premium_plot_scaled_pixel(
                    &state->premium_presentation_buffer,
                    target_x - (extrusion_step / 2),
                    target_y + extrusion_step,
                    blue_value,
                    green_value,
                    red_value,
                    (unsigned int)(120 - (extrusion_step * 22))
                );
            }
        }
    }

    return 1;
}

static int plasma_premium_prepare_bounded_surface(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame
)
{
    int max_lift;
    int max_shift;
    int width;
    int height;
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        state->field_primary == NULL ||
        state->premium_presentation_buffer.pixels == NULL
    ) {
        return 0;
    }

    width = treated_frame->size.width;
    height = treated_frame->size.height;
    if (
        state->premium_presentation_buffer.size.width != width ||
        state->premium_presentation_buffer.size.height != height
    ) {
        return 0;
    }

    ZeroMemory(
        state->premium_presentation_buffer.pixels,
        (size_t)state->premium_presentation_buffer.stride_bytes *
            (size_t)state->premium_presentation_buffer.size.height
    );

    max_lift = height / 7;
    if (max_lift < 4) {
        max_lift = 4;
    }
    max_shift = width / 28;
    if (max_shift < 2) {
        max_shift = 2;
    }

    for (y = height - 1; y >= 0; --y) {
        const unsigned char *source_row;

        source_row = treated_frame->visual_buffer->pixels +
            ((size_t)y * (size_t)treated_frame->visual_buffer->stride_bytes);
        for (x = 0; x < width; ++x) {
            unsigned int index;
            unsigned int scalar_value;
            unsigned int gradient_value;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            int lift;
            int shift;
            int target_x;
            int target_y;

            index = (unsigned int)((y * width) + x);
            scalar_value = (unsigned int)state->field_primary[index];
            gradient_value = (unsigned int)plasma_premium_abs_int(
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x + 1, y) -
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x - 1, y)
            );
            lift = (int)((scalar_value * (unsigned int)max_lift) / 255U);
            shift = (((int)plasma_premium_sample_scalar(state->field_primary, width, height, x + 1, y) -
                (int)plasma_premium_sample_scalar(state->field_primary, width, height, x - 1, y)) *
                max_shift) / 255;
            target_x = x + shift;
            target_y = y - (lift / 2);
            if (target_y < 0) {
                target_y = 0;
            }

            blue_value = (unsigned int)source_row[(x * 4) + 0] + (gradient_value / 4U);
            green_value = (unsigned int)source_row[(x * 4) + 1] + (gradient_value / 4U);
            red_value = (unsigned int)source_row[(x * 4) + 2] + (gradient_value / 4U);
            plasma_premium_plot_pixel(
                &state->premium_presentation_buffer,
                target_x,
                target_y,
                blue_value,
                green_value,
                red_value
            );
            plasma_premium_plot_scaled_pixel(
                &state->premium_presentation_buffer,
                target_x,
                target_y + 1,
                blue_value,
                green_value,
                red_value,
                144U
            );
        }
    }

    return 1;
}

int plasma_premium_prepare_presentation(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
)
{
    int result;

    if (
        plan == NULL ||
        state == NULL ||
        treated_frame == NULL ||
        treated_frame->visual_buffer == NULL ||
        target_out == NULL
    ) {
        return 0;
    }
    if (!plan->premium_enabled) {
        return 0;
    }
    if (
        (plan->premium_components & PLASMA_PREMIUM_COMPONENT_DIMENSIONAL_PRESENTATION) == 0UL ||
        !plasma_premium_plan_supports_presentation(plan)
    ) {
        return 0;
    }

    result = 0;
    switch (plan->presentation_mode) {
    case PLASMA_PRESENTATION_MODE_HEIGHTFIELD:
        result = plasma_premium_prepare_heightfield(plan, state, treated_frame);
        break;

    case PLASMA_PRESENTATION_MODE_CURTAIN:
        result = plasma_premium_prepare_curtain(plan, state, treated_frame);
        break;

    case PLASMA_PRESENTATION_MODE_RIBBON:
        result = plasma_premium_prepare_ribbon(plan, state, treated_frame);
        break;

    case PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION:
        result = plasma_premium_prepare_contour_extrusion(plan, state, treated_frame);
        break;

    case PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE:
        result = plasma_premium_prepare_bounded_surface(plan, state, treated_frame);
        break;

    default:
        return 0;
    }

    if (!result) {
        return 0;
    }

    screensave_visual_buffer_get_bitmap_view(&state->premium_presentation_buffer, &target_out->bitmap_view);
    target_out->destination_rect.x = 0;
    target_out->destination_rect.y = 0;
    target_out->destination_rect.width = state->drawable_size.width;
    target_out->destination_rect.height = state->drawable_size.height;
    return 1;
}
