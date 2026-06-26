#include "nocturne_core.h"

#include <stdlib.h>
#include <string.h>

#define NOCTURNE_FIXED_ONE ((ss_i32)65536)
#define NOCTURNE_FIXED_HALF ((ss_i32)32768)

#define NOCTURNE_STAGE_FADE_IN 0
#define NOCTURNE_STAGE_STEADY 1
#define NOCTURNE_STAGE_FADE_OUT 2

typedef struct nocturne_core_rng_tag {
    ss_u32 state;
} nocturne_core_rng;

struct nocturne_core_session_tag {
    nocturne_config config;
    nocturne_core_theme theme;
    ss_u32 width;
    ss_u32 height;
    ss_u32 detail_level;
    ss_u32 preview_mode;
    nocturne_core_rng rng;
    ss_u32 cycle_index;
    ss_u32 cycle_duration_ms;
    ss_u32 stage_elapsed_ms;
    ss_u32 drift_refresh_ms;
    ss_u32 reseed_count;
    int stage;
    int fade_level;
    ss_i32 primary_x;
    ss_i32 primary_y;
    ss_i32 primary_vx;
    ss_i32 primary_vy;
    ss_i32 secondary_x;
    ss_i32 secondary_y;
    ss_i32 secondary_vx;
    ss_i32 secondary_vy;
    int breath_direction;
    int breath_level;
    int ambient_level;
};

static void nocturne_core_rng_seed(nocturne_core_rng *state, ss_u32 seed)
{
    if (state == 0) {
        return;
    }
    state->state = seed != 0U ? seed : (ss_u32)0x0A1E0A1EU;
}

static ss_u32 nocturne_core_rng_next(nocturne_core_rng *state)
{
    if (state == 0) {
        return 0U;
    }
    state->state = state->state * (ss_u32)1664525U + (ss_u32)1013904223U;
    return state->state;
}

static ss_u32 nocturne_core_rng_range(nocturne_core_rng *state, ss_u32 upper_bound)
{
    if (upper_bound == 0U) {
        return 0U;
    }
    return nocturne_core_rng_next(state) % upper_bound;
}

static ss_u32 nocturne_core_cycle_duration_ms(ss_u32 detail_level, ss_u32 preview_mode)
{
    if (preview_mode != 0U) {
        return 24000U;
    }
    switch (detail_level) {
    case NOCTURNE_DETAIL_LOW:
        return 90000U;
    case NOCTURNE_DETAIL_HIGH:
        return 52000U;
    case NOCTURNE_DETAIL_STANDARD:
    default:
        return 70000U;
    }
}

static ss_u32 nocturne_core_refresh_interval_ms(const nocturne_core_session *session)
{
    if (session == 0) {
        return 10000U;
    }
    if (session->preview_mode != 0U) {
        return 7000U;
    }
    switch (session->detail_level) {
    case NOCTURNE_DETAIL_LOW:
        return 16000U;
    case NOCTURNE_DETAIL_HIGH:
        return 9000U;
    case NOCTURNE_DETAIL_STANDARD:
    default:
        return 12000U;
    }
}

static ss_u32 nocturne_core_fade_units_per_second(int fade_speed)
{
    switch (fade_speed) {
    case NOCTURNE_FADE_SLOW:
        return 52U;
    case NOCTURNE_FADE_GENTLE:
        return 72U;
    case NOCTURNE_FADE_STANDARD:
    default:
        return 110U;
    }
}

static ss_i32 nocturne_core_speed_units(int motion_strength, ss_u32 preview_mode)
{
    ss_i32 speed;

    switch (motion_strength) {
    case NOCTURNE_STRENGTH_STILL:
        speed = 180;
        break;
    case NOCTURNE_STRENGTH_SOFT:
        speed = 420;
        break;
    case NOCTURNE_STRENGTH_SUBTLE:
    default:
        speed = 300;
        break;
    }
    if (preview_mode != 0U) {
        speed = (speed * 3) / 4;
    }
    return speed;
}

static ss_i32 nocturne_core_random_velocity(
    nocturne_core_rng *rng,
    int motion_strength,
    ss_u32 preview_mode)
{
    ss_i32 speed;

    speed = nocturne_core_speed_units(motion_strength, preview_mode);
    if ((nocturne_core_rng_next(rng) & 1U) != 0U) {
        return speed;
    }
    return -speed;
}

