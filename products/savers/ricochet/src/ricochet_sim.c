#include <stdlib.h>

#include "ricochet_internal.h"

#define RICOCHET_FIXED_ONE 256L
#define RICOCHET_FIXED_HALF 128L

static long ricochet_abs_long(long value)
{
    return value < 0L ? -value : value;
}

void ricochet_rng_seed(ricochet_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x09C0C710UL;
}

unsigned long ricochet_rng_next(ricochet_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long ricochet_rng_range(ricochet_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return ricochet_rng_next(state) % upper_bound;
}

static const screensave_theme_descriptor *ricochet_resolve_theme(
    const screensave_saver_environment *environment
)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }

    theme = ricochet_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = ricochet_find_theme_descriptor(RICOCHET_DEFAULT_THEME_KEY);
    }

    return theme;
}

static int ricochet_effective_object_count(
    const ricochet_config *config,
    screensave_detail_level detail_level,
    int preview_mode
)
{
    int count;

    count = config != NULL ? config->object_count : 1;
    if (count < 1) {
        count = 1;
    }
    if (count > (int)RICOCHET_MAX_OBJECTS) {
        count = (int)RICOCHET_MAX_OBJECTS;
    }

    if (detail_level == SCREENSAVE_DETAIL_LEVEL_LOW && count > 3) {
        count = 3;
    }
    if (preview_mode && count > 3) {
        count = 3;
    }

    return count;
}

static int ricochet_trail_length(
    const ricochet_config *config,
    screensave_detail_level detail_level,
    int preview_mode
)
{
    int length;

    if (config == NULL) {
        return 0;
    }

    switch (config->trail_mode) {
    case RICOCHET_TRAIL_NONE:
        return 0;

    case RICOCHET_TRAIL_PHOSPHOR:
        length = 12;
        break;

    case RICOCHET_TRAIL_SHORT:
    default:
        length = 5;
        break;
    }

    if (detail_level == SCREENSAVE_DETAIL_LEVEL_LOW && length > 7) {
        length = 7;
    }
    if (detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH && length < 10 && config->trail_mode == RICOCHET_TRAIL_PHOSPHOR) {
        length = 10;
    }
    if (preview_mode && length > 5) {
        length = 5;
    }

    if (length > (int)RICOCHET_MAX_TRAIL_POINTS) {
        length = (int)RICOCHET_MAX_TRAIL_POINTS;
    }
    return length;
}

static int ricochet_body_size(
    const screensave_saver_session *session,
    unsigned int index
)
{
    int minimum_dimension;
    int size;

    minimum_dimension = session->drawable_size.width;
    if (session->drawable_size.height < minimum_dimension) {
        minimum_dimension = session->drawable_size.height;
    }
    if (minimum_dimension < 24) {
        minimum_dimension = 24;
    }

    size = minimum_dimension / 12;
    if (session->object_count > 1U) {
        size = minimum_dimension / (15 + (int)index * 2);
    }
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        size = (size * 4) / 5;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        size = (size * 6) / 5;
    }
    if (session->preview_mode) {
        size = (size * 4) / 5;
    }

    if (size < 8) {
        size = 8;
    } else if (size > 30) {
        size = 30;
    }

    return size;
}

static long ricochet_base_speed(int speed_mode, int preview_mode)
{
    long speed;

    switch (speed_mode) {
    case RICOCHET_SPEED_CALM:
        speed = 116L;
        break;

    case RICOCHET_SPEED_LIVELY:
        speed = 248L;
        break;

    case RICOCHET_SPEED_STANDARD:
    default:
        speed = 174L;
        break;
    }

    if (preview_mode) {
        speed = (speed * 4L) / 5L;
    }

    return speed;
}

static long ricochet_random_speed_component(
    ricochet_rng_state *rng,
    int speed_mode,
    int preview_mode
)
{
    long speed;
    long variation;

    speed = ricochet_base_speed(speed_mode, preview_mode);
    variation = (long)ricochet_rng_range(rng, 72UL);
    if ((ricochet_rng_next(rng) & 1UL) == 0UL) {
        speed += variation;
    } else {
        speed = -(speed + variation);
    }

    if (speed > -96L && speed < 96L) {
        if (speed < 0L) {
            speed = -96L;
        } else {
            speed = 96L;
        }
    }

    return speed;
}

static long ricochet_blend_velocity(long current_velocity, long target_velocity)
{
    long blended_velocity;

    blended_velocity = ((current_velocity * 3L) + target_velocity) / 4L;
    if (blended_velocity > -92L && blended_velocity < 92L) {
        if (blended_velocity < 0L) {
            blended_velocity = -92L;
        } else {
            blended_velocity = 92L;
        }
    }

    return blended_velocity;
}

