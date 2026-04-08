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
        plasma_is_premium_kind(active_kind);
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
            PLASMA_PREMIUM_COMPONENT_HEIGHTFIELD_PRESENTATION;
        plan->presentation_mode = PLASMA_PRESENTATION_MODE_HEIGHTFIELD;
        return;
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
                PLASMA_PREMIUM_COMPONENT_HEIGHTFIELD_PRESENTATION
            ) ||
            plan->premium_degrade_policy != plasma_premium_default_degrade_policy() ||
            plan->output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
            plan->output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
            plan->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
            plan->filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
            plan->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
            plan->accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
            plan->presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD
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

int plasma_premium_prepare_presentation(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
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
        target_out == NULL
    ) {
        return 0;
    }
    if (!plan->premium_enabled) {
        return 0;
    }
    if (
        plan->presentation_mode != PLASMA_PRESENTATION_MODE_HEIGHTFIELD ||
        (plan->premium_components & PLASMA_PREMIUM_COMPONENT_HEIGHTFIELD_PRESENTATION) == 0UL ||
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

    screensave_visual_buffer_get_bitmap_view(&state->premium_presentation_buffer, &target_out->bitmap_view);
    target_out->destination_rect.x = 0;
    target_out->destination_rect.y = 0;
    target_out->destination_rect.width = state->drawable_size.width;
    target_out->destination_rect.height = state->drawable_size.height;
    return 1;
}
