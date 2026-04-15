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
        divisor += 1;
    }
    if (plan->advanced_enabled && divisor > 2) {
        divisor -= 1;
    }
    if (plan->modern_enabled && divisor > 2) {
        divisor -= 1;
    }
    if (plan->premium_enabled && divisor > 2) {
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
    int need_modern_buffers;
    int need_premium_buffers;

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
    need_modern_buffers = plan->modern_enabled;
    need_premium_buffers = plan->premium_enabled;
    if (
        size_matches &&
        (
            (!need_advanced_buffers &&
                state->field_history == NULL &&
                state->advanced_treatment_buffer.pixels == NULL &&
                state->modern_treatment_buffer.pixels == NULL &&
                state->modern_presentation_buffer.pixels == NULL &&
                state->premium_treatment_buffer.pixels == NULL &&
                state->premium_presentation_buffer.pixels == NULL) ||
            (need_advanced_buffers &&
                state->field_history != NULL &&
                state->advanced_treatment_buffer.pixels != NULL &&
                state->advanced_treatment_buffer.size.width == desired_size.width &&
                state->advanced_treatment_buffer.size.height == desired_size.height &&
                (!need_modern_buffers ||
                    (state->modern_treatment_buffer.pixels != NULL &&
                        state->modern_treatment_buffer.size.width == desired_size.width &&
                        state->modern_treatment_buffer.size.height == desired_size.height &&
                        state->modern_presentation_buffer.pixels != NULL &&
                        state->modern_presentation_buffer.size.width == desired_size.width &&
                        state->modern_presentation_buffer.size.height == desired_size.height &&
                        (!need_premium_buffers ||
                            (state->premium_treatment_buffer.pixels != NULL &&
                                state->premium_treatment_buffer.size.width == desired_size.width &&
                                state->premium_treatment_buffer.size.height == desired_size.height &&
                                state->premium_presentation_buffer.pixels != NULL &&
                                state->premium_presentation_buffer.size.width == desired_size.width &&
                                state->premium_presentation_buffer.size.height == desired_size.height)) &&
                        (need_premium_buffers ||
                            (state->premium_treatment_buffer.pixels == NULL &&
                                state->premium_presentation_buffer.pixels == NULL)))) &&
                (need_modern_buffers ||
                    (state->modern_treatment_buffer.pixels == NULL &&
                        state->modern_presentation_buffer.pixels == NULL &&
                        state->premium_treatment_buffer.pixels == NULL &&
                        state->premium_presentation_buffer.pixels == NULL)))
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

    if (need_modern_buffers) {
        if (state->modern_treatment_buffer.pixels == NULL) {
            if (!screensave_visual_buffer_init(&state->modern_treatment_buffer, &desired_size)) {
                free(new_primary);
                free(new_secondary);
                free(new_history);
                return 0;
            }
        } else if (!screensave_visual_buffer_resize(&state->modern_treatment_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            free(new_history);
            return 0;
        }
        if (state->modern_presentation_buffer.pixels == NULL) {
            if (!screensave_visual_buffer_init(&state->modern_presentation_buffer, &desired_size)) {
                free(new_primary);
                free(new_secondary);
                free(new_history);
                return 0;
            }
        } else if (!screensave_visual_buffer_resize(&state->modern_presentation_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            free(new_history);
            return 0;
        }
    } else {
        screensave_visual_buffer_dispose(&state->modern_treatment_buffer);
        screensave_visual_buffer_dispose(&state->modern_presentation_buffer);
    }

    if (need_premium_buffers) {
        if (state->premium_treatment_buffer.pixels == NULL) {
            if (!screensave_visual_buffer_init(&state->premium_treatment_buffer, &desired_size)) {
                free(new_primary);
                free(new_secondary);
                free(new_history);
                return 0;
            }
        } else if (!screensave_visual_buffer_resize(&state->premium_treatment_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            free(new_history);
            return 0;
        }
        if (state->premium_presentation_buffer.pixels == NULL) {
            if (!screensave_visual_buffer_init(&state->premium_presentation_buffer, &desired_size)) {
                free(new_primary);
                free(new_secondary);
                free(new_history);
                return 0;
            }
        } else if (!screensave_visual_buffer_resize(&state->premium_presentation_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            free(new_history);
            return 0;
        }
    } else {
        screensave_visual_buffer_dispose(&state->premium_treatment_buffer);
        screensave_visual_buffer_dispose(&state->premium_presentation_buffer);
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

    return plasma_transition_effective_speed_units(plan, state, speed);
}

static unsigned long plasma_variation_interval_millis(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    if (plan == NULL || state == NULL) {
        return 14000UL;
    }

    if (state->preview_mode) {
        return 9000UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        return 12500UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_LATTICE) {
        return 12000UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_CHEMICAL) {
        return 14000UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_CAUSTIC) {
        return 16500UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_AURORA) {
        return 13500UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_SUBSTRATE) {
        return 17500UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_ARC) {
        return 10500UL;
    }
    if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
        return 18000UL;
    }
    return 14500UL;
}

static unsigned int plasma_fire_floor(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    unsigned int base_value;

    base_value = 172U;
    if (plan->speed_mode == PLASMA_SPEED_GENTLE) {
        base_value = 150U;
    } else if (plan->speed_mode == PLASMA_SPEED_LIVELY) {
        base_value = 198U;
    }

    if (state->preview_mode && base_value > 16U) {
        base_value -= 16U;
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

static void plasma_update_chemical(plasma_execution_state *state)
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
    phase_a = (unsigned int)((state->phase_millis / 9UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 13UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 7UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 5UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int wave_a;
            unsigned int wave_b;
            unsigned int wave_c;
            unsigned int cell_seed;
            unsigned int mix_value;
            unsigned int value;

            wave_a = plasma_triangle_wave((unsigned int)(x * 7) + phase_a);
            wave_b = plasma_triangle_wave((unsigned int)(y * 9) + phase_b);
            wave_c = plasma_triangle_wave((unsigned int)((x + y) * 5) + phase_c);
            cell_seed = plasma_triangle_wave(
                (unsigned int)(((x / 4) * 19) + ((y / 4) * 23)) + phase_d
            );
            mix_value = (wave_a + wave_b + wave_c + cell_seed) / 4U;

            if (mix_value < 72U) {
                value = (mix_value * mix_value) / 72U;
            } else if (mix_value < 168U) {
                value = 160U + ((mix_value - 72U) * 80U) / 96U;
            } else {
                value = 224U + ((mix_value - 168U) * 31U) / 87U;
            }
            if (value > 255U) {
                value = 255U;
            }

            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_lattice(plasma_execution_state *state)
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
    phase_a = (unsigned int)((state->phase_millis / 8UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 12UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 6UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 10UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int axial_a;
            unsigned int axial_b;
            unsigned int diagonal;
            unsigned int diamond;
            unsigned int value;
            unsigned int lattice_hit;

            axial_a = plasma_triangle_wave((unsigned int)(x * 13) + phase_a);
            axial_b = plasma_triangle_wave((unsigned int)(y * 11) + phase_b);
            diagonal = plasma_triangle_wave((unsigned int)((x + y) * 9) + phase_c);
            diamond = plasma_triangle_wave(
                (unsigned int)((plasma_abs_int(x - (width / 2)) + plasma_abs_int(y - (height / 2))) * 7) +
                phase_d
            );
            value = (axial_a + axial_b + diagonal + diamond) / 4U;
            lattice_hit = 0U;
            if ((((unsigned int)(x + y) + (phase_a / 16U)) & 7U) == 0U) {
                lattice_hit += 44U;
            }
            if ((((unsigned int)plasma_abs_int(x - y) + (phase_b / 16U)) & 7U) == 0U) {
                lattice_hit += 28U;
            }
            value += lattice_hit;
            if (value > 255U) {
                value = 255U;
            }

            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_caustic(plasma_execution_state *state)
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
    phase_d = (unsigned int)((state->phase_millis / 9UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            int warp_x;
            int warp_y;
            unsigned int wave_a;
            unsigned int wave_b;
            unsigned int wave_c;
            unsigned int highlight;
            unsigned int value;

            warp_x = x + (((int)plasma_triangle_wave((unsigned int)(y * 9) + phase_a)) - 128) / 18;
            warp_y = y + (((int)plasma_triangle_wave((unsigned int)(x * 7) + phase_b)) - 128) / 18;
            wave_a = plasma_triangle_wave(
                (unsigned int)(plasma_abs_int((warp_x * 5) + (warp_y * 3))) + phase_c
            );
            wave_b = plasma_triangle_wave(
                (unsigned int)(plasma_abs_int((warp_x * 2) - (warp_y * 7))) + phase_d
            );
            wave_c = plasma_triangle_wave(
                (unsigned int)(plasma_abs_int((warp_x * 4) - (warp_y * 4))) + phase_a
            );
            highlight = (unsigned int)plasma_abs_int((int)wave_a - (int)wave_b);
            value = (wave_a + wave_b + wave_c) / 3U;
            if (highlight > 96U) {
                value += (highlight - 96U) / 2U;
            }
            if (value > 255U) {
                value = 255U;
            }

            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_aurora(plasma_execution_state *state)
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
    phase_a = (unsigned int)((state->phase_millis / 6UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 10UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 15UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 8UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int column_wave;
            unsigned int vertical_wave;
            unsigned int fold_wave;
            unsigned int curtain_depth;
            unsigned int highlight;
            unsigned int value;

            column_wave = plasma_triangle_wave((unsigned int)(x * 9) + phase_a);
            vertical_wave = plasma_triangle_wave((unsigned int)(y * 5) + phase_b);
            fold_wave = plasma_triangle_wave((unsigned int)((x * 3) + (y * 7)) + phase_c);
            curtain_depth = plasma_triangle_wave(
                (unsigned int)(plasma_abs_int(x - (width / 2)) * 6) + phase_d
            );

            value = (column_wave + fold_wave + (255U - curtain_depth)) / 3U;
            value = (value + ((vertical_wave * (unsigned int)(y + 1)) / (unsigned int)height)) / 2U;
            highlight = 0U;
            if ((((unsigned int)x + (phase_a / 12U)) & 15U) < 2U) {
                highlight += 28U;
            }
            if (y > height / 3) {
                highlight += (unsigned int)((y - (height / 3)) * 40) / (unsigned int)height;
            }
            value += highlight;
            if (value > 255U) {
                value = 255U;
            }

            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_substrate(plasma_execution_state *state)
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
    phase_a = (unsigned int)((state->phase_millis / 11UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 7UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 13UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 17UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int bed_wave;
            unsigned int grain_wave;
            unsigned int vein_a;
            unsigned int vein_b;
            unsigned int branch_gap;
            unsigned int value;

            bed_wave = plasma_triangle_wave((unsigned int)((x * 4) + (y * 6)) + phase_a);
            grain_wave = plasma_triangle_wave(
                (unsigned int)(((x / 3) * 29) + ((y / 3) * 17)) + phase_b
            );
            vein_a = plasma_triangle_wave(
                (unsigned int)plasma_abs_int((x * 9) - (y * 7)) + phase_c
            );
            vein_b = plasma_triangle_wave(
                (unsigned int)plasma_abs_int((x * 5) + (y * 11)) + phase_d
            );
            branch_gap = (unsigned int)plasma_abs_int((int)vein_a - (int)vein_b);

            value = (bed_wave + grain_wave) / 2U;
            if (branch_gap < 44U) {
                value += (44U - branch_gap) * 3U;
            } else if (branch_gap > 180U) {
                value += (branch_gap - 180U) / 2U;
            }
            if ((((unsigned int)x + (unsigned int)(y * 2) + (phase_a / 10U)) & 31U) == 0U) {
                value += 22U;
            }
            if (value > 255U) {
                value = 255U;
            }

            state->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_arc(plasma_execution_state *state)
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
    phase_a = (unsigned int)((state->phase_millis / 5UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 9UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 4UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 12UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int discharge_a;
            unsigned int discharge_b;
            unsigned int filament;
            unsigned int branch;
            unsigned int value;

            discharge_a = (unsigned int)plasma_abs_int(
                (int)plasma_triangle_wave((unsigned int)(x * 17) + phase_a) -
                (int)plasma_triangle_wave((unsigned int)(y * 11) + phase_b)
            );
            discharge_b = (unsigned int)plasma_abs_int(
                (int)plasma_triangle_wave((unsigned int)((x + y) * 9) + phase_c) -
                (int)plasma_triangle_wave(
                    (unsigned int)(plasma_abs_int(x - y) * 13) + phase_d
                )
            );
            filament = 255U - ((discharge_a + discharge_b) / 2U);
            branch = plasma_triangle_wave((unsigned int)((x * 5) + (y * 3)) + phase_c);
            if (filament < 96U) {
                value = filament / 2U;
            } else {
                value = 148U + ((filament - 96U) * 107U) / 159U;
            }
            if (branch > 220U) {
                value += (branch - 220U) * 2U;
            }
            if (value > 255U) {
                value = 255U;
            }

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
    int smoothing_enabled;

    if (!plasma_transition_resolve_smoothing(plan, state, &smoothing_enabled, &blend_amount)) {
        return;
    }
    if (!smoothing_enabled) {
        return;
    }
    if (state->preview_mode && blend_amount > 80U) {
        blend_amount = 80U;
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
    } else if (plan->effect_mode == PLASMA_EFFECT_CHEMICAL) {
        warm_steps = 5;
    } else if (plan->effect_mode == PLASMA_EFFECT_AURORA) {
        warm_steps = 4;
    } else if (plan->effect_mode == PLASMA_EFFECT_SUBSTRATE) {
        warm_steps = 6;
    } else if (plan->effect_mode == PLASMA_EFFECT_ARC) {
        warm_steps = 5;
    } else if (
        plan->effect_mode == PLASMA_EFFECT_LATTICE ||
        plan->effect_mode == PLASMA_EFFECT_CAUSTIC
    ) {
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
        } else if (plan->effect_mode == PLASMA_EFFECT_CHEMICAL) {
            plasma_update_chemical(state);
        } else if (plan->effect_mode == PLASMA_EFFECT_LATTICE) {
            plasma_update_lattice(state);
        } else if (plan->effect_mode == PLASMA_EFFECT_CAUSTIC) {
            plasma_update_caustic(state);
        } else if (plan->effect_mode == PLASMA_EFFECT_AURORA) {
            plasma_update_aurora(state);
        } else if (plan->effect_mode == PLASMA_EFFECT_SUBSTRATE) {
            plasma_update_substrate(state);
        } else if (plan->effect_mode == PLASMA_EFFECT_ARC) {
            plasma_update_arc(state);
        } else {
            plasma_update_plasma(state);
        }
        plasma_apply_smoothing(plan, state);
        if (!plasma_advanced_apply_field_effects(plan, state)) {
            return;
        }
        if (!plasma_modern_apply_field_refinement(plan, state)) {
            return;
        }
        if (!plasma_premium_apply_field_refinement(plan, state)) {
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
    } else if (plan->effect_mode == PLASMA_EFFECT_LATTICE) {
        state->source_phase_a += 16UL;
        state->source_phase_b += 56UL;
        state->source_phase_c += 24UL;
    } else if (plan->effect_mode == PLASMA_EFFECT_CAUSTIC) {
        state->source_phase_a += 48UL;
        state->source_phase_b += 28UL;
        state->source_phase_c += 36UL;
        state->palette_phase = (state->palette_phase + 22UL) & 255UL;
    } else if (plan->effect_mode == PLASMA_EFFECT_CHEMICAL) {
        state->source_phase_a += 20UL;
        state->source_phase_b += 24UL;
        state->source_phase_c += 28UL;
    } else if (plan->effect_mode == PLASMA_EFFECT_AURORA) {
        state->source_phase_a += 40UL;
        state->source_phase_b += 18UL;
        state->source_phase_c += 46UL;
        state->palette_phase = (state->palette_phase + 12UL) & 255UL;
    } else if (plan->effect_mode == PLASMA_EFFECT_SUBSTRATE) {
        state->source_phase_a += 26UL;
        state->source_phase_b += 34UL;
        state->source_phase_c += 20UL;
    } else if (plan->effect_mode == PLASMA_EFFECT_ARC) {
        state->source_phase_a += 54UL;
        state->source_phase_b += 42UL;
        state->source_phase_c += 60UL;
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
    plasma_transition_runtime_bind(&state->transition, &session->plan);

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
    screensave_visual_buffer_dispose(&session->state.modern_treatment_buffer);
    screensave_visual_buffer_dispose(&session->state.modern_presentation_buffer);
    screensave_visual_buffer_dispose(&session->state.premium_treatment_buffer);
    screensave_visual_buffer_dispose(&session->state.premium_presentation_buffer);
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
    plasma_transition_runtime_bind(&state->transition, &session->plan);
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
    unsigned long transition_flags;

    if (session == NULL || environment == NULL) {
        return;
    }

    state = &session->state;
    delta_millis = environment->clock.delta_millis;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    transition_flags = plasma_transition_step(&session->plan, state, plasma_get_module(), environment);
    if ((transition_flags & PLASMA_TRANSITION_RUNTIME_REQUIRE_RESIZE) != 0UL) {
        if (!plasma_resize_visual_state(&session->plan, state)) {
            return;
        }
    }
    if ((transition_flags & PLASMA_TRANSITION_RUNTIME_REQUIRE_WARM_START) != 0UL) {
        plasma_warm_start_effect(&session->plan, state);
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
    } else if (session->plan.effect_mode == PLASMA_EFFECT_CHEMICAL) {
        plasma_update_chemical(state);
    } else if (session->plan.effect_mode == PLASMA_EFFECT_LATTICE) {
        plasma_update_lattice(state);
    } else if (session->plan.effect_mode == PLASMA_EFFECT_CAUSTIC) {
        plasma_update_caustic(state);
    } else if (session->plan.effect_mode == PLASMA_EFFECT_AURORA) {
        plasma_update_aurora(state);
    } else if (session->plan.effect_mode == PLASMA_EFFECT_SUBSTRATE) {
        plasma_update_substrate(state);
    } else if (session->plan.effect_mode == PLASMA_EFFECT_ARC) {
        plasma_update_arc(state);
    } else {
        plasma_update_plasma(state);
    }

    plasma_apply_smoothing(&session->plan, state);
    if (!plasma_advanced_apply_field_effects(&session->plan, state)) {
        return;
    }
    if (!plasma_modern_apply_field_refinement(&session->plan, state)) {
        return;
    }
    if (!plasma_premium_apply_field_refinement(&session->plan, state)) {
        return;
    }

    if (state->variation_elapsed_millis >= plasma_variation_interval_millis(&session->plan, state)) {
        state->variation_elapsed_millis = 0UL;
        plasma_refresh_composition(&session->plan, state);
    }
}
