#include <stdlib.h>

#include "ecosystems_internal.h"

static const screensave_theme_descriptor *ecosystems_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return ecosystems_find_theme_descriptor(binding->common_config->theme_key);
    }

    return ecosystems_find_theme_descriptor(ECOSYSTEMS_DEFAULT_THEME_KEY);
}

static const ecosystems_config *ecosystems_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(ecosystems_config)) {
        return NULL;
    }

    return (const ecosystems_config *)binding->product_config;
}

static unsigned long ecosystems_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 65UL;
    }

    switch (session->config.activity_mode) {
    case ECOSYSTEMS_ACTIVITY_CALM:
        return 90UL;

    case ECOSYSTEMS_ACTIVITY_LIVELY:
        return 34UL;

    case ECOSYSTEMS_ACTIVITY_STANDARD:
    default:
        return 58UL;
    }
}

static unsigned int ecosystems_population_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 16U;
    }

    switch (session->config.density_mode) {
    case ECOSYSTEMS_DENSITY_SPARSE:
        count = 14U;
        break;

    case ECOSYSTEMS_DENSITY_LUSH:
        count = 42U;
        break;

    case ECOSYSTEMS_DENSITY_STANDARD:
    default:
        count = 26U;
        break;
    }

    if (session->preview_mode && count > 18U) {
        count = 18U;
    }

    return count;
}

static void ecosystems_pick_regroup_target(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->regroup_x = 24 + (int)ecosystems_rng_range(
        &session->rng,
        (unsigned long)(session->drawable_size.width > 48 ? session->drawable_size.width - 48 : 1)
    );
    session->regroup_y = 24 + (int)ecosystems_rng_range(
        &session->rng,
        (unsigned long)(session->drawable_size.height > 48 ? session->drawable_size.height - 48 : 1)
    );
}

static void ecosystems_seed_agent(
    screensave_saver_session *session,
    ecosystems_agent *agent,
    unsigned int index
)
{
    int width_fixed;
    int height_fixed;
    int speed_base;

    if (session == NULL || agent == NULL) {
        return;
    }

    width_fixed = session->drawable_size.width * 16;
    height_fixed = session->drawable_size.height * 16;
    speed_base = 4 + (int)session->config.activity_mode * 2;

    agent->active = 1;
    agent->x_fixed = (int)ecosystems_rng_range(&session->rng, (unsigned long)(width_fixed > 0 ? width_fixed : 1));
    agent->y_fixed = (int)ecosystems_rng_range(&session->rng, (unsigned long)(height_fixed > 0 ? height_fixed : 1));
    agent->brightness = 96 + (int)ecosystems_rng_range(&session->rng, 144UL);
    agent->phase = (unsigned int)ecosystems_rng_range(&session->rng, 256UL);
    agent->group = index % 4U;

    switch (session->config.habitat_mode) {
    case ECOSYSTEMS_HABITAT_AVIARY:
        agent->vx_fixed = (speed_base + 6) * 2;
        agent->vy_fixed = (int)ecosystems_rng_range(&session->rng, 5UL) - 2;
        agent->y_fixed = (session->drawable_size.height / 5 + (int)ecosystems_rng_range(&session->rng, (unsigned long)(session->drawable_size.height / 2 + 1))) * 16;
        break;

    case ECOSYSTEMS_HABITAT_FIREFLIES:
        agent->vx_fixed = (int)ecosystems_rng_range(&session->rng, 7UL) - 3;
        agent->vy_fixed = (int)ecosystems_rng_range(&session->rng, 7UL) - 3;
        break;

    case ECOSYSTEMS_HABITAT_AQUARIUM:
    default:
        agent->vx_fixed = (speed_base + 4) * 2;
        agent->vy_fixed = (int)ecosystems_rng_range(&session->rng, 5UL) - 2;
        break;
    }
}

static void ecosystems_initialize_population(screensave_saver_session *session)
{
    unsigned int count;
    unsigned int index;

    if (session == NULL) {
        return;
    }

    count = ecosystems_population_target(session);
    if (count > ECOSYSTEMS_MAX_AGENTS) {
        count = ECOSYSTEMS_MAX_AGENTS;
    }

    session->agent_count = count;
    ecosystems_pick_regroup_target(session);
    for (index = 0U; index < count; ++index) {
        ecosystems_seed_agent(session, &session->agents[index], index);
    }
}

static void ecosystems_initialize_session(
    screensave_saver_session *session,
    const ecosystems_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = ecosystems_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x4178632DUL;
    ecosystems_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    session->ambient_phase = 0U;
    ecosystems_initialize_population(session);
}

static unsigned int ecosystems_triangle_wave(unsigned int phase)
{
    unsigned int local_phase;

    local_phase = phase & 255U;
    if (local_phase < 128U) {
        return local_phase * 2U;
    }

    return (255U - local_phase) * 2U;
}

