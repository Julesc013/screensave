#include "ricochet_core.h"

#include <stdlib.h>
#include <string.h>

#define RICOCHET_FIXED_ONE ((ss_i32)256)
#define RICOCHET_FIXED_HALF ((ss_i32)128)

typedef struct ricochet_core_rng_tag {
    ss_u32 state;
} ricochet_core_rng;

typedef struct ricochet_core_body_tag {
    ss_i32 x;
    ss_i32 y;
    ss_i32 vx;
    ss_i32 vy;
    int size;
    ss_u32 trail_head;
    ss_u32 trail_count;
    ss_i32 trail_x[RICOCHET_MAX_TRAIL_POINTS];
    ss_i32 trail_y[RICOCHET_MAX_TRAIL_POINTS];
    int trail_size[RICOCHET_MAX_TRAIL_POINTS];
    int flash_timer;
} ricochet_core_body;

struct ricochet_core_session_tag {
    ricochet_config config;
    ricochet_core_theme theme;
    ss_u32 width;
    ss_u32 height;
    ss_u32 detail_level;
    ss_u32 preview_mode;
    ricochet_core_rng rng;
    ss_u32 object_count;
    ricochet_core_body bodies[RICOCHET_MAX_OBJECTS];
    int celebration_timer;
    ss_u32 elapsed_ms;
    ss_u32 variation_elapsed_ms;
};

static ss_i32 ricochet_core_abs_i32(ss_i32 value)
{
    return value < 0 ? -value : value;
}

static void ricochet_core_rng_seed(ricochet_core_rng *state, ss_u32 seed)
{
    if (state == 0) {
        return;
    }
    state->state = seed != 0U ? seed : (ss_u32)0x09C0C710U;
}

static ss_u32 ricochet_core_rng_next(ricochet_core_rng *state)
{
    if (state == 0) {
        return 0U;
    }
    state->state = state->state * (ss_u32)1664525U + (ss_u32)1013904223U;
    return state->state;
}

static ss_u32 ricochet_core_rng_range(ricochet_core_rng *state, ss_u32 upper_bound)
{
    if (upper_bound == 0U) {
        return 0U;
    }
    return ricochet_core_rng_next(state) % upper_bound;
}

static int ricochet_core_effective_object_count(
    const ricochet_config *config,
    ss_u32 detail_level,
    ss_u32 preview_mode)
{
    int count;

    count = config != 0 ? config->object_count : 1;
    if (count < 1) {
        count = 1;
    }
    if (count > (int)RICOCHET_MAX_OBJECTS) {
        count = (int)RICOCHET_MAX_OBJECTS;
    }

    if (detail_level == RICOCHET_DETAIL_LOW && count > 3) {
        count = 3;
    }
    if (preview_mode != 0U && count > 3) {
        count = 3;
    }
    return count;
}

