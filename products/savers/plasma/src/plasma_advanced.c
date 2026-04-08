#include "plasma_internal.h"

static int plasma_is_advanced_request_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

static unsigned long plasma_advanced_default_degrade_policy(void)
{
    return
        PLASMA_ADVANCED_DEGRADE_DROP_HISTORY |
        PLASMA_ADVANCED_DEGRADE_DROP_DOMAIN_WARP |
        PLASMA_ADVANCED_DEGRADE_DROP_FLOW_TURBULENCE |
        PLASMA_ADVANCED_DEGRADE_DROP_GLOW_POST;
}

static unsigned char plasma_advanced_sample_scalar(
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

static unsigned char plasma_advanced_clamp_channel(unsigned int value)
{
    if (value > 255U) {
        return 255U;
    }

    return (unsigned char)value;
}

int plasma_is_advanced_kind(screensave_renderer_kind renderer_kind)
{
    return plasma_is_advanced_runtime_kind(renderer_kind);
}

void plasma_advanced_plan_init(struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return;
    }

    plan->requested_renderer_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
    plan->active_renderer_kind = SCREENSAVE_RENDERER_KIND_GDI;
    plan->advanced_capable = 0;
    plan->advanced_requested = 0;
    plan->advanced_enabled = 0;
    plan->advanced_degraded = 0;
    plan->advanced_components = 0UL;
    plan->advanced_degrade_policy = 0UL;
}

void plasma_advanced_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
)
{
    if (plan == NULL) {
        return;
    }

    plan->requested_renderer_kind = requested_kind;
    plan->active_renderer_kind = active_kind;
    plan->advanced_capable =
        plan->selection.selected_preset != NULL &&
        plan->selection.selected_preset->advanced_capable;
    plan->advanced_requested =
        plasma_is_advanced_request_kind(requested_kind) ||
        plasma_is_advanced_kind(active_kind);
    plan->advanced_enabled = 0;
    plan->advanced_degraded = 0;
    plan->advanced_components = 0UL;
    plan->advanced_degrade_policy = plan->advanced_capable
        ? plasma_advanced_default_degrade_policy()
        : 0UL;

    plan->output_family = PLASMA_OUTPUT_FAMILY_RASTER;
    plan->output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    plan->sampling_treatment = PLASMA_SAMPLING_TREATMENT_NONE;
    plan->filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    plan->emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    plan->accent_treatment = PLASMA_ACCENT_TREATMENT_NONE;
    plan->presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;

    if (
        module != NULL &&
        plan->advanced_capable &&
        plasma_is_advanced_kind(active_kind) &&
        screensave_saver_supports_renderer_kind(module, active_kind)
    ) {
        plan->advanced_enabled = 1;
        plan->advanced_components =
            PLASMA_ADVANCED_COMPONENT_HISTORY_FEEDBACK |
            PLASMA_ADVANCED_COMPONENT_DOMAIN_WARP |
            PLASMA_ADVANCED_COMPONENT_FLOW_TURBULENCE |
            PLASMA_ADVANCED_COMPONENT_GLOW_POST;
        plan->filter_treatment = PLASMA_FILTER_TREATMENT_BLUR;
        plan->accent_treatment = PLASMA_ACCENT_TREATMENT_OVERLAY_PASS;
        return;
    }

    if (
        plan->advanced_requested &&
        plan->advanced_capable &&
        plasma_is_lower_band_kind(active_kind)
    ) {
        plan->advanced_degraded = 1;
    }
}