static ss_i32 nocturne_core_blend_velocity(ss_i32 current_velocity, ss_i32 target_velocity)
{
    ss_i32 blended_velocity;

    blended_velocity = (current_velocity * 3) / 4;
    blended_velocity += target_velocity / 4;
    if (blended_velocity > -96 && blended_velocity < 96) {
        if (blended_velocity < 0) {
            blended_velocity = -96;
        } else {
            blended_velocity = 96;
        }
    }
    return blended_velocity;
}

static void nocturne_core_set_initial_positions(nocturne_core_session *session)
{
    ss_i32 width_fixed;
    ss_i32 height_fixed;
    ss_u32 random_width;
    ss_u32 random_height;

    width_fixed = (ss_i32)session->width * NOCTURNE_FIXED_ONE;
    height_fixed = (ss_i32)session->height * NOCTURNE_FIXED_ONE;
    random_width = session->width > 1U ? session->width - 1U : 1U;
    random_height = session->height > 1U ? session->height - 1U : 1U;

    session->primary_x =
        NOCTURNE_FIXED_HALF +
        (ss_i32)nocturne_core_rng_range(&session->rng, random_width) * NOCTURNE_FIXED_ONE;
    session->primary_y =
        NOCTURNE_FIXED_HALF +
        (ss_i32)nocturne_core_rng_range(&session->rng, random_height) * NOCTURNE_FIXED_ONE;
    session->secondary_x =
        NOCTURNE_FIXED_HALF +
        (ss_i32)nocturne_core_rng_range(&session->rng, random_width) * NOCTURNE_FIXED_ONE;
    session->secondary_y =
        NOCTURNE_FIXED_HALF +
        (ss_i32)nocturne_core_rng_range(&session->rng, random_height) * NOCTURNE_FIXED_ONE;

    if (session->width <= 1U) {
        session->primary_x = width_fixed / 2;
        session->secondary_x = width_fixed / 2;
    }
    if (session->height <= 1U) {
        session->primary_y = height_fixed / 2;
        session->secondary_y = height_fixed / 2;
    }
}

static void nocturne_core_reset_cycle(nocturne_core_session *session)
{
    if (session == 0) {
        return;
    }
    session->cycle_index += 1U;
    session->stage = NOCTURNE_STAGE_FADE_IN;
    session->stage_elapsed_ms = 0U;
    session->drift_refresh_ms = 0U;
    session->fade_level = 0;
    session->cycle_duration_ms = nocturne_core_cycle_duration_ms(session->detail_level, session->preview_mode);
    session->primary_vx = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->primary_vy = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->secondary_vx = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->secondary_vy = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->breath_direction = 1;
    session->breath_level = 48 + (int)nocturne_core_rng_range(&session->rng, 48U);
    session->ambient_level = 28 + (int)nocturne_core_rng_range(&session->rng, 28U);
    nocturne_core_set_initial_positions(session);
}

static void nocturne_core_refresh_steady_motion(nocturne_core_session *session)
{
    ss_i32 target_velocity;

    if (session == 0) {
        return;
    }
    target_velocity = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->secondary_vx = nocturne_core_blend_velocity(session->secondary_vx, target_velocity);
    target_velocity = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
    session->secondary_vy = nocturne_core_blend_velocity(session->secondary_vy, target_velocity);

    if (session->config.motion_mode != NOCTURNE_MOTION_NONE) {
        target_velocity = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
        session->primary_vx = nocturne_core_blend_velocity(session->primary_vx, target_velocity);
        target_velocity = nocturne_core_random_velocity(&session->rng, session->config.motion_strength, session->preview_mode);
        session->primary_vy = nocturne_core_blend_velocity(session->primary_vy, target_velocity);
    }
    session->ambient_level = 24 + (int)nocturne_core_rng_range(&session->rng, 40U);
    session->breath_level = 44 + (int)nocturne_core_rng_range(&session->rng, 52U);
}

static void nocturne_core_advance_axis(ss_i32 *position, ss_i32 *velocity, ss_u32 limit, ss_u32 delta_ms)
{
    ss_i32 minimum;
    ss_i32 maximum;

    if (position == 0 || velocity == 0) {
        return;
    }
    minimum = NOCTURNE_FIXED_HALF;
    maximum = (ss_i32)((limit > 1U ? limit - 1U : 1U) * (ss_u32)NOCTURNE_FIXED_ONE) - NOCTURNE_FIXED_HALF;
    *position += *velocity * (ss_i32)delta_ms;
    if (*position < minimum) {
        *position = minimum;
        *velocity = -*velocity;
    } else if (*position > maximum) {
        *position = maximum;
        *velocity = -*velocity;
    }
}

