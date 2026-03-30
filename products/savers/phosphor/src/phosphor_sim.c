#include <stdlib.h>

#include "phosphor_internal.h"

#define PHOSPHOR_PHASE_ONE 256UL
#define PHOSPHOR_PHASE_CYCLE (256UL * PHOSPHOR_PHASE_ONE)
#define PHOSPHOR_WAVE_ONE 1008L

static screensave_color phosphor_background_color(
    const screensave_saver_session *session
)
{
    screensave_color color;

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "drafting_board") == 0) {
        color.red = 230;
        color.green = 236;
        color.blue = 242;
    } else if (lstrcmpiA(session->theme->theme_key, "white_instrument") == 0) {
        color.red = 10;
        color.green = 14;
        color.blue = 16;
    } else if (lstrcmpiA(session->theme->theme_key, "amber_harmonics") == 0) {
        color.red = 10;
        color.green = 8;
        color.blue = 4;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_quiet") == 0) {
        color.red = 22;
        color.green = 26;
        color.blue = 28;
    } else if (lstrcmpiA(session->theme->theme_key, "blue_lab") == 0) {
        color.red = 6;
        color.green = 12;
        color.blue = 18;
    }

    return color;
}

static unsigned char phosphor_fade_channel(
    unsigned char current_value,
    unsigned char target_value,
    unsigned int keep_scale
)
{
    if (current_value >= target_value) {
        return (unsigned char)(target_value + (((unsigned int)(current_value - target_value) * keep_scale) / 255U));
    }

    return (unsigned char)(target_value - (((unsigned int)(target_value - current_value) * keep_scale) / 255U));
}

static void phosphor_decay_to_background(
    screensave_saver_session *session
)
{
    int x;
    int y;
    unsigned int keep_scale;
    screensave_color background;

    if (session == NULL || session->visual_buffer.pixels == NULL) {
        return;
    }

    keep_scale = 184U;
    switch (session->config.persistence_mode) {
    case PHOSPHOR_PERSISTENCE_SHORT:
        keep_scale = 136U;
        break;

    case PHOSPHOR_PERSISTENCE_LONG:
        keep_scale = 224U;
        break;

    case PHOSPHOR_PERSISTENCE_STANDARD:
    default:
        keep_scale = 184U;
        break;
    }

    if (session->config.curve_mode == PHOSPHOR_CURVE_DENSE && keep_scale < 236U) {
        keep_scale += 12U;
    } else if (session->config.curve_mode == PHOSPHOR_CURVE_HARMONOGRAPH && keep_scale > 12U) {
        keep_scale -= 12U;
    }

    if (session->preview_mode && keep_scale > 20U) {
        keep_scale -= 28U;
    }

    background = phosphor_background_color(session);
    for (y = 0; y < session->visual_buffer.size.height; ++y) {
        unsigned char *row;

        row = session->visual_buffer.pixels + ((size_t)y * (size_t)session->visual_buffer.stride_bytes);
        for (x = 0; x < session->visual_buffer.size.width; ++x) {
            row[(x * 4) + 0] = phosphor_fade_channel(row[(x * 4) + 0], background.blue, keep_scale);
            row[(x * 4) + 1] = phosphor_fade_channel(row[(x * 4) + 1], background.green, keep_scale);
            row[(x * 4) + 2] = phosphor_fade_channel(row[(x * 4) + 2], background.red, keep_scale);
            row[(x * 4) + 3] = 255U;
        }
    }
}

static long phosphor_wave(unsigned long phase)
{
    unsigned long coarse_phase;
    long distance;
    long value;

    coarse_phase = (phase / PHOSPHOR_PHASE_ONE) & 255UL;
    if (coarse_phase < 128UL) {
        distance = (long)coarse_phase;
    } else {
        distance = 255L - (long)coarse_phase;
    }

    value = (distance * (127L - distance)) / 4L;
    if (coarse_phase >= 128UL) {
        value = -value;
    }

    return value;
}

void phosphor_rng_seed(phosphor_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0C5110D5UL;
}

unsigned long phosphor_rng_next(phosphor_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long phosphor_rng_range(
    phosphor_rng_state *state,
    unsigned long upper_bound
)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return phosphor_rng_next(state) % upper_bound;
}

static const screensave_theme_descriptor *phosphor_resolve_theme(
    const screensave_saver_environment *environment
)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return phosphor_find_theme_descriptor(PHOSPHOR_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return phosphor_find_theme_descriptor(PHOSPHOR_DEFAULT_THEME_KEY);
    }

    theme = phosphor_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = phosphor_find_theme_descriptor(PHOSPHOR_DEFAULT_THEME_KEY);
    }

    return theme;
}