int plasma_advanced_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    if (plan == NULL) {
        return 0;
    }

    if (plan->advanced_capable != 0 && plan->advanced_capable != 1) {
        return 0;
    }
    if (
        plan->advanced_requested != 0 && plan->advanced_requested != 1
    ) {
        return 0;
    }
    if (
        plan->advanced_enabled != 0 && plan->advanced_enabled != 1
    ) {
        return 0;
    }
    if (
        plan->advanced_degraded != 0 && plan->advanced_degraded != 1
    ) {
        return 0;
    }

    if (plan->advanced_enabled) {
        if (
            !plan->advanced_capable ||
            !plan->advanced_requested ||
            !plasma_is_advanced_kind(plan->active_renderer_kind) ||
            module == NULL ||
            !screensave_saver_supports_renderer_kind(module, plan->active_renderer_kind) ||
            plan->advanced_degraded ||
            plan->advanced_components != (
                PLASMA_ADVANCED_COMPONENT_HISTORY_FEEDBACK |
                PLASMA_ADVANCED_COMPONENT_DOMAIN_WARP |
                PLASMA_ADVANCED_COMPONENT_FLOW_TURBULENCE |
                PLASMA_ADVANCED_COMPONENT_GLOW_POST
            ) ||
            plan->advanced_degrade_policy != plasma_advanced_default_degrade_policy() ||
            plan->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
            plan->filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
            plan->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
            plan->accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
            plan->output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
            plan->output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
            plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT
        ) {
            return 0;
        }

        return 1;
    }

    if (
        plan->advanced_components != 0UL ||
        plan->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
        plan->filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan->accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT
    ) {
        return 0;
    }

    if (
        plan->advanced_degraded &&
        (!plan->advanced_capable ||
            !plan->advanced_requested ||
            !plasma_is_lower_band_kind(plan->active_renderer_kind))
    ) {
        return 0;
    }

    return 1;
}

int plasma_advanced_apply_field_effects(
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
    if (!plan->advanced_enabled) {
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
            unsigned int value;
            unsigned int history_value;
            int offset_x;
            int offset_y;
            unsigned int sample_value;

            index = (unsigned int)((y * width) + x);
            value = (unsigned int)state->field_primary[index];
            history_value = (unsigned int)state->field_history[index];

            if ((plan->advanced_components & PLASMA_ADVANCED_COMPONENT_HISTORY_FEEDBACK) != 0UL) {
                value = ((value * 216U) + (history_value * 39U)) / 255U;
            }

            offset_x = 0;
            offset_y = 0;
            if ((plan->advanced_components & PLASMA_ADVANCED_COMPONENT_DOMAIN_WARP) != 0UL) {
                offset_x += (((int)history_value + (x * 3) + (int)(state->palette_phase & 7UL)) & 7) - 3;
                offset_y += (((int)value + (y * 5) + (int)((state->phase_millis / 9UL) & 7UL)) & 7) - 3;
            }
            if ((plan->advanced_components & PLASMA_ADVANCED_COMPONENT_FLOW_TURBULENCE) != 0UL) {
                offset_x += (((int)(state->source_phase_b + index + value) & 3) - 1);
                offset_y += (((int)(state->source_phase_c + index + history_value) & 3) - 1);
            }

            if (offset_x != 0 || offset_y != 0) {
                sample_value = (unsigned int)plasma_advanced_sample_scalar(
                    state->field_primary,
                    width,
                    height,
                    x + (offset_x / 2),
                    y + (offset_y / 2)
                );
                value = ((value * 164U) + (sample_value * 91U)) / 255U;
            }

            state->field_secondary[index] = (unsigned char)value;
        }
    }

    {
        unsigned char *swap_buffer;

        swap_buffer = state->field_primary;
        state->field_primary = state->field_secondary;
        state->field_secondary = swap_buffer;
    }

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int index;
            unsigned int history_value;
            unsigned int current_value;

            index = (unsigned int)((y * width) + x);
            history_value = (unsigned int)state->field_history[index];
            current_value = (unsigned int)state->field_primary[index];
            state->field_history[index] = (unsigned char)(((history_value * 175U) + (current_value * 80U)) / 255U);
        }
    }

    return 1;
}