static int ricochet_body_overlaps(
    const ricochet_body *existing_body,
    const ricochet_body *candidate_body
)
{
    long distance_x;
    long distance_y;
    long minimum_spacing;

    if (existing_body == NULL || candidate_body == NULL) {
        return 0;
    }

    distance_x = ricochet_abs_long(existing_body->x - candidate_body->x);
    distance_y = ricochet_abs_long(existing_body->y - candidate_body->y);
    minimum_spacing = (long)(((existing_body->size + candidate_body->size) * RICOCHET_FIXED_ONE) / 2);
    minimum_spacing += 3L * RICOCHET_FIXED_ONE;

    return distance_x < minimum_spacing && distance_y < minimum_spacing;
}

static void ricochet_reset_body_trail(ricochet_body *body, int trail_length)
{
    unsigned int index;

    body->trail_head = 0U;
    body->trail_count = (unsigned int)trail_length;
    for (index = 0U; index < RICOCHET_MAX_TRAIL_POINTS; ++index) {
        body->trail_x[index] = body->x;
        body->trail_y[index] = body->y;
        body->trail_size[index] = body->size;
    }
}

static void ricochet_record_trail(ricochet_body *body, int trail_length)
{
    if (trail_length <= 0) {
        body->trail_count = 0U;
        body->trail_head = 0U;
        return;
    }

    body->trail_x[body->trail_head] = body->x;
    body->trail_y[body->trail_head] = body->y;
    body->trail_size[body->trail_head] = body->size;
    body->trail_head = (body->trail_head + 1U) % (unsigned int)trail_length;
    if (body->trail_count < (unsigned int)trail_length) {
        body->trail_count += 1U;
    }
}

static void ricochet_initialize_body(
    screensave_saver_session *session,
    ricochet_body *body,
    unsigned int index
)
{
    int width_limit;
    int height_limit;
    long range_x;
    long range_y;
    int trail_length;
    unsigned int attempt_index;
    unsigned int body_index;
    int overlap_found;

    body->size = ricochet_body_size(session, index);
    width_limit = session->drawable_size.width - body->size;
    height_limit = session->drawable_size.height - body->size;
    if (width_limit < 1) {
        width_limit = 1;
    }
    if (height_limit < 1) {
        height_limit = 1;
    }

    for (attempt_index = 0U; attempt_index < 12U; ++attempt_index) {
        range_x = (long)(ricochet_rng_range(&session->rng, (unsigned long)width_limit) + (unsigned long)(body->size / 2));
        range_y = (long)(ricochet_rng_range(&session->rng, (unsigned long)height_limit) + (unsigned long)(body->size / 2));
        body->x = (range_x * RICOCHET_FIXED_ONE) + RICOCHET_FIXED_HALF;
        body->y = (range_y * RICOCHET_FIXED_ONE) + RICOCHET_FIXED_HALF;

        overlap_found = 0;
        for (body_index = 0U; body_index < index; ++body_index) {
            if (ricochet_body_overlaps(&session->bodies[body_index], body)) {
                overlap_found = 1;
                break;
            }
        }
        if (!overlap_found) {
            break;
        }
    }

    body->vx = ricochet_random_speed_component(
        &session->rng,
        session->config.speed_mode,
        session->preview_mode
    );
    body->vy = ricochet_random_speed_component(
        &session->rng,
        session->config.speed_mode,
        session->preview_mode
    );
    body->flash_timer = 0;

    trail_length = ricochet_trail_length(&session->config, session->detail_level, session->preview_mode);
    ricochet_reset_body_trail(body, trail_length);
}

static void ricochet_initialize_bodies(screensave_saver_session *session)
{
    unsigned int index;

    session->object_count = (unsigned int)ricochet_effective_object_count(
        &session->config,
        session->detail_level,
        session->preview_mode
    );
    if (session->object_count == 0U) {
        session->object_count = 1U;
    }

    for (index = 0U; index < session->object_count; ++index) {
        ricochet_initialize_body(session, &session->bodies[index], index);
    }

    for (; index < RICOCHET_MAX_OBJECTS; ++index) {
        ZeroMemory(&session->bodies[index], sizeof(session->bodies[index]));
    }

    session->celebration_timer = 0;
}

