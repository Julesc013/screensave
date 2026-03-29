#include "city_nocturne_internal.h"

static screensave_color city_nocturne_mix_color(
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

static screensave_color city_nocturne_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 8;
    color.green = 10;
    color.blue = 18;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "harbor_sodium_night") == 0) {
        color.red = 8;
        color.green = 11;
        color.blue = 16;
    } else if (lstrcmpiA(session->theme->theme_key, "wet_rooftop") == 0) {
        color.red = 10;
        color.green = 14;
        color.blue = 24;
    } else if (lstrcmpiA(session->theme->theme_key, "industrial_port") == 0) {
        color.red = 12;
        color.green = 12;
        color.blue = 18;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_city_sleep") == 0) {
        color.red = 7;
        color.green = 8;
        color.blue = 14;
    }

    return color;
}

static int city_nocturne_scene_horizon_y(const screensave_saver_session *session)
{
    int height;
    int horizon;

    if (session == NULL || session->drawable_size.height <= 0) {
        return 1;
    }

    height = session->drawable_size.height;
    switch (session->config.scene_mode) {
    case CITY_NOCTURNE_SCENE_HARBOR:
        horizon = (height * 3) / 5;
        break;
    case CITY_NOCTURNE_SCENE_WINDOW_RIVER:
        horizon = (height * 11) / 20;
        break;
    case CITY_NOCTURNE_SCENE_SKYLINE:
    default:
        horizon = (height * 2) / 3;
        break;
    }

    if (horizon < 24) {
        horizon = 24;
    }
    if (horizon > height - 18) {
        horizon = height - 18;
    }

    return horizon;
}

static int city_nocturne_scene_span(const screensave_saver_session *session)
{
    int width;

    if (session == NULL || session->drawable_size.width <= 0) {
        return 512;
    }

    width = session->drawable_size.width;
    return width * 2 + 192;
}

static void city_nocturne_draw_fill(
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

static void city_nocturne_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    if (renderer == NULL || points == NULL || point_count == 0U) {
        return;
    }

    screensave_renderer_draw_polyline(renderer, points, point_count, color);
}

static void city_nocturne_draw_sky(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color base_color,
    int horizon_y
)
{
    screensave_color upper_color;
    screensave_color mid_color;
    screensave_color low_color;
    int band_top;
    int band_mid;
    int band_low;

    upper_color = city_nocturne_mix_color(base_color, session->theme->primary_color, 28U);
    mid_color = city_nocturne_mix_color(base_color, session->theme->primary_color, 48U);
    low_color = city_nocturne_mix_color(base_color, session->theme->accent_color, 56U);
    band_top = horizon_y / 2;
    band_mid = horizon_y / 3;
    band_low = horizon_y / 6;

    city_nocturne_draw_fill(renderer, 0, 0, session->drawable_size.width, band_top, upper_color);
    city_nocturne_draw_fill(renderer, 0, band_top, session->drawable_size.width, band_mid, mid_color);
    city_nocturne_draw_fill(renderer, 0, horizon_y - band_low, session->drawable_size.width, band_low, low_color);
}

static void city_nocturne_draw_fog(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color base_color
)
{
    unsigned int index;
    int horizon_y;
    int width;

    if (session == NULL) {
        return;
    }

    horizon_y = city_nocturne_scene_horizon_y(session);
    width = session->drawable_size.width;
    for (index = 0U; index < session->fog_count; ++index) {
        screensave_color fog_color;
        int x;
        int y;
        int height;

        fog_color = city_nocturne_mix_color(base_color, session->theme->accent_color, 32U + (unsigned int)session->fog_bands[index].opacity);
        fog_color.red = (unsigned char)((unsigned int)fog_color.red * 2U / 3U);
        fog_color.green = (unsigned char)((unsigned int)fog_color.green * 2U / 3U);
        fog_color.blue = (unsigned char)((unsigned int)fog_color.blue * 2U / 3U);
        x = 0;
        if (session->fog_bands[index].drift > 0) {
            x = session->fog_bands[index].drift * (int)(session->camera_phase / 16U);
            if (x > width / 2) {
                x = width / 2;
            }
        }
        y = session->fog_bands[index].y;
        if (y < horizon_y - 48) {
            y = horizon_y - 48;
        }
        height = session->fog_bands[index].height;
        if (height < 4) {
            height = 4;
        }
        city_nocturne_draw_fill(renderer, x, y, width, height, fog_color);
    }
}

