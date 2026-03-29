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

static const screensave_theme_descriptor *plasma_resolve_theme(
    const screensave_saver_environment *environment
)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return plasma_find_theme_descriptor(PLASMA_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return plasma_find_theme_descriptor(PLASMA_DEFAULT_THEME_KEY);
    }

    theme = plasma_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = plasma_find_theme_descriptor(PLASMA_DEFAULT_THEME_KEY);
    }

    return theme;
}

static int plasma_resolution_divisor(
    const screensave_saver_session *session
)
{
    int divisor;

    divisor = 4;
    switch (session->config.resolution_mode) {
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

    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        divisor += 1;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH && divisor > 2) {
        divisor -= 1;
    }
    if (session->preview_mode) {
        divisor += 2;
    }
    if (divisor < 2) {
        divisor = 2;
    }

    return divisor;
}

static void plasma_compute_field_size(
    const screensave_saver_session *session,
    screensave_sizei *field_size
)
{
    int divisor;

    divisor = plasma_resolution_divisor(session);
    field_size->width = session->drawable_size.width / divisor;
    field_size->height = session->drawable_size.height / divisor;

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

static void plasma_zero_fields(screensave_saver_session *session)
{
    int cell_count;

    if (
        session == NULL ||
        session->field_primary == NULL ||
        session->field_secondary == NULL
    ) {
        return;
    }

    cell_count = plasma_field_cell_count(&session->field_size);
    ZeroMemory(session->field_primary, (size_t)cell_count);
    ZeroMemory(session->field_secondary, (size_t)cell_count);
}

static int plasma_resize_visual_state(
    screensave_saver_session *session
)
{
    screensave_sizei desired_size;
    int cell_count;
    unsigned char *new_primary;
    unsigned char *new_secondary;

    if (session == NULL) {
        return 0;
    }

    plasma_compute_field_size(session, &desired_size);
    if (
        session->field_primary != NULL &&
        session->field_secondary != NULL &&
        session->field_size.width == desired_size.width &&
        session->field_size.height == desired_size.height
    ) {
        return 1;
    }

    cell_count = plasma_field_cell_count(&desired_size);
    new_primary = (unsigned char *)malloc((size_t)cell_count);
    new_secondary = (unsigned char *)malloc((size_t)cell_count);
    if (new_primary == NULL || new_secondary == NULL) {
        free(new_primary);
        free(new_secondary);
        return 0;
    }

    ZeroMemory(new_primary, (size_t)cell_count);
    ZeroMemory(new_secondary, (size_t)cell_count);

    if (session->visual_buffer.pixels == NULL) {
        if (!screensave_visual_buffer_init(&session->visual_buffer, &desired_size)) {
            free(new_primary);
            free(new_secondary);
            return 0;
        }
    } else if (!screensave_visual_buffer_resize(&session->visual_buffer, &desired_size)) {
        free(new_primary);
        free(new_secondary);
        return 0;
    }

    free(session->field_primary);
    free(session->field_secondary);
    session->field_primary = new_primary;
    session->field_secondary = new_secondary;
    session->field_size = desired_size;
    return 1;
}

static unsigned long plasma_speed_units(
    const screensave_saver_session *session
)
{
    unsigned long speed;

    speed = 4UL;
    switch (session->config.speed_mode) {
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

    if (session->preview_mode && speed > 1UL) {
        speed -= 1UL;
    }

    return speed;
}

static unsigned int plasma_fire_floor(
    const screensave_saver_session *session
)
{
    unsigned int base_value;

    base_value = 180U;
    if (session->config.speed_mode == PLASMA_SPEED_GENTLE) {
        base_value = 156U;
    } else if (session->config.speed_mode == PLASMA_SPEED_LIVELY) {
        base_value = 208U;
    }

    if (session->preview_mode && base_value > 20U) {
        base_value -= 20U;
    }

    return base_value;
}

static void plasma_update_fire(screensave_saver_session *session)
{
    int width;
    int height;
    int x;
    int y;
    unsigned int base_value;

    width = session->field_size.width;
    height = session->field_size.height;
    base_value = plasma_fire_floor(session);

    for (x = 0; x < width; ++x) {
        unsigned int heat;

        heat = base_value + (unsigned int)plasma_rng_range(&session->rng, 76UL);
        if (heat > 255U) {
            heat = 255U;
        }
        session->field_secondary[((height - 1) * width) + x] = (unsigned char)heat;
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
                (unsigned int)session->field_primary[left_index] +
                (unsigned int)session->field_primary[center_index] +
                (unsigned int)session->field_primary[right_index] +
                (unsigned int)session->field_primary[far_index];
            value /= 4U;

            cooling = 6U;
            if (session->config.speed_mode == PLASMA_SPEED_GENTLE) {
                cooling = 4U;
            } else if (session->config.speed_mode == PLASMA_SPEED_LIVELY) {
                cooling = 9U;
            }

            if (value > cooling) {
                value -= cooling;
            } else {
                value = 0U;
            }

            session->field_secondary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_plasma(screensave_saver_session *session)
{
    int width;
    int height;
    int x;
    int y;
    unsigned int phase_a;
    unsigned int phase_b;
    unsigned int phase_c;
    unsigned int phase_d;

    width = session->field_size.width;
    height = session->field_size.height;
    phase_a = (unsigned int)((session->phase_millis / 7UL) & 255UL);
    phase_b = (unsigned int)((session->phase_millis / 11UL) & 255UL);
    phase_c = (unsigned int)((session->phase_millis / 5UL) & 255UL);
    phase_d = (unsigned int)((session->phase_millis / 13UL) & 255UL);

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            unsigned int value;

            value =
                plasma_triangle_wave((unsigned int)(x * 8) + phase_a) +
                plasma_triangle_wave((unsigned int)(y * 7) + phase_b) +
                plasma_triangle_wave((unsigned int)((x + y) * 5) + phase_c) +
                plasma_triangle_wave((unsigned int)((x * 3) + (y * 5)) + phase_d);
            value /= 4U;
            session->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_update_interference(screensave_saver_session *session)
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

    width = session->field_size.width;
    height = session->field_size.height;
    source_a_x = (int)(((unsigned long)width * plasma_triangle_wave((unsigned int)(session->source_phase_a & 255UL))) / 255UL);
    source_a_y = (int)(((unsigned long)height * plasma_triangle_wave((unsigned int)((session->source_phase_a / 2UL) & 255UL))) / 255UL);
    source_b_x = (int)(((unsigned long)width * plasma_triangle_wave((unsigned int)(session->source_phase_b & 255UL))) / 255UL);
    source_b_y = (int)(((unsigned long)height * plasma_triangle_wave((unsigned int)((session->source_phase_b / 3UL) & 255UL))) / 255UL);
    source_c_x = (int)(((unsigned long)width * plasma_triangle_wave((unsigned int)(session->source_phase_c & 255UL))) / 255UL);
    source_c_y = (int)(((unsigned long)height * plasma_triangle_wave((unsigned int)((session->source_phase_c / 5UL) & 255UL))) / 255UL);

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

            wave_a = plasma_triangle_wave((unsigned int)(distance_a * 8) + (unsigned int)(session->source_phase_a & 255UL));
            wave_b = plasma_triangle_wave((unsigned int)(distance_b * 6) + (unsigned int)(session->source_phase_b & 255UL));
            wave_c = plasma_triangle_wave((unsigned int)(distance_c * 5) + (unsigned int)(session->source_phase_c & 255UL));
            value = (wave_a + wave_b + wave_c) / 3U;
            session->field_primary[(y * width) + x] = (unsigned char)value;
        }
    }
}

static void plasma_apply_smoothing(screensave_saver_session *session)
{
    int width;
    int height;
    int x;
    int y;
    unsigned int blend_amount;

    if (session->config.smoothing_mode == PLASMA_SMOOTHING_OFF) {
        return;
    }

    blend_amount = 72U;
    if (session->config.smoothing_mode == PLASMA_SMOOTHING_GLOW) {
        blend_amount = 128U;
    }
    if (session->preview_mode && blend_amount > 64U) {
        blend_amount = 64U;
    }

    width = session->field_size.width;
    height = session->field_size.height;
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

                    blur_value += (unsigned int)session->field_primary[(neighbor_y * width) + neighbor_x];
                    sample_count += 1;
                }
            }

            if (sample_count > 0) {
                blur_value /= (unsigned int)sample_count;
            }
            index = (unsigned int)((y * width) + x);
            original_value = (unsigned int)session->field_primary[index];
            session->field_secondary[index] = (unsigned char)(
                ((original_value * (255U - blend_amount)) + (blur_value * blend_amount)) / 255U
            );
        }
    }

    {
        unsigned char *swap_buffer;

        swap_buffer = session->field_primary;
        session->field_primary = session->field_secondary;
        session->field_secondary = swap_buffer;
    }
}

