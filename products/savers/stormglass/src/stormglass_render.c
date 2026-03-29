#include "stormglass_internal.h"

static screensave_color stormglass_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 10;
    color.green = 16;
    color.blue = 24;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "sodium_vapor_city") == 0) {
        color.red = 20;
        color.green = 16;
        color.blue = 12;
    } else if (lstrcmpiA(session->theme->theme_key, "winter_pane") == 0) {
        color.red = 18;
        color.green = 24;
        color.blue = 30;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_weather") == 0) {
        color.red = 18;
        color.green = 18;
        color.blue = 18;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_midnight_rain") == 0) {
        color.red = 8;
        color.green = 12;
        color.blue = 18;
    }

    return color;
}

static screensave_color stormglass_mix_color(
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

static void stormglass_draw_fill(
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

static void stormglass_draw_window_frame(screensave_saver_session *session, screensave_renderer *renderer)
{
    screensave_color frame_color;
    screensave_pointi start_point;
    screensave_pointi end_point;
    int mid_x;

    if (session == NULL || renderer == NULL || session->theme == NULL) {
        return;
    }

    frame_color = stormglass_mix_color(stormglass_background_color(session), session->theme->primary_color, 48U);
    mid_x = session->drawable_size.width / 2;

    start_point.x = 0;
    start_point.y = 0;
    end_point.x = session->drawable_size.width - 1;
    end_point.y = 0;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, frame_color);
    start_point.y = session->drawable_size.height - 1;
    end_point.y = session->drawable_size.height - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, frame_color);

    start_point.x = 0;
    start_point.y = 0;
    end_point.x = 0;
    end_point.y = session->drawable_size.height - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, frame_color);
    start_point.x = session->drawable_size.width - 1;
    end_point.x = session->drawable_size.width - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, frame_color);

    if (session->drawable_size.width >= 160) {
        start_point.x = mid_x;
        start_point.y = 0;
        end_point.x = mid_x;
        end_point.y = session->drawable_size.height - 1;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, frame_color);
    }
}

static void stormglass_draw_background_layers(screensave_saver_session *session, screensave_renderer *renderer)
{
    unsigned int index;
    screensave_color base_color;
    screensave_color horizon_color;
    screensave_color light_glow;
    screensave_color light_core;
    stormglass_light *light;

    if (session == NULL || renderer == NULL || session->theme == NULL) {
        return;
    }

    base_color = stormglass_background_color(session);
    horizon_color = stormglass_mix_color(base_color, session->theme->primary_color, 56U + session->event_pulse * 6U);
    light_glow = stormglass_mix_color(base_color, session->theme->primary_color, 88U);
    light_core = stormglass_mix_color(base_color, session->theme->accent_color, 180U);

    screensave_renderer_clear(renderer, base_color);
    stormglass_draw_fill(
        renderer,
        0,
        session->drawable_size.height / 2,
        session->drawable_size.width,
        session->drawable_size.height / 2,
        horizon_color
    );

    for (index = 0U; index < session->light_count; ++index) {
        int glow_width;
        int glow_height;

        light = &session->lights[index];
        glow_width = light->width + light->brightness / 18;
        glow_height = light->height + light->brightness / 44;
        stormglass_draw_fill(renderer, light->x - glow_width / 4, light->y - glow_height / 3, glow_width, glow_height, light_glow);
        stormglass_draw_fill(renderer, light->x, light->y, light->width, light->height, light_core);
    }
}

static void stormglass_draw_bands(screensave_saver_session *session, screensave_renderer *renderer)
{
    unsigned int index;
    screensave_color band_color;

    if (session == NULL || renderer == NULL || session->theme == NULL) {
        return;
    }

    band_color = stormglass_mix_color(stormglass_background_color(session), session->theme->primary_color, 64U);
    for (index = 0U; index < session->band_count; ++index) {
        stormglass_draw_fill(
            renderer,
            0,
            session->bands[index].y,
            session->drawable_size.width,
            session->bands[index].thickness,
            band_color
        );
    }
}

static void stormglass_draw_rain_droplets(screensave_saver_session *session, screensave_renderer *renderer)
{
    unsigned int index;
    screensave_color streak_color;
    screensave_color head_color;
    screensave_pointi start_point;
    screensave_pointi end_point;
    stormglass_droplet *droplet;

    streak_color = stormglass_mix_color(stormglass_background_color(session), session->theme->primary_color, 132U);
    head_color = stormglass_mix_color(stormglass_background_color(session), session->theme->accent_color, 192U);
    for (index = 0U; index < session->droplet_count; ++index) {
        int x;
        int y;

        droplet = &session->droplets[index];
        if (!droplet->active) {
            continue;
        }

        x = droplet->x_fixed / 16;
        y = droplet->y_fixed / 16;
        start_point.x = x;
        start_point.y = y;
        end_point.x = x + droplet->drift_fixed / 2;
        end_point.y = y + droplet->trail_length;
        screensave_renderer_draw_line(renderer, &start_point, &end_point, streak_color);
        stormglass_draw_fill(renderer, x, y, 2, 2, head_color);
    }
}

static void stormglass_draw_winter_particles(screensave_saver_session *session, screensave_renderer *renderer)
{
    unsigned int index;
    screensave_color flake_color;
    screensave_pointi start_point;
    screensave_pointi end_point;
    stormglass_droplet *droplet;

    flake_color = stormglass_mix_color(stormglass_background_color(session), session->theme->accent_color, 210U);
    for (index = 0U; index < session->droplet_count; ++index) {
        int x;
        int y;

        droplet = &session->droplets[index];
        if (!droplet->active) {
            continue;
        }

        x = droplet->x_fixed / 16;
        y = droplet->y_fixed / 16;
        stormglass_draw_fill(renderer, x, y, 2, 2, flake_color);
        if ((droplet->phase & 31U) < 12U) {
            start_point.x = x - 1;
            start_point.y = y;
            end_point.x = x + 2;
            end_point.y = y;
            screensave_renderer_draw_line(renderer, &start_point, &end_point, flake_color);
        }
    }
}

void stormglass_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer *renderer;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    stormglass_draw_background_layers(session, renderer);
    if (session->config.pane_mode != STORMGLASS_PANE_CLEAR || session->config.scene_mode == STORMGLASS_SCENE_FOGGED) {
        stormglass_draw_bands(session, renderer);
    }
    if (session->config.scene_mode == STORMGLASS_SCENE_WINTER) {
        stormglass_draw_winter_particles(session, renderer);
    } else {
        stormglass_draw_rain_droplets(session, renderer);
    }
    stormglass_draw_window_frame(session, renderer);
}
