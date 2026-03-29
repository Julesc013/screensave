#include "retro_explorer_internal.h"

#define RETRO_EXPLORER_RENDER_BANDS 12U

static int retro_explorer_clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }

    return value;
}

static screensave_color retro_explorer_make_color(unsigned char red, unsigned char green, unsigned char blue)
{
    screensave_color color;

    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = 255;
    return color;
}

static screensave_color retro_explorer_mix_color(
    screensave_color start_color,
    screensave_color end_color,
    unsigned int amount
)
{
    unsigned int keep;

    if (amount > 255U) {
        amount = 255U;
    }
    keep = 255U - amount;
    start_color.red = (unsigned char)(((unsigned int)start_color.red * keep + (unsigned int)end_color.red * amount) / 255U);
    start_color.green = (unsigned char)(((unsigned int)start_color.green * keep + (unsigned int)end_color.green * amount) / 255U);
    start_color.blue = (unsigned char)(((unsigned int)start_color.blue * keep + (unsigned int)end_color.blue * amount) / 255U);
    return start_color;
}

static screensave_color retro_explorer_scale_color(screensave_color color, unsigned int scale)
{
    if (scale > 255U) {
        scale = 255U;
    }

    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static screensave_color retro_explorer_scene_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color = retro_explorer_make_color(12, 14, 18);
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "cold_lab") == 0) {
        color = retro_explorer_scale_color(session->theme->primary_color, 36U);
    } else if (lstrcmpiA(session->theme->theme_key, "industrial_tunnel") == 0) {
        color = retro_explorer_scale_color(session->theme->primary_color, 30U);
    } else if (lstrcmpiA(session->theme->theme_key, "neon_maze") == 0) {
        color = retro_explorer_scale_color(session->theme->primary_color, 24U);
    } else if (lstrcmpiA(session->theme->theme_key, "dusty_ruin") == 0) {
        color = retro_explorer_scale_color(session->theme->primary_color, 28U);
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_night_run") == 0) {
        color = retro_explorer_scale_color(session->theme->primary_color, 40U);
    }

    return color;
}

static int retro_explorer_scene_horizon(const screensave_saver_session *session, const retro_explorer_segment *segment)
{
    int horizon;

    if (session == NULL || segment == NULL) {
        return 80;
    }

    switch (session->config.scene_mode) {
    case RETRO_EXPLORER_SCENE_INDUSTRIAL:
        horizon = (session->drawable_size.height * 3) / 10;
        break;
    case RETRO_EXPLORER_SCENE_CANYON:
        horizon = (session->drawable_size.height * 2) / 5;
        break;
    case RETRO_EXPLORER_SCENE_CORRIDOR:
    default:
        horizon = (session->drawable_size.height * 29) / 100;
        break;
    }

    horizon += (segment->height - 24) / 5;
    horizon = retro_explorer_clamp_int(horizon, session->drawable_size.height / 5, session->drawable_size.height / 2);
    return horizon;
}

static int retro_explorer_slice_count(const screensave_saver_session *session)
{
    int slices;

    if (session == NULL) {
        return 8;
    }

    switch (session->detail_level) {
    case SCREENSAVE_DETAIL_LEVEL_LOW:
        slices = 6;
        break;
    case SCREENSAVE_DETAIL_LEVEL_HIGH:
        slices = 10;
        break;
    case SCREENSAVE_DETAIL_LEVEL_STANDARD:
    default:
        slices = 8;
        break;
    }

    if (session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON) {
        slices += 1;
    }
    if (session->preview_mode && slices > 8) {
        slices = 8;
    }
    if (slices > (int)RETRO_EXPLORER_RENDER_BANDS) {
        slices = (int)RETRO_EXPLORER_RENDER_BANDS;
    }

    return slices;
}