static void plasma_warm_start_effect(screensave_saver_session *session)
{
    int warm_steps;

    if (session == NULL) {
        return;
    }

    plasma_zero_fields(session);
    warm_steps = session->config.effect_mode == PLASMA_EFFECT_FIRE ? 18 : 1;
    while (warm_steps-- > 0) {
        if (session->config.effect_mode == PLASMA_EFFECT_FIRE) {
            unsigned char *swap_buffer;

            plasma_update_fire(session);
            swap_buffer = session->field_primary;
            session->field_primary = session->field_secondary;
            session->field_secondary = swap_buffer;
        } else if (session->config.effect_mode == PLASMA_EFFECT_INTERFERENCE) {
            plasma_update_interference(session);
        } else {
            plasma_update_plasma(session);
        }
        plasma_apply_smoothing(session);
        session->phase_millis += 33UL;
        session->palette_phase = (session->palette_phase + 3UL) & 255UL;
        session->source_phase_a += 5UL;
        session->source_phase_b += 3UL;
        session->source_phase_c += 7UL;
    }
}

int plasma_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    const plasma_config *configured;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    *session_out = NULL;
    session = (screensave_saver_session *)malloc(sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    ZeroMemory(session, sizeof(*session));
    session->drawable_size = environment->drawable_size;
    session->detail_level =
        environment->config_binding != NULL &&
        environment->config_binding->common_config != NULL
            ? environment->config_binding->common_config->detail_level
            : SCREENSAVE_DETAIL_LEVEL_STANDARD;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = plasma_resolve_theme(environment);
    session->config.effect_mode = PLASMA_EFFECT_FIRE;
    session->config.speed_mode = PLASMA_SPEED_GENTLE;
    session->config.resolution_mode = PLASMA_RESOLUTION_STANDARD;
    session->config.smoothing_mode = PLASMA_SMOOTHING_SOFT;

    configured = NULL;
    if (
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(plasma_config)
    ) {
        configured = (const plasma_config *)environment->config_binding->product_config;
    }
    if (configured != NULL) {
        session->config = *configured;
    }

    plasma_rng_seed(&session->rng, environment->seed.stream_seed ^ environment->seed.base_seed);
    session->palette_phase = environment->seed.base_seed & 255UL;
    session->source_phase_a = environment->seed.stream_seed & 255UL;
    session->source_phase_b = (environment->seed.stream_seed >> 7) & 255UL;
    session->source_phase_c = (environment->seed.stream_seed >> 13) & 255UL;

    if (!plasma_resize_visual_state(session)) {
        plasma_destroy_session(session);
        return 0;
    }

    plasma_warm_start_effect(session);
    *session_out = session;
    return 1;
}