static int ricochet_core_trail_length(
    const ricochet_config *config,
    ss_u32 detail_level,
    ss_u32 preview_mode)
{
    int length;

    if (config == 0) {
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

    if (detail_level == RICOCHET_DETAIL_LOW && length > 7) {
        length = 7;
    }
    if (detail_level == RICOCHET_DETAIL_HIGH && length < 10 && config->trail_mode == RICOCHET_TRAIL_PHOSPHOR) {
        length = 10;
    }
    if (preview_mode != 0U && length > 5) {
        length = 5;
    }
    if (length > (int)RICOCHET_MAX_TRAIL_POINTS) {
        length = (int)RICOCHET_MAX_TRAIL_POINTS;
    }
    return length;
}

static int ricochet_core_body_size(const ricochet_core_session *session, ss_u32 index)
{
    int minimum_dimension;
    int size;

    minimum_dimension = (int)session->width;
    if ((int)session->height < minimum_dimension) {
        minimum_dimension = (int)session->height;
    }
    if (minimum_dimension < 24) {
        minimum_dimension = 24;
    }

    size = minimum_dimension / 12;
    if (session->object_count > 1U) {
        size = minimum_dimension / (15 + (int)index * 2);
    }
    if (session->detail_level == RICOCHET_DETAIL_LOW) {
        size = (size * 4) / 5;
    } else if (session->detail_level == RICOCHET_DETAIL_HIGH) {
        size = (size * 6) / 5;
    }
    if (session->preview_mode != 0U) {
        size = (size * 4) / 5;
    }
    if (size < 8) {
        size = 8;
    } else if (size > 30) {
        size = 30;
    }
    return size;
}

static ss_i32 ricochet_core_base_speed(int speed_mode, ss_u32 preview_mode)
{
    ss_i32 speed;

    switch (speed_mode) {
    case RICOCHET_SPEED_CALM:
        speed = 116;
        break;
    case RICOCHET_SPEED_LIVELY:
        speed = 248;
        break;
    case RICOCHET_SPEED_STANDARD:
    default:
        speed = 174;
        break;
    }
    if (preview_mode != 0U) {
        speed = (speed * 4) / 5;
    }
    return speed;
}

static ss_i32 ricochet_core_random_speed_component(
    ricochet_core_rng *rng,
    int speed_mode,
    ss_u32 preview_mode)
{
    ss_i32 speed;
    ss_i32 variation;

    speed = ricochet_core_base_speed(speed_mode, preview_mode);
    variation = (ss_i32)ricochet_core_rng_range(rng, 72U);
    if ((ricochet_core_rng_next(rng) & 1U) == 0U) {
        speed += variation;
    } else {
        speed = -(speed + variation);
    }
    if (speed > -96 && speed < 96) {
        if (speed < 0) {
            speed = -96;
        } else {
            speed = 96;
        }
    }
    return speed;
}

static ss_i32 ricochet_core_blend_velocity(ss_i32 current_velocity, ss_i32 target_velocity)
{
    ss_i32 blended_velocity;

    blended_velocity = ((current_velocity * 3) + target_velocity) / 4;
    if (blended_velocity > -92 && blended_velocity < 92) {
        if (blended_velocity < 0) {
            blended_velocity = -92;
        } else {
            blended_velocity = 92;
        }
    }
    return blended_velocity;
}

static int ricochet_core_body_overlaps(
    const ricochet_core_body *existing_body,
    const ricochet_core_body *candidate_body)
{
    ss_i32 distance_x;
    ss_i32 distance_y;
    ss_i32 minimum_spacing;

    if (existing_body == 0 || candidate_body == 0) {
        return 0;
    }
    distance_x = ricochet_core_abs_i32(existing_body->x - candidate_body->x);
    distance_y = ricochet_core_abs_i32(existing_body->y - candidate_body->y);
    minimum_spacing = (ss_i32)(((existing_body->size + candidate_body->size) * RICOCHET_FIXED_ONE) / 2);
    minimum_spacing += 3 * RICOCHET_FIXED_ONE;
    return distance_x < minimum_spacing && distance_y < minimum_spacing;
}

static void ricochet_core_reset_body_trail(ricochet_core_body *body, int trail_length)
{
    ss_u32 index;

    body->trail_head = 0U;
    body->trail_count = (ss_u32)trail_length;
    for (index = 0U; index < RICOCHET_MAX_TRAIL_POINTS; ++index) {
        body->trail_x[index] = body->x;
        body->trail_y[index] = body->y;
        body->trail_size[index] = body->size;
    }
}

static void ricochet_core_record_trail(ricochet_core_body *body, int trail_length)
{
    if (trail_length <= 0) {
        body->trail_count = 0U;
        body->trail_head = 0U;
        return;
    }

    body->trail_x[body->trail_head] = body->x;
    body->trail_y[body->trail_head] = body->y;
    body->trail_size[body->trail_head] = body->size;
    body->trail_head = (body->trail_head + 1U) % (ss_u32)trail_length;
    if (body->trail_count < (ss_u32)trail_length) {
        body->trail_count += 1U;
    }
}

static void ricochet_core_initialize_body(
    ricochet_core_session *session,
    ricochet_core_body *body,
    ss_u32 index)
{
    int width_limit;
    int height_limit;
    ss_i32 range_x;
    ss_i32 range_y;
    int trail_length;
    ss_u32 attempt_index;
    ss_u32 body_index;
    int overlap_found;

    body->size = ricochet_core_body_size(session, index);
    width_limit = (int)session->width - body->size;
    height_limit = (int)session->height - body->size;
    if (width_limit < 1) {
        width_limit = 1;
    }
    if (height_limit < 1) {
        height_limit = 1;
    }

    for (attempt_index = 0U; attempt_index < 12U; ++attempt_index) {
        range_x = (ss_i32)(ricochet_core_rng_range(&session->rng, (ss_u32)width_limit) + (ss_u32)(body->size / 2));
        range_y = (ss_i32)(ricochet_core_rng_range(&session->rng, (ss_u32)height_limit) + (ss_u32)(body->size / 2));
        body->x = (range_x * RICOCHET_FIXED_ONE) + RICOCHET_FIXED_HALF;
        body->y = (range_y * RICOCHET_FIXED_ONE) + RICOCHET_FIXED_HALF;

        overlap_found = 0;
        for (body_index = 0U; body_index < index; ++body_index) {
            if (ricochet_core_body_overlaps(&session->bodies[body_index], body)) {
                overlap_found = 1;
                break;
            }
        }
        if (!overlap_found) {
            break;
        }
    }

    body->vx = ricochet_core_random_speed_component(&session->rng, session->config.speed_mode, session->preview_mode);
    body->vy = ricochet_core_random_speed_component(&session->rng, session->config.speed_mode, session->preview_mode);
    body->flash_timer = 0;
    trail_length = ricochet_core_trail_length(&session->config, session->detail_level, session->preview_mode);
    ricochet_core_reset_body_trail(body, trail_length);
}

static void ricochet_core_initialize_bodies(ricochet_core_session *session)
{
    ss_u32 index;

    session->object_count = (ss_u32)ricochet_core_effective_object_count(
        &session->config,
        session->detail_level,
        session->preview_mode);
    if (session->object_count == 0U) {
        session->object_count = 1U;
    }

    for (index = 0U; index < session->object_count; ++index) {
        ricochet_core_initialize_body(session, &session->bodies[index], index);
    }
    for (; index < RICOCHET_MAX_OBJECTS; ++index) {
        memset(&session->bodies[index], 0, sizeof(session->bodies[index]));
    }
    session->celebration_timer = 0;
}

static void ricochet_core_step_body(
    ricochet_core_session *session,
    ricochet_core_body *body,
    ss_u32 delta_ms,
    int trail_length)
{
    ss_i32 half_size;
    ss_i32 minimum_x;
    ss_i32 maximum_x;
    ss_i32 minimum_y;
    ss_i32 maximum_y;
    int hit_x;
    int hit_y;

    hit_x = 0;
    hit_y = 0;
    body->x += (body->vx * (ss_i32)delta_ms) / 33;
    body->y += (body->vy * (ss_i32)delta_ms) / 33;

    half_size = ((ss_i32)body->size * RICOCHET_FIXED_ONE) / 2;
    minimum_x = half_size;
    minimum_y = half_size;
    maximum_x = ((ss_i32)(session->width - 1U) * RICOCHET_FIXED_ONE) - half_size;
    maximum_y = ((ss_i32)(session->height - 1U) * RICOCHET_FIXED_ONE) - half_size;
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
        session->celebration_timer = session->preview_mode != 0U ? 4 : 7;
    }
    if (hit_x || hit_y) {
        ss_i32 orthogonal_nudge;

        body->flash_timer = session->preview_mode != 0U ? 3 : 6;
        orthogonal_nudge = (ss_i32)ricochet_core_rng_range(&session->rng, 40U) - 20;
        if (hit_x) {
            body->vy = ricochet_core_blend_velocity(body->vy, body->vy + orthogonal_nudge);
        }
        if (hit_y) {
            body->vx = ricochet_core_blend_velocity(body->vx, body->vx + orthogonal_nudge);
        }
    }
    ricochet_core_record_trail(body, trail_length);
}