int plasma_advanced_apply_blur_filter(
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
    if (plan->filter_treatment == PLASMA_FILTER_TREATMENT_NONE) {
        return 1;
    }
    if (
        !plan->advanced_enabled ||
        plan->filter_treatment != PLASMA_FILTER_TREATMENT_BLUR ||
        state->advanced_treatment_buffer.pixels == NULL ||
        state->advanced_treatment_buffer.size.width != visual_buffer->size.width ||
        state->advanced_treatment_buffer.size.height != visual_buffer->size.height
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;

        target_row = state->advanced_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->advanced_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            int sample_x;
            int sample_y;
            unsigned int blue_total;
            unsigned int green_total;
            unsigned int red_total;
            unsigned int sample_count;

            blue_total = 0U;
            green_total = 0U;
            red_total = 0U;
            sample_count = 0U;
            for (sample_y = -1; sample_y <= 1; ++sample_y) {
                int neighbor_y;

                neighbor_y = y + sample_y;
                if (neighbor_y < 0) {
                    neighbor_y = 0;
                } else if (neighbor_y >= visual_buffer->size.height) {
                    neighbor_y = visual_buffer->size.height - 1;
                }

                for (sample_x = -1; sample_x <= 1; ++sample_x) {
                    const unsigned char *source_row;
                    int neighbor_x;

                    neighbor_x = x + sample_x;
                    if (neighbor_x < 0) {
                        neighbor_x = 0;
                    } else if (neighbor_x >= visual_buffer->size.width) {
                        neighbor_x = visual_buffer->size.width - 1;
                    }

                    source_row = visual_buffer->pixels + ((size_t)neighbor_y * (size_t)visual_buffer->stride_bytes);
                    blue_total += (unsigned int)source_row[(neighbor_x * 4) + 0];
                    green_total += (unsigned int)source_row[(neighbor_x * 4) + 1];
                    red_total += (unsigned int)source_row[(neighbor_x * 4) + 2];
                    sample_count += 1U;
                }
            }

            target_row[(x * 4) + 0] = (unsigned char)(blue_total / sample_count);
            target_row[(x * 4) + 1] = (unsigned char)(green_total / sample_count);
            target_row[(x * 4) + 2] = (unsigned char)(red_total / sample_count);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;
        const unsigned char *source_row;

        target_row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        source_row = state->advanced_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->advanced_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            target_row[(x * 4) + 0] = source_row[(x * 4) + 0];
            target_row[(x * 4) + 1] = source_row[(x * 4) + 1];
            target_row[(x * 4) + 2] = source_row[(x * 4) + 2];
            target_row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

int plasma_advanced_apply_overlay_accent(
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
    if (plan->accent_treatment == PLASMA_ACCENT_TREATMENT_NONE) {
        return 1;
    }
    if (
        !plan->advanced_enabled ||
        (plan->advanced_components & PLASMA_ADVANCED_COMPONENT_GLOW_POST) == 0UL ||
        plan->accent_treatment != PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        plan->theme == NULL ||
        state->advanced_treatment_buffer.pixels == NULL ||
        state->advanced_treatment_buffer.size.width != visual_buffer->size.width ||
        state->advanced_treatment_buffer.size.height != visual_buffer->size.height
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *target_row;
        const unsigned char *source_row;

        target_row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        source_row = state->advanced_treatment_buffer.pixels +
            ((size_t)y * (size_t)state->advanced_treatment_buffer.stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int intensity;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            intensity = (unsigned int)source_row[(x * 4) + 0];
            if ((unsigned int)source_row[(x * 4) + 1] > intensity) {
                intensity = (unsigned int)source_row[(x * 4) + 1];
            }
            if ((unsigned int)source_row[(x * 4) + 2] > intensity) {
                intensity = (unsigned int)source_row[(x * 4) + 2];
            }

            blue_value =
                (unsigned int)target_row[(x * 4) + 0] +
                (((unsigned int)plan->theme->accent_color.blue * intensity) / 255U) / 5U;
            green_value =
                (unsigned int)target_row[(x * 4) + 1] +
                (((unsigned int)plan->theme->accent_color.green * intensity) / 255U) / 5U;
            red_value =
                (unsigned int)target_row[(x * 4) + 2] +
                (((unsigned int)plan->theme->accent_color.red * intensity) / 255U) / 5U;

            target_row[(x * 4) + 0] = plasma_advanced_clamp_channel(blue_value);
            target_row[(x * 4) + 1] = plasma_advanced_clamp_channel(green_value);
            target_row[(x * 4) + 2] = plasma_advanced_clamp_channel(red_value);
            target_row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}
