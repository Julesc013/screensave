#include <stdlib.h>

#include "observatory_internal.h"

static const screensave_theme_descriptor *observatory_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return observatory_find_theme_descriptor(binding->common_config->theme_key);
    }

    return observatory_find_theme_descriptor(OBSERVATORY_DEFAULT_THEME_KEY);
}

static const observatory_config *observatory_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(observatory_config)) {
        return NULL;
    }

    return (const observatory_config *)binding->product_config;
}

static unsigned long observatory_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 72UL;
    }

    switch (session->config.speed_mode) {
    case OBSERVATORY_SPEED_STILL:
        return 110UL;
    case OBSERVATORY_SPEED_BRISK:
        return 36UL;
    case OBSERVATORY_SPEED_STANDARD:
    default:
        return 64UL;
    }
}

static unsigned int observatory_body_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 5U;
    }

    switch (session->config.detail_mode) {
    case OBSERVATORY_DETAIL_SPARSE:
        count = 4U;
        break;
    case OBSERVATORY_DETAIL_RICH:
        count = 10U;
        break;
    case OBSERVATORY_DETAIL_STANDARD:
    default:
        count = 7U;
        break;
    }

    if (session->config.scene_mode == OBSERVATORY_SCENE_CHART_ROOM && count > 1U) {
        count -= 1U;
    }
    if (session->preview_mode && count > 6U) {
        count = 6U;
    }
    if (count > OBSERVATORY_MAX_BODIES) {
        count = OBSERVATORY_MAX_BODIES;
    }

    return count;
}

static unsigned int observatory_star_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 24U;
    }

    switch (session->config.detail_mode) {
    case OBSERVATORY_DETAIL_SPARSE:
        count = 22U;
        break;
    case OBSERVATORY_DETAIL_RICH:
        count = 54U;
        break;
    case OBSERVATORY_DETAIL_STANDARD:
    default:
        count = 36U;
        break;
    }

    if (session->preview_mode && count > 24U) {
        count = 24U;
    }
    if (count > OBSERVATORY_MAX_STARS) {
        count = OBSERVATORY_MAX_STARS;
    }

    return count;
}

static void observatory_seed_body(screensave_saver_session *session, observatory_body *body, unsigned int index)
{
    int radius_step;

    if (session == NULL || body == NULL) {
        return;
    }

    radius_step = session->config.scene_mode == OBSERVATORY_SCENE_ORRERY ? 14 : 18;
    body->orbit_radius = 26 + (int)index * radius_step + (int)observatory_rng_range(&session->rng, 10UL);
    body->phase = (unsigned int)observatory_rng_range(&session->rng, 256UL);
    body->rate = 1U + (unsigned int)(index % 3U) + (unsigned int)session->config.speed_mode;
    body->size = 2 + (int)(index % 3U);
    body->brightness = 112 + (int)observatory_rng_range(&session->rng, 112UL);
}

static void observatory_seed_star(screensave_saver_session *session, observatory_star *star)
{
    if (session == NULL || star == NULL) {
        return;
    }

    star->x = (int)observatory_rng_range(&session->rng, (unsigned long)(session->drawable_size.width > 0 ? session->drawable_size.width : 1));
    star->y = (int)observatory_rng_range(&session->rng, (unsigned long)(session->drawable_size.height > 0 ? session->drawable_size.height : 1));
    star->brightness = 96 + (int)observatory_rng_range(&session->rng, 144UL);
}

static void observatory_initialize_scene(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->body_count = observatory_body_target(session);
    session->star_count = observatory_star_target(session);
    session->ambient_phase = 0U;
    session->sweep_phase = 0U;
    session->central_pulse = 0U;

    for (index = 0U; index < session->body_count; ++index) {
        observatory_seed_body(session, &session->bodies[index], index);
    }
    for (index = 0U; index < session->star_count; ++index) {
        observatory_seed_star(session, &session->stars[index]);
    }
}

static void observatory_initialize_session(
    screensave_saver_session *session,
    const observatory_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = observatory_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x4F627331UL;
    observatory_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    observatory_initialize_scene(session);
}

static void observatory_run_step(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->ambient_phase = (session->ambient_phase + 1U + (unsigned int)session->config.scene_mode) & 255U;
    session->sweep_phase = (session->sweep_phase + 1U + (unsigned int)session->config.speed_mode) & 255U;
    if (session->central_pulse > 0U) {
        session->central_pulse -= 1U;
    }
    for (index = 0U; index < session->body_count; ++index) {
        session->bodies[index].phase = (session->bodies[index].phase + session->bodies[index].rate) & 255U;
    }
}

void observatory_rng_seed(observatory_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0B5E71A1UL;
}

unsigned long observatory_rng_next(observatory_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long observatory_rng_range(observatory_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return observatory_rng_next(state) % upper_bound;
}

int observatory_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    observatory_config config;
    const observatory_config *bound_config;
    screensave_common_config safe_common;
    observatory_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    observatory_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = observatory_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    observatory_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    observatory_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void observatory_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void observatory_resize_session(screensave_saver_session *session, const screensave_saver_environment *environment)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    observatory_initialize_scene(session);
}

void observatory_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long interval;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->step_accumulator += environment->clock.delta_millis;
    session->event_accumulator += environment->clock.delta_millis;
    interval = observatory_step_interval(session);
    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        observatory_run_step(session);
    }

    if (session->event_accumulator >= 3600UL) {
        session->event_accumulator = 0UL;
        session->central_pulse = 8U + (unsigned int)observatory_rng_range(&session->rng, 6UL);
    }
}
