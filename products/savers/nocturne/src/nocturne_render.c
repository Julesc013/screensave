#include "nocturne_internal.h"

#define NOCTURNE_FIXED_ONE 65536L

static screensave_color nocturne_scale_color(
    screensave_color base_color,
    int fade_level,
    int motion_strength,
    int preview_mode
)
{
    unsigned int scale;

    scale = (unsigned int)fade_level;
    if (motion_strength == NOCTURNE_STRENGTH_STILL) {
        scale = (scale * 3U) / 4U;
    } else if (motion_strength == NOCTURNE_STRENGTH_SOFT) {
        scale = (scale * 5U) / 4U;
    }

    if (preview_mode) {
        scale = (scale * 3U) / 4U;
    }
    if (scale > 255U) {
        scale = 255U;
    }

    base_color.red = (unsigned char)(((unsigned int)base_color.red * scale) / 255U);
    base_color.green = (unsigned char)(((unsigned int)base_color.green * scale) / 255U);
    base_color.blue = (unsigned char)(((unsigned int)base_color.blue * scale) / 255U);
    return base_color;
}

static int nocturne_fixed_to_int(long value)
{
    return (int)(value / NOCTURNE_FIXED_ONE);
}

static int nocturne_mark_size(const screensave_saver_session *session)
{
    int size;

    size = session->drawable_size.height / 18;
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        size = session->drawable_size.height / 22;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        size = session->drawable_size.height / 14;
    }

    if (size < 4) {
        size = 4;
    }

    return size;
}

static void nocturne_render_drift_mark(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color primary,
    screensave_color accent
)
{
    screensave_recti rect;
    int size;

    size = nocturne_mark_size(session);
    rect.x = nocturne_fixed_to_int(session->primary_x) - (size / 2);
    rect.y = nocturne_fixed_to_int(session->primary_y) - (size / 2);
    rect.width = size;
    rect.height = size;

    screensave_renderer_fill_rect(renderer, &rect, primary);
    screensave_renderer_draw_frame_rect(renderer, &rect, accent);
}

static void nocturne_render_quiet_line(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color primary,
    screensave_color accent
)
{
    screensave_pointi points[3];
    screensave_recti marker;
    int size;

    points[0].x = nocturne_fixed_to_int(session->primary_x);
    points[0].y = nocturne_fixed_to_int(session->primary_y);
    points[1].x = session->drawable_size.width / 2;
    points[1].y = (points[0].y + nocturne_fixed_to_int(session->secondary_y)) / 2;
    points[2].x = nocturne_fixed_to_int(session->secondary_x);
    points[2].y = nocturne_fixed_to_int(session->secondary_y);

    screensave_renderer_draw_polyline(renderer, points, 3U, accent);

    size = 4;
    marker.x = points[0].x - 2;
    marker.y = points[0].y - 2;
    marker.width = size;
    marker.height = size;
    screensave_renderer_fill_rect(renderer, &marker, primary);
}

static void nocturne_render_monolith(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color primary,
    screensave_color accent
)
{
    screensave_recti rect;
    int width;
    int height;

    width = session->preview_mode ? 4 : 6;
    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        width = 8;
    }

    height = session->drawable_size.height / 3;
    if (height < 12) {
        height = 12;
    }

    rect.x = nocturne_fixed_to_int(session->primary_x) - (width / 2);
    rect.y = nocturne_fixed_to_int(session->primary_y) - (height / 2);
    rect.width = width;
    rect.height = height;

    screensave_renderer_fill_rect(renderer, &rect, primary);
    screensave_renderer_draw_frame_rect(renderer, &rect, accent);
}

static void nocturne_render_breath(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color primary,
    screensave_color accent
)
{
    screensave_recti rect;
    int width;
    int height;

    width = (session->drawable_size.width * session->breath_level) / 600;
    height = (session->drawable_size.height * session->breath_level) / 900;
    if (width < 16) {
        width = 16;
    }
    if (height < 10) {
        height = 10;
    }

    rect.width = width;
    rect.height = height;
    rect.x = (session->drawable_size.width - rect.width) / 2;
    rect.y = (session->drawable_size.height - rect.height) / 2;
    screensave_renderer_fill_rect(renderer, &rect, primary);
    screensave_renderer_draw_frame_rect(renderer, &rect, accent);
}

void nocturne_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;
    screensave_color primary;
    screensave_color accent;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    screensave_renderer_clear(environment->renderer, background);

    primary = nocturne_scale_color(
        session->theme->primary_color,
        session->fade_level,
        session->config.motion_strength,
        session->preview_mode
    );
    accent = nocturne_scale_color(
        session->theme->accent_color,
        session->fade_level,
        session->config.motion_strength,
        session->preview_mode
    );

    switch (session->config.motion_mode) {
    case NOCTURNE_MOTION_NONE:
        break;

    case NOCTURNE_MOTION_DRIFT_MARK:
        nocturne_render_drift_mark(session, environment->renderer, primary, accent);
        break;

    case NOCTURNE_MOTION_QUIET_LINE:
        nocturne_render_quiet_line(session, environment->renderer, primary, accent);
        break;

    case NOCTURNE_MOTION_BREATH:
        nocturne_render_breath(session, environment->renderer, primary, accent);
        break;

    case NOCTURNE_MOTION_MONOLITH:
    default:
        nocturne_render_monolith(session, environment->renderer, primary, accent);
        break;
    }
}
