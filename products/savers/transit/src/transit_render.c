#include "transit_internal.h"

static screensave_color transit_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 8;
    color.green = 12;
    color.blue = 22;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "freight_night_run") == 0) {
        color.red = 12;
        color.green = 16;
        color.blue = 22;
    } else if (lstrcmpiA(session->theme->theme_key, "neon_tunnel") == 0) {
        color.red = 12;
        color.green = 10;
        color.blue = 20;
    } else if (lstrcmpiA(session->theme->theme_key, "harbor_lights") == 0) {
        color.red = 6;
        color.green = 14;
        color.blue = 20;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_industrial_edge") == 0) {
        color.red = 12;
        color.green = 12;
        color.blue = 14;
    }

    return color;
}

static screensave_color transit_mix_color(
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

static void transit_draw_fill(
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

static int transit_triangle_wave(unsigned int phase)
{
    unsigned int local_phase;

    local_phase = phase & 255U;
    if (local_phase < 128U) {
        return (int)local_phase - 64;
    }

    return 64 - (int)(local_phase - 128U);
}

static int transit_horizon_y(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 80;
    }

    switch (session->config.scene_mode) {
    case TRANSIT_SCENE_HARBOR:
        return session->drawable_size.height / 2;
    case TRANSIT_SCENE_RAIL:
        return session->drawable_size.height / 3;
    case TRANSIT_SCENE_MOTORWAY:
    default:
        return session->drawable_size.height / 3 + 12;
    }
}

static int transit_project_y(const screensave_saver_session *session, int depth_fixed)
{
    int horizon_y;
    int height;

    horizon_y = transit_horizon_y(session);
    height = session->drawable_size.height - horizon_y;
    return horizon_y + ((1024 - depth_fixed) * height) / 1024;
}

static int transit_project_x(const screensave_saver_session *session, int lane, int depth_fixed)
{
    int center_x;
    int spread;

    center_x = session->drawable_size.width / 2 + transit_triangle_wave(session->sway_phase) / 4;
    spread = 12 + ((1024 - depth_fixed) * 3) / 20;
    return center_x + lane * spread;
}

static void transit_draw_corridor_base(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    int horizon_y
)
{
    screensave_color sky_color;
    screensave_color ground_color;
    screensave_color line_color;
    screensave_pointi start_point;
    screensave_pointi end_point;

    sky_color = transit_background_color(session);
    ground_color = transit_mix_color(sky_color, session->theme->primary_color, 64U);
    line_color = transit_mix_color(sky_color, session->theme->accent_color, 112U + session->event_pulse * 6U);

    screensave_renderer_clear(renderer, sky_color);
    transit_draw_fill(renderer, 0, horizon_y, session->drawable_size.width, session->drawable_size.height - horizon_y, ground_color);

    if (session->config.scene_mode == TRANSIT_SCENE_MOTORWAY || session->config.scene_mode == TRANSIT_SCENE_RAIL) {
        start_point.x = 0;
        start_point.y = session->drawable_size.height - 1;
        end_point.x = session->drawable_size.width / 2 - 24;
        end_point.y = horizon_y;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, line_color);
        start_point.x = session->drawable_size.width - 1;
        end_point.x = session->drawable_size.width / 2 + 24;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, line_color);
    }
}

static void transit_draw_motorway(
    screensave_saver_session *session,
    screensave_renderer *renderer
)
{
    int horizon_y;
    int marker_index;
    screensave_color marker_color;
    screensave_recti marker_rect;

    horizon_y = transit_horizon_y(session);
    transit_draw_corridor_base(session, renderer, horizon_y);
    marker_color = transit_mix_color(transit_background_color(session), session->theme->accent_color, 144U);

    for (marker_index = 0; marker_index < 8; ++marker_index) {
        int depth_fixed;
        int y;
        int height;
        int width;

        depth_fixed = 960 - ((int)(session->route_phase * 22U) + marker_index * 120) % 960;
        y = transit_project_y(session, depth_fixed);
        height = 4 + (1024 - depth_fixed) / 120;
        width = 2 + (1024 - depth_fixed) / 180;
        marker_rect.x = session->drawable_size.width / 2 - width / 2;
        marker_rect.y = y;
        marker_rect.width = width;
        marker_rect.height = height;
        screensave_renderer_fill_rect(renderer, &marker_rect, marker_color);
    }
}

static void transit_draw_rail(screensave_saver_session *session, screensave_renderer *renderer)
{
    int horizon_y;
    int tie_index;
    screensave_color tie_color;
    screensave_pointi start_point;
    screensave_pointi end_point;

    horizon_y = transit_horizon_y(session);
    transit_draw_corridor_base(session, renderer, horizon_y);
    tie_color = transit_mix_color(transit_background_color(session), session->theme->accent_color, 136U);

    for (tie_index = 0; tie_index < 10; ++tie_index) {
        int depth_fixed;
        int y;

        depth_fixed = 980 - ((int)(session->route_phase * 18U) + tie_index * 96) % 980;
        y = transit_project_y(session, depth_fixed);
        start_point.x = transit_project_x(session, -1, depth_fixed);
        start_point.y = y;
        end_point.x = transit_project_x(session, 1, depth_fixed);
        end_point.y = y;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, tie_color);
    }
}

static void transit_draw_harbor(screensave_saver_session *session, screensave_renderer *renderer)
{
    int horizon_y;
    screensave_color water_line;

    horizon_y = transit_horizon_y(session);
    transit_draw_corridor_base(session, renderer, horizon_y);
    water_line = transit_mix_color(transit_background_color(session), session->theme->primary_color, 116U);
    transit_draw_fill(renderer, 0, horizon_y, session->drawable_size.width, 2, water_line);
}

static void transit_draw_lights(screensave_saver_session *session, screensave_renderer *renderer)
{
    unsigned int index;
    int horizon_y;
    screensave_color glow_color;
    screensave_color core_color;

    horizon_y = transit_horizon_y(session);
    glow_color = transit_mix_color(transit_background_color(session), session->theme->primary_color, 112U);
    core_color = transit_mix_color(transit_background_color(session), session->theme->accent_color, 188U);
    for (index = 0U; index < session->light_count; ++index) {
        int x;
        int y;
        int size;
        transit_light *light;

        light = &session->lights[index];
        if (!light->active) {
            continue;
        }

        x = transit_project_x(session, light->lane, light->depth_fixed);
        y = transit_project_y(session, light->depth_fixed);
        size = 2 + (1024 - light->depth_fixed) / 220 + light->class_id;
        transit_draw_fill(renderer, x - size, y - size / 2, size * 2, size, glow_color);
        transit_draw_fill(renderer, x, y, size, size, core_color);
        if (session->config.scene_mode == TRANSIT_SCENE_HARBOR || session->config.scene_mode == TRANSIT_SCENE_MOTORWAY) {
            transit_draw_fill(renderer, x, y + (y - horizon_y) / 5 + 2, 1, size + 3, glow_color);
        }
    }
}

void transit_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer *renderer;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    switch (session->config.scene_mode) {
    case TRANSIT_SCENE_RAIL:
        transit_draw_rail(session, renderer);
        break;
    case TRANSIT_SCENE_HARBOR:
        transit_draw_harbor(session, renderer);
        break;
    case TRANSIT_SCENE_MOTORWAY:
    default:
        transit_draw_motorway(session, renderer);
        break;
    }
    transit_draw_lights(session, renderer);
}
