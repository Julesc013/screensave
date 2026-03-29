#include "template_saver_internal.h"

static screensave_color template_saver_make_color(
    unsigned char red,
    unsigned char green,
    unsigned char blue
)
{
    screensave_color color;

    color.red = red;
    color.green = green;
    color.blue = blue;
    color.alpha = 0xFF;
    return color;
}

static screensave_color template_saver_darken(screensave_color color, unsigned char amount)
{
    if (color.red > amount) {
        color.red = (unsigned char)(color.red - amount);
    } else {
        color.red = 0U;
    }
    if (color.green > amount) {
        color.green = (unsigned char)(color.green - amount);
    } else {
        color.green = 0U;
    }
    if (color.blue > amount) {
        color.blue = (unsigned char)(color.blue - amount);
    } else {
        color.blue = 0U;
    }
    return color;
}

static screensave_color template_saver_lighten(screensave_color color, unsigned char amount)
{
    unsigned int channel;

    channel = (unsigned int)color.red + (unsigned int)amount;
    color.red = (unsigned char)(channel > 255U ? 255U : channel);
    channel = (unsigned int)color.green + (unsigned int)amount;
    color.green = (unsigned char)(channel > 255U ? 255U : channel);
    channel = (unsigned int)color.blue + (unsigned int)amount;
    color.blue = (unsigned char)(channel > 255U ? 255U : channel);
    return color;
}

static int template_saver_spacing_margin(const template_saver_config *config)
{
    if (config == NULL) {
        return 24;
    }

    switch (config->spacing_mode) {
    case TEMPLATE_SAVER_SPACING_TIGHT:
        return 12;
    case TEMPLATE_SAVER_SPACING_WIDE:
        return 36;
    case TEMPLATE_SAVER_SPACING_BALANCED:
    default:
        return 24;
    }
}

static screensave_color template_saver_background_color(const screensave_theme_descriptor *theme)
{
    if (theme == NULL) {
        return template_saver_make_color(20U, 20U, 20U);
    }

    return template_saver_darken(theme->primary_color, 112U);
}

static screensave_color template_saver_accent_color(
    screensave_saver_session *session,
    const screensave_theme_descriptor *theme
)
{
    screensave_color accent;
    unsigned char pulse;

    if (theme == NULL) {
        accent = template_saver_make_color(220U, 220U, 220U);
    } else {
        accent = theme->accent_color;
    }

    pulse = (unsigned char)((session->pulse_phase / 32UL) % 48UL);
    if (session->config.accent_mode == TEMPLATE_SAVER_ACCENT_PULSE) {
        return template_saver_lighten(accent, pulse);
    }
    if (session->config.accent_mode == TEMPLATE_SAVER_ACCENT_BANDS) {
        return template_saver_lighten(accent, (unsigned char)(pulse / 2U));
    }
    return accent;
}

static void template_saver_draw_bands(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color color
)
{
    screensave_recti rect;
    int band_height;

    band_height = session->drawable_size.height / 6;
    if (band_height < 4) {
        band_height = 4;
    }

    rect.x = 0;
    rect.y = session->drawable_size.height / 4;
    rect.width = session->drawable_size.width;
    rect.height = band_height;
    screensave_renderer_fill_rect(renderer, &rect, template_saver_darken(color, 64U));

    rect.y = session->drawable_size.height - rect.y - band_height;
    screensave_renderer_fill_rect(renderer, &rect, template_saver_darken(color, 48U));
}

static void template_saver_draw_marker(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color fill_color,
    screensave_color line_color
)
{
    screensave_recti rect;
    screensave_pointi start_point;
    screensave_pointi end_point;
    int size;

    size = session->preview_mode ? 12 : 18;
    if (session->config.motion_mode == TEMPLATE_SAVER_MOTION_FOCUS) {
        size += 4;
    }

    rect.x = (int)session->marker_x - size / 2;
    rect.y = (int)session->marker_y - size / 2;
    rect.width = size;
    rect.height = size;
    screensave_renderer_fill_rect(renderer, &rect, fill_color);
    screensave_renderer_draw_frame_rect(renderer, &rect, line_color);

    start_point.x = rect.x;
    start_point.y = rect.y;
    end_point.x = rect.x + rect.width;
    end_point.y = rect.y + rect.height;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, line_color);

    start_point.x = rect.x + rect.width;
    start_point.y = rect.y;
    end_point.x = rect.x;
    end_point.y = rect.y + rect.height;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, line_color);
}

void template_saver_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;
    screensave_color primary;
    screensave_color accent;
    screensave_recti frame_rect;
    int margin;

    if (session == NULL || environment == NULL || environment->renderer == NULL) {
        return;
    }

    background = template_saver_background_color(session->theme);
    primary = session->theme != NULL ? session->theme->primary_color : template_saver_make_color(180U, 180U, 180U);
    accent = template_saver_accent_color(session, session->theme);

    screensave_renderer_clear(environment->renderer, background);
    if (session->config.accent_mode == TEMPLATE_SAVER_ACCENT_BANDS) {
        template_saver_draw_bands(session, environment->renderer, accent);
    }

    margin = template_saver_spacing_margin(&session->config);
    frame_rect.x = margin / 2;
    frame_rect.y = margin / 2;
    frame_rect.width = session->drawable_size.width - margin;
    frame_rect.height = session->drawable_size.height - margin;
    if (frame_rect.width > 4 && frame_rect.height > 4) {
        screensave_renderer_draw_frame_rect(environment->renderer, &frame_rect, accent);
    }

    template_saver_draw_marker(session, environment->renderer, primary, accent);
}