static void nocturne_core_step_positions(nocturne_core_session *session, ss_u32 delta_ms)
{
    if (session == 0) {
        return;
    }
    nocturne_core_advance_axis(&session->primary_x, &session->primary_vx, session->width, delta_ms);
    nocturne_core_advance_axis(&session->primary_y, &session->primary_vy, session->height, delta_ms);
    nocturne_core_advance_axis(&session->secondary_x, &session->secondary_vx, session->width, delta_ms);
    nocturne_core_advance_axis(&session->secondary_y, &session->secondary_vy, session->height, delta_ms);

    if (session->config.motion_mode == NOCTURNE_MOTION_BREATH) {
        int delta_units;

        delta_units = (int)((delta_ms * 48U) / 1000U);
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

static void nocturne_core_step_stage(nocturne_core_session *session, ss_u32 delta_ms)
{
    ss_u32 fade_delta;

    if (session == 0) {
        return;
    }
    fade_delta = (delta_ms * nocturne_core_fade_units_per_second(session->config.fade_speed)) / 1000U;
    if (fade_delta == 0U && delta_ms > 0U) {
        fade_delta = 1U;
    }

    session->stage_elapsed_ms += delta_ms;
    if (session->stage == NOCTURNE_STAGE_FADE_IN) {
        session->fade_level += (int)fade_delta;
        if (session->fade_level >= 255) {
            session->fade_level = 255;
            session->stage = NOCTURNE_STAGE_STEADY;
            session->stage_elapsed_ms = 0U;
        }
        return;
    }
    if (session->stage == NOCTURNE_STAGE_STEADY) {
        if (session->stage_elapsed_ms >= session->cycle_duration_ms) {
            session->stage = NOCTURNE_STAGE_FADE_OUT;
            session->stage_elapsed_ms = 0U;
        }
        return;
    }
    session->fade_level -= (int)fade_delta;
    if (session->fade_level <= 0) {
        session->fade_level = 0;
        session->reseed_count += 1U;
        nocturne_core_reset_cycle(session);
    }
}

static ss_v2_color_rgba8 nocturne_core_make_color(ss_u8 red, ss_u8 green, ss_u8 blue, ss_u8 alpha)
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

static ss_v2_color_rgba8 nocturne_core_scale_color(
    ss_v2_color_rgba8 base_color,
    int fade_level,
    int motion_strength,
    ss_u32 preview_mode)
{
    ss_u32 scale;

    scale = (ss_u32)fade_level;
    if (motion_strength == NOCTURNE_STRENGTH_STILL) {
        scale = (scale * 3U) / 4U;
    } else if (motion_strength == NOCTURNE_STRENGTH_SOFT) {
        scale = (scale * 5U) / 4U;
    }
    if (preview_mode != 0U) {
        scale = (scale * 3U) / 4U;
    }
    if (scale > 255U) {
        scale = 255U;
    }
    base_color.red = (ss_u8)(((ss_u32)base_color.red * scale) / 255U);
    base_color.green = (ss_u8)(((ss_u32)base_color.green * scale) / 255U);
    base_color.blue = (ss_u8)(((ss_u32)base_color.blue * scale) / 255U);
    return base_color;
}

static ss_v2_color_rgba8 nocturne_core_scale_color_amount(ss_v2_color_rgba8 base_color, ss_u32 scale)
{
    if (scale > 255U) {
        scale = 255U;
    }
    base_color.red = (ss_u8)(((ss_u32)base_color.red * scale) / 255U);
    base_color.green = (ss_u8)(((ss_u32)base_color.green * scale) / 255U);
    base_color.blue = (ss_u8)(((ss_u32)base_color.blue * scale) / 255U);
    return base_color;
}

static ss_i32 nocturne_core_fixed_to_i32(ss_i32 value)
{
    return value / NOCTURNE_FIXED_ONE;
}

static ss_u32 nocturne_core_mark_size(const nocturne_core_session *session)
{
    ss_u32 size;

    size = session->height / 18U;
    if (session->detail_level == NOCTURNE_DETAIL_LOW) {
        size = session->height / 22U;
    } else if (session->detail_level == NOCTURNE_DETAIL_HIGH) {
        size = session->height / 14U;
    }
    if (size < 4U) {
        size = 4U;
    }
    return size;
}

static void nocturne_core_rect(ss_v2_recti *rect, ss_i32 x, ss_i32 y, ss_u32 width, ss_u32 height)
{
    rect->struct_size = (ss_u32)sizeof(*rect);
    rect->abi_version = SS_V2_ABI_VERSION;
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
}

static void nocturne_core_point(ss_v2_pointi *point, ss_i32 x, ss_i32 y)
{
    point->struct_size = (ss_u32)sizeof(*point);
    point->abi_version = SS_V2_ABI_VERSION;
    point->x = x;
    point->y = y;
}

static int nocturne_core_draw_clear(ss_v2_draw_target *target, const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->clear == 0) {
        return 0;
    }
    return target->ops->clear(target->user_data, color) == SS_V2_STATUS_OK;
}

static int nocturne_core_draw_fill_rect(ss_v2_draw_target *target, const ss_v2_recti *rect, const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->fill_rect == 0) {
        return 0;
    }
    return target->ops->fill_rect(target->user_data, rect, color) == SS_V2_STATUS_OK;
}

