#include <stdlib.h>

#include "stormglass_internal.h"

static const screensave_theme_descriptor *stormglass_resolve_theme(const screensave_config_binding *binding)
{
    if (binding != NULL && binding->common_config != NULL) {
        return stormglass_find_theme_descriptor(binding->common_config->theme_key);
    }

    return stormglass_find_theme_descriptor(STORMGLASS_DEFAULT_THEME_KEY);
}

static const stormglass_config *stormglass_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(stormglass_config)) {
        return NULL;
    }

    return (const stormglass_config *)binding->product_config;
}

static unsigned long stormglass_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 60UL;
    }

    switch (session->config.intensity_mode) {
    case STORMGLASS_INTENSITY_QUIET:
        return 86UL;
    case STORMGLASS_INTENSITY_HEAVY:
        return 34UL;
    case STORMGLASS_INTENSITY_STANDARD:
    default:
        return 56UL;
    }
}

static unsigned int stormglass_droplet_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 24U;
    }

    switch (session->config.scene_mode) {
    case STORMGLASS_SCENE_FOGGED:
        count = 18U + (unsigned int)session->config.intensity_mode * 10U;
        break;
    case STORMGLASS_SCENE_WINTER:
        count = 20U + (unsigned int)session->config.intensity_mode * 14U;
        break;
    case STORMGLASS_SCENE_RAIN:
    default:
        count = 26U + (unsigned int)session->config.intensity_mode * 18U;
        break;
    }

    if (session->config.pane_mode == STORMGLASS_PANE_CONDENSED && count + 8U < STORMGLASS_MAX_DROPLETS) {
        count += 8U;
    }
    if (session->preview_mode && count > 24U) {
        count = 24U;
    }
    if (count > STORMGLASS_MAX_DROPLETS) {
        count = STORMGLASS_MAX_DROPLETS;
    }

    return count;
}

static unsigned int stormglass_light_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 4U;
    }

    switch (session->config.scene_mode) {
    case STORMGLASS_SCENE_FOGGED:
        count = 4U;
        break;
    case STORMGLASS_SCENE_WINTER:
        count = 5U;
        break;
    case STORMGLASS_SCENE_RAIN:
    default:
        count = 6U;
        break;
    }

    if (session->config.intensity_mode == STORMGLASS_INTENSITY_HEAVY && count < STORMGLASS_MAX_LIGHTS) {
        count += 2U;
    }
    if (session->preview_mode && count > 5U) {
        count = 5U;
    }

    return count;
}

static unsigned int stormglass_band_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 2U;
    }

    switch (session->config.pane_mode) {
    case STORMGLASS_PANE_CLEAR:
        count = 1U;
        break;
    case STORMGLASS_PANE_CONDENSED:
        count = 5U;
        break;
    case STORMGLASS_PANE_MISTED:
    default:
        count = 3U;
        break;
    }

    if (session->config.scene_mode == STORMGLASS_SCENE_FOGGED && count < STORMGLASS_MAX_BANDS) {
        count += 1U;
    }
    if (session->preview_mode && count > 3U) {
        count = 3U;
    }
    if (count > STORMGLASS_MAX_BANDS) {
        count = STORMGLASS_MAX_BANDS;
    }

    return count;
}

static unsigned int stormglass_triangle_wave(unsigned int phase)
{
    unsigned int local_phase;

    local_phase = phase & 255U;
    if (local_phase < 128U) {
        return local_phase * 2U;
    }

    return (255U - local_phase) * 2U;
}

static void stormglass_seed_light(screensave_saver_session *session, stormglass_light *light)
{
    int width_limit;
    int height_limit;

    if (session == NULL || light == NULL) {
        return;
    }

    width_limit = session->drawable_size.width > 40 ? session->drawable_size.width - 40 : 1;
    height_limit = session->drawable_size.height > 60 ? session->drawable_size.height - 60 : 1;
    light->x = (int)stormglass_rng_range(&session->rng, (unsigned long)width_limit);
    light->y = session->drawable_size.height / 3 + (int)stormglass_rng_range(&session->rng, (unsigned long)height_limit);
    light->width = 12 + (int)stormglass_rng_range(&session->rng, 28UL);
    light->height = 3 + (int)stormglass_rng_range(&session->rng, 8UL);
    light->brightness = 72 + (int)stormglass_rng_range(&session->rng, 112UL);
    light->drift = (int)stormglass_rng_range(&session->rng, 3UL) - 1;
    light->phase = (unsigned int)stormglass_rng_range(&session->rng, 256UL);
}