static ss_u32 ricochet_core_variation_interval_ms(const ricochet_core_session *session)
{
    if (session == 0) {
        return 15000U;
    }
    if (session->preview_mode != 0U) {
        return 9000U;
    }
    if (session->object_count <= 1U) {
        return 14000U;
    }
    return 18000U;
}

static void ricochet_core_refresh_rhythm(ricochet_core_session *session)
{
    ss_u32 body_index;
    ricochet_core_body *body;
    ss_i32 target_velocity;

    if (session == 0 || session->object_count == 0U) {
        return;
    }
    body_index = ricochet_core_rng_range(&session->rng, session->object_count);
    body = &session->bodies[body_index];
    target_velocity = ricochet_core_random_speed_component(&session->rng, session->config.speed_mode, session->preview_mode);
    body->vx = ricochet_core_blend_velocity(body->vx, target_velocity);
    target_velocity = ricochet_core_random_speed_component(&session->rng, session->config.speed_mode, session->preview_mode);
    body->vy = ricochet_core_blend_velocity(body->vy, target_velocity);
    body->flash_timer = session->preview_mode != 0U ? 2 : 4;
}

static ss_i32 ricochet_core_fixed_to_i32(ss_i32 value)
{
    return value / RICOCHET_FIXED_ONE;
}

