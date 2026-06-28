#include <limits.h>
#include <stdlib.h>

#include "plasma_internal.h"

#define PLASMA_DEFAULT_DEMO_GDI_MAX_FIELD_WIDTH 1280
#define PLASMA_DEFAULT_DEMO_GDI_MAX_FIELD_HEIGHT 720
#define PLASMA_DEFAULT_DEMO_GL11_MAX_FIELD_WIDTH 1920
#define PLASMA_DEFAULT_DEMO_GL11_MAX_FIELD_HEIGHT 1080
#define PLASMA_DEFAULT_DEMO_GL21_MAX_FIELD_WIDTH 2560
#define PLASMA_DEFAULT_DEMO_GL21_MAX_FIELD_HEIGHT 1440
#define PLASMA_DEFAULT_DEMO_GL_HIGH_MAX_FIELD_WIDTH 6144
#define PLASMA_DEFAULT_DEMO_GL_HIGH_MAX_FIELD_HEIGHT 2304
#define PLASMA_DEFAULT_DEMO_MIN_FIELD_WIDTH 320
#define PLASMA_DEFAULT_DEMO_MIN_FIELD_HEIGHT 180
#define PLASMA_DEFAULT_DEMO_DOMAIN_SIZE 3072
#define PLASMA_DEFAULT_DEMO_DOMAIN_MAX 8192
#define PLASMA_DEFAULT_DEMO_EDGE_GUARD 192

static int plasma_field_size_is_safe(const screensave_sizei *size)
{
    int stride_bytes;

    if (size == NULL || size->width <= 0 || size->height <= 0) {
        return 0;
    }
    if (size->width > INT_MAX / 4) {
        return 0;
    }
    stride_bytes = size->width * 4;
    if (size->height > INT_MAX / size->width) {
        return 0;
    }
    if (size->height > INT_MAX / stride_bytes) {
        return 0;
    }
    return 1;
}