static void phosphor_seed_ratios(
    screensave_saver_session *session
)
{
    if (session == NULL) {
        return;
    }

    if (session->config.curve_mode == PHOSPHOR_CURVE_HARMONOGRAPH) {
        session->ratio_a = 2U + (unsigned int)phosphor_rng_range(&session->rng, 3UL);
        session->ratio_b = 3U + (unsigned int)phosphor_rng_range(&session->rng, 3UL);
        session->ratio_c = 4U + (unsigned int)phosphor_rng_range(&session->rng, 3UL);
        session->ratio_d = 5U + (unsigned int)phosphor_rng_range(&session->rng, 3UL);
    } else if (session->config.curve_mode == PHOSPHOR_CURVE_DENSE) {
        session->ratio_a = 5U + (unsigned int)phosphor_rng_range(&session->rng, 4UL);
        session->ratio_b = 7U + (unsigned int)phosphor_rng_range(&session->rng, 4UL);
        session->ratio_c = 9U + (unsigned int)phosphor_rng_range(&session->rng, 4UL);
        session->ratio_d = 11U + (unsigned int)phosphor_rng_range(&session->rng, 4UL);
    } else {
        session->ratio_a = 2U + (unsigned int)phosphor_rng_range(&session->rng, 4UL);
        session->ratio_b = 3U + (unsigned int)phosphor_rng_range(&session->rng, 4UL);
        session->ratio_c = 1U + (unsigned int)phosphor_rng_range(&session->rng, 3UL);
        session->ratio_d = 2U + (unsigned int)phosphor_rng_range(&session->rng, 3UL);
    }

    if (session->ratio_a == session->ratio_b) {
        session->ratio_b += 1U;
    }
    if (session->ratio_c == session->ratio_d) {
        session->ratio_d += 1U;
    }
}

static unsigned int phosphor_sample_count(
    const screensave_saver_session *session
)
{
    unsigned int sample_count;

    sample_count = 160U;
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        sample_count = 88U;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        sample_count = 240U;
    }

    if (session->config.curve_mode == PHOSPHOR_CURVE_DENSE && sample_count < 280U) {
        sample_count += 40U;
    }
    if (session->preview_mode && sample_count > 104U) {
        sample_count = 104U;
    }

    return sample_count;
}

static void phosphor_sample_curve_point(
    const screensave_saver_session *session,
    unsigned int sample_index,
    unsigned int sample_count,
    screensave_pointi *point_out
)
{
    unsigned long t;
    long wave_x;
    long wave_y;
    long secondary_x;
    long secondary_y;
    long radius_x;
    long radius_y;
    long center_x;
    long center_y;
    long fade_primary;
    long fade_secondary;

    t = ((unsigned long)sample_index * PHOSPHOR_PHASE_CYCLE) / (unsigned long)(sample_count > 0U ? sample_count : 1U);
    secondary_x = 0L;
    secondary_y = 0L;

    if (session->config.curve_mode == PHOSPHOR_CURVE_HARMONOGRAPH) {
        fade_primary = (long)(sample_count - sample_index);
        fade_secondary = (long)(sample_count - (sample_index / 2U));
        wave_x = phosphor_wave((t * session->ratio_a) + session->phase_a);
        wave_y = phosphor_wave((t * session->ratio_b) + session->phase_b);
        secondary_x = phosphor_wave((t * session->ratio_c) + session->phase_c);
        secondary_y = phosphor_wave((t * session->ratio_d) + session->phase_d);
        wave_x = ((wave_x * fade_primary) + (secondary_x * (fade_secondary / 2L))) / (long)sample_count;
        wave_y = ((wave_y * fade_primary) + (secondary_y * (fade_secondary / 2L))) / (long)sample_count;
    } else if (session->config.curve_mode == PHOSPHOR_CURVE_DENSE) {
        wave_x = phosphor_wave((t * session->ratio_a) + session->phase_a);
        wave_y = phosphor_wave((t * session->ratio_b) + session->phase_b);
        secondary_x = phosphor_wave((t * session->ratio_c) + session->phase_c);
        secondary_y = phosphor_wave((t * session->ratio_d) + session->phase_d);
        wave_x = ((wave_x * 6L) + (secondary_x * 4L)) / 10L;
        wave_y = ((wave_y * 6L) + (secondary_y * 4L)) / 10L;
    } else {
        wave_x = phosphor_wave((t * session->ratio_a) + session->phase_a);
        wave_y = phosphor_wave((t * session->ratio_b) + session->phase_b);
        secondary_x = phosphor_wave((t * (session->ratio_a + session->ratio_c)) + session->phase_c);
        secondary_y = phosphor_wave((t * (session->ratio_b + session->ratio_d)) + session->phase_d);
        wave_x = ((wave_x * 7L) + (secondary_x * 3L)) / 10L;
        wave_y = ((wave_y * 7L) + (secondary_y * 3L)) / 10L;
    }

    radius_x = ((long)session->drawable_size.width * 38L) / 100L;
    radius_y = ((long)session->drawable_size.height * 38L) / 100L;
    if (session->config.curve_mode == PHOSPHOR_CURVE_HARMONOGRAPH) {
        radius_x = (radius_x * 9L) / 10L;
        radius_y = (radius_y * 9L) / 10L;
    }
    if (session->preview_mode) {
        radius_x = (radius_x * 9L) / 10L;
        radius_y = (radius_y * 9L) / 10L;
    }

    center_x = (long)(session->drawable_size.width / 2);
    center_y = (long)(session->drawable_size.height / 2);
    point_out->x = (int)(center_x + ((wave_x * radius_x) / PHOSPHOR_WAVE_ONE));
    point_out->y = (int)(center_y + ((wave_y * radius_y) / PHOSPHOR_WAVE_ONE));
}