static ss_v2_color_rgba8 ricochet_core_make_color(ss_u8 red, ss_u8 green, ss_u8 blue, ss_u8 alpha)
{
    ss_v2_color_rgba8 color;

    color.struct_size = (ss_u32)sizeof(color);
    color.abi_version = SS_V2_ABI_VERSION;
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = alpha;
    return color;
}

static ss_v2_color_rgba8 ricochet_core_scale_color(ss_v2_color_rgba8 color, ss_u32 scale)
{
    color.red = (ss_u8)(((ss_u32)color.red * scale) / 255U);
    color.green = (ss_u8)(((ss_u32)color.green * scale) / 255U);
    color.blue = (ss_u8)(((ss_u32)color.blue * scale) / 255U);
    return color;
}

static ss_v2_color_rgba8 ricochet_core_lerp_color(
    ss_v2_color_rgba8 start_color,
    ss_v2_color_rgba8 end_color,
    ss_u32 amount)
{
    if (amount > 255U) {
        amount = 255U;
    }
    start_color.red = (ss_u8)(((ss_u32)start_color.red * (255U - amount) + (ss_u32)end_color.red * amount) / 255U);
    start_color.green = (ss_u8)(((ss_u32)start_color.green * (255U - amount) + (ss_u32)end_color.green * amount) / 255U);
    start_color.blue = (ss_u8)(((ss_u32)start_color.blue * (255U - amount) + (ss_u32)end_color.blue * amount) / 255U);
    start_color.alpha = (ss_u8)(((ss_u32)start_color.alpha * (255U - amount) + (ss_u32)end_color.alpha * amount) / 255U);
    return start_color;
}

static void ricochet_core_rect(ss_v2_recti *rect, ss_i32 x, ss_i32 y, ss_u32 width, ss_u32 height)
{
    rect->struct_size = (ss_u32)sizeof(*rect);
    rect->abi_version = SS_V2_ABI_VERSION;
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
}

static void ricochet_core_point(ss_v2_pointi *point, ss_i32 x, ss_i32 y)
{
    point->struct_size = (ss_u32)sizeof(*point);
    point->abi_version = SS_V2_ABI_VERSION;
    point->x = x;
    point->y = y;
}

static int ricochet_core_draw_clear(ss_v2_draw_target *target, const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->clear == 0) {
        return 0;
    }
    return target->ops->clear(target->user_data, color) == SS_V2_STATUS_OK;
}

static int ricochet_core_draw_fill_rect(ss_v2_draw_target *target, const ss_v2_recti *rect, const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->fill_rect == 0) {
        return 0;
    }
    return target->ops->fill_rect(target->user_data, rect, color) == SS_V2_STATUS_OK;
}

static int ricochet_core_draw_frame_rect(ss_v2_draw_target *target, const ss_v2_recti *rect, const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->frame_rect == 0) {
        return 0;
    }
    return target->ops->frame_rect(target->user_data, rect, color) == SS_V2_STATUS_OK;
}

static int ricochet_core_draw_line(
    ss_v2_draw_target *target,
    const ss_v2_pointi *start_point,
    const ss_v2_pointi *end_point,
    const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->line == 0) {
        return 0;
    }
    return target->ops->line(target->user_data, start_point, end_point, color) == SS_V2_STATUS_OK;
}