static int nocturne_core_draw_frame_rect(ss_v2_draw_target *target, const ss_v2_recti *rect, const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->frame_rect == 0) {
        return 0;
    }
    return target->ops->frame_rect(target->user_data, rect, color) == SS_V2_STATUS_OK;
}

static int nocturne_core_draw_polyline(
    ss_v2_draw_target *target,
    const ss_v2_pointi *points,
    ss_u32 point_count,
    const ss_v2_color_rgba8 *color)
{
    if (target == 0 || target->ops == 0 || target->ops->polyline == 0) {
        return 0;
    }
    return target->ops->polyline(target->user_data, points, point_count, color) == SS_V2_STATUS_OK;
}

static int nocturne_core_render_mark_at(
    ss_v2_draw_target *target,
    ss_i32 center_x,
    ss_i32 center_y,
    ss_u32 size,
    int outline_only,
    ss_v2_color_rgba8 primary,
    ss_v2_color_rgba8 accent)
{
    ss_v2_recti rect;

    nocturne_core_rect(&rect, center_x - (ss_i32)(size / 2U), center_y - (ss_i32)(size / 2U), size, size);
    if (!outline_only && !nocturne_core_draw_fill_rect(target, &rect, &primary)) {
        return 0;
    }
    return nocturne_core_draw_frame_rect(target, &rect, &accent);
}

static int nocturne_core_render_drift_mark(
    nocturne_core_session *session,
    ss_v2_draw_target *target,
    ss_v2_color_rgba8 primary,
    ss_v2_color_rgba8 accent)
{
    ss_u32 size;
    ss_i32 ghost_x;
    ss_i32 ghost_y;
    ss_v2_color_rgba8 ghost_color;

    size = nocturne_core_mark_size(session);
    ghost_x = nocturne_core_fixed_to_i32(session->secondary_x);
    ghost_y = nocturne_core_fixed_to_i32(session->secondary_y);
    ghost_color = nocturne_core_scale_color_amount(accent, (ss_u32)(48 + session->ambient_level));
    if (!nocturne_core_render_mark_at(target, ghost_x, ghost_y, size - 1U, 1, ghost_color, ghost_color)) {
        return 0;
    }
    return nocturne_core_render_mark_at(
        target,
        nocturne_core_fixed_to_i32(session->primary_x),
        nocturne_core_fixed_to_i32(session->primary_y),
        size,
        0,
        primary,
        accent);
}