static void stormglass_seed_band(screensave_saver_session *session, stormglass_band *band)
{
    if (session == NULL || band == NULL) {
        return;
    }

    band->y = 8 + (int)stormglass_rng_range(
        &session->rng,
        (unsigned long)(session->drawable_size.height > 16 ? session->drawable_size.height - 16 : 1)
    );
    band->thickness = 10 + (int)stormglass_rng_range(&session->rng, 26UL);
    band->drift = (int)stormglass_rng_range(&session->rng, 3UL) - 1;
    band->brightness = 26 + (int)stormglass_rng_range(&session->rng, 58UL);
}

static void stormglass_seed_droplet(
    screensave_saver_session *session,
    stormglass_droplet *droplet,
    int top_seeded
)
{
    int width_fixed;
    int y_span;

    if (session == NULL || droplet == NULL) {
        return;
    }

    width_fixed = session->drawable_size.width * 16;
    y_span = session->drawable_size.height > 0 ? session->drawable_size.height : 1;
    droplet->active = 1;
    droplet->x_fixed = (int)stormglass_rng_range(&session->rng, (unsigned long)(width_fixed > 0 ? width_fixed : 1));
    droplet->y_fixed = top_seeded
        ? -(int)stormglass_rng_range(&session->rng, 640UL)
        : (int)stormglass_rng_range(&session->rng, (unsigned long)(y_span * 16));
    droplet->brightness = 80 + (int)stormglass_rng_range(&session->rng, 144UL);
    droplet->phase = (unsigned int)stormglass_rng_range(&session->rng, 256UL);

    switch (session->config.scene_mode) {
    case STORMGLASS_SCENE_FOGGED:
        droplet->speed_fixed = 5 + (int)stormglass_rng_range(&session->rng, 8UL);
        droplet->drift_fixed = (int)stormglass_rng_range(&session->rng, 5UL) - 2;
        droplet->trail_length = 7 + (int)stormglass_rng_range(&session->rng, 10UL);
        break;
    case STORMGLASS_SCENE_WINTER:
        droplet->speed_fixed = 4 + (int)stormglass_rng_range(&session->rng, 9UL);
        droplet->drift_fixed = (int)stormglass_rng_range(&session->rng, 13UL) - 6;
        droplet->trail_length = 3 + (int)stormglass_rng_range(&session->rng, 5UL);
        break;
    case STORMGLASS_SCENE_RAIN:
    default:
        droplet->speed_fixed = 14 + (int)stormglass_rng_range(&session->rng, 18UL);
        droplet->drift_fixed = (int)stormglass_rng_range(&session->rng, 9UL) - 4;
        droplet->trail_length = 8 + (int)stormglass_rng_range(&session->rng, 10UL);
        break;
    }
}

static void stormglass_initialize_scene(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->droplet_count = stormglass_droplet_target(session);
    session->light_count = stormglass_light_target(session);
    session->band_count = stormglass_band_target(session);
    session->ambient_phase = 0U;
    session->event_pulse = 0U;
    session->light_shift = 0;

    for (index = 0U; index < session->light_count; ++index) {
        stormglass_seed_light(session, &session->lights[index]);
    }
    for (index = 0U; index < session->band_count; ++index) {
        stormglass_seed_band(session, &session->bands[index]);
    }
    for (index = 0U; index < session->droplet_count; ++index) {
        stormglass_seed_droplet(session, &session->droplets[index], 0);
    }
}

static void stormglass_initialize_session(
    screensave_saver_session *session,
    const stormglass_config *config,
    const screensave_saver_environment *environment
)
{
    unsigned long seed;

    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = stormglass_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x53746731UL;
    stormglass_rng_seed(&session->rng, seed);
    session->step_accumulator = 0UL;
    session->event_accumulator = 0UL;
    stormglass_initialize_scene(session);
}

static void stormglass_step_lights(screensave_saver_session *session)
{
    unsigned int index;
    stormglass_light *light;
    int width_limit;

    if (session == NULL) {
        return;
    }

    width_limit = session->drawable_size.width > 0 ? session->drawable_size.width : 1;
    for (index = 0U; index < session->light_count; ++index) {
        light = &session->lights[index];
        light->phase = (light->phase + 3U + (unsigned int)session->config.intensity_mode) & 255U;
        light->x += light->drift + session->light_shift;
        if (light->x < -light->width) {
            light->x = width_limit;
        } else if (light->x > width_limit) {
            light->x = -light->width;
        }
        light->brightness = 68 + (int)(stormglass_triangle_wave(light->phase) / 2U);
    }
}

