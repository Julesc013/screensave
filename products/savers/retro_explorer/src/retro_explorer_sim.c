#include <stdlib.h>

#include "retro_explorer_internal.h"

static const screensave_theme_descriptor *retro_explorer_resolve_theme(
    const screensave_config_binding *binding
)
{
    if (binding != NULL && binding->common_config != NULL) {
        return retro_explorer_find_theme_descriptor(binding->common_config->theme_key);
    }

    return retro_explorer_find_theme_descriptor(RETRO_EXPLORER_DEFAULT_THEME_KEY);
}

static const retro_explorer_config *retro_explorer_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(retro_explorer_config)) {
        return NULL;
    }

    return (const retro_explorer_config *)binding->product_config;
}

static unsigned int retro_explorer_target_segment_count(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 8U;
    }

    switch (session->detail_level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        count = 8U;
        break;
    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        count = 12U;
        break;
    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        count = 10U;
        break;
    }

    if (session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON && count < 11U) {
        count += 1U;
    }
    if (session->preview_mode && count > 8U) {
        count = 8U;
    }
    if (count > RETRO_EXPLORER_MAX_SEGMENTS) {
        count = RETRO_EXPLORER_MAX_SEGMENTS;
    }

    return count;
}

static unsigned long retro_explorer_step_interval(const screensave_saver_session *session)
{
    unsigned long interval;

    if (session == NULL) {
        return 64UL;
    }

    switch (session->config.speed_mode) {
    case RETRO_EXPLORER_SPEED_CALM:
        interval = 96UL;
        break;
    case RETRO_EXPLORER_SPEED_BRISK:
        interval = 40UL;
        break;
    case RETRO_EXPLORER_SPEED_STANDARD:
    default:
        interval = 64UL;
        break;
    }

    if (session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON) {
        interval += 8UL;
    } else if (session->config.scene_mode == RETRO_EXPLORER_SCENE_INDUSTRIAL && interval > 8UL) {
        interval -= 4UL;
    }

    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW && interval > 8UL) {
        interval += 8UL;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH && interval > 16UL) {
        interval -= 8UL;
    }

    return interval;
}

static int retro_explorer_clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }

    return value;
}

static void retro_explorer_seed_segment(
    screensave_saver_session *session,
    retro_explorer_segment *segment,
    unsigned int index
)
{
    int curve_limit;
    int width_base;
    int height_base;
    int openness_base;
    unsigned long length_base;

    if (session == NULL || segment == NULL) {
        return;
    }

    segment->variant = (int)retro_explorer_rng_range(&session->rng, 3UL);
    switch (session->config.scene_mode) {
    case RETRO_EXPLORER_SCENE_INDUSTRIAL:
        width_base = 34;
        height_base = 24;
        openness_base = 7;
        curve_limit = 4;
        length_base = 10UL;
        segment->light_interval = 2 + (int)retro_explorer_rng_range(&session->rng, 2UL);
        segment->sign_interval = 2 + (int)retro_explorer_rng_range(&session->rng, 3UL);
        break;
    case RETRO_EXPLORER_SCENE_CANYON:
        width_base = 44;
        height_base = 30;
        openness_base = 11;
        curve_limit = 5;
        length_base = 11UL;
        segment->light_interval = 4 + (int)retro_explorer_rng_range(&session->rng, 3UL);
        segment->sign_interval = 0;
        break;
    case RETRO_EXPLORER_SCENE_CORRIDOR:
    default:
        width_base = 28;
        height_base = 20;
        openness_base = 5;
        curve_limit = 3;
        length_base = 12UL;
        segment->light_interval = 2 + (int)retro_explorer_rng_range(&session->rng, 3UL);
        segment->sign_interval = 3 + (int)retro_explorer_rng_range(&session->rng, 3UL);
        break;
    }

    segment->length_steps = (unsigned int)(length_base + retro_explorer_rng_range(&session->rng, 8UL));
    if (session->config.speed_mode == RETRO_EXPLORER_SPEED_CALM) {
        segment->length_steps += 4U;
    } else if (session->config.speed_mode == RETRO_EXPLORER_SPEED_BRISK && segment->length_steps > 6U) {
        segment->length_steps -= 2U;
    }

    segment->width = width_base + (int)retro_explorer_rng_range(&session->rng, 12UL);
    segment->height = height_base + (int)retro_explorer_rng_range(&session->rng, 12UL);
    segment->curve = (int)retro_explorer_rng_range(&session->rng, (unsigned long)(curve_limit * 2 + 1)) - curve_limit;
    segment->openness = openness_base + (int)retro_explorer_rng_range(&session->rng, 8UL);
    segment->curve += (int)index - ((int)session->segment_count / 2);
    if (segment->light_interval <= 0) {
        segment->light_interval = 1;
    }
}