static void ecosystems_wrap_agent(
    ecosystems_agent *agent,
    const screensave_saver_session *session
)
{
    int width_fixed;
    int height_fixed;

    if (agent == NULL || session == NULL) {
        return;
    }

    width_fixed = session->drawable_size.width * 16;
    height_fixed = session->drawable_size.height * 16;

    if (agent->x_fixed < -64) {
        agent->x_fixed = width_fixed + 32;
    } else if (agent->x_fixed > width_fixed + 64) {
        agent->x_fixed = -32;
    }

    if (agent->y_fixed < -64) {
        agent->y_fixed = height_fixed + 32;
    } else if (agent->y_fixed > height_fixed + 64) {
        agent->y_fixed = -32;
    }
}

static void ecosystems_step_aquarium_agent(
    screensave_saver_session *session,
    ecosystems_agent *agent
)
{
    int drift;

    drift = (session->regroup_y * 16 - agent->y_fixed) / 64;
    agent->x_fixed += agent->vx_fixed;
    agent->y_fixed += agent->vy_fixed + drift;
    agent->phase = (agent->phase + 5U) & 255U;
    agent->brightness = 120 + (int)(ecosystems_triangle_wave(agent->phase) / 3U);

    if ((ecosystems_rng_next(&session->rng) & 15UL) == 0UL) {
        agent->vy_fixed += (int)ecosystems_rng_range(&session->rng, 3UL) - 1;
    }

    if (agent->vy_fixed < -10) {
        agent->vy_fixed = -10;
    } else if (agent->vy_fixed > 10) {
        agent->vy_fixed = 10;
    }
}

static void ecosystems_step_aviary_agent(
    screensave_saver_session *session,
    ecosystems_agent *agent
)
{
    int drift_x;
    int drift_y;

    drift_x = (session->regroup_x * 16 - agent->x_fixed) / 120;
    drift_y = (session->regroup_y * 16 - agent->y_fixed) / 96;
    agent->x_fixed += agent->vx_fixed + drift_x;
    agent->y_fixed += agent->vy_fixed + drift_y;
    agent->phase = (agent->phase + 7U) & 255U;
    agent->brightness = 132 + (int)(ecosystems_triangle_wave(agent->phase) / 4U);

    if ((ecosystems_rng_next(&session->rng) & 31UL) == 0UL) {
        agent->vy_fixed = (int)ecosystems_rng_range(&session->rng, 5UL) - 2;
    }
}

static void ecosystems_step_firefly_agent(
    screensave_saver_session *session,
    ecosystems_agent *agent
)
{
    int drift_x;
    int drift_y;

    drift_x = (session->regroup_x * 16 - agent->x_fixed) / 160;
    drift_y = (session->regroup_y * 16 - agent->y_fixed) / 160;
    agent->vx_fixed += (int)ecosystems_rng_range(&session->rng, 5UL) - 2 + drift_x;
    agent->vy_fixed += (int)ecosystems_rng_range(&session->rng, 5UL) - 2 + drift_y;
    if (agent->vx_fixed < -12) {
        agent->vx_fixed = -12;
    } else if (agent->vx_fixed > 12) {
        agent->vx_fixed = 12;
    }
    if (agent->vy_fixed < -12) {
        agent->vy_fixed = -12;
    } else if (agent->vy_fixed > 12) {
        agent->vy_fixed = 12;
    }
    agent->x_fixed += agent->vx_fixed;
    agent->y_fixed += agent->vy_fixed;
    agent->phase = (agent->phase + 9U + agent->group) & 255U;
    agent->brightness = 72 + (int)(ecosystems_triangle_wave(agent->phase) / 2U);
}

static void ecosystems_run_step(screensave_saver_session *session)
{
    unsigned int index;
    ecosystems_agent *agent;

    if (session == NULL) {
        return;
    }

    session->ambient_phase = (session->ambient_phase + 1U + (unsigned int)session->config.activity_mode) & 255U;
    for (index = 0U; index < session->agent_count; ++index) {
        agent = &session->agents[index];
        if (!agent->active) {
            continue;
        }

        switch (session->config.habitat_mode) {
        case ECOSYSTEMS_HABITAT_AVIARY:
            ecosystems_step_aviary_agent(session, agent);
            break;

        case ECOSYSTEMS_HABITAT_FIREFLIES:
            ecosystems_step_firefly_agent(session, agent);
            break;

        case ECOSYSTEMS_HABITAT_AQUARIUM:
        default:
            ecosystems_step_aquarium_agent(session, agent);
            break;
        }

        ecosystems_wrap_agent(agent, session);
    }
}

void ecosystems_rng_seed(ecosystems_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x51F0BEEFUL;
}

unsigned long ecosystems_rng_next(ecosystems_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long ecosystems_rng_range(ecosystems_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return ecosystems_rng_next(state) % upper_bound;
}

int ecosystems_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    ecosystems_config config;
    const ecosystems_config *bound_config;
    screensave_common_config safe_common;
    ecosystems_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    ecosystems_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = ecosystems_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    ecosystems_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    ecosystems_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void ecosystems_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void ecosystems_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    ecosystems_initialize_population(session);
}

void ecosystems_step_session(
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
    interval = ecosystems_step_interval(session);

    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        ecosystems_run_step(session);
    }

    if (session->event_accumulator >= 2400UL) {
        session->event_accumulator = 0UL;
        ecosystems_pick_regroup_target(session);
    }
}