static int nocturne_core_render_quiet_line(
    nocturne_core_session *session,
    ss_v2_draw_target *target,
    ss_v2_color_rgba8 primary,
    ss_v2_color_rgba8 accent)
{
    ss_v2_pointi points[3];
    ss_v2_recti marker;
    ss_v2_recti secondary_marker;
    ss_v2_color_rgba8 ghost_color;

    nocturne_core_point(&points[0], nocturne_core_fixed_to_i32(session->primary_x), nocturne_core_fixed_to_i32(session->primary_y));
    nocturne_core_point(
        &points[1],
        (ss_i32)(session->width / 2U),
        (points[0].y + nocturne_core_fixed_to_i32(session->secondary_y)) / 2);
    nocturne_core_point(&points[2], nocturne_core_fixed_to_i32(session->secondary_x), nocturne_core_fixed_to_i32(session->secondary_y));
    if (!nocturne_core_draw_polyline(target, points, 3U, &accent)) {
        return 0;
    }

    nocturne_core_rect(&marker, points[0].x - 2, points[0].y - 2, 4U, 4U);
    if (!nocturne_core_draw_fill_rect(target, &marker, &primary)) {
        return 0;
    }

    ghost_color = nocturne_core_scale_color_amount(accent, (ss_u32)(40 + session->ambient_level));
    nocturne_core_rect(&secondary_marker, points[2].x - 2, points[2].y - 2, 4U, 4U);
    return nocturne_core_draw_frame_rect(target, &secondary_marker, &ghost_color);
}

static int nocturne_core_render_monolith_at(
    ss_v2_draw_target *target,
    ss_i32 center_x,
    ss_i32 center_y,
    ss_u32 width,
    ss_u32 height,
    ss_v2_color_rgba8 primary,
    ss_v2_color_rgba8 accent)
{
    ss_v2_recti rect;

    nocturne_core_rect(&rect, center_x - (ss_i32)(width / 2U), center_y - (ss_i32)(height / 2U), width, height);
    if (!nocturne_core_draw_fill_rect(target, &rect, &primary)) {
        return 0;
    }
    return nocturne_core_draw_frame_rect(target, &rect, &accent);
}

static int nocturne_core_render_monolith(
    nocturne_core_session *session,
    ss_v2_draw_target *target,
    ss_v2_color_rgba8 primary,
    ss_v2_color_rgba8 accent)
{
    ss_u32 width;
    ss_u32 height;
    ss_v2_color_rgba8 ghost_fill;
    ss_v2_color_rgba8 ghost_outline;

    width = session->preview_mode != 0U ? 4U : 6U;
    if (session->detail_level == NOCTURNE_DETAIL_HIGH) {
        width = 8U;
    }
    height = session->height / 3U;
    if (height < 12U) {
        height = 12U;
    }
    ghost_fill = nocturne_core_scale_color_amount(primary, (ss_u32)(28 + session->ambient_level));
    ghost_outline = nocturne_core_scale_color_amount(accent, (ss_u32)(52 + session->ambient_level));
    if (!nocturne_core_render_monolith_at(
            target,
            nocturne_core_fixed_to_i32(session->secondary_x),
            nocturne_core_fixed_to_i32(session->secondary_y),
            width,
            (height * 9U) / 10U,
            ghost_fill,
            ghost_outline)) {
        return 0;
    }
    return nocturne_core_render_monolith_at(
        target,
        nocturne_core_fixed_to_i32(session->primary_x),
        nocturne_core_fixed_to_i32(session->primary_y),
        width,
        height,
        primary,
        accent);
}

static int nocturne_core_render_breath(
    nocturne_core_session *session,
    ss_v2_draw_target *target,
    ss_v2_color_rgba8 primary,
    ss_v2_color_rgba8 accent)
{
    ss_v2_recti rect;
    ss_v2_recti inner_rect;
    ss_u32 width;
    ss_u32 height;
    ss_i32 rect_x;
    ss_i32 rect_y;
    ss_v2_color_rgba8 inner_color;

    width = (session->width * (ss_u32)session->breath_level) / 600U;
    height = (session->height * (ss_u32)session->breath_level) / 900U;
    if (width < 16U) {
        width = 16U;
    }
    if (height < 10U) {
        height = 10U;
    }

    rect_x = ((ss_i32)session->width - (ss_i32)width) / 2;
    rect_y = ((ss_i32)session->height - (ss_i32)height) / 2;
    nocturne_core_rect(&rect, rect_x, rect_y, width, height);
    if (!nocturne_core_draw_fill_rect(target, &rect, &primary)) {
        return 0;
    }
    if (!nocturne_core_draw_frame_rect(target, &rect, &accent)) {
        return 0;
    }

    inner_rect = rect;
    inner_rect.x += 4;
    inner_rect.y += 3;
    if (inner_rect.width > 8U && inner_rect.height > 6U) {
        inner_rect.width -= 8U;
        inner_rect.height -= 6U;
        inner_color = nocturne_core_scale_color_amount(accent, (ss_u32)(24 + session->ambient_level));
        return nocturne_core_draw_frame_rect(target, &inner_rect, &inner_color);
    }
    return 1;
}

