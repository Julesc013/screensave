#include "observatory_internal.h"

static const int g_observatory_unit_x[16] = {
    1024, 946, 724, 392, 0, -392, -724, -946,
    -1024, -946, -724, -392, 0, 392, 724, 946
};

static const int g_observatory_unit_y[16] = {
    0, 392, 724, 946, 1024, 946, 724, 392,
    0, -392, -724, -946, -1024, -946, -724, -392
};

static screensave_color observatory_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 10;
    color.green = 12;
    color.blue = 18;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "monochrome_chart_room") == 0) {
        color.red = 18;
        color.green = 18;
        color.blue = 18;
    } else if (lstrcmpiA(session->theme->theme_key, "deep_blue_dome") == 0) {
        color.red = 10;
        color.green = 16;
        color.blue = 30;
    } else if (lstrcmpiA(session->theme->theme_key, "eclipse_watch") == 0) {
        color.red = 8;
        color.green = 8;
        color.blue = 12;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_museum_astronomy") == 0) {
        color.red = 12;
        color.green = 14;
        color.blue = 18;
    }

    return color;
}

static screensave_color observatory_mix_color(
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

static void observatory_draw_fill(
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

static void observatory_draw_ring(
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int radius,
    screensave_color color
)
{
    screensave_pointi points[17];
    unsigned int index;

    for (index = 0U; index < 16U; ++index) {
        points[index].x = center_x + (g_observatory_unit_x[index] * radius) / 1024;
        points[index].y = center_y + (g_observatory_unit_y[index] * radius) / 1024;
    }
    points[16] = points[0];
    screensave_renderer_draw_polyline(renderer, points, 17U, color);
}

static void observatory_draw_stars(screensave_saver_session *session, screensave_renderer *renderer)
{
    unsigned int index;
    screensave_color star_color;

    star_color = observatory_mix_color(observatory_background_color(session), session->theme->accent_color, 180U);
    for (index = 0U; index < session->star_count; ++index) {
        observatory_draw_fill(renderer, session->stars[index].x, session->stars[index].y, 1, 1, star_color);
    }
}

static void observatory_draw_bodies(screensave_saver_session *session, screensave_renderer *renderer, int center_x, int center_y)
{
    unsigned int index;
    screensave_color body_color;
    screensave_color ring_color;

    ring_color = observatory_mix_color(observatory_background_color(session), session->theme->primary_color, 96U + session->central_pulse * 4U);
    body_color = observatory_mix_color(observatory_background_color(session), session->theme->accent_color, 200U);
    for (index = 0U; index < session->body_count; ++index) {
        int vector_index;
        int x;
        int y;
        observatory_body *body;

        body = &session->bodies[index];
        vector_index = (body->phase / 16U) & 15U;
        observatory_draw_ring(renderer, center_x, center_y, body->orbit_radius, ring_color);
        x = center_x + (g_observatory_unit_x[vector_index] * body->orbit_radius) / 1024;
        y = center_y + (g_observatory_unit_y[vector_index] * body->orbit_radius) / 1024;
        observatory_draw_fill(renderer, x - body->size / 2, y - body->size / 2, body->size + 1, body->size + 1, body_color);
    }
}

void observatory_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer *renderer;
    screensave_color base_color;
    screensave_color frame_color;
    screensave_color center_color;
    int center_x;
    int center_y;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    base_color = observatory_background_color(session);
    frame_color = observatory_mix_color(base_color, session->theme->primary_color, 92U);
    center_color = observatory_mix_color(base_color, session->theme->accent_color, 180U + session->central_pulse * 4U);
    center_x = session->drawable_size.width / 2;
    center_y = session->drawable_size.height / 2;

    screensave_renderer_clear(renderer, base_color);
    observatory_draw_stars(session, renderer);
    if (session->config.scene_mode == OBSERVATORY_SCENE_CHART_ROOM) {
        observatory_draw_ring(renderer, center_x, center_y, session->drawable_size.height / 3, frame_color);
    } else if (session->config.scene_mode == OBSERVATORY_SCENE_DOME) {
        observatory_draw_ring(renderer, center_x, session->drawable_size.height - 12, session->drawable_size.height / 2, frame_color);
        center_y = session->drawable_size.height / 2 - 12;
    }
    observatory_draw_bodies(session, renderer, center_x, center_y);
    observatory_draw_fill(renderer, center_x - 2, center_y - 2, 5, 5, center_color);
}