static void city_nocturne_draw_windows(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    const city_nocturne_structure *structure,
    int base_x,
    int base_y,
    int structure_width,
    int structure_height,
    screensave_color window_on_color,
    screensave_color window_off_color
)
{
    int row;
    int column;
    int cell_width;
    int cell_height;
    int light_bias;

    if (session == NULL || renderer == NULL || structure == NULL) {
        return;
    }

    cell_width = structure_width / (structure->window_columns + 1);
    cell_height = structure_height / (structure->window_rows + 1);
    if (cell_width < 2) {
        cell_width = 2;
    }
    if (cell_height < 2) {
        cell_height = 2;
    }
    light_bias = structure->light_bias + (int)(session->light_phase & 15U);
    for (row = 0; row < structure->window_rows; ++row) {
        for (column = 0; column < structure->window_columns; ++column) {
            int window_x;
            int window_y;
            screensave_color color;
            unsigned int gate;

            window_x = base_x + 2 + (column * cell_width) + (column % 2);
            window_y = base_y + 6 + (row * cell_height);
            gate = (unsigned int)(row * 31 + column * 17 + (int)structure->seed_offset + (int)session->pulse_phase + structure->world_x);
            if ((gate % 100U) < (unsigned int)light_bias) {
                color = city_nocturne_mix_color(window_off_color, window_on_color, (unsigned int)(120 + (light_bias % 96)));
            } else {
                color = window_off_color;
            }
            city_nocturne_draw_fill(renderer, window_x, window_y, cell_width - 1, cell_height - 1, color);
        }
    }
}

