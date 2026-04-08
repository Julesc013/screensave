#include <stdlib.h>

#include "plasma_internal.h"

static int plasma_field_cell_count(const screensave_sizei *size)
{
    return size->width * size->height;
}

static int plasma_abs_int(int value)
{
    return value < 0 ? -value : value;
}

static unsigned int plasma_triangle_wave(unsigned int phase)
{
    phase &= 255U;
    if (phase < 64U) {
        return 128U + (phase * 2U);
    }
    if (phase < 128U) {
        return 255U - ((phase - 64U) * 2U);
    }
    if (phase < 192U) {
        return 127U - ((phase - 128U) * 2U);
    }

    return (phase - 192U) * 2U;
}

void plasma_rng_seed(plasma_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0E8BEE01UL;
}

unsigned long plasma_rng_next(plasma_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long plasma_rng_range(plasma_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return plasma_rng_next(state) % upper_bound;
}

static int plasma_resolution_divisor(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    int divisor;

    divisor = 4;
    switch (plan->resolution_mode) {
    case PLASMA_RESOLUTION_COARSE:
        divisor = 6;
        break;

    case PLASMA_RESOLUTION_FINE:
        divisor = 3;
        break;

    case PLASMA_RESOLUTION_STANDARD:
    default:
        divisor = 4;
        break;
    }

    if (plan->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        divisor += 1;
    } else if (plan->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH && divisor > 2) {
        divisor -= 1;
    }
    if (state->preview_mode) {
        divisor += 2;
    }
    if (plan->advanced_enabled && divisor > 2) {
        divisor -= 1;
    }
    if (divisor < 2) {
        divisor = 2;
    }

    return divisor;
}

static void plasma_compute_field_size(
    const plasma_plan *plan,
    const plasma_execution_state *state,
    screensave_sizei *field_size
)
{
    int divisor;

    divisor = plasma_resolution_divisor(plan, state);
    field_size->width = state->drawable_size.width / divisor;
    field_size->height = state->drawable_size.height / divisor;

    if (field_size->width < 40) {
        field_size->width = 40;
    } else if (field_size->width > 320) {
        field_size->width = 320;
    }
    if (field_size->height < 30) {
        field_size->height = 30;
    } else if (field_size->height > 240) {
        field_size->height = 240;
    }
}

static void plasma_zero_fields(plasma_execution_state *state)
{
    int cell_count;

    if (
        state == NULL ||
        state->field_primary == NULL ||
        state->field_secondary == NULL
    ) {
        return;
    }

    cell_count = plasma_field_cell_count(&state->field_size);
    ZeroMemory(state->field_primary, (size_t)cell_count);
    ZeroMemory(state->field_secondary, (size_t)cell_count);
    if (state->field_history != NULL) {
        ZeroMemory(state->field_history, (size_t)cell_count);
    }
}

static int plasma_resize_visual_state(
    const plasma_plan *plan,
    plasma_execution_state *state
)
{
    screensave_sizei desired_size;
    int cell_count;
    unsigned char *new_primary;
    unsigned char *new_secondary;
    unsigned char *new_history;
    int size_matches;
    int need_advanced_buffers;

    if (plan == NULL || state == NULL) {
        return 0;
    }

    plasma_compute_field_size(plan, state, &desired_size);
    size_matches =
        state->field_primary != NULL &&
        state->field_secondary != NULL &&
        state->field_size.width == desired_size.width &&
        state->field_size.height == desired_size.height;
    need_advanced_buffers = plan->advanced_enabled;
    if (
        size_matches &&
        (
            (!need_advanced_buffers &&
                state->field_history == NULL &&
                state->advanced_treatment_buffer.pixels == NULL) ||
            (need_advanced_buffers &&
                state->field_history != NULL &&
                state->advanced_treatment_buffer.pixels != NULL &&
                state->advanced_treatment_buffer.size.width == desired_size.width &&
                state->advanced_treatment_buffer.size.height == desired_size.height)
        )
    ) {
        return 1;
    }

    cell_count = plasma_field_cell_count(&desired_size);
    new_primary = NULL;
    new_secondary = NULL;
    new_history = NULL;
    if (!size_matches) {
        new_primary = (unsigned char *)malloc((size_t)cell_count);
        new_secondary = (unsigned char *)malloc((size_t)cell_count);
    }
    if (need_advanced_buffers && (state->field_history == NULL || !size_matches)) {
        new_history = (unsigned char *)malloc((size_t)cell_count);
    }
    if (
        (!size_matches && (new_primary == NULL || new_secondary == NULL)) ||
        (need_advanced_buffers && (state->field_history == NULL || !size_matches) && new_history == NULL)
    ) {
        free(new_primary);
        free(new_secondary);
        free(new_history);
        return 0;
    }

    if (new_primary != NULL) {
        ZeroMemory(new_primary, (size_t)cell_count);
    }
    if (new_secondary != NULL) {
        ZeroMemory(new_secondary, (size_t)cell_count);
    }
    if (new_history != NULL) {
        ZeroMemory(new_history, (size_t)cell_count);
    }

    if (state->visual_buffer.pixels == NULL) {
        if (!screensave_visual_buffer_init(&state->visual_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            free(new_history);
            return 0;
        }
    } else if (!screensave_visual_buffer_resize(&state->visual_buffer, &desired_size)) {
        free(new_primary);
        free(new_secondary);
        free(new_history);
        return 0;
    }

    if (need_advanced_buffers) {
        if (state->advanced_treatment_buffer.pixels == NULL) {
            if (!screensave_visual_buffer_init(&state->advanced_treatment_buffer, &desired_size)) {
                free(new_primary);
                free(new_secondary);
                free(new_history);
                return 0;
            }
        } else if (!screensave_visual_buffer_resize(&state->advanced_treatment_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            free(new_history);
            return 0;
        }
    } else {
        screensave_visual_buffer_dispose(&state->advanced_treatment_buffer);
        free(state->field_history);
        state->field_history = NULL;
    }

    if (new_primary != NULL) {
        free(state->field_primary);
        free(state->field_secondary);
        state->field_primary = new_primary;
        state->field_secondary = new_secondary;
    }
    if (new_history != NULL) {
        free(state->field_history);
        state->field_history = new_history;
    }
    state->field_size = desired_size;
    return 1;
}

static unsigned long plasma_speed_units(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    unsigned long speed;

    speed = 4UL;
    switch (plan->speed_mode) {
    case PLASMA_SPEED_GENTLE:
        speed = 2UL;
        break;

    case PLASMA_SPEED_LIVELY:
        speed = 7UL;
        break;

    case PLASMA_SPEED_STANDARD:
    default:
        speed = 4UL;
        break;
    }

    if (state->preview_mode && speed > 1UL) {
        speed -= 1UL;
    }

    return speed;
}

static unsigned long plasma_variation_interval_millis(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    if (plan == NULL || state == NULL) {
        return 12000UL;
    }

    if (state->preview_mode) {
        return 7000UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        return 11000UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
        return 15000UL;
    }
    return 13000UL;
}

static unsigned int plasma_fire_floor(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    unsigned int base_value;

    base_value = 180U;
    if (plan->speed_mode == PLASMA_SPEED_GENTLE) {
        base_value = 156U;
    } else if (plan->speed_mode == PLASMA_SPEED_LIVELY) {
        base_value = 208U;
    }

    if (state->preview_mode && base_value > 20U) {
        base_value -= 20U;
    }

    return base_value;
}

static void plasma_update_fire(
    const plasma_plan *plan,
    plasma_execution_state *state
)
{
    int width;
    int height;
    int x;
    int y;
    unsigned int base_value;

    width = state->field_size.width;
    height = state->field_size.height;
    base_value = plasma_fire_floor(plan, state);

    for (x = 0; x < width; ++x) {
        unsigned int heat;

        heat = base_value + (unsigned int)plasma_rng_range(&state->rng, 76UL);
        if (heat > 255U) {
            heat = 255U;
        }
        state->field_secondary[((height - 1) * width) + x] = (unsigned char)heat;
    }

    for (y = 0; y < height - 1; ++y) {
        for (x = 0; x < width; ++x) {
            int below_row;
            int left_index;
            int center_index;
            int right_index;
            int far_index;
            unsigned int value;
            unsigned int cooling;

            below_row = y + 1;
            left_index = (below_row * width) + (x > 0 ? x - 1 : x);
            center_index = (below_row * width) + x;
            right_index = (below_row * width) + (x + 1 < width ? x + 1 : x);
            far_index = ((below_row + 1 < height ? below_row + 1 : below_row) * width) + x;

            value =
                (unsigned int)state->field_primary[left_index] +
                (unsigned int)state->field_primary[center_index] +
                (unsigned int)state->field_primary[right_index] +
                (unsigned int)state->field_primary[far_index];
            value /= 4U;

            cooling = 6U;
            if (plan->speed_mode == PLASMA_SPEED_GENTLE) {
                cooling = 4U;
            } else if (plan->speed_mode == PLASMA_SPEED_LIVELY) {
                cooling = 9U;
            }

            if (value > cooling) {
                value -= cooling;
            } else {
                value = 0U;
            }

            state->field_secondary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_plasma(plasma_execution_state *state)
{
    int width;
    int height;
    int x;
    int y;
    unsigned int phase_a;
    unsigned int phase_b;
    unsigned int phase_c;
    unsigned int phase_d;

    width = state->field_size.width;
    height = state->field_size.height;
    phase_a = (unsigned int)((state->phase_millis / 7UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 11UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 5UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 13UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int value;

            value =
                plasma_triangle_wave((unsigned int)(x * 8) + phase_a) +
                plasma_triangle_wave((unsigned int)(y * 7) + phase_b) +
                plasma_triangle_wave((unsigned int)((x + y) * 5) + phase_c) +
                plasma_triangle_wave((unsigned int)((x * 3) + (y * 5)) + phase_d);
            value /= 4U;
            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_interference(plasma_execution_state *state)
{
    int width;
    int height;
    int x;
    int y;
    int source_a_x;
    int source_a_y;
    int source_b_x;
    int source_b_y;
    int source_c_x;
    int source_c_y;

    width = state->field_size.width;
    height = state->field_size.height;
    source_a_x = (int)(((unsigned long)width * plasma_triangle_wave((unsigned int)(state->source_phase_a & 255UL))) / 255UL);
    source_a_y = (int)(((unsigned long)height * plasma_triangle_wave((unsigned int)((state->source_phase_a / 2UL) & 255UL))) / 255UL);
    source_b_x = (int)(((unsigned long)width * plasma_triangle_wave((unsigned int)(state->source_phase_b & 255UL))) / 255UL);
    source_b_y = (int)(((unsigned long)height * plasma_triangle_wave((unsigned int)((state->source_phase_b / 3UL) & 255UL))) / 255UL);
    source_c_x = (int)(((unsigned long)width * plasma_triangle_wave((unsigned int)(state->source_phase_c & 255UL))) / 255UL);
    source_c_y = (int)(((unsigned long)height * plasma_triangle_wave((unsigned int)((state->source_phase_c / 5UL) & 255UL))) / 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int value;
            unsigned int wave_a;
            unsigned int wave_b;
            unsigned int wave_c;
            int distance_a;
            int distance_b;
            int distance_c;

            distance_a = plasma_abs_int(x - source_a_x) + plasma_abs_int(y - source_a_y);
            distance_b = plasma_abs_int(x - source_b_x) + plasma_abs_int(y - source_b_y);
            distance_c = plasma_abs_int(x - source_c_x) + plasma_abs_int(y - source_c_y);

            wave_a = plasma_triangle_wave((unsigned int)(distance_a * 8) + (unsigned int)(state->source_phase_a & 255UL));
            wave_b = plasma_triangle_wave((unsigned int)(distance_b * 6) + (unsigned int)(state->source_phase_b & 255UL));
            wave_c = plasma_triangle_wave((unsigned int)(distance_c * 5) + (unsigned int)(state->source_phase_c & 255UL));
            value = (wave_a + wave_b + wave_c) / 3U;
            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_apply_smoothing(
    const plasma_plan *plan,
    plasma_execution_state *state
)
{
    int width;
    int height;
    int x;
    int y;
    unsigned int blend_amount;

    if (plan->smoothing_mode == PLASMA_SMOOTHING_OFF) {
        return;
    }

    blend_amount = 72U;
    if (plan->smoothing_mode == PLASMA_SMOOTHING_GLOW) {
        blend_amount = 128U;
    }
    if (state->preview_mode && blend_amount > 64U) {
        blend_amount = 64U;
    }

    width = state->field_size.width;
    height = state->field_size.height;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            int sample_count;
            int sample_x;
            int sample_y;
            int neighbor_x;
            int neighbor_y;
            unsigned int blur_value;
            unsigned int original_value;
            unsigned int index;

            blur_value = 0U;
            sample_count = 0;
            for (sample_y = -1; sample_y <= 1; ++sample_y) {
                neighbor_y = y + sample_y;
                if (neighbor_y < 0) {
                    neighbor_y = 0;
                } else if (neighbor_y >= height) {
                    neighbor_y = height - 1;
                }

                for (sample_x = -1; sample_x <= 1; ++sample_x) {
                    neighbor_x = x + sample_x;
                    if (neighbor_x < 0) {
                        neighbor_x = 0;
                    } else if (neighbor_x >= width) {
                        neighbor_x = width - 1;
                    }

                    blur_value += (unsigned int)state->field_primary[(neighbor_y * width) + neighbor_x];
                    sample_count += 1;
                }
            }

            if (sample_count > 0) {
                blur_value /= (unsigned int)sample_count;
            }
            index = (unsigned int)((y * width) + x);
            original_value = (unsigned int)state->field_primary[index];
            state->field_secondary[index] = (unsigned char)(
                ((original_value * (255U - blend_amount)) + (blur_value * blend_amount)) / 255U
            );
        }
    }

    {
        unsigned char *swap_buffer;

        swap_buffer = state->field_primary;
        state->field_primary = state->field_secondary;
        state->field_secondary = swap_buffer;
    }
}

static void plasma_warm_start_effect(
    const plasma_plan *plan,
    plasma_execution_state *state
)
{
    int warm_steps;

    if (plan == NULL || state == NULL) {
        return;
    }

    plasma_zero_fields(state);
    warm_steps = 2;
    if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
        warm_steps = 18;
    } else if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        warm_steps = 4;
    }
    while (warm_steps-- > 0) {
        if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
            unsigned char *swap_buffer;

            plasma_update_fire(plan, state);
            swap_buffer = state->field_primary;
            state->field_primary = state->field_secondary;
            state->field_secondary = swap_buffer;
        } else if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
            plasma_update_interference(state);
        } else {
            plasma_update_plasma(state);
        }
        plasma_apply_smoothing(plan, state);
        if (!plasma_advanced_apply_field_effects(plan, state)) {
            return;
        }
        state->phase_millis += 33UL;
        state->palette_phase = (state->palette_phase + 3UL) & 255UL;
        state->source_phase_a += 5UL;
        state->source_phase_b += 3UL;
        state->source_phase_c += 7UL;
    }
}

static void plasma_refresh_composition(
    const plasma_plan *plan,
    plasma_execution_state *state
)
{
    if (plan == NULL || state == NULL) {
        return;
    }

    state->phase_millis += 96UL + plasma_rng_range(&state->rng, 320UL);
    state->palette_phase = (state->palette_phase + 32UL + plasma_rng_range(&state->rng, 96UL)) & 255UL;
    state->source_phase_a += 32UL + plasma_rng_range(&state->rng, 128UL);
    state->source_phase_b += 24UL + plasma_rng_range(&state->rng, 112UL);
    state->source_phase_c += 40UL + plasma_rng_range(&state->rng, 144UL);

    if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        state->source_phase_a += 32UL;
        state->source_phase_b += 48UL;
    } else if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
        state->palette_phase = (state->palette_phase + 18UL) & 255UL;
    }
}

int plasma_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    plasma_execution_state *state;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    *session_out = NULL;
    session = (screensave_saver_session *)malloc(sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    ZeroMemory(session, sizeof(*session));
    if (!plasma_plan_compile(&session->plan, module, environment)) {
        free(session);
        return 0;
    }

    state = &session->state;
    state->drawable_size = environment->drawable_size;
    state->active_renderer_kind = plasma_resolve_renderer_kind(environment);
    state->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    plasma_plan_bind_renderer_kind(
        &session->plan,
        module,
        plasma_resolve_requested_renderer_kind(environment),
        state->active_renderer_kind
    );
    if (!plasma_plan_validate_for_renderer_kind(&session->plan, module, state->active_renderer_kind)) {
        free(session);
        return 0;
    }

    plasma_rng_seed(&state->rng, session->plan.resolved_rng_seed);
    state->palette_phase = session->plan.base_seed & 255UL;
    state->source_phase_a = session->plan.stream_seed & 255UL;
    state->source_phase_b = (session->plan.stream_seed >> 7) & 255UL;
    state->source_phase_c = (session->plan.stream_seed >> 13) & 255UL;
    state->variation_elapsed_millis = 0UL;

    if (!plasma_resize_visual_state(&session->plan, state)) {
        plasma_destroy_session(session);
        return 0;
    }

    plasma_warm_start_effect(&session->plan, state);
    *session_out = session;
    return 1;
}

void plasma_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session->state.field_primary);
    free(session->state.field_secondary);
    free(session->state.field_history);
    screensave_visual_buffer_dispose(&session->state.advanced_treatment_buffer);
    screensave_visual_buffer_dispose(&session->state.visual_buffer);
    free(session);
}

void plasma_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    plasma_execution_state *state;

    if (session == NULL || environment == NULL) {
        return;
    }

    state = &session->state;
    state->drawable_size = environment->drawable_size;
    state->active_renderer_kind = plasma_resolve_renderer_kind(environment);
    state->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    plasma_plan_bind_renderer_kind(
        &session->plan,
        plasma_get_module(),
        plasma_resolve_requested_renderer_kind(environment),
        state->active_renderer_kind
    );
    if (!plasma_plan_validate_for_renderer_kind(&session->plan, plasma_get_module(), state->active_renderer_kind)) {
        return;
    }
    if (plasma_resize_visual_state(&session->plan, state)) {
        plasma_warm_start_effect(&session->plan, state);
    }
}