static unsigned int phosphor_build_mirror_points(
    const screensave_saver_session *session,
    const screensave_pointi *base_point,
    screensave_pointi *points_out
)
{
    unsigned int point_count;

    point_count = 0U;
    points_out[point_count++] = *base_point;

    if (session->config.mirror_mode == PHOSPHOR_MIRROR_HORIZONTAL ||
        session->config.mirror_mode == PHOSPHOR_MIRROR_QUAD) {
        points_out[point_count].x = session->drawable_size.width - 1 - base_point->x;
        points_out[point_count].y = base_point->y;
        point_count += 1U;
    }

    if (session->config.mirror_mode == PHOSPHOR_MIRROR_QUAD) {
        points_out[point_count].x = base_point->x;
        points_out[point_count].y = session->drawable_size.height - 1 - base_point->y;
        point_count += 1U;

        points_out[point_count].x = session->drawable_size.width - 1 - base_point->x;
        points_out[point_count].y = session->drawable_size.height - 1 - base_point->y;
        point_count += 1U;
    }

    return point_count;
}

static void phosphor_draw_trace(
    screensave_saver_session *session
)
{
    unsigned int sample_index;
    unsigned int sample_count;
    unsigned int mirror_count;
    unsigned int mirror_index;
    screensave_pointi base_point;
    screensave_pointi previous_points[4];
    screensave_pointi current_points[4];
    screensave_color accent_color;
    screensave_color primary_color;
    unsigned int primary_intensity;

    if (session == NULL || session->visual_buffer.pixels == NULL || session->theme == NULL) {
        return;
    }

    sample_count = phosphor_sample_count(session);
    accent_color = session->theme->accent_color;
    primary_color = session->theme->primary_color;
    primary_intensity = session->config.persistence_mode == PHOSPHOR_PERSISTENCE_LONG ? 200U : 176U;

    phosphor_sample_curve_point(session, 0U, sample_count, &base_point);
    mirror_count = phosphor_build_mirror_points(session, &base_point, previous_points);

    for (sample_index = 1U; sample_index < sample_count; ++sample_index) {
        phosphor_sample_curve_point(session, sample_index, sample_count, &base_point);
        (void)phosphor_build_mirror_points(session, &base_point, current_points);

        for (mirror_index = 0U; mirror_index < mirror_count; ++mirror_index) {
            screensave_visual_buffer_draw_line(
                &session->visual_buffer,
                &previous_points[mirror_index],
                &current_points[mirror_index],
                accent_color,
                56U
            );
            screensave_visual_buffer_draw_line(
                &session->visual_buffer,
                &previous_points[mirror_index],
                &current_points[mirror_index],
                primary_color,
                primary_intensity
            );
            previous_points[mirror_index] = current_points[mirror_index];
        }
    }
}

static unsigned long phosphor_drift_scale(
    const screensave_saver_session *session
)
{
    unsigned long scale;

    scale = 2UL;
    if (session->config.drift_mode == PHOSPHOR_DRIFT_CALM) {
        scale = 1UL;
    } else if (session->config.drift_mode == PHOSPHOR_DRIFT_WIDE) {
        scale = 4UL;
    }

    if (session->preview_mode) {
        scale = 1UL;
    }

    return scale;
}