static void city_nocturne_draw_structure(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    const city_nocturne_structure *structure,
    int horizon_y,
    int scene_span,
    screensave_color base_color,
    screensave_color accent_color
)
{
    int shift;
    int x;
    int y;
    int width;
    int height;
    screensave_color block_color;
    screensave_color edge_color;
    screensave_color window_on_color;
    screensave_color window_off_color;
    screensave_recti outline;
    screensave_pointi roof_points[5];
    int roof_top;
    int roof_left;
    int roof_right;
    int roof_mid;

    if (session == NULL || renderer == NULL || structure == NULL) {
        return;
    }

    shift = (int)((session->route_offset * (structure->depth + 1)) / 4);
    x = structure->world_x - shift;
    while (x < -structure->width) {
        x += scene_span;
    }
    while (x > session->drawable_size.width + structure->width) {
        x -= scene_span;
    }

    width = structure->width;
    height = structure->height;
    y = horizon_y - height;
    if (y > horizon_y - 12) {
        y = horizon_y - 12;
    }

    block_color = city_nocturne_mix_color(base_color, accent_color, 40U + (unsigned int)(structure->depth * 18));
    if (session->config.scene_mode == CITY_NOCTURNE_SCENE_HARBOR) {
        block_color = city_nocturne_mix_color(block_color, session->theme->primary_color, 32U);
    } else if (session->config.scene_mode == CITY_NOCTURNE_SCENE_WINDOW_RIVER) {
        block_color = city_nocturne_mix_color(block_color, session->theme->accent_color, 48U);
    }
    edge_color = city_nocturne_mix_color(base_color, session->theme->accent_color, 96U + (unsigned int)(structure->depth * 16));
    window_on_color = city_nocturne_mix_color(session->theme->accent_color, edge_color, 168U);
    window_off_color = city_nocturne_mix_color(base_color, edge_color, 54U);

    city_nocturne_draw_fill(renderer, x, y, width, height, block_color);
    outline.x = x;
    outline.y = y;
    outline.width = width;
    outline.height = height;
    screensave_renderer_draw_frame_rect(renderer, &outline, edge_color);

    roof_top = y;
    roof_left = x;
    roof_right = x + width;
    roof_mid = x + (width / 2);
    if (structure->roof_style == 0) {
        roof_points[0].x = roof_left;
        roof_points[0].y = roof_top;
        roof_points[1].x = roof_mid;
        roof_points[1].y = roof_top - 4;
        roof_points[2].x = roof_right;
        roof_points[2].y = roof_top;
        city_nocturne_draw_polyline(renderer, roof_points, 3U, edge_color);
    } else if (structure->roof_style == 1) {
        roof_points[0].x = roof_left;
        roof_points[0].y = roof_top;
        roof_points[1].x = roof_left + (width / 3);
        roof_points[1].y = roof_top - 3;
        roof_points[2].x = roof_mid;
        roof_points[2].y = roof_top - 1;
        roof_points[3].x = roof_right - (width / 4);
        roof_points[3].y = roof_top - 5;
        roof_points[4].x = roof_right;
        roof_points[4].y = roof_top;
        city_nocturne_draw_polyline(renderer, roof_points, 5U, edge_color);
    } else {
        city_nocturne_draw_fill(renderer, x + 2, y - 1, width - 4, 2, edge_color);
    }

    city_nocturne_draw_windows(
        session,
        renderer,
        structure,
        x,
        y,
        width,
        height,
        window_on_color,
        window_off_color
    );

    if (session->config.scene_mode == CITY_NOCTURNE_SCENE_HARBOR) {
        screensave_pointi crane_points[4];

        crane_points[0].x = x + width / 3;
        crane_points[0].y = y;
        crane_points[1].x = x + width / 3;
        crane_points[1].y = y - 14 - structure->depth * 2;
        crane_points[2].x = x + width / 2;
        crane_points[2].y = y - 10 - structure->depth * 2;
        crane_points[3].x = x + width / 2;
        crane_points[3].y = y;
        city_nocturne_draw_polyline(renderer, crane_points, 4U, edge_color);
    }
}

static void city_nocturne_draw_lights(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color base_color,
    int horizon_y,
    int scene_span
)
{
    unsigned int index;
    screensave_color light_color;

    if (session == NULL || renderer == NULL) {
        return;
    }

    light_color = city_nocturne_mix_color(base_color, session->theme->accent_color, 192U);
    for (index = 0U; index < session->light_count; ++index) {
        int x;
        int y;
        int width;
        int height;
        int phase;
        int brightness;

        x = session->lights[index].world_x - (int)((session->route_offset * (session->lights[index].depth + 1)) / 4);
        while (x < -scene_span) {
            x += scene_span;
        }
        while (x > session->drawable_size.width + scene_span) {
            x -= scene_span;
        }
        y = session->lights[index].world_y;
        if (y < 0) {
            y = 0;
        }
        width = session->lights[index].width;
        height = session->lights[index].height;
        phase = (int)((session->light_phase + session->lights[index].phase + (unsigned int)(index * 11U)) & 255U);
        brightness = session->lights[index].brightness + phase / 4;
        if (brightness > 255) {
            brightness = 255;
        }
        light_color = city_nocturne_mix_color(base_color, session->theme->accent_color, (unsigned int)brightness);
        city_nocturne_draw_fill(renderer, x, y, width, height, light_color);
        if (session->config.scene_mode != CITY_NOCTURNE_SCENE_SKYLINE) {
            screensave_color reflection_color;

            reflection_color = city_nocturne_mix_color(base_color, session->theme->primary_color, (unsigned int)(96 + phase / 2));
            city_nocturne_draw_fill(renderer, x - 1, horizon_y + 2, width + 2, 4 + (phase % 10), reflection_color);
        }
    }
}

