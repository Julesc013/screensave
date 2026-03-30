#include "phosphor_internal.h"

static screensave_color phosphor_background_color(
    const screensave_saver_session *session
)
{
    screensave_color color;

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "drafting_board") == 0) {
        color.red = 230;
        color.green = 236;
        color.blue = 242;
    } else if (lstrcmpiA(session->theme->theme_key, "white_instrument") == 0) {
        color.red = 10;
        color.green = 14;
        color.blue = 16;
    } else if (lstrcmpiA(session->theme->theme_key, "amber_harmonics") == 0) {
        color.red = 10;
        color.green = 8;
        color.blue = 4;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_quiet") == 0) {
        color.red = 22;
        color.green = 26;
        color.blue = 28;
    } else if (lstrcmpiA(session->theme->theme_key, "blue_lab") == 0) {
        color.red = 6;
        color.green = 12;
        color.blue = 18;
    }

    return color;
}

static screensave_color phosphor_scale_color(
    screensave_color color,
    unsigned int scale
)
{
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

void phosphor_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_bitmap_view bitmap_view;
    screensave_recti destination_rect;
    screensave_recti frame_rect;
    screensave_pointi start_point;
    screensave_pointi end_point;
    screensave_color accent_color;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    screensave_renderer_clear(environment->renderer, phosphor_background_color(session));
    screensave_visual_buffer_get_bitmap_view(&session->visual_buffer, &bitmap_view);
    destination_rect.x = 0;
    destination_rect.y = 0;
    destination_rect.width = session->drawable_size.width;
    destination_rect.height = session->drawable_size.height;
    (void)screensave_renderer_blit_bitmap(environment->renderer, &bitmap_view, &destination_rect);

    if (session->preview_mode) {
        return;
    }

    accent_color = phosphor_scale_color(session->theme->accent_color, 72U);
    frame_rect.x = 10;
    frame_rect.y = 10;
    frame_rect.width = session->drawable_size.width - 20;
    frame_rect.height = session->drawable_size.height - 20;
    if (frame_rect.width > 0 && frame_rect.height > 0) {
        screensave_renderer_draw_frame_rect(environment->renderer, &frame_rect, accent_color);
    }

    start_point.x = session->drawable_size.width / 2;
    start_point.y = (session->drawable_size.height / 2) - 12;
    end_point.x = start_point.x;
    end_point.y = (session->drawable_size.height / 2) + 12;
    screensave_renderer_draw_line(environment->renderer, &start_point, &end_point, accent_color);

    start_point.x = (session->drawable_size.width / 2) - 12;
    start_point.y = session->drawable_size.height / 2;
    end_point.x = (session->drawable_size.width / 2) + 12;
    end_point.y = start_point.y;
    screensave_renderer_draw_line(environment->renderer, &start_point, &end_point, accent_color);
}