static int nocturne_core_render_vignette(
    nocturne_core_session *session,
    ss_v2_draw_target *target,
    ss_v2_color_rgba8 accent)
{
    ss_v2_recti frame_rect;
    ss_v2_color_rgba8 frame_color;

    if (session == 0 || target == 0 || session->preview_mode != 0U || session->config.motion_mode == NOCTURNE_MOTION_NONE) {
        return 1;
    }
    if (session->width <= 12U || session->height <= 12U) {
        return 1;
    }
    nocturne_core_rect(&frame_rect, 6, 6, session->width - 12U, session->height - 12U);
    frame_color = nocturne_core_scale_color_amount(accent, (ss_u32)(18 + session->ambient_level));
    return nocturne_core_draw_frame_rect(target, &frame_rect, &frame_color);
}

int nocturne_core_create(const nocturne_core_desc *desc, nocturne_core_session **session_out)
{
    nocturne_core_session *session;

    if (session_out == 0 || desc == 0 || desc->width == 0U || desc->height == 0U || desc->config == 0 || desc->theme == 0) {
        return 0;
    }
    *session_out = 0;
    session = (nocturne_core_session *)malloc(sizeof(*session));
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
    nocturne_core_rng_seed(&session->rng, desc->stream_seed ^ desc->base_seed);
    nocturne_core_reset_cycle(session);
    *session_out = session;
    return 1;
}

void nocturne_core_destroy(nocturne_core_session *session)
{
    free(session);
}

void nocturne_core_resize(nocturne_core_session *session, ss_u32 width, ss_u32 height, ss_u32 preview_mode)
{
    if (session == 0 || width == 0U || height == 0U) {
        return;
    }
    session->width = width;
    session->height = height;
    session->preview_mode = preview_mode != 0U ? 1U : 0U;
    nocturne_core_reset_cycle(session);
}

void nocturne_core_advance(nocturne_core_session *session, ss_u32 delta_ms)
{
    if (session == 0) {
        return;
    }
    if (delta_ms > 200U) {
        delta_ms = 200U;
    }
    nocturne_core_step_positions(session, delta_ms);
    nocturne_core_step_stage(session, delta_ms);
    if (session->stage == NOCTURNE_STAGE_STEADY) {
        session->drift_refresh_ms += delta_ms;
        if (session->drift_refresh_ms >= nocturne_core_refresh_interval_ms(session)) {
            session->drift_refresh_ms = 0U;
            nocturne_core_refresh_steady_motion(session);
        }
    }
}

int nocturne_core_render(nocturne_core_session *session, ss_v2_draw_target *target)
{
    ss_v2_color_rgba8 background;
    ss_v2_color_rgba8 primary;
    ss_v2_color_rgba8 accent;
    int ok;

    if (session == 0 || target == 0 || target->ops == 0) {
        return 0;
    }
    background = nocturne_core_make_color(0U, 0U, 0U, 255U);
    if (!nocturne_core_draw_clear(target, &background)) {
        return 0;
    }
    primary = nocturne_core_scale_color(
        session->theme.primary_color,
        session->fade_level,
        session->config.motion_strength,
        session->preview_mode);
    accent = nocturne_core_scale_color(
        session->theme.accent_color,
        session->fade_level,
        session->config.motion_strength,
        session->preview_mode);

    ok = 1;
    switch (session->config.motion_mode) {
    case NOCTURNE_MOTION_NONE:
        break;
    case NOCTURNE_MOTION_DRIFT_MARK:
        ok = nocturne_core_render_drift_mark(session, target, primary, accent);
        break;
    case NOCTURNE_MOTION_QUIET_LINE:
        ok = nocturne_core_render_quiet_line(session, target, primary, accent);
        break;
    case NOCTURNE_MOTION_BREATH:
        ok = nocturne_core_render_breath(session, target, primary, accent);
        break;
    case NOCTURNE_MOTION_MONOLITH:
    default:
        ok = nocturne_core_render_monolith(session, target, primary, accent);
        break;
    }
    if (!ok) {
        return 0;
    }
    return nocturne_core_render_vignette(session, target, accent);
}