static void city_nocturne_draw_ground(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color base_color,
    int horizon_y
)
{
    screensave_color ground_color;
    screensave_color rail_color;
    screensave_recti rail_rect;
    int height;

    if (session == NULL || renderer == NULL) {
        return;
    }

    ground_color = city_nocturne_mix_color(base_color, session->theme->primary_color, 22U);
    rail_color = city_nocturne_mix_color(base_color, session->theme->accent_color, 72U);
    height = session->drawable_size.height - horizon_y;
    if (height < 10) {
        height = 10;
    }
    city_nocturne_draw_fill(renderer, 0, horizon_y, session->drawable_size.width, height, ground_color);
    rail_rect.x = 0;
    rail_rect.y = horizon_y + 3;
    rail_rect.width = session->drawable_size.width;
    rail_rect.height = 2;
    screensave_renderer_draw_frame_rect(renderer, &rail_rect, rail_color);
}

static void city_nocturne_draw_reflections(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    int horizon_y
)
{
    unsigned int index;

    if (session == NULL || renderer == NULL) {
        return;
    }

    if (session->config.scene_mode == CITY_NOCTURNE_SCENE_SKYLINE) {
        return;
    }

    for (index = 0U; index < session->structure_count; ++index) {
        int x;
        int width;
        int reflection_height;
        screensave_color color;
        screensave_recti reflection_rect;

        x = session->structures[index].world_x - (int)((session->route_offset * (session->structures[index].depth + 1)) / 4);
        while (x < -session->drawable_size.width) {
            x += city_nocturne_scene_span(session);
        }
        while (x > session->drawable_size.width * 2) {
            x -= city_nocturne_scene_span(session);
        }
        width = session->structures[index].width;
        reflection_height = 6 + (int)((session->pulse_phase + session->structures[index].seed_offset) & 15U);
        color = city_nocturne_mix_color(session->theme->primary_color, session->theme->accent_color, 128U);
        reflection_rect.x = x;
        reflection_rect.y = horizon_y + 6 + session->structures[index].depth * 2;
        reflection_rect.width = width;
        reflection_rect.height = reflection_height;
        screensave_renderer_fill_rect(renderer, &reflection_rect, color);
    }
}

static void city_nocturne_draw_scene(
    const screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color base_color
)
{
    int horizon_y;
    int scene_span;
    int width;
    unsigned int index;

    if (session == NULL || renderer == NULL) {
        return;
    }

    horizon_y = city_nocturne_scene_horizon_y(session);
    scene_span = city_nocturne_scene_span(session);
    width = session->drawable_size.width;

    city_nocturne_draw_sky(session, renderer, base_color, horizon_y);
    city_nocturne_draw_fog(session, renderer, base_color);
    city_nocturne_draw_ground(session, renderer, base_color, horizon_y);
    city_nocturne_draw_reflections(session, renderer, horizon_y);

    for (index = 0U; index < session->structure_count; ++index) {
        city_nocturne_draw_structure(
            session,
            renderer,
            &session->structures[index],
            horizon_y,
            scene_span,
            base_color,
            session->theme->primary_color
        );
    }

    city_nocturne_draw_lights(session, renderer, base_color, horizon_y, scene_span);

    if (session->config.scene_mode == CITY_NOCTURNE_SCENE_WINDOW_RIVER) {
        screensave_color river_color;

        river_color = city_nocturne_mix_color(base_color, session->theme->accent_color, 120U);
        city_nocturne_draw_fill(renderer, 0, horizon_y - 6, width, 2, river_color);
        city_nocturne_draw_fill(renderer, 0, horizon_y + 10, width, 1, river_color);
    }
}

void city_nocturne_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_renderer *renderer;
    screensave_color base_color;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    base_color = city_nocturne_background_color(session);
    screensave_renderer_clear(renderer, base_color);
    city_nocturne_draw_scene(session, renderer, base_color);
}
