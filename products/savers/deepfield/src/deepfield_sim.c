#include <stdlib.h>

#include "deepfield_internal.h"

#define DEEPFIELD_FIXED_ONE 256L

void deepfield_rng_seed(deepfield_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0D331F17UL;
}

unsigned long deepfield_rng_next(deepfield_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long deepfield_rng_range(deepfield_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return deepfield_rng_next(state) % upper_bound;
}

static const screensave_theme_descriptor *deepfield_resolve_theme(
    const screensave_saver_environment *environment
)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return deepfield_find_theme_descriptor(DEEPFIELD_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return deepfield_find_theme_descriptor(DEEPFIELD_DEFAULT_THEME_KEY);
    }

    theme = deepfield_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = deepfield_find_theme_descriptor(DEEPFIELD_DEFAULT_THEME_KEY);
    }

    return theme;
}

static unsigned int deepfield_star_count(
    const deepfield_config *config,
    screensave_detail_level detail_level,
    int preview_mode
)
{
    unsigned int count;

    if (config == NULL) {
        return 48U;
    }

    if (config->scene_mode == DEEPFIELD_SCENE_FLYTHROUGH) {
        switch (config->density_mode) {
        case DEEPFIELD_DENSITY_SPARSE:
            count = 48U;
            break;

        case DEEPFIELD_DENSITY_RICH:
            count = 132U;
            break;

        case DEEPFIELD_DENSITY_STANDARD:
        default:
            count = 88U;
            break;
        }
    } else {
        switch (config->density_mode) {
        case DEEPFIELD_DENSITY_SPARSE:
            count = 40U;
            break;

        case DEEPFIELD_DENSITY_RICH:
            count = 112U;
            break;

        case DEEPFIELD_DENSITY_STANDARD:
        default:
            count = 72U;
            break;
        }
    }

    if (detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        count = (count * 3U) / 4U;
    } else if (detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        count = (count * 5U) / 4U;
    }
    if (preview_mode && count > 56U) {
        count = 56U;
    }
    if (count > DEEPFIELD_MAX_STARS) {
        count = DEEPFIELD_MAX_STARS;
    }

    return count;
}

static long deepfield_speed_units(const deepfield_config *config, int preview_mode)
{
    long speed;

    if (config == NULL) {
        return 12L;
    }

    if (config->scene_mode == DEEPFIELD_SCENE_FLYTHROUGH) {
        switch (config->speed_mode) {
        case DEEPFIELD_SPEED_CALM:
            speed = 8L;
            break;

        case DEEPFIELD_SPEED_SURGE:
            speed = 18L;
            break;

        case DEEPFIELD_SPEED_CRUISE:
        default:
            speed = 12L;
            break;
        }
    } else {
        switch (config->speed_mode) {
        case DEEPFIELD_SPEED_CALM:
            speed = 22L;
            break;

        case DEEPFIELD_SPEED_SURGE:
            speed = 48L;
            break;

        case DEEPFIELD_SPEED_CRUISE:
        default:
            speed = 34L;
            break;
        }
    }

    if (preview_mode) {
        speed = (speed * 4L) / 5L;
    }

    return speed;
}

static void deepfield_reset_parallax_star(
    screensave_saver_session *session,
    deepfield_star *star,
    int place_on_right
)
{
    long width_fixed;
    long height_fixed;

    width_fixed = (long)session->drawable_size.width * DEEPFIELD_FIXED_ONE;
    height_fixed = (long)session->drawable_size.height * DEEPFIELD_FIXED_ONE;

    star->layer = (unsigned char)(1U + deepfield_rng_range(&session->rng, 3UL));
    star->twinkle = (unsigned char)deepfield_rng_range(&session->rng, 128UL);
    if (place_on_right) {
        star->x = width_fixed + ((long)deepfield_rng_range(&session->rng, 48UL) * DEEPFIELD_FIXED_ONE);
    } else {
        star->x = (long)deepfield_rng_range(&session->rng, (unsigned long)(width_fixed > 0L ? width_fixed : 1L));
    }
    star->y = (long)deepfield_rng_range(&session->rng, (unsigned long)(height_fixed > 0L ? height_fixed : 1L));
    star->z = (long)star->layer;
}