static void retro_explorer_build_route(screensave_saver_session *session, unsigned long route_seed)
{
    unsigned int index;
    unsigned int count;

    if (session == NULL) {
        return;
    }

    retro_explorer_rng_seed(&session->rng, route_seed);
    count = retro_explorer_target_segment_count(session);
    session->segment_count = count;
    session->current_segment_index = 0U;
    session->current_segment_step = 0U;
    session->camera_offset = (int)retro_explorer_rng_range(&session->rng, 12UL) - 6;
    session->portal_pulse = 8UL;

    for (index = 0U; index < count; ++index) {
        retro_explorer_seed_segment(session, &session->segments[index], index);
    }
}

static void retro_explorer_advance_route(screensave_saver_session *session)
{
    const retro_explorer_segment *segment;

    if (session == NULL || session->segment_count == 0U) {
        return;
    }

    segment = &session->segments[session->current_segment_index];
    session->current_segment_step += 1U;
    session->camera_offset = (session->camera_offset * 7) / 8 + segment->curve;
    session->camera_offset = retro_explorer_clamp_int(
        session->camera_offset,
        -(session->drawable_size.width / 4),
        session->drawable_size.width / 4
    );

    if (session->portal_pulse > 0UL) {
        session->portal_pulse -= 1UL;
    }

    if (session->current_segment_step < segment->length_steps) {
        return;
    }

    session->current_segment_step = 0U;
    session->current_segment_index += 1U;
    session->portal_pulse = 5UL;
    if (session->current_segment_index < session->segment_count) {
        return;
    }

    session->world_cycle += 1UL;
    session->route_seed += 0x9E3779B9UL;
    retro_explorer_build_route(session, session->route_seed);
    session->portal_pulse = 12UL;
}

void retro_explorer_rng_seed(retro_explorer_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x52455831UL;
}

unsigned long retro_explorer_rng_next(retro_explorer_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long retro_explorer_rng_range(retro_explorer_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return retro_explorer_rng_next(state) % upper_bound;
}

int retro_explorer_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    retro_explorer_config config;
    const retro_explorer_config *bound_config;
    screensave_common_config safe_common;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    retro_explorer_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = retro_explorer_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }
    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    retro_explorer_config_clamp(&safe_common, &config, sizeof(config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    session->config = config;
    session->theme = retro_explorer_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    session->detail_level = safe_common.detail_level;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->route_seed = environment->seed.stream_seed != 0UL ? environment->seed.stream_seed : 0x52455831UL;
    if (safe_common.use_deterministic_seed && safe_common.deterministic_seed != 0UL) {
        session->route_seed = safe_common.deterministic_seed;
    }
    retro_explorer_build_route(session, session->route_seed);
    *session_out = session;
    return 1;
}

void retro_explorer_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void retro_explorer_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->camera_offset = retro_explorer_clamp_int(
        session->camera_offset,
        -(session->drawable_size.width / 4),
        session->drawable_size.width / 4
    );
}

void retro_explorer_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long interval;
    unsigned long delta;

    if (session == NULL || environment == NULL) {
        return;
    }

    interval = retro_explorer_step_interval(session);
    delta = environment->clock.delta_millis;
    if (delta == 0UL) {
        delta = 16UL;
    }

    session->travel_millis += delta;
    while (session->travel_millis >= interval) {
        session->travel_millis -= interval;
        retro_explorer_advance_route(session);
    }
}
