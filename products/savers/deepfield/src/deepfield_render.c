#include "deepfield_internal.h"

#define DEEPFIELD_FIXED_ONE 256L

static screensave_color deepfield_scale_color(screensave_color color, unsigned int scale)
{
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static void deepfield_camera_offset(
    const screensave_saver_session *session,
    int *offset_x,
    int *offset_y
)
{
    long shift_x;
    long shift_y;

    shift_x = 0L;
    shift_y = 0L;
    if (session->config.camera_mode == DEEPFIELD_CAMERA_DRIFT) {
        shift_x = (long)((session->camera_phase_millis % 6000UL) / 240UL) - 12L;
        shift_y = (long)((session->camera_phase_millis % 5000UL) / 200UL) - 12L;
    } else if (session->config.camera_mode == DEEPFIELD_CAMERA_ARC) {
        shift_x = (long)((session->camera_phase_millis % 8000UL) / 180UL) - 22L;
        shift_y = (long)((session->camera_phase_millis % 7000UL) / 280UL) - 12L;
    }

    if (offset_x != NULL) {
        *offset_x = (int)shift_x;
    }
    if (offset_y != NULL) {
        *offset_y = (int)shift_y;
    }
}

static void deepfield_render_star_point(
    screensave_renderer *renderer,
    int x,
    int y,
    int size,
    screensave_color color
)
{
    screensave_recti rect;

    if (size < 1) {
        size = 1;
    }

    rect.width = size;
    rect.height = size;
    rect.x = x - (size / 2);
    rect.y = y - (size / 2);
    screensave_renderer_fill_rect(renderer, &rect, color);
}

static void deepfield_render_parallax(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned int index;
    int camera_x;
    int camera_y;
    int x;
    int y;
    int size;
    unsigned int scale;
    screensave_color color;
    screensave_pointi start_point;
    screensave_pointi end_point;
    const deepfield_star *star;

    deepfield_camera_offset(session, &camera_x, &camera_y);
    for (index = 0U; index < session->star_count; ++index) {
        star = &session->stars[index];
        x = (int)(star->x / DEEPFIELD_FIXED_ONE) + (camera_x / (int)star->layer);
        y = (int)(star->y / DEEPFIELD_FIXED_ONE) + (camera_y / (int)star->layer);
        if (x < -8 || y < -8 || x > session->drawable_size.width + 8 || y > session->drawable_size.height + 8) {
            continue;
        }

        scale = 88U + ((unsigned int)star->layer * 48U) + (unsigned int)(star->twinkle & 31U);
        if (session->pulse_remaining_millis > 0UL) {
            scale += session->config.pulse_mode == DEEPFIELD_PULSE_WARP ? 48U : 20U;
        }
        if (scale > 255U) {
            scale = 255U;
        }

        color = deepfield_scale_color(
            star->layer >= 3 ? session->theme->accent_color : session->theme->primary_color,
            scale
        );
        size = (int)star->layer;
        if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW && size > 2) {
            size = 2;
        }
        deepfield_render_star_point(environment->renderer, x, y, size, color);

        if (
            session->pulse_remaining_millis > 0UL &&
            star->layer >= 2 &&
            session->detail_level != SCREENSAVE_DETAIL_LEVEL_LOW
        ) {
            start_point.x = x;
            start_point.y = y;
            end_point.x = x - (int)star->layer - 1;
            end_point.y = y;
            screensave_renderer_draw_line(environment->renderer, &start_point, &end_point, color);
        }
    }
}

static void deepfield_render_flythrough(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned int index;
    int center_x;
    int center_y;
    int focal;
    int camera_x;
    int camera_y;
    int screen_x;
    int screen_y;
    int size;
    unsigned int scale;
    screensave_color color;
    screensave_pointi start_point;
    screensave_pointi end_point;
    const deepfield_star *star;

    deepfield_camera_offset(session, &camera_x, &camera_y);
    center_x = (session->drawable_size.width / 2) + camera_x;
    center_y = (session->drawable_size.height / 2) + camera_y;
    focal = session->drawable_size.width;
    if (session->drawable_size.height < focal) {
        focal = session->drawable_size.height;
    }
    if (focal < 64) {
        focal = 64;
    }

    for (index = 0U; index < session->star_count; ++index) {
        star = &session->stars[index];
        if (star->z <= 0L) {
            continue;
        }

        screen_x = center_x + (int)((star->x * (long)(focal / 2)) / star->z);
        screen_y = center_y + (int)((star->y * (long)(focal / 2)) / star->z);
        if (
            screen_x < -8 ||
            screen_y < -8 ||
            screen_x > session->drawable_size.width + 8 ||
            screen_y > session->drawable_size.height + 8
        ) {
            continue;
        }

        if (star->z < 96L) {
            size = 3;
        } else if (star->z < 200L) {
            size = 2;
        } else {
            size = 1;
        }
        if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW && size > 2) {
            size = 2;
        }

        scale = 72U + (unsigned int)((720L - (star->z > 720L ? 720L : star->z)) / 4L);
        if (session->pulse_remaining_millis > 0UL) {
            scale += session->config.pulse_mode == DEEPFIELD_PULSE_WARP ? 52U : 20U;
        }
        if (scale > 255U) {
            scale = 255U;
        }

        color = deepfield_scale_color(session->theme->primary_color, scale);
        deepfield_render_star_point(environment->renderer, screen_x, screen_y, size, color);

        if (
            session->pulse_remaining_millis > 0UL &&
            session->detail_level != SCREENSAVE_DETAIL_LEVEL_LOW &&
            star->z < 220L
        ) {
            start_point.x = screen_x - ((screen_x - center_x) / 10);
            start_point.y = screen_y - ((screen_y - center_y) / 10);
            end_point.x = screen_x;
            end_point.y = screen_y;
            screensave_renderer_draw_line(
                environment->renderer,
                &start_point,
                &end_point,
                deepfield_scale_color(session->theme->accent_color, scale)
            );
        }
    }
}

void deepfield_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    screensave_renderer_clear(environment->renderer, background);

    if (session->config.scene_mode == DEEPFIELD_SCENE_FLYTHROUGH) {
        deepfield_render_flythrough(session, environment);
    } else {
        deepfield_render_parallax(session, environment);
    }
}