static int plasma_field_cell_count(const screensave_sizei *size)
{
    if (!plasma_field_size_is_safe(size)) {
        return 0;
    }

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

static unsigned int plasma_mix_u8(
    unsigned int left,
    unsigned int right,
    unsigned int amount
)
{
    if (amount > 255U) {
        amount = 255U;
    }

    return ((left * (255U - amount)) + (right * amount)) / 255U;
}

static int plasma_mix_int(
    int left,
    int right,
    unsigned int amount
)
{
    long mixed;

    if (amount > 255U) {
        amount = 255U;
    }

    mixed =
        ((long)left * (long)(255U - amount)) +
        ((long)right * (long)amount);
    return (int)(mixed / 255L);
}

static unsigned int plasma_smoothstep_u8(unsigned int amount)
{
    unsigned long value;

    if (amount > 255U) {
        amount = 255U;
    }

    value = (unsigned long)amount;
    return (unsigned int)((value * value * (765UL - (2UL * value))) / (255UL * 255UL));
}

static unsigned int plasma_classic_demo_curve_wave(unsigned int phase)
{
    unsigned int triangle;

    phase &= 255U;
    if (phase < 128U) {
        triangle = phase * 2U;
    } else {
        triangle = (255U - phase) * 2U;
    }

    return plasma_smoothstep_u8(triangle);
}

static int plasma_default_demo_path(
    const plasma_plan *plan,
    const plasma_execution_state *state
)
{
    if (plan == NULL || state == NULL || state->preview_mode) {
        return 0;
    }

    return plan->effect_mode == PLASMA_EFFECT_PLASMA &&
        plan->speed_mode == PLASMA_SPEED_GENTLE &&
        plan->resolution_mode == PLASMA_RESOLUTION_FINE &&
        plan->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH &&
        plan->output_family == PLASMA_OUTPUT_FAMILY_RASTER &&
        plan->output_mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER &&
        plan->presentation_mode == PLASMA_PRESENTATION_MODE_FLAT;
}

static void plasma_compute_default_demo_field_size(
    const plasma_execution_state *state,
    screensave_sizei *field_size
)
{
    int source_width;
    int source_height;
    int max_width;
    int max_height;
    long scaled;

    source_width = state->drawable_size.width > 0 ? state->drawable_size.width : 1;
    source_height = state->drawable_size.height > 0 ? state->drawable_size.height : 1;
    max_width = PLASMA_DEFAULT_DEMO_GDI_MAX_FIELD_WIDTH;
    max_height = PLASMA_DEFAULT_DEMO_GDI_MAX_FIELD_HEIGHT;
    if (state->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL11) {
        max_width = PLASMA_DEFAULT_DEMO_GL11_MAX_FIELD_WIDTH;
        max_height = PLASMA_DEFAULT_DEMO_GL11_MAX_FIELD_HEIGHT;
    } else if (state->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL21) {
        max_width = PLASMA_DEFAULT_DEMO_GL21_MAX_FIELD_WIDTH;
        max_height = PLASMA_DEFAULT_DEMO_GL21_MAX_FIELD_HEIGHT;
    } else if (
        state->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        state->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL46
    ) {
        max_width = PLASMA_DEFAULT_DEMO_GL_HIGH_MAX_FIELD_WIDTH;
        max_height = PLASMA_DEFAULT_DEMO_GL_HIGH_MAX_FIELD_HEIGHT;
    }

    field_size->width = source_width;
    field_size->height = source_height;

    if (field_size->width > max_width) {
        scaled =
            ((long)field_size->height * (long)max_width) /
            (long)field_size->width;
        field_size->width = max_width;
        field_size->height = (int)scaled;
    }
    if (field_size->height > max_height) {
        scaled =
            ((long)field_size->width * (long)max_height) /
            (long)field_size->height;
        field_size->height = max_height;
        field_size->width = (int)scaled;
    }

    if (source_width >= PLASMA_DEFAULT_DEMO_MIN_FIELD_WIDTH &&
        field_size->width < PLASMA_DEFAULT_DEMO_MIN_FIELD_WIDTH) {
        field_size->width = PLASMA_DEFAULT_DEMO_MIN_FIELD_WIDTH;
    }
    if (source_height >= PLASMA_DEFAULT_DEMO_MIN_FIELD_HEIGHT &&
        field_size->height < PLASMA_DEFAULT_DEMO_MIN_FIELD_HEIGHT) {
        field_size->height = PLASMA_DEFAULT_DEMO_MIN_FIELD_HEIGHT;
    }
    if (field_size->width < 40) {
        field_size->width = 40;
    }
    if (field_size->height < 30) {
        field_size->height = 30;
    }
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

    if (plasma_default_demo_path(plan, state)) {
        plasma_compute_default_demo_field_size(state, field_size);
        return;
    }

    divisor = plasma_resolution_divisor(plan, state);
    if (divisor == 1) {
        field_size->width = state->drawable_size.width > 0 ? state->drawable_size.width : 1;
        field_size->height = state->drawable_size.height > 0 ? state->drawable_size.height : 1;
    } else {
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
    cell_count = plasma_field_cell_count(&desired_size);
    if (cell_count <= 0) {
        return 0;
    }
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

    if (
        plan->effect_mode == PLASMA_EFFECT_PLASMA &&
        plan->speed_mode == PLASMA_SPEED_GENTLE &&
        !state->preview_mode
    ) {
        speed = 2UL;
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
    if (plan->effect_mode == PLASMA_EFFECT_PLASMA) {
        return 26000UL;
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

typedef struct plasma_demo_wave_params_tag {
    int x_coeff;
    int y_coeff;
    int fold_coeff;
    int source_coeff;
    unsigned int source_weight;
    unsigned int phase_divisor;
    unsigned int phase_offset;
    unsigned int source_id;
} plasma_demo_wave_params;

typedef struct plasma_demo_family_params_tag {
    plasma_demo_wave_params waves[4];
} plasma_demo_family_params;

typedef struct plasma_demo_profile_tag {
    int scale_delta;
    unsigned int warp_amount;
    unsigned int complexity_amount;
    unsigned int phase_bias;
} plasma_demo_profile;

typedef struct plasma_demo_runtime_wave_tag {
    int x_coeff;
    int y_coeff;
    int fold_coeff;
    int source_coeff;
    int source_x;
    int source_y;
    unsigned int source_weight;
    unsigned int phase_base;
} plasma_demo_runtime_wave;

typedef struct plasma_demo_morph_context_tag {
    plasma_demo_runtime_wave waves[4];
    plasma_demo_profile profile;
    int domain_width;
    int domain_height;
    unsigned int warp_base;
    unsigned int extra_phase_base;
} plasma_demo_morph_context;

static const plasma_demo_family_params g_plasma_demo_families[] = {
    {
        {
            { 5, 0, 0, 3, 0U, 96U, 0U, 0U },
            { 0, 4, 0, 4, 0U, 128U, 64U, 1U },
            { 3, 3, 1, 4, 32U, 160U, 128U, 2U },
            { 2, 3, 2, 5, 16U, 224U, 192U, 0U }
        }
    },
    {
        {
            { 2, 1, 0, 5, 224U, 112U, 0U, 0U },
            { 1, 2, 0, 4, 196U, 144U, 56U, 1U },
            { 2, 2, 1, 3, 176U, 176U, 112U, 2U },
            { 1, 3, 1, 4, 144U, 208U, 184U, 0U }
        }
    },
    {
        {
            { 4, 2, 0, 3, 48U, 104U, 18U, 1U },
            { 2, -5, 2, 4, 24U, 136U, 82U, 2U },
            { 3, 3, 2, 2, 72U, 168U, 146U, 0U },
            { -2, 6, 1, 3, 40U, 216U, 210U, 1U }
        }
    },
    {
        {
            { 1, 1, 4, 4, 96U, 120U, 36U, 2U },
            { 3, -2, 3, 5, 128U, 152U, 100U, 0U },
            { 2, 5, 1, 3, 84U, 184U, 164U, 1U },
            { -3, 2, 2, 4, 112U, 232U, 228U, 2U }
        }
    }
};

static unsigned int plasma_demo_hash_u8(
    const plasma_execution_state *state,
    unsigned long cycle_index,
    unsigned long salt
)
{
    unsigned long value;

    value =
        state->demo_seed ^
        (cycle_index * 1103515245UL) ^
        (salt * 2654435761UL);
    value ^= value >> 16;
    value = value * 1664525UL + 1013904223UL;
    value ^= value >> 13;
    return (unsigned int)((value >> 8) & 255UL);
}

static unsigned int plasma_demo_family_for_cycle(
    const plasma_execution_state *state,
    unsigned long cycle_index
)
{
    return plasma_demo_hash_u8(state, cycle_index, 17UL) %
        (unsigned int)(sizeof(g_plasma_demo_families) / sizeof(g_plasma_demo_families[0]));
}

static void plasma_demo_profile_for_cycle(
    const plasma_execution_state *state,
    unsigned long cycle_index,
    plasma_demo_profile *profile
)
{
    unsigned int scale;

    if (profile == NULL) {
        return;
    }

    scale = plasma_demo_hash_u8(state, cycle_index, 23UL) % 5U;
    profile->scale_delta = (int)scale - 2;
    profile->warp_amount = 20U + (plasma_demo_hash_u8(state, cycle_index, 31UL) % 76U);
    profile->complexity_amount = 24U + (plasma_demo_hash_u8(state, cycle_index, 43UL) % 104U);
    profile->phase_bias = plasma_demo_hash_u8(state, cycle_index, 59UL);
}

static void plasma_demo_profile_morph(
    const plasma_demo_profile *source,
    const plasma_demo_profile *target,
    unsigned int amount,
    plasma_demo_profile *profile
)
{
    if (source == NULL || target == NULL || profile == NULL) {
        return;
    }

    profile->scale_delta = plasma_mix_int(source->scale_delta, target->scale_delta, amount);
    profile->warp_amount = plasma_mix_u8(source->warp_amount, target->warp_amount, amount);
    profile->complexity_amount = plasma_mix_u8(source->complexity_amount, target->complexity_amount, amount);
    profile->phase_bias = plasma_mix_u8(source->phase_bias, target->phase_bias, amount);
}

static int plasma_demo_clamp_domain_size(long value)
{
    if (value < (long)PLASMA_DEFAULT_DEMO_DOMAIN_SIZE) {
        return PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    }
    if (value > (long)PLASMA_DEFAULT_DEMO_DOMAIN_MAX) {
        return PLASMA_DEFAULT_DEMO_DOMAIN_MAX;
    }
    return (int)value;
}

static void plasma_demo_domain_size(
    const plasma_execution_state *state,
    int *width_out,
    int *height_out
)
{
    int source_width;
    int source_height;
    long domain_width;
    long domain_height;

    if (width_out == NULL || height_out == NULL) {
        return;
    }

    source_width = 1;
    source_height = 1;
    if (state != NULL) {
        source_width = state->drawable_size.width > 0 ? state->drawable_size.width : state->field_size.width;
        source_height = state->drawable_size.height > 0 ? state->drawable_size.height : state->field_size.height;
    }
    if (source_width <= 0) {
        source_width = 1;
    }
    if (source_height <= 0) {
        source_height = 1;
    }

    if (source_width >= source_height) {
        domain_height = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
        domain_width =
            ((long)PLASMA_DEFAULT_DEMO_DOMAIN_SIZE * (long)source_width) /
            (long)source_height;
    } else {
        domain_width = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
        domain_height =
            ((long)PLASMA_DEFAULT_DEMO_DOMAIN_SIZE * (long)source_height) /
            (long)source_width;
    }

    *width_out = plasma_demo_clamp_domain_size(domain_width);
    *height_out = plasma_demo_clamp_domain_size(domain_height);
}

static void plasma_demo_source_position(
    const plasma_execution_state *state,
    unsigned int source_id,
    int domain_width,
    int domain_height,
    int *x_out,
    int *y_out
)
{
    unsigned int phase_x;
    unsigned int phase_y;
    long span_x;
    long span_y;

    if (x_out == NULL || y_out == NULL) {
        return;
    }

    if ((source_id % 3U) == 1U) {
        phase_x = (unsigned int)((state->source_phase_b / 7UL) & 255UL);
        phase_y = (unsigned int)(((state->source_phase_b / 11UL) + 85UL) & 255UL);
    } else if ((source_id % 3U) == 2U) {
        phase_x = (unsigned int)(((state->source_phase_c / 9UL) + 170UL) & 255UL);
        phase_y = (unsigned int)((state->source_phase_c / 13UL) & 255UL);
    } else {
        phase_x = (unsigned int)((state->source_phase_a / 5UL) & 255UL);
        phase_y = (unsigned int)(((state->source_phase_a / 8UL) + 42UL) & 255UL);
    }

    if (domain_width <= 0) {
        domain_width = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    }
    if (domain_height <= 0) {
        domain_height = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    }

    span_x = ((long)domain_width * 58L) / 100L;
    span_y = ((long)domain_height * 58L) / 100L;
    *x_out = (domain_width / 2) +
        (int)(((span_x * (long)plasma_triangle_wave(phase_x)) / 255L) - (span_x / 2L));
    *y_out = (domain_height / 2) +
        (int)(((span_y * (long)plasma_triangle_wave(phase_y)) / 255L) - (span_y / 2L));
}

static void plasma_demo_runtime_wave_morph(
    const plasma_execution_state *state,
    const plasma_demo_wave_params *source_wave,
    const plasma_demo_wave_params *target_wave,
    const plasma_demo_profile *profile,
    unsigned int amount,
    int domain_width,
    int domain_height,
    plasma_demo_runtime_wave *runtime_wave
)
{
    unsigned int phase_divisor;
    unsigned int phase_offset;
    unsigned int source_id;

    if (
        state == NULL ||
        source_wave == NULL ||
        target_wave == NULL ||
        profile == NULL ||
        runtime_wave == NULL
    ) {
        return;
    }

    runtime_wave->x_coeff =
        plasma_mix_int(source_wave->x_coeff, target_wave->x_coeff, amount) + profile->scale_delta;
    runtime_wave->y_coeff =
        plasma_mix_int(source_wave->y_coeff, target_wave->y_coeff, amount) + profile->scale_delta;
    runtime_wave->fold_coeff =
        plasma_mix_int(source_wave->fold_coeff, target_wave->fold_coeff, amount);
    runtime_wave->source_coeff =
        plasma_mix_int(source_wave->source_coeff, target_wave->source_coeff, amount);
    runtime_wave->source_weight =
        plasma_mix_u8(source_wave->source_weight, target_wave->source_weight, amount);
    phase_divisor =
        plasma_mix_u8(source_wave->phase_divisor, target_wave->phase_divisor, amount);
    phase_offset =
        plasma_mix_u8(source_wave->phase_offset, target_wave->phase_offset, amount);
    source_id = amount < 128U ? source_wave->source_id : target_wave->source_id;

    if (runtime_wave->x_coeff == 0) {
        runtime_wave->x_coeff = source_wave->x_coeff < 0 || target_wave->x_coeff < 0 ? -1 : 1;
    }
    if (runtime_wave->y_coeff == 0) {
        runtime_wave->y_coeff = source_wave->y_coeff < 0 || target_wave->y_coeff < 0 ? -1 : 1;
    }
    if (runtime_wave->fold_coeff < 0) {
        runtime_wave->fold_coeff = 0;
    }
    if (runtime_wave->source_coeff < 1) {
        runtime_wave->source_coeff = 1;
    }
    if (phase_divisor < 64U) {
        phase_divisor = 64U;
    }

    plasma_demo_source_position(
        state,
        source_id,
        domain_width,
        domain_height,
        &runtime_wave->source_x,
        &runtime_wave->source_y
    );
    runtime_wave->phase_base =
        (unsigned int)((state->phase_millis / (unsigned long)phase_divisor) & 255UL) +
        phase_offset +
        (profile->phase_bias / 3U);
}

static void plasma_demo_morph_context_build(
    const plasma_execution_state *state,
    unsigned int family_index,
    unsigned int target_family_index,
    unsigned int amount,
    int domain_width,
    int domain_height,
    plasma_demo_morph_context *context
)
{
    const plasma_demo_family_params *source_family;
    const plasma_demo_family_params *target_family;
    plasma_demo_profile source_profile;
    plasma_demo_profile target_profile;
    unsigned int family_count;
    unsigned int index;
    unsigned long cycle_index;

    if (state == NULL || context == NULL) {
        return;
    }

    family_count = (unsigned int)(sizeof(g_plasma_demo_families) / sizeof(g_plasma_demo_families[0]));
    source_family = &g_plasma_demo_families[family_index % family_count];
    target_family = &g_plasma_demo_families[target_family_index % family_count];
    cycle_index = (state->phase_millis / 160UL) / 256UL;
    context->domain_width = domain_width > 0 ? domain_width : PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    context->domain_height = domain_height > 0 ? domain_height : PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;

    plasma_demo_profile_for_cycle(state, cycle_index, &source_profile);
    plasma_demo_profile_for_cycle(state, cycle_index + 1UL, &target_profile);
    plasma_demo_profile_morph(&source_profile, &target_profile, amount, &context->profile);
    context->warp_base =
        context->profile.phase_bias +
        (unsigned int)((state->source_phase_c / 19UL) & 255UL);
    context->extra_phase_base =
        (unsigned int)((state->phase_millis / 256UL) & 255UL) +
        context->profile.phase_bias;

    for (index = 0U; index < 4U; ++index) {
        plasma_demo_runtime_wave_morph(
            state,
            &source_family->waves[index],
            &target_family->waves[index],
            &context->profile,
            amount,
            context->domain_width,
            context->domain_height,
            &context->waves[index]
        );
    }
}

static unsigned int plasma_demo_wave_value(
    const plasma_execution_state *state,
    const plasma_demo_runtime_wave *wave,
    const plasma_demo_morph_context *context,
    int x,
    int y
)
{
    int distance;
    int linear_phase;
    int source_phase;
    int domain_phase;
    int warp_phase;
    int phase;

    if (state == NULL || wave == NULL || context == NULL) {
        return 0U;
    }

    distance = plasma_abs_int(x - wave->source_x) + plasma_abs_int(y - wave->source_y);
    linear_phase =
        (x * wave->x_coeff) +
        (y * wave->y_coeff) +
        (plasma_abs_int(x - y) * wave->fold_coeff);
    source_phase = distance * wave->source_coeff;
    domain_phase = plasma_mix_int(linear_phase, source_phase, wave->source_weight);
    warp_phase =
        (((int)plasma_triangle_wave(
            (unsigned int)(((x / 13) + (y / 17)) * 5) +
            context->warp_base
        ) - 128) * (int)context->profile.warp_amount) / 128;
    phase =
        domain_phase +
        warp_phase +
        (int)wave->phase_base;

    return plasma_triangle_wave((unsigned int)phase);
}

static int plasma_demo_normalize_coord(int value, int size, int domain_size)
{
    long range;

    if (size <= 1) {
        return 0;
    }
    if (domain_size <= (PLASMA_DEFAULT_DEMO_EDGE_GUARD * 2) + 1) {
        domain_size = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    }

    range = (long)domain_size - 1L - ((long)PLASMA_DEFAULT_DEMO_EDGE_GUARD * 2L);
    return (int)(
        (long)PLASMA_DEFAULT_DEMO_EDGE_GUARD +
        (((long)value * range) / (long)(size - 1))
    );
}

static unsigned int plasma_classic_demo_equation_value(
    const plasma_execution_state *state,
    int x,
    int y,
    const plasma_demo_morph_context *context
)
{
    unsigned int value;
    unsigned int extra_value;

    if (state == NULL || context == NULL) {
        return 0U;
    }

    value =
        plasma_demo_wave_value(state, &context->waves[0], context, x, y) +
        plasma_demo_wave_value(state, &context->waves[1], context, x, y) +
        plasma_demo_wave_value(state, &context->waves[2], context, x, y) +
        plasma_demo_wave_value(state, &context->waves[3], context, x, y);
    value /= 4U;

    extra_value = plasma_triangle_wave(
        (unsigned int)plasma_abs_int((x * 2) - (y * 3)) +
        context->extra_phase_base
    );
    extra_value = (value + extra_value) / 2U;
    return plasma_mix_u8(value, extra_value, context->profile.complexity_amount);
}

static unsigned int plasma_classic_demo_field_value(
    const plasma_execution_state *state,
    int x,
    int y,
    int domain_width,
    int domain_height
)
{
    unsigned int phase_a;
    unsigned int phase_b;
    unsigned int phase_c;
    unsigned int phase_d;
    unsigned int source_a_x;
    unsigned int source_a_y;
    unsigned int source_b_x;
    unsigned int source_b_y;
    unsigned int wave_a;
    unsigned int wave_b;
    unsigned int wave_c;
    unsigned int wave_d;
    unsigned int wave_e;
    unsigned int value;
    int distance_a;
    int distance_b;
    int center_x;
    int center_y;
    int span_x;
    int span_y;

    if (state == NULL) {
        return 0U;
    }
    if (domain_width <= 0) {
        domain_width = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    }
    if (domain_height <= 0) {
        domain_height = PLASMA_DEFAULT_DEMO_DOMAIN_SIZE;
    }

    phase_a = (unsigned int)((state->phase_millis / 24UL) & 255UL);
    phase_b = (unsigned int)((state->phase_millis / 31UL) & 255UL);
    phase_c = (unsigned int)((state->phase_millis / 43UL) & 255UL);
    phase_d = (unsigned int)((state->phase_millis / 59UL) & 255UL);

    center_x = domain_width / 2;
    center_y = domain_height / 2;
    span_x = (domain_width * 31) / 100;
    span_y = (domain_height * 31) / 100;
    if (span_x < 1) {
        span_x = 1;
    }
    if (span_y < 1) {
        span_y = 1;
    }

    source_a_x = (unsigned int)(
        center_x +
        ((int)(plasma_classic_demo_curve_wave(phase_a + 33U) * (unsigned int)span_x) / 255) -
        (span_x / 2)
    );
    source_a_y = (unsigned int)(
        center_y +
        ((int)(plasma_classic_demo_curve_wave(phase_b + 97U) * (unsigned int)span_y) / 255) -
        (span_y / 2)
    );
    source_b_x = (unsigned int)(
        center_x +
        ((int)(plasma_classic_demo_curve_wave(phase_c + 151U) * (unsigned int)span_x) / 255) -
        (span_x / 2)
    );
    source_b_y = (unsigned int)(
        center_y +
        ((int)(plasma_classic_demo_curve_wave(phase_d + 211U) * (unsigned int)span_y) / 255) -
        (span_y / 2)
    );

    distance_a = plasma_abs_int(x - (int)source_a_x) + plasma_abs_int(y - (int)source_a_y);
    distance_b = plasma_abs_int(x - (int)source_b_x) + plasma_abs_int(y - (int)source_b_y);

    wave_a = plasma_classic_demo_curve_wave((unsigned int)(x / 11) + phase_a);
    wave_b = plasma_classic_demo_curve_wave((unsigned int)(y / 13) + phase_b + 37U);
    wave_c = plasma_classic_demo_curve_wave((unsigned int)((x + y) / 17) + phase_c + 73U);
    wave_d = plasma_classic_demo_curve_wave((unsigned int)(distance_a / 19) + phase_d + 109U);
    wave_e = plasma_classic_demo_curve_wave((unsigned int)(distance_b / 23) + phase_b + 149U);

    value = (wave_a + wave_b + wave_c + wave_d + wave_e) / 5U;
    value = plasma_mix_u8(value, plasma_classic_demo_curve_wave(value + phase_c), 56U);
    if (value > 255U) {
        value = 255U;
    }

    return value;
}

static void plasma_update_plasma(plasma_execution_state *state)
{
    int width;
    int height;
    int domain_width;
    int domain_height;
    int x;
    int y;
    unsigned long family_position;
    unsigned long family_cycle;
    unsigned int family_amount;
    unsigned int family_index;
    unsigned int target_family_index;
    unsigned int family_count;
    plasma_demo_morph_context morph_context;

    width = state->field_size.width;
    height = state->field_size.height;
    plasma_demo_domain_size(state, &domain_width, &domain_height);

    family_position = state->phase_millis / 192UL;
    family_cycle = family_position / 256UL;
    family_amount = plasma_smoothstep_u8((unsigned int)(family_position & 255UL));
    family_count = (unsigned int)(sizeof(g_plasma_demo_families) / sizeof(g_plasma_demo_families[0]));
    family_index = plasma_demo_family_for_cycle(state, family_cycle);
    target_family_index = plasma_demo_family_for_cycle(state, family_cycle + 1UL);
    if (family_count == 0U) {
        family_index = 0U;
        target_family_index = 0U;
    }
    plasma_demo_morph_context_build(
        state,
        family_index,
        target_family_index,
        family_amount,
        domain_width,
        domain_height,
        &morph_context
    );

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            int domain_x;
            int domain_y;
            unsigned int lava_value;
            unsigned int morph_value;
            unsigned int value;

            domain_x = plasma_demo_normalize_coord(x, width, domain_width);
            domain_y = plasma_demo_normalize_coord(y, height, domain_height);
            lava_value = plasma_classic_demo_field_value(
                state,
                domain_x,
                domain_y,
                domain_width,
                domain_height
            );
            morph_value = plasma_classic_demo_equation_value(
                state,
                domain_x,
                domain_y,
                &morph_context
            );
            value = plasma_mix_u8(lava_value, morph_value, 96U);
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

    if (plan->effect_mode == PLASMA_EFFECT_PLASMA) {
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
    state->demo_seed =
        session->plan.resolved_rng_seed ^
        (session->plan.base_seed << 3) ^
        (session->plan.stream_seed >> 5) ^
        0x504C5632UL;

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
    if (plasma_default_demo_path(&session->plan, state)) {
        state->palette_phase = (state->palette_phase + ((delta_millis * speed_units) / 48UL)) & 255UL;
        state->source_phase_a += (delta_millis * speed_units) / 37UL;
        state->source_phase_b += (delta_millis * speed_units) / 53UL;
        state->source_phase_c += (delta_millis * speed_units) / 71UL;
    } else {
        state->palette_phase = (state->palette_phase + ((delta_millis * speed_units) / 10UL) + 1UL) & 255UL;
        state->source_phase_a += (delta_millis * (speed_units + 1UL)) / 11UL + 1UL;
        state->source_phase_b += (delta_millis * (speed_units + 3UL)) / 17UL + 1UL;
        state->source_phase_c += (delta_millis * (speed_units + 5UL)) / 23UL + 1UL;
    }

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