static screensave_color retro_explorer_band_color(
    const screensave_saver_session *session,
    const retro_explorer_segment *segment,
    int band_index,
    int band_count,
    int is_path_band
)
{
    unsigned int amount;
    unsigned int pulse;
    screensave_color color;
    screensave_color base_color;
    screensave_color path_color;
    screensave_color wall_color;

    base_color = retro_explorer_scene_background_color(session);
    wall_color = session != NULL && session->theme != NULL
        ? session->theme->primary_color
        : retro_explorer_make_color(48, 56, 64);
    path_color = session != NULL && session->theme != NULL
        ? session->theme->accent_color
        : retro_explorer_make_color(168, 176, 188);

    amount = (unsigned int)((band_index * 255) / (band_count > 0 ? band_count : 1));
    pulse = session != NULL && session->portal_pulse > 0UL ? (unsigned int)(session->portal_pulse * 8UL) : 0U;
    if (pulse > 255U) {
        pulse = 255U;
    }
    if (segment != NULL) {
        amount += (unsigned int)(segment->variant * 10);
    }
    if (amount > 255U) {
        amount = 255U;
    }

    if (is_path_band) {
        color = retro_explorer_mix_color(base_color, path_color, 80U + amount / 2U + pulse / 2U);
    } else {
        color = retro_explorer_mix_color(base_color, wall_color, 44U + amount / 2U);
    }

    if (session != NULL && session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON && !is_path_band) {
        color = retro_explorer_mix_color(color, retro_explorer_make_color(128, 84, 48), 48U + amount / 4U);
    } else if (session != NULL && session->config.scene_mode == RETRO_EXPLORER_SCENE_INDUSTRIAL && !is_path_band) {
        color = retro_explorer_mix_color(color, retro_explorer_make_color(32, 40, 44), 48U + amount / 4U);
    }

    if (session != NULL && session->config.scene_mode == RETRO_EXPLORER_SCENE_CORRIDOR && is_path_band) {
        color = retro_explorer_mix_color(color, retro_explorer_make_color(84, 100, 118), 32U + pulse / 3U);
    }

    return color;
}

static void retro_explorer_fill_clamped_rect(
    screensave_renderer *renderer,
    int left,
    int top,
    int right,
    int bottom,
    screensave_color color,
    const screensave_saver_session *session
)
{
    screensave_recti rect;
    int drawable_right;
    int drawable_bottom;

    if (renderer == NULL || session == NULL || right <= left || bottom <= top) {
        return;
    }

    drawable_right = session->drawable_size.width;
    drawable_bottom = session->drawable_size.height;
    if (left < 0) {
        left = 0;
    }
    if (top < 0) {
        top = 0;
    }
    if (right > drawable_right) {
        right = drawable_right;
    }
    if (bottom > drawable_bottom) {
        bottom = drawable_bottom;
    }
    if (right <= left || bottom <= top) {
        return;
    }

    rect.x = left;
    rect.y = top;
    rect.width = right - left;
    rect.height = bottom - top;
    screensave_renderer_fill_rect(renderer, &rect, color);
}

static void retro_explorer_draw_feature_light(
    screensave_renderer *renderer,
    int x,
    int y,
    int size,
    screensave_color color
)
{
    screensave_recti rect;

    if (renderer == NULL || size <= 0) {
        return;
    }

    rect.x = x;
    rect.y = y;
    rect.width = size;
    rect.height = size;
    screensave_renderer_fill_rect(renderer, &rect, color);
}

