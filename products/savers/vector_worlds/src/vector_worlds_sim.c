#include <stdlib.h>

#include "vector_worlds_internal.h"

static const screensave_theme_descriptor *vector_worlds_resolve_theme(const screensave_config_binding *binding)
{
    const screensave_theme_descriptor *theme;

    if (binding != NULL && binding->common_config != NULL) {
        theme = vector_worlds_find_theme_descriptor(binding->common_config->theme_key);
        if (theme != NULL) {
            return theme;
        }
    }

    return vector_worlds_find_theme_descriptor(VECTOR_WORLDS_DEFAULT_THEME_KEY);
}

static const vector_worlds_config *vector_worlds_resolve_config(const screensave_config_binding *binding)
{
    if (binding == NULL || binding->product_config == NULL || binding->product_config_size != sizeof(vector_worlds_config)) {
        return NULL;
    }

    return (const vector_worlds_config *)binding->product_config;
}

static unsigned long vector_worlds_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 64UL;
    }

    switch (session->config.speed_mode) {
    case VECTOR_WORLDS_SPEED_CALM:
        return 96UL;
    case VECTOR_WORLDS_SPEED_SURGE:
        return 36UL;
    case VECTOR_WORLDS_SPEED_STANDARD:
    default:
        return 58UL;
    }
}

static unsigned int vector_worlds_object_target(const screensave_saver_session *session)
{
    unsigned int count;

    if (session == NULL) {
        return 9U;
    }

    switch (session->config.detail_mode) {
    case VECTOR_WORLDS_DETAIL_LOW:
        count = 8U;
        break;
    case VECTOR_WORLDS_DETAIL_HIGH:
        count = 16U;
        break;
    case VECTOR_WORLDS_DETAIL_STANDARD:
    default:
        count = 12U;
        break;
    }

    if (session->config.scene_mode == VECTOR_WORLDS_SCENE_TUNNEL) {
        count += 2U;
    }
    if (session->preview_mode && count > 10U) {
        count = 10U;
    }
    if (count > VECTOR_WORLDS_MAX_OBJECTS) {
        count = VECTOR_WORLDS_MAX_OBJECTS;
    }

    return count;
}

static unsigned long vector_worlds_hash(unsigned long value)
{
    value ^= value >> 16;
    value *= 1664525UL;
    value += 1013904223UL;
    value ^= value >> 13;
    return value;
}

static int vector_worlds_signed_range(unsigned long value, int magnitude)
{
    unsigned long span;

    if (magnitude <= 0) {
        return 0;
    }

    span = (unsigned long)(magnitude * 2 + 1);
    return (int)(value % span) - magnitude;
}

static void vector_worlds_seed_object(
    screensave_saver_session *session,
    vector_worlds_object *object,
    unsigned int index
)
{
    unsigned long salt;
    int base_offset;

    if (session == NULL || object == NULL) {
        return;
    }

    salt = session->scene_variant * 131UL + (unsigned long)(index * 977U) + session->scene_seed;
    base_offset = (int)(index * 24U);

    object->phase = (unsigned int)vector_worlds_hash(salt + 1UL);
    object->twist = vector_worlds_signed_range(vector_worlds_hash(salt + 2UL), 512);
    object->scale = 18 + vector_worlds_signed_range(vector_worlds_hash(salt + 3UL), 8);

    switch (session->config.scene_mode) {
    case VECTOR_WORLDS_SCENE_TUNNEL:
        object->kind = (int)(index % 3U);
        object->x = vector_worlds_signed_range(vector_worlds_hash(salt + 4UL), 48);
        object->y = vector_worlds_signed_range(vector_worlds_hash(salt + 5UL), 28);
        object->z = base_offset * 8 + vector_worlds_signed_range(vector_worlds_hash(salt + 6UL), 32);
        break;

    case VECTOR_WORLDS_SCENE_TERRAIN:
        object->kind = (int)((index + 1U) % 4U);
        object->x = base_offset * 16 - 140 + vector_worlds_signed_range(vector_worlds_hash(salt + 4UL), 24);
        object->y = vector_worlds_signed_range(vector_worlds_hash(salt + 5UL), 72);
        object->z = vector_worlds_signed_range(vector_worlds_hash(salt + 6UL), 240);
        break;

    case VECTOR_WORLDS_SCENE_FIELD:
    default:
        object->kind = (int)(index % 3U);
        object->x = vector_worlds_signed_range(vector_worlds_hash(salt + 4UL), 160);
        object->y = vector_worlds_signed_range(vector_worlds_hash(salt + 5UL), 96);
        object->z = base_offset * 12 + vector_worlds_signed_range(vector_worlds_hash(salt + 6UL), 90);
        break;
    }
}

static void vector_worlds_initialize_scene(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    session->object_count = vector_worlds_object_target(session);
    for (index = 0U; index < session->object_count; ++index) {
        vector_worlds_seed_object(session, &session->objects[index], index);
    }
}