static void ricochet_step_body(
    screensave_saver_session *session,
    ricochet_body *body,
    unsigned long delta_millis,
    int trail_length
)
{
    long half_size;
    long minimum_x;
    long maximum_x;
    long minimum_y;
    long maximum_y;
    int hit_x;
    int hit_y;

    hit_x = 0;
    hit_y = 0;

    body->x += (body->vx * (long)delta_millis) / 33L;
    body->y += (body->vy * (long)delta_millis) / 33L;

    half_size = ((long)body->size * RICOCHET_FIXED_ONE) / 2L;
    minimum_x = half_size;
    minimum_y = half_size;
    maximum_x = ((long)(session->drawable_size.width - 1) * RICOCHET_FIXED_ONE) - half_size;
    maximum_y = ((long)(session->drawable_size.height - 1) * RICOCHET_FIXED_ONE) - half_size;

    if (maximum_x < minimum_x) {
        maximum_x = minimum_x;
    }
    if (maximum_y < minimum_y) {
        maximum_y = minimum_y;
    }

    if (body->x < minimum_x) {
        body->x = minimum_x + (minimum_x - body->x);
        body->vx = -body->vx;
        hit_x = 1;
    } else if (body->x > maximum_x) {
        body->x = maximum_x - (body->x - maximum_x);
        body->vx = -body->vx;
        hit_x = 1;
    }

    if (body->y < minimum_y) {
        body->y = minimum_y + (minimum_y - body->y);
        body->vy = -body->vy;
        hit_y = 1;
    } else if (body->y > maximum_y) {
        body->y = maximum_y - (body->y - maximum_y);
        body->vy = -body->vy;
        hit_y = 1;
    }

    if (hit_x && hit_y) {
        session->celebration_timer = session->preview_mode ? 4 : 7;
    }
    if (hit_x || hit_y) {
        long orthogonal_nudge;

        body->flash_timer = session->preview_mode ? 3 : 6;
        orthogonal_nudge = (long)ricochet_rng_range(&session->rng, 40UL) - 20L;
        if (hit_x) {
            body->vy = ricochet_blend_velocity(body->vy, body->vy + orthogonal_nudge);
        }
        if (hit_y) {
            body->vx = ricochet_blend_velocity(body->vx, body->vx + orthogonal_nudge);
        }
    }

    ricochet_record_trail(body, trail_length);
}

static unsigned long ricochet_variation_interval_millis(
    const screensave_saver_session *session
)
{
    if (session == NULL) {
        return 15000UL;
    }

    if (session->preview_mode) {
        return 9000UL;
    }
    if (session->object_count <= 1U) {
        return 14000UL;
    }
    return 18000UL;
}

static void ricochet_refresh_rhythm(screensave_saver_session *session)
{
    unsigned int body_index;
    ricochet_body *body;
    long target_velocity;

    if (session == NULL || session->object_count == 0U) {
        return;
    }

    body_index = (unsigned int)ricochet_rng_range(&session->rng, (unsigned long)session->object_count);
    body = &session->bodies[body_index];
    target_velocity = ricochet_random_speed_component(&session->rng, session->config.speed_mode, session->preview_mode);
    body->vx = ricochet_blend_velocity(body->vx, target_velocity);
    target_velocity = ricochet_random_speed_component(&session->rng, session->config.speed_mode, session->preview_mode);
    body->vy = ricochet_blend_velocity(body->vy, target_velocity);
    body->flash_timer = session->preview_mode ? 2 : 4;
}

int ricochet_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    const ricochet_config *configured;

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
    session->theme = ricochet_resolve_theme(environment);

    session->config.object_mode = RICOCHET_OBJECT_BLOCK;
    session->config.object_count = 1;
    session->config.speed_mode = RICOCHET_SPEED_STANDARD;
    session->config.trail_mode = RICOCHET_TRAIL_SHORT;
    configured = NULL;
    if (
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(ricochet_config)
    ) {
        configured = (const ricochet_config *)environment->config_binding->product_config;
    }
    if (configured != NULL) {
        session->config = *configured;
    }

    ricochet_rng_seed(&session->rng, environment->seed.stream_seed ^ environment->seed.base_seed);
    ricochet_initialize_bodies(session);
    *session_out = session;
    return 1;
}

void ricochet_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    free(session);
}

void ricochet_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    ricochet_initialize_bodies(session);
}

void ricochet_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long delta_millis;
    unsigned int index;
    int trail_length;

    if (session == NULL || environment == NULL) {
        return;
    }

    delta_millis = environment->clock.delta_millis;
    if (delta_millis > 200UL) {
        delta_millis = 200UL;
    }

    session->elapsed_millis += delta_millis;
    session->variation_elapsed_millis += delta_millis;
    trail_length = ricochet_trail_length(&session->config, session->detail_level, session->preview_mode);
    for (index = 0U; index < session->object_count; ++index) {
        ricochet_step_body(session, &session->bodies[index], delta_millis, trail_length);
        if (session->bodies[index].flash_timer > 0) {
            session->bodies[index].flash_timer -= 1;
        }
    }

    if (session->celebration_timer > 0) {
        session->celebration_timer -= 1;
    }
    if (session->variation_elapsed_millis >= ricochet_variation_interval_millis(session)) {
        session->variation_elapsed_millis = 0UL;
        ricochet_refresh_rhythm(session);
    }
}