static int ricochet_core_draw_disc(
    ss_v2_draw_target *target,
    int center_x,
    int center_y,
    int size,
    ss_v2_color_rgba8 fill_color,
    ss_v2_color_rgba8 outline_color)
{
    ss_v2_recti rect;
    int radius;
    int step;
    int span;

    radius = size / 2;
    if (radius < 2) {
        radius = 2;
    }
    for (step = -radius; step <= radius; ++step) {
        span = radius;
        if (step < 0) {
            if (-step > (radius * 3) / 4) {
                span = (radius * 2) / 3;
            } else if (-step > radius / 2) {
                span = (radius * 5) / 6;
            }
        } else {
            if (step > (radius * 3) / 4) {
                span = (radius * 2) / 3;
            } else if (step > radius / 2) {
                span = (radius * 5) / 6;
            }
        }
        ricochet_core_rect(&rect, center_x - span, center_y + step, (ss_u32)((span * 2) + 1), 1U);
        if (!ricochet_core_draw_fill_rect(target, &rect, &fill_color)) {
            return 0;
        }
    }
    ricochet_core_rect(&rect, center_x - radius, center_y - radius, (ss_u32)(radius * 2), (ss_u32)(radius * 2));
    return ricochet_core_draw_frame_rect(target, &rect, &outline_color);
}

static int ricochet_core_draw_emblem(
    ss_v2_draw_target *target,
    int center_x,
    int center_y,
    int size,
    ss_v2_color_rgba8 fill_color,
    ss_v2_color_rgba8 outline_color)
{
    ss_v2_recti rect;
    ss_v2_pointi start_point;
    ss_v2_pointi end_point;
    int arm;

    ricochet_core_rect(&rect, center_x - (size / 2), center_y - (size / 2), (ss_u32)size, (ss_u32)size);
    if (!ricochet_core_draw_fill_rect(target, &rect, &fill_color)) {
        return 0;
    }
    if (!ricochet_core_draw_frame_rect(target, &rect, &outline_color)) {
        return 0;
    }
    arm = size / 3;
    if (arm < 2) {
        arm = 2;
    }
    ricochet_core_point(&start_point, center_x - arm, center_y);
    ricochet_core_point(&end_point, center_x + arm, center_y);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &outline_color)) {
        return 0;
    }
    ricochet_core_point(&start_point, center_x, center_y - arm);
    ricochet_core_point(&end_point, center_x, center_y + arm);
    return ricochet_core_draw_line(target, &start_point, &end_point, &outline_color);
}

static int ricochet_core_draw_block(
    ss_v2_draw_target *target,
    int center_x,
    int center_y,
    int size,
    ss_v2_color_rgba8 fill_color,
    ss_v2_color_rgba8 outline_color)
{
    ss_v2_recti rect;

    ricochet_core_rect(&rect, center_x - (size / 2), center_y - (size / 2), (ss_u32)size, (ss_u32)size);
    if (!ricochet_core_draw_fill_rect(target, &rect, &fill_color)) {
        return 0;
    }
    return ricochet_core_draw_frame_rect(target, &rect, &outline_color);
}

static int ricochet_core_draw_shape(
    ricochet_core_session *session,
    ss_v2_draw_target *target,
    int center_x,
    int center_y,
    int size,
    ss_v2_color_rgba8 fill_color,
    ss_v2_color_rgba8 outline_color)
{
    switch (session->config.object_mode) {
    case RICOCHET_OBJECT_DISC:
        return ricochet_core_draw_disc(target, center_x, center_y, size, fill_color, outline_color);
    case RICOCHET_OBJECT_EMBLEM:
        return ricochet_core_draw_emblem(target, center_x, center_y, size, fill_color, outline_color);
    case RICOCHET_OBJECT_BLOCK:
    default:
        return ricochet_core_draw_block(target, center_x, center_y, size, fill_color, outline_color);
    }
}