static void deepfield_reset_flythrough_star(screensave_saver_session *session, deepfield_star *star)
{
    long spread_x;
    long spread_y;
    long value;

    spread_x = (long)session->drawable_size.width * 3L;
    spread_y = (long)session->drawable_size.height * 2L;
    if (spread_x < 48L) {
        spread_x = 48L;
    }
    if (spread_y < 32L) {
        spread_y = 32L;
    }

    value = (long)deepfield_rng_range(&session->rng, (unsigned long)(spread_x * 2L + 1L));
    star->x = value - spread_x;
    value = (long)deepfield_rng_range(&session->rng, (unsigned long)(spread_y * 2L + 1L));
    star->y = value - spread_y;
    star->z = 96L + (long)deepfield_rng_range(&session->rng, 720UL);
    star->layer = (unsigned char)(1U + deepfield_rng_range(&session->rng, 3UL));
    star->twinkle = (unsigned char)deepfield_rng_range(&session->rng, 128UL);
}

static void deepfield_initialize_stars(screensave_saver_session *session)
{
    unsigned int index;

    session->star_count = deepfield_star_count(&session->config, session->detail_level, session->preview_mode);
    for (index = 0U; index < session->star_count; ++index) {
        if (session->config.scene_mode == DEEPFIELD_SCENE_FLYTHROUGH) {
            deepfield_reset_flythrough_star(session, &session->stars[index]);
        } else {
            deepfield_reset_parallax_star(session, &session->stars[index], 0);
        }
    }

    for (; index < DEEPFIELD_MAX_STARS; ++index) {
        ZeroMemory(&session->stars[index], sizeof(session->stars[index]));
    }

    session->camera_phase_millis = 0UL;
    session->pulse_elapsed_millis = 0UL;
    session->pulse_remaining_millis = 0UL;
}

static void deepfield_step_pulse(screensave_saver_session *session, unsigned long delta_millis)
{
    unsigned long cycle_length;
    unsigned long pulse_length;

    if (session->config.pulse_mode == DEEPFIELD_PULSE_NONE) {
        session->pulse_elapsed_millis = 0UL;
        session->pulse_remaining_millis = 0UL;
        return;
    }

    session->pulse_elapsed_millis += delta_millis;
    if (session->pulse_remaining_millis > delta_millis) {
        session->pulse_remaining_millis -= delta_millis;
    } else {
        session->pulse_remaining_millis = 0UL;
    }

    if (session->config.pulse_mode == DEEPFIELD_PULSE_WARP) {
        cycle_length = session->preview_mode ? 8000UL : 12000UL;
        pulse_length = session->preview_mode ? 600UL : 1100UL;
    } else {
        cycle_length = session->preview_mode ? 10000UL : 15000UL;
        pulse_length = session->preview_mode ? 420UL : 760UL;
    }

    if (session->pulse_elapsed_millis >= cycle_length) {
        session->pulse_elapsed_millis = 0UL;
        session->pulse_remaining_millis = pulse_length;
    }
}

static long deepfield_camera_shift(
    unsigned long phase_millis,
    unsigned long period_millis,
    long amplitude
)
{
    unsigned long phase;
    unsigned long half_period;
    long value;

    if (period_millis == 0UL || amplitude == 0L) {
        return 0L;
    }

    phase = phase_millis % period_millis;
    half_period = period_millis / 2UL;
    if (half_period == 0UL) {
        return 0L;
    }

    if (phase < half_period) {
        value = -amplitude + (long)((phase * (unsigned long)(amplitude * 2L)) / half_period);
    } else {
        phase -= half_period;
        value = amplitude - (long)((phase * (unsigned long)(amplitude * 2L)) / half_period);
    }

    return value;
}