static unsigned long phosphor_variation_interval_millis(
    const screensave_saver_session *session
)
{
    if (session == NULL) {
        return 12000UL;
    }

    if (session->preview_mode) {
        return 7000UL;
    }
    if (session->config.curve_mode == PHOSPHOR_CURVE_HARMONOGRAPH) {
        return 15000UL;
    }
    if (session->config.curve_mode == PHOSPHOR_CURVE_DENSE) {
        return 11000UL;
    }
    return 12500UL;
}

static void phosphor_refresh_ratios(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    phosphor_seed_ratios(session);
    session->phase_a += (48UL + phosphor_rng_range(&session->rng, 160UL)) * PHOSPHOR_PHASE_ONE;
    session->phase_b += (32UL + phosphor_rng_range(&session->rng, 144UL)) * PHOSPHOR_PHASE_ONE;
    session->phase_c += (40UL + phosphor_rng_range(&session->rng, 192UL)) * PHOSPHOR_PHASE_ONE;
    session->phase_d += (24UL + phosphor_rng_range(&session->rng, 128UL)) * PHOSPHOR_PHASE_ONE;
}

static void phosphor_reset_visual_state(screensave_saver_session *session)
{
    screensave_color background;

    if (session == NULL) {
        return;
    }

    background = phosphor_background_color(session);
    screensave_visual_buffer_clear(&session->visual_buffer, background);
}

static void phosphor_warm_start(screensave_saver_session *session)
{
    int warm_steps;

    if (session == NULL) {
        return;
    }

    phosphor_reset_visual_state(session);
    warm_steps = session->config.persistence_mode == PHOSPHOR_PERSISTENCE_LONG ? 8 : 4;
    if (session->config.curve_mode == PHOSPHOR_CURVE_HARMONOGRAPH) {
        warm_steps += 2;
    }
    while (warm_steps-- > 0) {
        phosphor_decay_to_background(session);
        phosphor_draw_trace(session);
        session->phase_a += 1408UL;
        session->phase_b += 2304UL;
        session->phase_c += 1792UL;
        session->phase_d += 1152UL;
    }
}

int phosphor_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    const phosphor_config *configured;

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
    session->theme = phosphor_resolve_theme(environment);
    session->config.curve_mode = PHOSPHOR_CURVE_LISSAJOUS;
    session->config.persistence_mode = PHOSPHOR_PERSISTENCE_STANDARD;
    session->config.drift_mode = PHOSPHOR_DRIFT_STANDARD;
    session->config.mirror_mode = PHOSPHOR_MIRROR_NONE;

    configured = NULL;
    if (
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(phosphor_config)
    ) {
        configured = (const phosphor_config *)environment->config_binding->product_config;
    }
    if (configured != NULL) {
        session->config = *configured;
    }

    phosphor_rng_seed(&session->rng, environment->seed.stream_seed ^ environment->seed.base_seed);
    session->phase_a = (environment->seed.base_seed & 255UL) * PHOSPHOR_PHASE_ONE;
    session->phase_b = ((environment->seed.base_seed >> 8) & 255UL) * PHOSPHOR_PHASE_ONE;
    session->phase_c = (environment->seed.stream_seed & 255UL) * PHOSPHOR_PHASE_ONE;
    session->phase_d = ((environment->seed.stream_seed >> 8) & 255UL) * PHOSPHOR_PHASE_ONE;
    session->variation_elapsed_millis = 0UL;

    if (!screensave_visual_buffer_init(&session->visual_buffer, &session->drawable_size)) {
        phosphor_destroy_session(session);
        return 0;
    }

    phosphor_seed_ratios(session);
    phosphor_warm_start(session);
    *session_out = session;
    return 1;
}

void phosphor_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    screensave_visual_buffer_dispose(&session->visual_buffer);
    free(session);
}

void phosphor_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    if (screensave_visual_buffer_resize(&session->visual_buffer, &session->drawable_size)) {
        phosphor_warm_start(session);
    }
}

void phosphor_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta_millis;
    unsigned long drift_scale;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta_millis = environment->clock.delta_millis;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    drift_scale = phosphor_drift_scale(session);
    session->elapsed_millis += delta_millis;
    session->variation_elapsed_millis += delta_millis;
    session->phase_a += delta_millis * (18UL + drift_scale);
    session->phase_b += delta_millis * (28UL + (drift_scale * 2UL));
    session->phase_c += delta_millis * (22UL + drift_scale);
    session->phase_d += delta_millis * (14UL + drift_scale);

    phosphor_decay_to_background(session);
    phosphor_draw_trace(session);

    if (session->variation_elapsed_millis >= phosphor_variation_interval_millis(session)) {
        session->variation_elapsed_millis = 0UL;
        phosphor_refresh_ratios(session);
    }
}
