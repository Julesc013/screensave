#include "plasma_internal.h"

static screensave_color plasma_treatment_background_color(void)
{
    screensave_color color;

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 255;
    return color;
}

static screensave_color plasma_treatment_palette_color(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    unsigned int value
)
{
    screensave_color base_color;
    screensave_color white_color;
    screensave_color highlight_color;
    unsigned int palette_index;
    unsigned int amount;

    base_color = plasma_treatment_background_color();
    white_color.red = 255;
    white_color.green = 255;
    white_color.blue = 255;
    white_color.alpha = 255;
    highlight_color = screensave_color_lerp(plan->theme->accent_color, white_color, 112U);

    palette_index = (value + (unsigned int)(state->palette_phase & 255UL)) & 255U;
    if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
        palette_index = (value + (unsigned int)((state->palette_phase / 2UL) & 255UL)) & 255U;
        if (palette_index < 64U) {
            amount = (palette_index * 255U) / 64U;
            return screensave_color_lerp(base_color, plan->theme->primary_color, amount);
        }
        if (palette_index < 176U) {
            amount = ((palette_index - 64U) * 255U) / 112U;
            return screensave_color_lerp(plan->theme->primary_color, plan->theme->accent_color, amount);
        }

        amount = ((palette_index - 176U) * 255U) / 79U;
        return screensave_color_lerp(plan->theme->accent_color, highlight_color, amount);
    }

    if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        if (palette_index < 112U) {
            amount = (palette_index * 255U) / 112U;
            return screensave_color_lerp(base_color, plan->theme->accent_color, amount);
        }
        if (palette_index < 208U) {
            amount = ((palette_index - 112U) * 255U) / 96U;
            return screensave_color_lerp(plan->theme->accent_color, plan->theme->primary_color, amount);
        }

        amount = ((palette_index - 208U) * 255U) / 47U;
        return screensave_color_lerp(plan->theme->primary_color, highlight_color, amount);
    }

    if (palette_index < 96U) {
        amount = (palette_index * 255U) / 96U;
        return screensave_color_lerp(base_color, plan->theme->primary_color, amount);
    }
    if (palette_index < 192U) {
        amount = ((palette_index - 96U) * 255U) / 96U;
        return screensave_color_lerp(plan->theme->primary_color, plan->theme->accent_color, amount);
    }

    amount = ((palette_index - 192U) * 255U) / 63U;
    return screensave_color_lerp(plan->theme->accent_color, highlight_color, amount);
}

static int plasma_theme_map_raster_output(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer
)
{
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        output == NULL ||
        visual_buffer == NULL ||
        plan->theme == NULL ||
        output->scalar_values == NULL ||
        visual_buffer->pixels == NULL
    ) {
        return 0;
    }

    if (
        visual_buffer->size.width != output->size.width ||
        visual_buffer->size.height != output->size.height
    ) {
        return 0;
    }

    for (y = 0; y < output->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < output->size.width; ++x) {
            screensave_color color;
            unsigned int value;

            value = (unsigned int)output->scalar_values[(y * output->size.width) + x];
            color = plasma_treatment_palette_color(plan, state, value);
            row[(x * 4) + 0] = color.blue;
            row[(x * 4) + 1] = color.green;
            row[(x * 4) + 2] = color.red;
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_apply_sampling_treatment(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    (void)state;
    (void)visual_buffer;

    if (plan == NULL) {
        return 0;
    }

    return plan->sampling_treatment == PLASMA_SAMPLING_TREATMENT_NONE;
}

static int plasma_apply_filter_treatment(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->filter_treatment == PLASMA_FILTER_TREATMENT_NONE) {
        return 1;
    }

    if (!plasma_advanced_apply_blur_filter(plan, state, visual_buffer)) {
        return 0;
    }

    return plasma_modern_apply_filter_refinement(plan, state, visual_buffer);
}

static int plasma_apply_emulation_treatment(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    (void)state;
    (void)visual_buffer;

    if (plan == NULL) {
        return 0;
    }

    return plan->emulation_treatment == PLASMA_EMULATION_TREATMENT_NONE;
}

static int plasma_apply_accent_treatment(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->accent_treatment == PLASMA_ACCENT_TREATMENT_NONE) {
        return 1;
    }

    return plasma_advanced_apply_overlay_accent(plan, state, visual_buffer);
}

int plasma_treatment_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE) {
        return 0;
    }
    if (
        plan->filter_treatment != PLASMA_FILTER_TREATMENT_NONE &&
        !(plan->advanced_enabled && plan->filter_treatment == PLASMA_FILTER_TREATMENT_BLUR)
    ) {
        return 0;
    }
    if (plan->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE) {
        return 0;
    }

    return
        plan->accent_treatment == PLASMA_ACCENT_TREATMENT_NONE ||
        (plan->advanced_enabled && plan->accent_treatment == PLASMA_ACCENT_TREATMENT_OVERLAY_PASS);
}

int plasma_treatment_apply(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer,
    plasma_treated_frame *frame_out
)
{
    if (
        frame_out == NULL ||
        output == NULL ||
        visual_buffer == NULL ||
        state == NULL ||
        !plasma_treatment_validate_plan(plan)
    ) {
        return 0;
    }

    if (!plasma_theme_map_raster_output(plan, state, output, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_sampling_treatment(plan, state, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_filter_treatment(plan, (struct plasma_execution_state_tag *)state, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_emulation_treatment(plan, state, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_accent_treatment(plan, (struct plasma_execution_state_tag *)state, visual_buffer)) {
        return 0;
    }
    if (!plasma_premium_apply_post_refinement(plan, (struct plasma_execution_state_tag *)state, visual_buffer)) {
        return 0;
    }

    frame_out->family = output->family;
    frame_out->mode = output->mode;
    frame_out->size = output->size;
    frame_out->visual_buffer = visual_buffer;
    return 1;
}