static void vector_worlds_initialize_session(
    screensave_saver_session *session,
    const vector_worlds_config *config,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || config == NULL || environment == NULL) {
        return;
    }

    session->config = *config;
    session->theme = vector_worlds_resolve_theme(environment->config_binding);
    session->drawable_size = environment->drawable_size;
    if (session->drawable_size.width <= 0) {
        session->drawable_size.width = 1;
    }
    if (session->drawable_size.height <= 0) {
        session->drawable_size.height = 1;
    }
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW ? 1 : 0;
    session->scene_seed = environment->seed.base_seed ^ environment->seed.stream_seed;
    if (session->scene_seed == 0UL) {
        session->scene_seed = 0x56475731UL;
    }
    vector_worlds_rng_seed(&session->rng, session->scene_seed);
    session->step_accumulator = 0UL;
    session->camera_phase = 0UL;
    session->route_phase = 0UL;
    session->event_phase = 0UL;
    session->scene_variant = 0UL;
    vector_worlds_initialize_scene(session);
}

static void vector_worlds_mutate_scene(screensave_saver_session *session)
{
    vector_worlds_object *object;
    unsigned int index;
    int drift;

    if (session == NULL || session->object_count == 0U) {
        return;
    }

    index = (unsigned int)vector_worlds_rng_range(&session->rng, (unsigned long)session->object_count);
    object = &session->objects[index];
    drift = vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 14);
    object->phase += (unsigned int)vector_worlds_rng_range(&session->rng, 32UL);
    object->twist += drift;
    object->scale += vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 2);
    if (object->scale < 10) {
        object->scale = 10;
    }
    if (object->scale > 36) {
        object->scale = 36;
    }

    if (session->config.scene_mode == VECTOR_WORLDS_SCENE_TUNNEL) {
        object->x += vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 6);
        object->y += vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 4);
    } else if (session->config.scene_mode == VECTOR_WORLDS_SCENE_TERRAIN) {
        object->y += vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 8);
    } else {
        object->x += vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 10);
        object->z += vector_worlds_signed_range(vector_worlds_rng_next(&session->rng), 12);
    }

    session->scene_variant = (session->scene_variant + 1UL + (unsigned long)vector_worlds_rng_range(&session->rng, 3UL)) & 255UL;
}

void vector_worlds_rng_seed(vector_worlds_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x56475731UL;
}

unsigned long vector_worlds_rng_next(vector_worlds_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long vector_worlds_rng_range(vector_worlds_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return vector_worlds_rng_next(state) % upper_bound;
}

int vector_worlds_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    vector_worlds_config config;
    const vector_worlds_config *bound_config;
    screensave_common_config safe_common;
    vector_worlds_config safe_config;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    vector_worlds_config_set_defaults(&safe_common, &config, sizeof(config));
    bound_config = vector_worlds_resolve_config(environment->config_binding);
    if (bound_config != NULL) {
        config = *bound_config;
    }

    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        safe_common = *environment->config_binding->common_config;
    } else {
        screensave_common_config_set_defaults(&safe_common);
    }
    safe_config = config;
    vector_worlds_config_clamp(&safe_common, &safe_config, sizeof(safe_config));

    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    vector_worlds_initialize_session(session, &safe_config, environment);
    *session_out = session;
    return 1;
}

void vector_worlds_destroy_session(screensave_saver_session *session)
{
    if (session != NULL) {
        free(session);
    }
}

void vector_worlds_resize_session(screensave_saver_session *session, const screensave_saver_environment *environment)
{
    if (session != NULL && environment != NULL) {
        session->drawable_size = environment->drawable_size;
        if (session->drawable_size.width <= 0) {
            session->drawable_size.width = 1;
        }
        if (session->drawable_size.height <= 0) {
            session->drawable_size.height = 1;
        }
    }
}

void vector_worlds_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta_millis;
    unsigned long step_interval;
    unsigned long event_interval;
    unsigned int index;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta_millis = environment->clock.delta_millis;
    step_interval = vector_worlds_step_interval(session);
    event_interval = session->config.scene_mode == VECTOR_WORLDS_SCENE_TUNNEL ? 5UL : 7UL;

    session->step_accumulator += delta_millis;
    while (session->step_accumulator >= step_interval) {
        session->step_accumulator -= step_interval;
        session->camera_phase += 8UL + (unsigned long)(session->config.speed_mode * 5);
        session->route_phase += 5UL + (unsigned long)(session->config.speed_mode * 7);
        session->event_phase += 1UL;

        for (index = 0U; index < session->object_count; ++index) {
            session->objects[index].phase += 1U + (unsigned int)session->config.speed_mode;
        }

        if ((session->event_phase % event_interval) == 0UL) {
            vector_worlds_mutate_scene(session);
        }
    }
}