static int ricochet_core_render_trails(
    ricochet_core_session *session,
    ss_v2_draw_target *target,
    const ricochet_core_body *body)
{
    ss_u32 age;
    ss_u32 index;
    ss_u32 scale;
    int trail_limit;
    int center_x;
    int center_y;
    ss_v2_color_rgba8 fill_color;
    ss_v2_color_rgba8 outline_color;

    trail_limit = (int)body->trail_count;
    for (age = 0U; age < (ss_u32)trail_limit; ++age) {
        if (body->trail_count == 0U) {
            break;
        }
        index = (body->trail_head + body->trail_count - age - 1U) % body->trail_count;
        scale = 144U;
        if (trail_limit > 1) {
            scale = 28U + ((((ss_u32)trail_limit - age) * 164U) / (ss_u32)trail_limit);
        }
        if (session->config.trail_mode == RICOCHET_TRAIL_PHOSPHOR) {
            scale += 24U;
        }
        if (scale > 232U) {
            scale = 232U;
        }
        fill_color = ricochet_core_scale_color(session->theme.primary_color, scale);
        outline_color = ricochet_core_scale_color(session->theme.accent_color, scale);
        center_x = (int)ricochet_core_fixed_to_i32(body->trail_x[index]);
        center_y = (int)ricochet_core_fixed_to_i32(body->trail_y[index]);
        if (!ricochet_core_draw_shape(
                session,
                target,
                center_x,
                center_y,
                body->trail_size[index],
                fill_color,
                outline_color)) {
            return 0;
        }
    }
    return 1;
}

static int ricochet_core_render_corner_celebration(
    ricochet_core_session *session,
    ss_v2_draw_target *target,
    ss_v2_color_rgba8 accent_color)
{
    ss_v2_pointi start_point;
    ss_v2_pointi end_point;
    int inset;
    int arm;

    if (session == 0 || target == 0 || session->celebration_timer <= 0 || session->preview_mode != 0U) {
        return 1;
    }
    inset = 10;
    arm = 16;

    ricochet_core_point(&start_point, inset, inset + arm);
    ricochet_core_point(&end_point, inset, inset);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }
    ricochet_core_point(&end_point, inset + arm, inset);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }

    ricochet_core_point(&start_point, (ss_i32)session->width - inset - 1, inset + arm);
    ricochet_core_point(&end_point, (ss_i32)session->width - inset - 1, inset);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }
    ricochet_core_point(&end_point, (ss_i32)session->width - inset - arm - 1, inset);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }

    ricochet_core_point(&start_point, inset, (ss_i32)session->height - inset - arm - 1);
    ricochet_core_point(&end_point, inset, (ss_i32)session->height - inset - 1);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }
    ricochet_core_point(&end_point, inset + arm, (ss_i32)session->height - inset - 1);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }

    ricochet_core_point(&start_point, (ss_i32)session->width - inset - 1, (ss_i32)session->height - inset - arm - 1);
    ricochet_core_point(&end_point, (ss_i32)session->width - inset - 1, (ss_i32)session->height - inset - 1);
    if (!ricochet_core_draw_line(target, &start_point, &end_point, &accent_color)) {
        return 0;
    }
    ricochet_core_point(&end_point, (ss_i32)session->width - inset - arm - 1, (ss_i32)session->height - inset - 1);
    return ricochet_core_draw_line(target, &start_point, &end_point, &accent_color);
}

int ricochet_core_create(const ricochet_core_desc *desc, ricochet_core_session **session_out)
{
    ricochet_core_session *session;

    if (session_out == 0 || desc == 0 || desc->config == 0 || desc->theme == 0 || desc->width == 0U || desc->height == 0U) {
        return 0;
    }
    *session_out = 0;
    session = (ricochet_core_session *)malloc(sizeof(*session));
    if (session == 0) {
        return 0;
    }
    memset(session, 0, sizeof(*session));
    session->config = *desc->config;
    session->theme = *desc->theme;
    session->width = desc->width;
    session->height = desc->height;
    session->detail_level = desc->detail_level;
    session->preview_mode = desc->preview_mode != 0U ? 1U : 0U;
    ricochet_core_rng_seed(&session->rng, desc->stream_seed ^ desc->base_seed);
    ricochet_core_initialize_bodies(session);
    *session_out = session;
    return 1;
}

void ricochet_core_destroy(ricochet_core_session *session)
{
    free(session);
}

