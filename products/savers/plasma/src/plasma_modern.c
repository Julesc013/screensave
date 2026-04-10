#include "plasma_internal.h"

static unsigned long plasma_modern_default_degrade_policy(void)
{
    return
        PLASMA_MODERN_DEGRADE_DROP_REFINED_FIELD |
        PLASMA_MODERN_DEGRADE_DROP_REFINED_FILTER |
        PLASMA_MODERN_DEGRADE_DROP_PRESENTATION_BUFFER;
}

static unsigned char plasma_modern_sample_scalar(
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

static unsigned char plasma_modern_sample_channel(
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

static unsigned char plasma_modern_clamp_channel(unsigned int value)
{
    if (value > 255U) {
        return 255U;
    }

    return (unsigned char)value;
}

int plasma_is_modern_kind(screensave_renderer_kind renderer_kind)
{
    return plasma_is_modern_runtime_kind(renderer_kind);
}

void plasma_modern_plan_init(struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return;
    }

    plan->modern_capable = 0;
    plan->modern_requested = 0;
    plan->modern_enabled = 0;
    plan->modern_degraded = 0;
    plan->modern_components = 0UL;
    plan->modern_degrade_policy = 0UL;
}

void plasma_modern_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
)
{
    if (plan == NULL) {
        return;
    }

    plan->modern_capable =
        plan->selection.selected_preset != NULL &&
        plan->selection.selected_preset->modern_capable;
    plan->modern_requested =
        plasma_is_modern_kind(requested_kind) ||
        plasma_is_modern_kind(active_kind);
    plan->modern_enabled = 0;
    plan->modern_degraded = 0;
    plan->modern_components = 0UL;
    plan->modern_degrade_policy = plan->modern_capable
        ? plasma_modern_default_degrade_policy()
        : 0UL;

    if (
        module != NULL &&
        plan->modern_capable &&
        plan->advanced_enabled &&
        plasma_is_modern_kind(active_kind) &&
        screensave_saver_supports_renderer_kind(module, active_kind)
    ) {
        plan->modern_enabled = 1;
        plan->modern_components =
            PLASMA_MODERN_COMPONENT_REFINED_FIELD |
            PLASMA_MODERN_COMPONENT_REFINED_FILTER |
            PLASMA_MODERN_COMPONENT_PRESENTATION_BUFFER;
        return;
    }

    if (
        plan->modern_requested &&
        plan->modern_capable &&
        !plasma_is_modern_kind(active_kind)
    ) {
        plan->modern_degraded = 1;
    }
}

int plasma_modern_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->modern_capable != 0 && plan->modern_capable != 1) {
        return 0;
    }
    if (plan->modern_requested != 0 && plan->modern_requested != 1) {
        return 0;
    }
    if (plan->modern_enabled != 0 && plan->modern_enabled != 1) {
        return 0;
    }
    if (plan->modern_degraded != 0 && plan->modern_degraded != 1) {
        return 0;
    }

    if (plan->modern_enabled) {
        if (
            !plan->modern_capable ||
            !plan->modern_requested ||
            !plan->advanced_enabled ||
            plan->advanced_degraded ||
            !plasma_is_modern_kind(plan->active_renderer_kind) ||
            module == NULL ||
            !screensave_saver_supports_renderer_kind(module, plan->active_renderer_kind) ||
            plan->modern_degraded ||
            plan->modern_components != (
                PLASMA_MODERN_COMPONENT_REFINED_FIELD |
                PLASMA_MODERN_COMPONENT_REFINED_FILTER |
                PLASMA_MODERN_COMPONENT_PRESENTATION_BUFFER
            ) ||
            plan->modern_degrade_policy != plasma_modern_default_degrade_policy()
        ) {
            return 0;
        }

        return 1;
    }

    if (plan->modern_components != 0UL) {
        return 0;
    }

    if (plan->modern_degraded) {
        if (
            !plan->modern_capable ||
            !plan->modern_requested ||
            plasma_is_modern_kind(plan->active_renderer_kind)
        ) {
            return 0;
        }

        if (plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL21) {
            return plan->advanced_enabled && !plan->advanced_degraded;
        }

        return plasma_is_lower_band_kind(plan->active_renderer_kind);
    }

    return 1;
}

