#include <stdlib.h>

#include "transit_internal.h"

static const screensave_theme_descriptor *transit_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return transit_find_theme_descriptor(binding->common_config->theme_key);
    }

    return transit_find_theme_descriptor(TRANSIT_DEFAULT_THEME_KEY);
}

static const transit_config *transit_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(transit_config)) {
        return NULL;
    }

    return (const transit_config *)binding->product_config;
}

static unsigned long transit_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 60UL;
    }

    switch (session->config.speed_mode) {
    case TRANSIT_SPEED_GLIDE:
        return 90UL;
    case TRANSIT_SPEED_EXPRESS:
        return 30UL;
    case TRANSIT_SPEED_CRUISE:
    default:
        return 54UL;
    }
}

static unsigned long transit_event_interval(const screensave_saver_session *session)
{
    unsigned long interval;

    if (session == NULL) {
        return 3200UL;
    }

    switch (session->config.speed_mode) {
    case TRANSIT_SPEED_GLIDE:
        interval = 4200UL;
        break;
    case TRANSIT_SPEED_EXPRESS:
        interval = 2200UL;
        break;
    case TRANSIT_SPEED_CRUISE:
    default:
        interval = 3200UL;
        break;
    }

    if (session->preview_mode) {
        interval += 800UL;
    }

    return interval;
}

static int transit_depth_step(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 16;
    }

    switch (session->config.speed_mode) {
    case TRANSIT_SPEED_GLIDE:
        return 14;
    case TRANSIT_SPEED_EXPRESS:
        return 36;
    case TRANSIT_SPEED_CRUISE:
    default:
        return 24;
    }
}

static unsigned int transit_light_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 16U;
    }

    switch (session->config.light_mode) {
    case TRANSIT_LIGHTS_SPARSE:
        count = 18U;
        break;
    case TRANSIT_LIGHTS_DENSE:
        count = 42U;
        break;
    case TRANSIT_LIGHTS_STANDARD:
    default:
        count = 28U;
        break;
    }

    if (session->config.scene_mode == TRANSIT_SCENE_HARBOR && count < TRANSIT_MAX_LIGHTS) {
        count += 4U;
    }
    if (session->preview_mode && count > 18U) {
        count = 18U;
    }
    if (count > TRANSIT_MAX_LIGHTS) {
        count = TRANSIT_MAX_LIGHTS;
    }

    return count;
}

static unsigned int transit_triangle_wave(unsigned int phase)
{
    unsigned int local_phase;

    local_phase = phase & 255U;
    if (local_phase < 128U) {
        return local_phase * 2U;
    }

    return (255U - local_phase) * 2U;
}

static void transit_seed_light(screensave_saver_session *session, transit_light *light)
{
    int lane_limit;

    if (session == NULL || light == NULL) {
        return;
    }

    switch (session->config.scene_mode) {
    case TRANSIT_SCENE_RAIL:
        lane_limit = 3;
        break;
    case TRANSIT_SCENE_HARBOR:
        lane_limit = 4;
        break;
    case TRANSIT_SCENE_MOTORWAY:
    default:
        lane_limit = 5;
        break;
    }

    light->active = 1;
    light->lane = (int)transit_rng_range(&session->rng, (unsigned long)(lane_limit * 2 + 1)) - lane_limit;
    light->depth_fixed = 256 + (int)transit_rng_range(&session->rng, 768UL);
    light->class_id = (int)transit_rng_range(&session->rng, 3UL);
    light->brightness = 88 + (int)transit_rng_range(&session->rng, 144UL);
    light->phase = (unsigned int)transit_rng_range(&session->rng, 256UL);
}

static void transit_initialize_scene(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->light_count = transit_light_target(session);
    session->route_phase = (unsigned int)transit_rng_range(&session->rng, 256UL);
    session->sway_phase = (unsigned int)transit_rng_range(&session->rng, 256UL);
    session->event_pulse = (unsigned int)transit_rng_range(&session->rng, 3UL);
    for (index = 0U; index < session->light_count; ++index) {
        transit_seed_light(session, &session->lights[index]);
    }
}

static void transit_initialize_session(
    screensave_saver_session *session,
    const transit_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = transit_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x4E547231UL;
    transit_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    transit_initialize_scene(session);
}

static void transit_step_light(screensave_saver_session *session, transit_light *light)
{
    int step;

    if (session == NULL || light == NULL || !light->active) {
        return;
    }

    step = transit_depth_step(session) + light->class_id * 2;
    light->depth_fixed -= step;
    light->phase = (light->phase + 4U + (unsigned int)light->class_id) & 255U;
    light->brightness = 96 + (int)(transit_triangle_wave(light->phase) / 2U);
    if (light->depth_fixed < 48) {
        transit_seed_light(session, light);
    }
}

static void transit_run_step(screensave_saver_session *session)
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
        transit_step_light(session, &session->lights[index]);
    }
}

void transit_rng_seed(transit_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0xB031C5B1UL;
}

unsigned long transit_rng_next(transit_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long transit_rng_range(transit_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return transit_rng_next(state) % upper_bound;
}

int transit_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    transit_config config;
    const transit_config *bound_config;
    screensave_common_config safe_common;
    transit_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    transit_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = transit_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    transit_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    transit_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void transit_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void transit_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = transit_resolve_theme(environment->config_binding);
    transit_initialize_scene(session);
}

void transit_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long interval;
    unsigned long event_interval;
    unsigned int refresh_count;
    unsigned int refresh_index;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->step_accumulator += environment->clock.delta_millis;
    session->event_accumulator += environment->clock.delta_millis;
    interval = transit_step_interval(session);
    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        transit_run_step(session);
    }

    event_interval = transit_event_interval(session);
    while (session->event_accumulator >= event_interval) {
        session->event_accumulator -= event_interval;
        session->event_pulse = 4U + (unsigned int)transit_rng_range(&session->rng, 8UL);
        refresh_count = session->preview_mode ? 1U : 2U;
        for (refresh_index = 0U; refresh_index < refresh_count; ++refresh_index) {
            if (session->light_count == 0U) {
                break;
            }
            transit_seed_light(
                session,
                &session->lights[transit_rng_range(&session->rng, (unsigned long)session->light_count)]
            );
        }
        session->route_phase = (session->route_phase + 17U + (unsigned int)transit_rng_range(&session->rng, 24UL)) & 255U;
        session->sway_phase = (session->sway_phase + 9U + (unsigned int)transit_rng_range(&session->rng, 16UL)) & 255U;
    }
}