static void stormglass_step_bands(screensave_saver_session *session)
{
    unsigned int index;
    stormglass_band *band;
    int height_limit;

    if (session == NULL) {
        return;
    }

    height_limit = session->drawable_size.height > 0 ? session->drawable_size.height : 1;
    for (index = 0U; index < session->band_count; ++index) {
        band = &session->bands[index];
        band->y += band->drift;
        if (band->y < -band->thickness) {
            band->y = height_limit;
        } else if (band->y > height_limit) {
            band->y = -band->thickness;
        }
        band->brightness = 26 + (int)(stormglass_triangle_wave(session->ambient_phase + index * 24U) / 6U);
    }
}

static void stormglass_step_droplet(screensave_saver_session *session, stormglass_droplet *droplet)
{
    int width_fixed;
    int height_fixed;

    if (session == NULL || droplet == NULL || !droplet->active) {
        return;
    }

    width_fixed = session->drawable_size.width * 16;
    height_fixed = session->drawable_size.height * 16;
    switch (session->config.scene_mode) {
    case STORMGLASS_SCENE_FOGGED:
        droplet->y_fixed += droplet->speed_fixed;
        droplet->x_fixed += droplet->drift_fixed + session->light_shift;
        droplet->phase = (droplet->phase + 3U) & 255U;
        droplet->brightness = 72 + (int)(stormglass_triangle_wave(droplet->phase) / 3U);
        break;
    case STORMGLASS_SCENE_WINTER:
        droplet->y_fixed += droplet->speed_fixed;
        droplet->x_fixed += droplet->drift_fixed + session->light_shift * 2;
        droplet->phase = (droplet->phase + 5U) & 255U;
        droplet->brightness = 116 + (int)(stormglass_triangle_wave(droplet->phase) / 3U);
        break;
    case STORMGLASS_SCENE_RAIN:
    default:
        droplet->y_fixed += droplet->speed_fixed + (int)session->config.intensity_mode * 2;
        droplet->x_fixed += droplet->drift_fixed;
        droplet->phase = (droplet->phase + 7U) & 255U;
        if ((stormglass_rng_next(&session->rng) & 31UL) == 0UL && session->config.pane_mode == STORMGLASS_PANE_CONDENSED) {
            droplet->trail_length += 1;
            if (droplet->trail_length > 22) {
                droplet->trail_length = 22;
            }
        }
        break;
    }

    if (droplet->y_fixed > height_fixed + 80 || droplet->x_fixed < -80 || droplet->x_fixed > width_fixed + 80) {
        stormglass_seed_droplet(session, droplet, 1);
    }
}

static void stormglass_run_step(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->ambient_phase = (session->ambient_phase + 1U + (unsigned int)session->config.intensity_mode) & 255U;
    if (session->event_pulse > 0U) {
        session->event_pulse -= 1U;
    }
    stormglass_step_lights(session);
    stormglass_step_bands(session);
    for (index = 0U; index < session->droplet_count; ++index) {
        stormglass_step_droplet(session, &session->droplets[index]);
    }
}

void stormglass_rng_seed(stormglass_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0xF3195D1BUL;
}

unsigned long stormglass_rng_next(stormglass_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long stormglass_rng_range(stormglass_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return stormglass_rng_next(state) % upper_bound;
}

int stormglass_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    stormglass_config config;
    const stormglass_config *bound_config;
    screensave_common_config safe_common;
    stormglass_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    stormglass_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = stormglass_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    stormglass_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    stormglass_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void stormglass_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void stormglass_resize_session(screensave_saver_session *session, const screensave_saver_environment *environment)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    stormglass_initialize_scene(session);
}

void stormglass_step_session(
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
    interval = stormglass_step_interval(session);
    while (session->step_accumulator >= interval) {
        session->step_accumulator -= interval;
        stormglass_run_step(session);
    }

    if (session->event_accumulator >= 2800UL) {
        session->event_accumulator = 0UL;
        session->light_shift = (int)stormglass_rng_range(&session->rng, 5UL) - 2;
        if (session->config.scene_mode == STORMGLASS_SCENE_RAIN && session->config.intensity_mode == STORMGLASS_INTENSITY_HEAVY) {
            session->event_pulse = 8U + (unsigned int)stormglass_rng_range(&session->rng, 6UL);
        } else {
            session->event_pulse = 2U + (unsigned int)stormglass_rng_range(&session->rng, 5UL);
        }
    }
}