int plasma_modern_apply_field_refinement(
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
    if (!plan->modern_enabled) {
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
            unsigned int refined_value;

            index = (unsigned int)((y * width) + x);
            center_value = (unsigned int)state->field_primary[index];
            history_value = (unsigned int)state->field_history[index];
            cross_average =
                (unsigned int)plasma_modern_sample_scalar(state->field_primary, width, height, x - 1, y) +
                (unsigned int)plasma_modern_sample_scalar(state->field_primary, width, height, x + 1, y) +
                (unsigned int)plasma_modern_sample_scalar(state->field_primary, width, height, x, y - 1) +
                (unsigned int)plasma_modern_sample_scalar(state->field_primary, width, height, x, y + 1);
            cross_average /= 4U;

            refined_value =
                ((center_value * 168U) + (cross_average * 63U) + (history_value * 24U)) / 255U;
            if (center_value > cross_average) {
                refined_value += (center_value - cross_average) / 4U;
            }

            state->field_secondary[index] = plasma_modern_clamp_channel(refined_value);
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

int plasma_modern_apply_filter_refinement(
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
    if (!plan->modern_enabled) {
        return 1;
    }
    if (
        (plan->modern_components & PLASMA_MODERN_COMPONENT_REFINED_FILTER) == 0UL ||
        state->modern_treatment_buffer.pixels == NULL ||
        state->modern_treatment_buffer.size.width != visual_buffer->size.width ||
        state->modern_treatment_buffer.size.height != visual_buffer->size.height
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;

        target_row = state->modern_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->modern_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            blue_value =
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y, 0) * 5U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y, 0) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y, 0) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y - 1, 0) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y + 1, 0) * 2U) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y - 1, 0) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y - 1, 0) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y + 1, 0) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y + 1, 0);
            green_value =
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y, 1) * 5U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y, 1) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y, 1) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y - 1, 1) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y + 1, 1) * 2U) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y - 1, 1) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y - 1, 1) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y + 1, 1) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y + 1, 1);
            red_value =
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y, 2) * 5U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y, 2) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y, 2) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y - 1, 2) * 2U) +
                ((unsigned int)plasma_modern_sample_channel(visual_buffer, x, y + 1, 2) * 2U) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y - 1, 2) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y - 1, 2) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x - 1, y + 1, 2) +
                (unsigned int)plasma_modern_sample_channel(visual_buffer, x + 1, y + 1, 2);

            target_row[(x * 4) + 0] = (unsigned char)(blue_value / 17U);
            target_row[(x * 4) + 1] = (unsigned char)(green_value / 17U);
            target_row[(x * 4) + 2] = (unsigned char)(red_value / 17U);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;
        const unsigned char *source_row;

        target_row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        source_row = state->modern_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->modern_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            blue_value =
                ((unsigned int)target_row[(x * 4) + 0] * 96U) +
                ((unsigned int)source_row[(x * 4) + 0] * 159U);
            green_value =
                ((unsigned int)target_row[(x * 4) + 1] * 96U) +
                ((unsigned int)source_row[(x * 4) + 1] * 159U);
            red_value =
                ((unsigned int)target_row[(x * 4) + 2] * 96U) +
                ((unsigned int)source_row[(x * 4) + 2] * 159U);

            target_row[(x * 4) + 0] = (unsigned char)(blue_value / 255U);
            target_row[(x * 4) + 1] = (unsigned char)(green_value / 255U);
            target_row[(x * 4) + 2] = (unsigned char)(red_value / 255U);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

int plasma_modern_prepare_presentation(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
)
{
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
    if (!plan->modern_enabled) {
        return 0;
    }
    if (
        (plan->modern_components & PLASMA_MODERN_COMPONENT_PRESENTATION_BUFFER) == 0UL ||
        state->modern_presentation_buffer.pixels == NULL ||
        state->modern_presentation_buffer.size.width != treated_frame->size.width ||
        state->modern_presentation_buffer.size.height != treated_frame->size.height
    ) {
        return 0;
    }

    for (y = 0; y < treated_frame->size.height; ++y) {
        unsigned char *target_row;
        const unsigned char *source_row;

        target_row = state->modern_presentation_buffer.pixels +
            ((size_t)y * (size_t)state->modern_presentation_buffer.stride_bytes);
        source_row = treated_frame->visual_buffer->pixels +
            ((size_t)y * (size_t)treated_frame->visual_buffer->stride_bytes);
        for (x = 0; x < treated_frame->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            unsigned int luma_hint;
            unsigned int highlight_bias;

            blue_value = (unsigned int)source_row[(x * 4) + 0];
            green_value = (unsigned int)source_row[(x * 4) + 1];
            red_value = (unsigned int)source_row[(x * 4) + 2];
            luma_hint = blue_value;
            if (green_value > luma_hint) {
                luma_hint = green_value;
            }
            if (red_value > luma_hint) {
                luma_hint = red_value;
            }

            highlight_bias = luma_hint > 224U ? (luma_hint - 224U) / 4U : 0U;
            target_row[(x * 4) + 0] = plasma_modern_clamp_channel(blue_value + highlight_bias);
            target_row[(x * 4) + 1] = plasma_modern_clamp_channel(green_value + highlight_bias);
            target_row[(x * 4) + 2] = plasma_modern_clamp_channel(red_value + highlight_bias);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    screensave_visual_buffer_get_bitmap_view(&state->modern_presentation_buffer, &target_out->bitmap_view);
    target_out->destination_rect.x = 0;
    target_out->destination_rect.y = 0;
    target_out->destination_rect.width = state->drawable_size.width;
    target_out->destination_rect.height = state->drawable_size.height;
    return 1;
}
