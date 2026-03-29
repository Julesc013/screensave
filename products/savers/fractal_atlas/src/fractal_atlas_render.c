#include "fractal_atlas_internal.h"

static screensave_color fractal_atlas_background_color(const screensave_saver_session *session)
{
    screensave_color color;

    color.red = 6;
    color.green = 8;
    color.blue = 12;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "atlas_monochrome") == 0) {
        color.red = 12;
        color.green = 12;
        color.blue = 14;
    } else if (lstrcmpiA(session->theme->theme_key, "liquid_nebula") == 0) {
        color.red = 8;
        color.green = 10;
        color.blue = 22;
    } else if (lstrcmpiA(session->theme->theme_key, "terminal_mathematics") == 0) {
        color.red = 4;
        color.green = 12;
        color.blue = 8;
    } else if (lstrcmpiA(session->theme->theme_key, "deep_ultraviolet") == 0) {
        color.red = 12;
        color.green = 6;
        color.blue = 18;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_print_room") == 0) {
        color.red = 16;
        color.green = 14;
        color.blue = 12;
    }

    return color;
}

static screensave_color fractal_atlas_value_color(
    const screensave_saver_session *session,
    unsigned short value
)
{
    screensave_color background;
    screensave_color white_color;
    unsigned int palette_index;
    unsigned int amount;

    background = fractal_atlas_background_color(session);
    white_color.red = 255;
    white_color.green = 255;
    white_color.blue = 255;
    white_color.alpha = 255;

    if (session == NULL || session->theme == NULL) {
        return background;
    }

    if ((int)value >= session->max_iterations) {
        return screensave_color_lerp(background, session->theme->primary_color, 48U + (session->palette_phase / 8U));
    }

    palette_index = ((unsigned int)value * 255U) / (unsigned int)session->max_iterations;
    palette_index = (palette_index + session->palette_phase) & 255U;
    if (palette_index < 96U) {
        amount = (palette_index * 255U) / 96U;
        return screensave_color_lerp(background, session->theme->primary_color, amount);
    }
    if (palette_index < 192U) {
        amount = ((palette_index - 96U) * 255U) / 96U;
        return screensave_color_lerp(session->theme->primary_color, session->theme->accent_color, amount);
    }

    amount = ((palette_index - 192U) * 255U) / 63U;
    return screensave_color_lerp(session->theme->accent_color, white_color, amount);
}

static void fractal_atlas_colorize_buffer(screensave_saver_session *session)
{
    int x;
    int y;
    screensave_color background;

    if (
        session == NULL ||
        session->visual_buffer.pixels == NULL ||
        session->escape_values == NULL
    ) {
        return;
    }

    background = fractal_atlas_background_color(session);
    for (y = 0; y < session->field_size.height; ++y) {
        unsigned char *row;

        row = session->visual_buffer.pixels + ((size_t)y * (size_t)session->visual_buffer.stride_bytes);
        for (x = 0; x < session->field_size.width; ++x) {
            screensave_color color;

            if (y >= session->refinement_row) {
                color = background;
            } else {
                color = fractal_atlas_value_color(
                    session,
                    session->escape_values[(y * session->field_size.width) + x]
                );
            }
            row[(x * 4) + 0] = color.blue;
            row[(x * 4) + 1] = color.green;
            row[(x * 4) + 2] = color.red;
            row[(x * 4) + 3] = 255U;
        }
    }
}

void fractal_atlas_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_bitmap_view bitmap_view;
    screensave_recti destination_rect;
    screensave_color background;

    if (session == NULL || environment == NULL || environment->renderer == NULL) {
        return;
    }

    background = fractal_atlas_background_color(session);
    screensave_renderer_clear(environment->renderer, background);
    fractal_atlas_colorize_buffer(session);

    screensave_visual_buffer_get_bitmap_view(&session->visual_buffer, &bitmap_view);
    destination_rect.x = 0;
    destination_rect.y = 0;
    destination_rect.width = session->drawable_size.width;
    destination_rect.height = session->drawable_size.height;
    (void)screensave_renderer_blit_bitmap(environment->renderer, &bitmap_view, &destination_rect);
}