static void deepfield_step_parallax(screensave_saver_session *session, unsigned long delta_millis)
{
    unsigned int index;
    long speed;
    long vertical_bias;
    long margin;
    deepfield_star *star;

    margin = 16L * DEEPFIELD_FIXED_ONE;
    speed = deepfield_speed_units(&session->config, session->preview_mode);
    if (session->pulse_remaining_millis > 0UL) {
        if (session->config.pulse_mode == DEEPFIELD_PULSE_WARP) {
            speed += 18L;
        } else {
            speed += 8L;
        }
    }

    vertical_bias = 0L;
    if (session->config.camera_mode == DEEPFIELD_CAMERA_DRIFT) {
        vertical_bias = deepfield_camera_shift(session->camera_phase_millis, 6000UL, 96L);
    } else if (session->config.camera_mode == DEEPFIELD_CAMERA_ARC) {
        vertical_bias = deepfield_camera_shift(session->camera_phase_millis, 8000UL, 128L);
    }

    for (index = 0U; index < session->star_count; ++index) {
        star = &session->stars[index];
        star->x -= ((speed + (long)star->layer * 10L) * (long)delta_millis) / 33L;
        star->y += (vertical_bias * (long)star->layer * (long)delta_millis) / 33000L;

        if (star->x < -margin) {
            deepfield_reset_parallax_star(session, star, 1);
        } else {
            if (star->y < -margin) {
                star->y += ((long)session->drawable_size.height * DEEPFIELD_FIXED_ONE) + margin;
            } else if (star->y > ((long)session->drawable_size.height * DEEPFIELD_FIXED_ONE) + margin) {
                star->y -= ((long)session->drawable_size.height * DEEPFIELD_FIXED_ONE) + margin;
            }
        }
    }
}

static void deepfield_step_flythrough(screensave_saver_session *session, unsigned long delta_millis)
{
    unsigned int index;
    long speed;
    deepfield_star *star;

    speed = deepfield_speed_units(&session->config, session->preview_mode);
    if (session->pulse_remaining_millis > 0UL) {
        if (session->config.pulse_mode == DEEPFIELD_PULSE_WARP) {
            speed += 18L;
        } else {
            speed += 8L;
        }
    }

    for (index = 0U; index < session->star_count; ++index) {
        star = &session->stars[index];
        star->z -= ((speed + (long)star->layer * 2L) * (long)delta_millis) / 33L;
        if (star->z < 24L) {
            deepfield_reset_flythrough_star(session, star);
        }
    }
}

int deepfield_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    const deepfield_config *configured;

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
    session->theme = deepfield_resolve_theme(environment);

    session->config.scene_mode = DEEPFIELD_SCENE_PARALLAX;
    session->config.density_mode = DEEPFIELD_DENSITY_STANDARD;
    session->config.speed_mode = DEEPFIELD_SPEED_CALM;
    session->config.camera_mode = DEEPFIELD_CAMERA_OBSERVE;
    session->config.pulse_mode = DEEPFIELD_PULSE_NONE;

    configured = NULL;
    if (
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(deepfield_config)
    ) {
        configured = (const deepfield_config *)environment->config_binding->product_config;
    }
    if (configured != NULL) {
        session->config = *configured;
    }

    deepfield_rng_seed(&session->rng, environment->seed.stream_seed ^ environment->seed.base_seed);
    deepfield_initialize_stars(session);
    *session_out = session;
    return 1;
}

void deepfield_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void deepfield_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    deepfield_initialize_stars(session);
}

void deepfield_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta_millis;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta_millis = environment->clock.delta_millis;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    session->camera_phase_millis += delta_millis;
    deepfield_step_pulse(session, delta_millis);

    if (session->config.scene_mode == DEEPFIELD_SCENE_FLYTHROUGH) {
        deepfield_step_flythrough(session, delta_millis);
    } else {
        deepfield_step_parallax(session, delta_millis);
    }
}