void plasma_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session->field_primary);
    free(session->field_secondary);
    screensave_visual_buffer_dispose(&session->visual_buffer);
    free(session);
}

void plasma_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    if (plasma_resize_visual_state(session)) {
        plasma_warm_start_effect(session);
    }
}

void plasma_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta_millis;
    unsigned long speed_units;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta_millis = environment->clock.delta_millis;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    speed_units = plasma_speed_units(session);
    session->phase_millis += delta_millis * speed_units;
    session->palette_phase = (session->palette_phase + ((delta_millis * speed_units) / 10UL) + 1UL) & 255UL;
    session->source_phase_a += (delta_millis * (speed_units + 1UL)) / 11UL + 1UL;
    session->source_phase_b += (delta_millis * (speed_units + 3UL)) / 17UL + 1UL;
    session->source_phase_c += (delta_millis * (speed_units + 5UL)) / 23UL + 1UL;

    if (session->config.effect_mode == PLASMA_EFFECT_FIRE) {
        unsigned char *swap_buffer;

        plasma_update_fire(session);
        swap_buffer = session->field_primary;
        session->field_primary = session->field_secondary;
        session->field_secondary = swap_buffer;
    } else if (session->config.effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        plasma_update_interference(session);
    } else {
        plasma_update_plasma(session);
    }

    plasma_apply_smoothing(session);
}