static void retro_explorer_draw_route_surface(
    screensave_saver_session *session,
    const screensave_saver_environment *environment,
    const retro_explorer_segment *segment
)
{
    screensave_renderer *renderer;
    screensave_pointi left_points[RETRO_EXPLORER_RENDER_BANDS + 1U];
    screensave_pointi right_points[RETRO_EXPLORER_RENDER_BANDS + 1U];
    int slice_count;
    int horizon;
    int floor_y;
    int band_index;
    int band_y;
    int next_band_y;
    int center_x;
    int center_curve;
    int width_half;
    screensave_color band_color;
    screensave_color outline_color;
    screensave_color highlight_color;
    screensave_color path_light;
    screensave_color sign_color;
    screensave_pointi route_points[RETRO_EXPLORER_RENDER_BANDS + 1U];

    if (session == NULL || environment == NULL || environment->renderer == NULL || segment == NULL || session->theme == NULL) {
        return;
    }

    renderer = environment->renderer;
    slice_count = retro_explorer_slice_count(session);
    horizon = retro_explorer_scene_horizon(session, segment);
    floor_y = session->drawable_size.height - 4;
    center_x = (session->drawable_size.width / 2) + session->camera_offset;
    outline_color = retro_explorer_mix_color(session->theme->primary_color, retro_explorer_scene_background_color(session), 96U);
    highlight_color = retro_explorer_mix_color(session->theme->accent_color, retro_explorer_make_color(255, 255, 255), 48U);
    path_light = retro_explorer_mix_color(session->theme->accent_color, retro_explorer_make_color(255, 255, 255), 112U);
    sign_color = retro_explorer_mix_color(session->theme->accent_color, retro_explorer_make_color(255, 255, 255), 160U);

    for (band_index = 0; band_index <= slice_count; ++band_index) {
        int depth_y;
        int depth_adjust;
        int width_growth;
        int curve_offset;
        int panel_top;
        int panel_bottom;

        depth_y = horizon + ((floor_y - horizon) * band_index) / slice_count;
        depth_adjust = (segment->openness * band_index) / slice_count;
        width_growth = segment->width + depth_adjust;
        curve_offset = (segment->curve * band_index) / slice_count;
        center_curve = center_x + curve_offset;
        width_half = width_growth;

        if (session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON) {
            width_half += (band_index * segment->variant) / 2;
        } else if (session->config.scene_mode == RETRO_EXPLORER_SCENE_INDUSTRIAL) {
            width_half += (band_index * (segment->variant + 1)) / 4;
        }

        left_points[band_index].x = center_curve - width_half;
        left_points[band_index].y = depth_y;
        right_points[band_index].x = center_curve + width_half;
        right_points[band_index].y = depth_y;
        route_points[band_index].x = center_curve;
        route_points[band_index].y = depth_y;

        if (band_index == 0) {
            continue;
        }

        panel_top = left_points[band_index - 1].y;
        panel_bottom = left_points[band_index].y;
        band_color = retro_explorer_band_color(session, segment, band_index, slice_count, 1);
        retro_explorer_fill_clamped_rect(
            renderer,
            left_points[band_index].x,
            panel_top,
            right_points[band_index].x,
            panel_bottom,
            band_color,
            session
        );

        retro_explorer_fill_clamped_rect(
            renderer,
            0,
            panel_top,
            left_points[band_index].x,
            panel_bottom,
            retro_explorer_band_color(session, segment, band_index, slice_count, 0),
            session
        );
        retro_explorer_fill_clamped_rect(
            renderer,
            right_points[band_index].x,
            panel_top,
            session->drawable_size.width,
            panel_bottom,
            retro_explorer_band_color(session, segment, band_index, slice_count, 0),
            session
        );

        if (segment->light_interval > 0 && ((unsigned int)band_index + session->current_segment_step) % (unsigned int)segment->light_interval == 0U) {
            int side;
            int light_y;
            int light_size;
            int light_x;

            side = ((band_index + segment->variant) & 1) == 0 ? -1 : 1;
            light_y = panel_top + ((panel_bottom - panel_top) / 2) - 1;
            light_size = session->config.scene_mode == RETRO_EXPLORER_SCENE_CORRIDOR ? 2 : 3;
            light_x = side < 0 ? left_points[band_index].x + 2 : right_points[band_index].x - 4;
            retro_explorer_draw_feature_light(
                renderer,
                light_x,
                light_y,
                light_size,
                retro_explorer_mix_color(highlight_color, path_light, session->portal_pulse > 0UL ? 96U : 48U)
            );
        }

        if (
            segment->sign_interval > 0 &&
            session->config.scene_mode != RETRO_EXPLORER_SCENE_CANYON &&
            ((unsigned int)band_index + session->current_segment_index) % (unsigned int)segment->sign_interval == 0U
        ) {
            int sign_x;
            int sign_y;
            int sign_width;
            int sign_height;

            sign_height = 2 + (segment->variant & 1);
            sign_width = 3 + (segment->variant % 3);
            sign_y = panel_top + 2;
            if ((band_index & 1) == 0) {
                sign_x = left_points[band_index].x + 3;
            } else {
                sign_x = right_points[band_index].x - (sign_width + 3);
            }
            retro_explorer_fill_clamped_rect(
                renderer,
                sign_x,
                sign_y,
                sign_x + sign_width,
                sign_y + sign_height,
                sign_color,
                session
            );
        }

        if (session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON && (band_index & 1) == 0) {
            int ridge_width;
            int ridge_height;
            int ridge_x;

            ridge_width = 4 + segment->variant;
            ridge_height = 3 + (segment->variant & 1);
            ridge_x = left_points[band_index].x - ridge_width;
            retro_explorer_fill_clamped_rect(
                renderer,
                ridge_x,
                panel_top + 1,
                ridge_x + ridge_width,
                panel_top + 1 + ridge_height,
                retro_explorer_mix_color(band_color, retro_explorer_make_color(224, 164, 92), 72U),
                session
            );
        }
    }

    screensave_renderer_draw_polyline(renderer, left_points, (unsigned int)(slice_count + 1), outline_color);
    screensave_renderer_draw_polyline(renderer, right_points, (unsigned int)(slice_count + 1), outline_color);
    screensave_renderer_draw_polyline(renderer, route_points, (unsigned int)(slice_count + 1), path_light);

    if (session->portal_pulse > 0UL) {
        screensave_recti portal_rect;
        int portal_width;
        int portal_height;

        portal_width = 4 + (int)(session->portal_pulse > 4UL ? 4UL : session->portal_pulse);
        portal_height = 4 + (int)(session->portal_pulse > 4UL ? 2UL : session->portal_pulse / 2UL);
        portal_rect.x = center_x - (portal_width / 2);
        portal_rect.y = horizon - (portal_height / 2);
        portal_rect.width = portal_width;
        portal_rect.height = portal_height;
        screensave_renderer_fill_rect(
            renderer,
            &portal_rect,
            retro_explorer_mix_color(highlight_color, session->theme->accent_color, 160U)
        );
    }
}

void retro_explorer_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;
    screensave_color sky_tint;
    screensave_recti sky_rect;
    const retro_explorer_segment *segment;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    if (session->segment_count == 0U) {
        return;
    }

    segment = &session->segments[session->current_segment_index];
    background = retro_explorer_scene_background_color(session);
    screensave_renderer_clear(environment->renderer, background);

    if (session->config.scene_mode == RETRO_EXPLORER_SCENE_CANYON) {
        sky_tint = retro_explorer_mix_color(background, session->theme->accent_color, 64U);
    } else if (session->config.scene_mode == RETRO_EXPLORER_SCENE_INDUSTRIAL) {
        sky_tint = retro_explorer_mix_color(background, session->theme->primary_color, 48U);
    } else {
        sky_tint = retro_explorer_mix_color(background, session->theme->accent_color, 40U);
    }

    sky_rect.x = 0;
    sky_rect.y = 0;
    sky_rect.width = session->drawable_size.width;
    sky_rect.height = retro_explorer_scene_horizon(session, segment);
    screensave_renderer_fill_rect(environment->renderer, &sky_rect, sky_tint);

    retro_explorer_draw_route_surface(session, environment, segment);
}
