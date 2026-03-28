#include "ecosystems_internal.h"

static screensave_color ecosystems_mix_color(
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

static screensave_color ecosystems_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 6;
    color.green = 18;
    color.blue = 28;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "aviary_silhouettes") == 0) {
        color.red = 42;
        color.green = 54;
        color.blue = 68;
    } else if (lstrcmpiA(session->theme->theme_key, "dusk_fireflies") == 0) {
        color.red = 26;
        color.green = 18;
        color.blue = 34;
    } else if (lstrcmpiA(session->theme->theme_key, "wetland_reeds") == 0) {
        color.red = 18;
        color.green = 28;
        color.blue = 18;
    } else if (lstrcmpiA(session->theme->theme_key, "dark_oceanic") == 0) {
        color.red = 4;
        color.green = 10;
        color.blue = 20;
    }

    return color;
}

static void ecosystems_draw_fill(
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

static void ecosystems_draw_bands(
    screensave_renderer *renderer,
    const screensave_saver_session *session
)
{
    screensave_color background;
    screensave_color primary_band;
    screensave_color accent_band;

    if (renderer == NULL || session == NULL || session->theme == NULL) {
        return;
    }

    background = ecosystems_background_color(session);
    primary_band = ecosystems_mix_color(background, session->theme->primary_color, 88U);
    accent_band = ecosystems_mix_color(background, session->theme->accent_color, 56U);

    ecosystems_draw_fill(renderer, 0, 0, session->drawable_size.width, session->drawable_size.height / 2, primary_band);
    ecosystems_draw_fill(
        renderer,
        0,
        session->drawable_size.height / 2,
        session->drawable_size.width,
        session->drawable_size.height / 3,
        accent_band
    );
}

static void ecosystems_draw_reeds(
    screensave_renderer *renderer,
    const screensave_saver_session *session
)
{
    int index;
    screensave_pointi start_point;
    screensave_pointi end_point;
    screensave_color reed_color;

    if (renderer == NULL || session == NULL || session->theme == NULL) {
        return;
    }

    reed_color = ecosystems_mix_color(ecosystems_background_color(session), session->theme->primary_color, 160U);
    for (index = 0; index < session->drawable_size.width; index += 18) {
        start_point.x = index;
        start_point.y = session->drawable_size.height;
        end_point.x = index + ((index / 18) & 1 ? -6 : 6);
        end_point.y = session->drawable_size.height - 24 - ((index / 18) % 3) * 8;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, reed_color);
    }
}

static void ecosystems_draw_aquarium_agent(
    screensave_renderer *renderer,
    const ecosystems_agent *agent,
    screensave_color color
)
{
    screensave_pointi points[4];
    int x;
    int y;
    int direction;

    if (renderer == NULL || agent == NULL || !agent->active) {
        return;
    }

    x = agent->x_fixed / 16;
    y = agent->y_fixed / 16;
    direction = agent->vx_fixed >= 0 ? 1 : -1;

    points[0].x = x - 4 * direction;
    points[0].y = y;
    points[1].x = x + 3 * direction;
    points[1].y = y - 2;
    points[2].x = x + 6 * direction;
    points[2].y = y;
    points[3].x = x + 3 * direction;
    points[3].y = y + 2;
    screensave_renderer_draw_polyline(renderer, points, 4U, color);
}

static void ecosystems_draw_aviary_agent(
    screensave_renderer *renderer,
    const ecosystems_agent *agent,
    screensave_color color
)
{
    screensave_pointi points[3];
    int x;
    int y;

    if (renderer == NULL || agent == NULL || !agent->active) {
        return;
    }

    x = agent->x_fixed / 16;
    y = agent->y_fixed / 16;
    points[0].x = x - 4;
    points[0].y = y + 1;
    points[1].x = x;
    points[1].y = y - 2;
    points[2].x = x + 4;
    points[2].y = y + 1;
    screensave_renderer_draw_polyline(renderer, points, 3U, color);
}

static void ecosystems_draw_firefly_agent(
    screensave_renderer *renderer,
    const ecosystems_agent *agent,
    screensave_color base_color
)
{
    screensave_color glow_color;
    int size;
    int x;
    int y;

    if (renderer == NULL || agent == NULL || !agent->active) {
        return;
    }

    x = agent->x_fixed / 16;
    y = agent->y_fixed / 16;
    glow_color = ecosystems_mix_color(ecosystems_background_color(NULL), base_color, (unsigned int)agent->brightness);
    glow_color = ecosystems_mix_color(base_color, glow_color, (unsigned int)agent->brightness);
    size = agent->brightness > 170 ? 3 : 2;
    ecosystems_draw_fill(renderer, x, y, size, size, glow_color);
}

void ecosystems_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned int index;
    screensave_color draw_color;
    screensave_color frame_color;
    screensave_recti outer_frame;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    screensave_renderer_clear(environment->renderer, ecosystems_background_color(session));
    ecosystems_draw_bands(environment->renderer, session);
    if (
        session->config.habitat_mode == ECOSYSTEMS_HABITAT_FIREFLIES ||
        lstrcmpiA(session->theme->theme_key, "wetland_reeds") == 0
    ) {
        ecosystems_draw_reeds(environment->renderer, session);
    }

    for (index = 0U; index < session->agent_count; ++index) {
        draw_color = ecosystems_mix_color(
            session->theme->primary_color,
            session->theme->accent_color,
            (unsigned int)session->agents[index].brightness
        );

        switch (session->config.habitat_mode) {
        case ECOSYSTEMS_HABITAT_AVIARY:
            ecosystems_draw_aviary_agent(environment->renderer, &session->agents[index], draw_color);
            break;

        case ECOSYSTEMS_HABITAT_FIREFLIES:
            ecosystems_draw_firefly_agent(environment->renderer, &session->agents[index], session->theme->accent_color);
            break;

        case ECOSYSTEMS_HABITAT_AQUARIUM:
        default:
            ecosystems_draw_aquarium_agent(environment->renderer, &session->agents[index], draw_color);
            break;
        }
    }

    if (!session->preview_mode) {
        frame_color = ecosystems_mix_color(ecosystems_background_color(session), session->theme->accent_color, 120U);
        outer_frame.x = 8;
        outer_frame.y = 8;
        outer_frame.width = session->drawable_size.width - 16;
        outer_frame.height = session->drawable_size.height - 16;
        if (outer_frame.width > 0 && outer_frame.height > 0) {
            screensave_renderer_draw_frame_rect(environment->renderer, &outer_frame, frame_color);
        }
    }
}