void plasma_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    plasma_execution_state *state;
    unsigned long delta_millis;
    unsigned long speed_units;

    if (session == NULL || environment == NULL) {
        return;
    }

    state = &session->state;
    delta_millis = environment->clock.delta_millis;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    speed_units = plasma_speed_units(&session->plan, state);
    state->variation_elapsed_millis += delta_millis;
    state->phase_millis += delta_millis * speed_units;
    state->palette_phase = (state->palette_phase + ((delta_millis * speed_units) / 10UL) + 1UL) & 255UL;
    state->source_phase_a += (delta_millis * (speed_units + 1UL)) / 11UL + 1UL;
    state->source_phase_b += (delta_millis * (speed_units + 3UL)) / 17UL + 1UL;
    state->source_phase_c += (delta_millis * (speed_units + 5UL)) / 23UL + 1UL;

    if (session->plan.effect_mode == PLASMA_EFFECT_FIRE) {
        unsigned char *swap_buffer;

        plasma_update_fire(&session->plan, state);
        swap_buffer = state->field_primary;
        state->field_primary = state->field_secondary;
        state->field_secondary = swap_buffer;
    } else if (session->plan.effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        plasma_update_interference(state);
    } else {
        plasma_update_plasma(state);
    }

    plasma_apply_smoothing(&session->plan, state);
    if (!plasma_advanced_apply_field_effects(&session->plan, state)) {
        return;
    }

    if (state->variation_elapsed_millis >= plasma_variation_interval_millis(&session->plan, state)) {
        state->variation_elapsed_millis = 0UL;
        plasma_refresh_composition(&session->plan, state);
    }
}
