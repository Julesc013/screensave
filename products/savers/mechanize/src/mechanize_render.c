#include "mechanize_internal.h"

static const int g_mechanize_unit_x[16] = {
    1024, 946, 724, 392, 0, -392, -724, -946,
    -1024, -946, -724, -392, 0, 392, 724, 946
};

static const int g_mechanize_unit_y[16] = {
    0, 392, 724, 946, 1024, 946, 724, 392,
    0, -392, -724, -946, -1024, -946, -724, -392
};

static screensave_color mechanize_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 18;
    color.green = 16;
    color.blue = 12;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "steel_machine_room") == 0) {
        color.red = 16;
        color.green = 20;
        color.blue = 24;
    } else if (lstrcmpiA(session->theme->theme_key, "black_enamel_instrument") == 0) {
        color.red = 10;
        color.green = 10;
        color.blue = 12;
    } else if (lstrcmpiA(session->theme->theme_key, "industrial_green") == 0) {
        color.red = 12;
        color.green = 20;
        color.blue = 12;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_museum") == 0) {
        color.red = 28;
        color.green = 24;
        color.blue = 20;
    }

    return color;
}

static screensave_color mechanize_mix_color(
    screensave_color base,
    screensave_color accent,
    unsigned int accent_amount
)
{
    unsigned int base_amount;

    if (accent_amount > 255U) {
        accent_amount = 255U;
    }
    base_amount = 255U - accent_amount;

    base.red = (unsigned char)(((unsigned int)base.red * base_amount + (unsigned int)accent.red * accent_amount) / 255U);
    base.green = (unsigned char)(((unsigned int)base.green * base_amount + (unsigned int)accent.green * accent_amount) / 255U);
    base.blue = (unsigned char)(((unsigned int)base.blue * base_amount + (unsigned int)accent.blue * accent_amount) / 255U);
    return base;
}

static void mechanize_draw_fill(
    screensave_renderer *renderer,
    int x,
    int y,
    int width,
    int height,
    screensave_color color
)
{
    screensave_recti rect;

    if (renderer == NULL || width <= 0 || height <= 0) {
        return;
    }

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    screensave_renderer_fill_rect(renderer, &rect, color);
}

static void mechanize_draw_frame(
    screensave_renderer *renderer,
    int x,
    int y,
    int width,
    int height,
    screensave_color color
)
{
    screensave_recti rect;

    if (renderer == NULL || width <= 0 || height <= 0) {
        return;
    }

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    screensave_renderer_draw_frame_rect(renderer, &rect, color);
}

static void mechanize_draw_gear(
    screensave_renderer *renderer,
    const mechanize_gear *gear,
    screensave_color outline_color,
    screensave_color hub_color
)
{
    screensave_pointi points[17];
    unsigned int point_index;
    unsigned int vector_index;
    int radius;
    int local_radius;

    if (renderer == NULL || gear == NULL || !gear->active || gear->radius <= 0) {
        return;
    }

    radius = gear->radius;
    for (point_index = 0U; point_index < 16U; ++point_index) {
        vector_index = (gear->phase / 16U + point_index) & 15U;
        local_radius = (point_index & 1U) ? (radius * 7) / 8 : radius;
        points[point_index].x = gear->x + (g_mechanize_unit_x[vector_index] * local_radius) / 1024;
        points[point_index].y = gear->y + (g_mechanize_unit_y[vector_index] * local_radius) / 1024;
    }
    points[16] = points[0];
    screensave_renderer_draw_polyline(renderer, points, 17U, outline_color);
    mechanize_draw_fill(renderer, gear->x - 2, gear->y - 2, 5, 5, hub_color);
}

static void mechanize_draw_pointer(
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int radius,
    unsigned int phase,
    screensave_color color
)
{
    screensave_pointi start_point;
    screensave_pointi end_point;
    unsigned int vector_index;
    int pointer_radius;

    if (renderer == NULL || radius <= 0) {
        return;
    }

    vector_index = (phase / 16U) & 15U;
    pointer_radius = (radius * 3) / 4;
    start_point.x = center_x;
    start_point.y = center_y;
    end_point.x = center_x + (g_mechanize_unit_x[vector_index] * pointer_radius) / 1024;
    end_point.y = center_y + (g_mechanize_unit_y[vector_index] * pointer_radius) / 1024;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, color);
}

