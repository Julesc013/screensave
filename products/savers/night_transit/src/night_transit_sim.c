#include <stdlib.h>

#include "night_transit_internal.h"

static const screensave_theme_descriptor *night_transit_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return night_transit_find_theme_descriptor(binding->common_config->theme_key);
    }

    return night_transit_find_theme_descriptor(NIGHT_TRANSIT_DEFAULT_THEME_KEY);
}

static const night_transit_config *night_transit_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(night_transit_config)) {
        return NULL;
    }

    return (const night_transit_config *)binding->product_config;
}

static unsigned long night_transit_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 60UL;
    }

    switch (session->config.speed_mode) {
    case NIGHT_TRANSIT_SPEED_GLIDE:
        return 90UL;
    case NIGHT_TRANSIT_SPEED_EXPRESS:
        return 30UL;
    case NIGHT_TRANSIT_SPEED_CRUISE:
    default:
        return 54UL;
    }
}

static int night_transit_depth_step(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 16;
    }

    switch (session->config.speed_mode) {
    case NIGHT_TRANSIT_SPEED_GLIDE:
        return 14;
    case NIGHT_TRANSIT_SPEED_EXPRESS:
        return 36;
    case NIGHT_TRANSIT_SPEED_CRUISE:
    default:
        return 24;
    }
}

static unsigned int night_transit_light_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 16U;
    }

    switch (session->config.light_mode) {
    case NIGHT_TRANSIT_LIGHTS_SPARSE:
        count = 18U;
        break;
    case NIGHT_TRANSIT_LIGHTS_DENSE:
        count = 42U;
        break;
    case NIGHT_TRANSIT_LIGHTS_STANDARD:
    default:
        count = 28U;
        break;
    }

    if (session->config.scene_mode == NIGHT_TRANSIT_SCENE_HARBOR && count < NIGHT_TRANSIT_MAX_LIGHTS) {
        count += 4U;
    }
    if (session->preview_mode && count > 18U) {
        count = 18U;
    }
    if (count > NIGHT_TRANSIT_MAX_LIGHTS) {
        count = NIGHT_TRANSIT_MAX_LIGHTS;
    }

    return count;
}

static unsigned int night_transit_triangle_wave(unsigned int phase)
{
    unsigned int local_phase;

    local_phase = phase & 255U;
    if (local_phase < 128U) {
        return local_phase * 2U;
    }

    return (255U - local_phase) * 2U;
}

static void night_transit_seed_light(screensave_saver_session *session, night_transit_light *light)
{
    int lane_limit;

    if (session == NULL || light == NULL) {
        return;
    }

    switch (session->config.scene_mode) {
    case NIGHT_TRANSIT_SCENE_RAIL:
        lane_limit = 3;
        break;
    case NIGHT_TRANSIT_SCENE_HARBOR:
        lane_limit = 4;
        break;
    case NIGHT_TRANSIT_SCENE_MOTORWAY:
    default:
        lane_limit = 5;
        break;
    }

    light->active = 1;
    light->lane = (int)night_transit_rng_range(&session->rng, (unsigned long)(lane_limit * 2 + 1)) - lane_limit;
    light->depth_fixed = 256 + (int)night_transit_rng_range(&session->rng, 768UL);
    light->class_id = (int)night_transit_rng_range(&session->rng, 3UL);
    light->brightness = 88 + (int)night_transit_rng_range(&session->rng, 144UL);
    light->phase = (unsigned int)night_transit_rng_range(&session->rng, 256UL);
}

static void night_transit_initialize_scene(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->light_count = night_transit_light_target(session);
    session->route_phase = 0U;
    session->sway_phase = 0U;
    session->event_pulse = 0U;
    for (index = 0U; index < session->light_count; ++index) {
        night_transit_seed_light(session, &session->lights[index]);
    }
}

static void night_transit_initialize_session(
    screensave_saver_session *session,
    const night_transit_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = night_transit_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x4E547231UL;
    night_transit_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    night_transit_initialize_scene(session);
}

static void night_transit_step_light(screensave_saver_session *session, night_transit_light *light)
{
    int step;

    if (session == NULL || light == NULL || !light->active) {
        return;
    }

    step = night_transit_depth_step(session) + light->class_id * 2;
    light->depth_fixed -= step;
    light->phase = (light->phase + 4U + (unsigned int)light->class_id) & 255U;
    light->brightness = 96 + (int)(night_transit_triangle_wave(light->phase) / 2U);
    if (light->depth_fixed < 48) {
        night_transit_seed_light(session, light);
    }
}

static void night_transit_run_step(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->route_phase = (session->route_phase + 1U + (unsigned int)session->config.speed_mode) & 255U;
    session->sway_phase = (session->sway_phase + 2U + (unsigned int)session->config.scene_mode) & 255U;
    if (session->event_pulse > 0U) {
        session->event_pulse -= 1U;
    }

    for (index = 0U; index < session->light_count; ++index) {
        night_transit_step_light(session, &session->lights[index]);
    }
}

void night_transit_rng_seed(night_transit_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0xB031C5B1UL;
}

unsigned long night_transit_rng_next(night_transit_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long night_transit_rng_range(night_transit_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return night_transit_rng_next(state) % upper_bound;
}

int night_transit_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    night_transit_config config;
    const night_transit_config *bound_config;
    screensave_common_config safe_common;
    night_transit_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    night_transit_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = night_transit_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    night_transit_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    night_transit_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void night_transit_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void night_transit_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    night_transit_initialize_scene(session);
}

void night_transit_step_session(
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
    interval = night_transit_step_interval(session);
    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        night_transit_run_step(session);
    }

    if (session->event_accumulator >= 3200UL) {
        session->event_accumulator = 0UL;
        session->event_pulse = 4U + (unsigned int)night_transit_rng_range(&session->rng, 8UL);
    }
}
