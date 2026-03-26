#include <stdlib.h>

#include "nocturne_internal.h"

#define NOCTURNE_FIXED_ONE 65536L
#define NOCTURNE_FIXED_HALF 32768L

#define NOCTURNE_STAGE_FADE_IN 0
#define NOCTURNE_STAGE_STEADY 1
#define NOCTURNE_STAGE_FADE_OUT 2

static void nocturne_emit_session_diag(
    const screensave_saver_environment *environment,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (environment == NULL || environment->diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        environment->diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_SAVER,
        code,
        "nocturne",
        text
    );
}

void nocturne_rng_seed(nocturne_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0A1E0A1EUL;
}

unsigned long nocturne_rng_next(nocturne_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long nocturne_rng_range(nocturne_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return nocturne_rng_next(state) % upper_bound;
}

static unsigned long nocturne_cycle_duration_millis(
    screensave_detail_level detail_level,
    int preview_mode
)
{
    if (preview_mode) {
        return 24000UL;
    }

    switch (detail_level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        return 90000UL;

    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        return 52000UL;

    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        return 70000UL;
    }
}

static unsigned long nocturne_fade_units_per_second(int fade_speed)
{
    switch (fade_speed) {
    case NOCTURNE_FADE_SLOW:
        return 52UL;

    case NOCTURNE_FADE_GENTLE:
        return 72UL;

    case NOCTURNE_FADE_STANDARD:
    default:
        return 110UL;
    }
}

static long nocturne_speed_units(int motion_strength, int preview_mode)
{
    long speed;

    switch (motion_strength) {
    case NOCTURNE_STRENGTH_STILL:
        speed = 180L;
        break;

    case NOCTURNE_STRENGTH_SOFT:
        speed = 420L;
        break;

    case NOCTURNE_STRENGTH_SUBTLE:
    default:
        speed = 300L;
        break;
    }

    if (preview_mode) {
        speed = (speed * 3L) / 4L;
    }

    return speed;
}

static long nocturne_random_velocity(
    nocturne_rng_state *rng,
    int motion_strength,
    int preview_mode
)
{
    long speed;

    speed = nocturne_speed_units(motion_strength, preview_mode);
    if ((nocturne_rng_next(rng) & 1UL) != 0UL) {
        return speed;
    }

    return -speed;
}

static void nocturne_set_initial_positions(screensave_saver_session *session)
{
    long width_fixed;
    long height_fixed;

    width_fixed = (long)session->drawable_size.width * NOCTURNE_FIXED_ONE;
    height_fixed = (long)session->drawable_size.height * NOCTURNE_FIXED_ONE;

    session->primary_x =
        NOCTURNE_FIXED_HALF +
        (long)nocturne_rng_range(&session->rng, (unsigned long)((session->drawable_size.width > 1) ? (session->drawable_size.width - 1) : 1)) *
            NOCTURNE_FIXED_ONE;
    session->primary_y =
        NOCTURNE_FIXED_HALF +
        (long)nocturne_rng_range(&session->rng, (unsigned long)((session->drawable_size.height > 1) ? (session->drawable_size.height - 1) : 1)) *
            NOCTURNE_FIXED_ONE;
    session->secondary_x =
        NOCTURNE_FIXED_HALF +
        (long)nocturne_rng_range(&session->rng, (unsigned long)((session->drawable_size.width > 1) ? (session->drawable_size.width - 1) : 1)) *
            NOCTURNE_FIXED_ONE;
    session->secondary_y =
        NOCTURNE_FIXED_HALF +
        (long)nocturne_rng_range(&session->rng, (unsigned long)((session->drawable_size.height > 1) ? (session->drawable_size.height - 1) : 1)) *
            NOCTURNE_FIXED_ONE;

    if (session->drawable_size.width <= 1) {
        session->primary_x = width_fixed / 2L;
        session->secondary_x = width_fixed / 2L;
    }
    if (session->drawable_size.height <= 1) {
        session->primary_y = height_fixed / 2L;
        session->secondary_y = height_fixed / 2L;
    }
}

static void nocturne_reset_cycle(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->cycle_index += 1UL;
    session->stage = NOCTURNE_STAGE_FADE_IN;
    session->stage_elapsed_millis = 0UL;
    session->fade_level = 0;
    session->cycle_duration_millis = nocturne_cycle_duration_millis(session->detail_level, session->preview_mode);
    session->primary_vx = nocturne_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->primary_vy = nocturne_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->secondary_vx = nocturne_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->secondary_vy = nocturne_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->breath_direction = 1;
    session->breath_level = 48 + (int)nocturne_rng_range(&session->rng, 48UL);
    nocturne_set_initial_positions(session);
}

static void nocturne_advance_axis(long *position, long *velocity, int limit, unsigned long delta_millis)
{
    long minimum;
    long maximum;

    if (position == NULL || velocity == NULL) {
        return;
    }

    minimum = NOCTURNE_FIXED_HALF;
    maximum = (long)((limit > 1 ? limit - 1 : 1) * NOCTURNE_FIXED_ONE) - NOCTURNE_FIXED_HALF;

    *position += *velocity * (long)delta_millis;
    if (*position < minimum) {
        *position = minimum;
        *velocity = -*velocity;
    } else if (*position > maximum) {
        *position = maximum;
        *velocity = -*velocity;
    }
}

static void nocturne_step_positions(screensave_saver_session *session, unsigned long delta_millis)
{
    if (session == NULL) {
        return;
    }

    nocturne_advance_axis(&session->primary_x, &session->primary_vx, session->drawable_size.width, delta_millis);
    nocturne_advance_axis(&session->primary_y, &session->primary_vy, session->drawable_size.height, delta_millis);
    nocturne_advance_axis(&session->secondary_x, &session->secondary_vx, session->drawable_size.width, delta_millis);
    nocturne_advance_axis(&session->secondary_y, &session->secondary_vy, session->drawable_size.height, delta_millis);

    if (session->config.motion_mode == NOCTURNE_MOTION_BREATH) {
        int delta_units;

        delta_units = (int)((delta_millis * 48UL) / 1000UL);
        if (delta_units < 1) {
            delta_units = 1;
        }
        session->breath_level += delta_units * session->breath_direction;
        if (session->breath_level >= 120) {
            session->breath_level = 120;
            session->breath_direction = -1;
        } else if (session->breath_level <= 36) {
            session->breath_level = 36;
            session->breath_direction = 1;
        }
    }
}

static void nocturne_step_stage(screensave_saver_session *session, unsigned long delta_millis)
{
    unsigned long fade_delta;

    if (session == NULL) {
        return;
    }

    fade_delta = (delta_millis * nocturne_fade_units_per_second(session->config.fade_speed)) / 1000UL;
    if (fade_delta == 0UL && delta_millis > 0UL) {
        fade_delta = 1UL;
    }

    session->stage_elapsed_millis += delta_millis;
    if (session->stage == NOCTURNE_STAGE_FADE_IN) {
        session->fade_level += (int)fade_delta;
        if (session->fade_level >= 255) {
            session->fade_level = 255;
            session->stage = NOCTURNE_STAGE_STEADY;
            session->stage_elapsed_millis = 0UL;
        }
        return;
    }

    if (session->stage == NOCTURNE_STAGE_STEADY) {
        if (session->stage_elapsed_millis >= session->cycle_duration_millis) {
            session->stage = NOCTURNE_STAGE_FADE_OUT;
            session->stage_elapsed_millis = 0UL;
        }
        return;
    }

    session->fade_level -= (int)fade_delta;
    if (session->fade_level <= 0) {
        session->fade_level = 0;
        session->reseed_count += 1UL;
        nocturne_reset_cycle(session);
    }
}

static const screensave_theme_descriptor *nocturne_resolve_theme(const screensave_saver_environment *environment)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return nocturne_find_theme_descriptor(NOCTURNE_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return nocturne_find_theme_descriptor(NOCTURNE_DEFAULT_THEME_KEY);
    }

    theme = nocturne_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = nocturne_find_theme_descriptor(NOCTURNE_DEFAULT_THEME_KEY);
    }

    return theme;
}