static void mechanize_draw_gear_train(
    screensave_saver_session *session,
    screensave_renderer *renderer
)
{
    unsigned int index;
    screensave_color outline_color;
    screensave_color hub_color;
    screensave_color rail_color;
    screensave_pointi start_point;
    screensave_pointi end_point;

    outline_color = session->theme->primary_color;
    hub_color = session->theme->accent_color;
    rail_color = mechanize_mix_color(mechanize_background_color(session), session->theme->accent_color, 120U);

    for (index = 1U; index < MECHANIZE_MAX_GEARS; ++index) {
        if (!session->gears[index - 1U].active || !session->gears[index].active) {
            continue;
        }

        start_point.x = session->gears[index - 1U].x;
        start_point.y = session->gears[index - 1U].y;
        end_point.x = session->gears[index].x;
        end_point.y = session->gears[index].y;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, rail_color);
    }

    for (index = 0U; index < MECHANIZE_MAX_GEARS; ++index) {
        mechanize_draw_gear(renderer, &session->gears[index], outline_color, hub_color);
    }

    if (session->event_pulse > 0U) {
        mechanize_draw_fill(
            renderer,
            12,
            session->drawable_size.height - 18,
            (session->drawable_size.width - 24) * (int)session->counter_window / 1000,
            6,
            session->theme->accent_color
        );
    }
}

static void mechanize_draw_cam_bank(
    screensave_saver_session *session,
    screensave_renderer *renderer
)
{
    unsigned int index;
    screensave_color outline_color;
    screensave_color accent_color;
    screensave_color follower_color;
    screensave_pointi start_point;
    screensave_pointi end_point;

    outline_color = session->theme->primary_color;
    accent_color = session->theme->accent_color;
    follower_color = mechanize_mix_color(mechanize_background_color(session), accent_color, 136U);

    for (index = 0U; index < MECHANIZE_MAX_GEARS; ++index) {
        int follower_height;

        if (!session->gears[index].active) {
            continue;
        }

        follower_height = session->follower_heights[index] + session->gears[index].radius;
        start_point.x = session->gears[index].x;
        start_point.y = session->gears[index].y - session->gears[index].radius - 2;
        end_point.x = start_point.x;
        end_point.y = start_point.y - follower_height;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, follower_color);
        mechanize_draw_fill(
            renderer,
            end_point.x - 6,
            end_point.y - 4,
            12,
            8,
            accent_color
        );
        mechanize_draw_gear(renderer, &session->gears[index], outline_color, accent_color);
    }
}

static void mechanize_draw_dial_assembly(
    screensave_saver_session *session,
    screensave_renderer *renderer
)
{
    unsigned int index;
    screensave_color frame_color;
    screensave_color pointer_color;

    frame_color = session->theme->primary_color;
    pointer_color = session->theme->accent_color;

    for (index = 0U; index < MECHANIZE_MAX_DIALS; ++index) {
        if (!session->dials[index].active) {
            continue;
        }

        mechanize_draw_frame(
            renderer,
            session->dials[index].x - session->dials[index].radius,
            session->dials[index].y - session->dials[index].radius,
            session->dials[index].radius * 2,
            session->dials[index].radius * 2,
            frame_color
        );
        mechanize_draw_pointer(
            renderer,
            session->dials[index].x,
            session->dials[index].y,
            session->dials[index].radius,
            session->dials[index].angle,
            pointer_color
        );
        mechanize_draw_fill(
            renderer,
            session->dials[index].x - 2,
            session->dials[index].y - 2,
            5,
            5,
            pointer_color
        );
    }

    for (index = 0U; index < MECHANIZE_MAX_GEARS; ++index) {
        mechanize_draw_gear(renderer, &session->gears[index], frame_color, pointer_color);
    }
}

void mechanize_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_recti outer_frame;
    screensave_color frame_color;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    screensave_renderer_clear(environment->renderer, mechanize_background_color(session));

    switch (session->config.scene_mode) {
    case MECHANIZE_SCENE_CAM_BANK:
        mechanize_draw_cam_bank(session, environment->renderer);
        break;

    case MECHANIZE_SCENE_DIAL_ASSEMBLY:
        mechanize_draw_dial_assembly(session, environment->renderer);
        break;

    case MECHANIZE_SCENE_GEAR_TRAIN:
    default:
        mechanize_draw_gear_train(session, environment->renderer);
        break;
    }

    if (!session->preview_mode) {
        frame_color = mechanize_mix_color(mechanize_background_color(session), session->theme->accent_color, 120U);
        outer_frame.x = 8;
        outer_frame.y = 8;
        outer_frame.width = session->drawable_size.width - 16;
        outer_frame.height = session->drawable_size.height - 16;
        if (outer_frame.width > 0 && outer_frame.height > 0) {
            screensave_renderer_draw_frame_rect(environment->renderer, &outer_frame, frame_color);
        }
    }
}