void ricochet_core_resize(ricochet_core_session *session, ss_u32 width, ss_u32 height, ss_u32 preview_mode)
{
    if (session == 0 || width == 0U || height == 0U) {
        return;
    }
    session->width = width;
    session->height = height;
    session->preview_mode = preview_mode != 0U ? 1U : 0U;
    ricochet_core_initialize_bodies(session);
}

void ricochet_core_advance(ricochet_core_session *session, ss_u32 delta_ms)
{
    ss_u32 index;
    int trail_length;

    if (session == 0) {
        return;
    }
    if (delta_ms > 200U) {
        delta_ms = 200U;
    }
    session->elapsed_ms += delta_ms;
    session->variation_elapsed_ms += delta_ms;
    trail_length = ricochet_core_trail_length(&session->config, session->detail_level, session->preview_mode);
    for (index = 0U; index < session->object_count; ++index) {
        ricochet_core_step_body(session, &session->bodies[index], delta_ms, trail_length);
        if (session->bodies[index].flash_timer > 0) {
            session->bodies[index].flash_timer -= 1;
        }
    }
    if (session->celebration_timer > 0) {
        session->celebration_timer -= 1;
    }
    if (session->variation_elapsed_ms >= ricochet_core_variation_interval_ms(session)) {
        session->variation_elapsed_ms = 0U;
        ricochet_core_refresh_rhythm(session);
    }
}

int ricochet_core_render(ricochet_core_session *session, ss_v2_draw_target *target)
{
    ss_v2_color_rgba8 background;
    ss_v2_color_rgba8 fill_color;
    ss_v2_color_rgba8 outline_color;
    ss_v2_color_rgba8 shadow_color;
    ss_v2_color_rgba8 accent_color;
    ss_v2_recti frame_rect;
    ss_u32 index;
    int center_x;
    int center_y;

    if (session == 0 || target == 0 || target->ops == 0) {
        return 0;
    }
    background = ricochet_core_make_color(0U, 0U, 0U, 255U);
    if (!ricochet_core_draw_clear(target, &background)) {
        return 0;
    }

    fill_color = session->theme.primary_color;
    outline_color = session->theme.accent_color;
    shadow_color = ricochet_core_scale_color(session->theme.accent_color, 48U);

    if (session->config.trail_mode != RICOCHET_TRAIL_NONE) {
        for (index = 0U; index < session->object_count; ++index) {
            if (!ricochet_core_render_trails(session, target, &session->bodies[index])) {
                return 0;
            }
        }
    }

    for (index = 0U; index < session->object_count; ++index) {
        center_x = (int)ricochet_core_fixed_to_i32(session->bodies[index].x);
        center_y = (int)ricochet_core_fixed_to_i32(session->bodies[index].y);
        if (session->bodies[index].flash_timer > 0) {
            fill_color = ricochet_core_lerp_color(session->theme.primary_color, outline_color, 92U);
            outline_color = ricochet_core_lerp_color(session->theme.accent_color, fill_color, 128U);
        } else {
            fill_color = session->theme.primary_color;
            outline_color = session->theme.accent_color;
        }
        if (session->preview_mode == 0U) {
            if (!ricochet_core_draw_shape(
                    session,
                    target,
                    center_x + 1,
                    center_y + 1,
                    session->bodies[index].size,
                    shadow_color,
                    shadow_color)) {
                return 0;
            }
        }
        if (!ricochet_core_draw_shape(
                session,
                target,
                center_x,
                center_y,
                session->bodies[index].size,
                fill_color,
                outline_color)) {
            return 0;
        }
    }

    if (session->celebration_timer > 0 && session->preview_mode == 0U) {
        if (session->width > 16U && session->height > 16U) {
            ricochet_core_rect(&frame_rect, 8, 8, session->width - 16U, session->height - 16U);
            accent_color = ricochet_core_scale_color(session->theme.accent_color, 104U);
            if (!ricochet_core_draw_frame_rect(target, &frame_rect, &accent_color)) {
                return 0;
            }
        }
        if (!ricochet_core_render_corner_celebration(
                session,
                target,
                ricochet_core_scale_color(session->theme.accent_color, 188U))) {
            return 0;
        }
    }
    return 1;
}