int nocturne_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    const nocturne_config *configured;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    *session_out = NULL;
    session = (screensave_saver_session *)malloc(sizeof(*session));
    if (session == NULL) {
        nocturne_emit_session_diag(
            environment,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6201UL,
            "Nocturne could not allocate its session state."
        );
        return 0;
    }

    ZeroMemory(session, sizeof(*session));
    session->drawable_size = environment->drawable_size;
    session->detail_level = environment->config_binding != NULL && environment->config_binding->common_config != NULL
        ? environment->config_binding->common_config->detail_level
        : SCREENSAVE_DETAIL_LEVEL_STANDARD;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = nocturne_resolve_theme(environment);

    session->config.motion_mode = NOCTURNE_MOTION_MONOLITH;
    session->config.fade_speed = NOCTURNE_FADE_STANDARD;
    session->config.motion_strength = NOCTURNE_STRENGTH_SUBTLE;
    configured = NULL;
    if (
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(nocturne_config)
    ) {
        configured = (const nocturne_config *)environment->config_binding->product_config;
    }
    if (configured != NULL) {
        session->config = *configured;
    }

    nocturne_rng_seed(&session->rng, environment->seed.stream_seed ^ environment->seed.base_seed);
    nocturne_reset_cycle(session);
    *session_out = session;
    return 1;
}

void nocturne_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void nocturne_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    nocturne_reset_cycle(session);
}

void nocturne_step_session(
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

    nocturne_step_positions(session, delta_millis);
    nocturne_step_stage(session, delta_millis);
}
