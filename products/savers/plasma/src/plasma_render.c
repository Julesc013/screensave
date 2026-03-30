#include "plasma_internal.h"

static screensave_color plasma_background_color(void)
{
    screensave_color color;

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 255;
    return color;
}

static screensave_color plasma_palette_color(
    const screensave_saver_session *session,
    unsigned int value
)
{
    screensave_color base_color;
    screensave_color white_color;
    screensave_color highlight_color;
    unsigned int palette_index;
    unsigned int amount;

    base_color = plasma_background_color();
    white_color.red = 255;
    white_color.green = 255;
    white_color.blue = 255;
    white_color.alpha = 255;
    highlight_color = screensave_color_lerp(session->theme->accent_color, white_color, 112U);

    palette_index = (value + (unsigned int)(session->palette_phase & 255UL)) & 255U;
    if (session->config.effect_mode == PLASMA_EFFECT_FIRE) {
        palette_index = (value + (unsigned int)((session->palette_phase / 2UL) & 255UL)) & 255U;
        if (palette_index < 64U) {
            amount = (palette_index * 255U) / 64U;
            return screensave_color_lerp(base_color, session->theme->primary_color, amount);
        }
        if (palette_index < 176U) {
            amount = ((palette_index - 64U) * 255U) / 112U;
            return screensave_color_lerp(session->theme->primary_color, session->theme->accent_color, amount);
        }

        amount = ((palette_index - 176U) * 255U) / 79U;
        return screensave_color_lerp(session->theme->accent_color, highlight_color, amount);
    }

    if (session->config.effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        if (palette_index < 112U) {
            amount = (palette_index * 255U) / 112U;
            return screensave_color_lerp(base_color, session->theme->accent_color, amount);
        }
        if (palette_index < 208U) {
            amount = ((palette_index - 112U) * 255U) / 96U;
            return screensave_color_lerp(session->theme->accent_color, session->theme->primary_color, amount);
        }

        amount = ((palette_index - 208U) * 255U) / 47U;
        return screensave_color_lerp(session->theme->primary_color, highlight_color, amount);
    }

    if (palette_index < 96U) {
        amount = (palette_index * 255U) / 96U;
        return screensave_color_lerp(base_color, session->theme->primary_color, amount);
    }
    if (palette_index < 192U) {
        amount = ((palette_index - 96U) * 255U) / 96U;
        return screensave_color_lerp(session->theme->primary_color, session->theme->accent_color, amount);
    }

    amount = ((palette_index - 192U) * 255U) / 63U;
    return screensave_color_lerp(session->theme->accent_color, highlight_color, amount);
}

static void plasma_render_field_to_buffer(
    screensave_saver_session *session
)
{
    int x;
    int y;

    if (
        session == NULL ||
        session->visual_buffer.pixels == NULL ||
        session->field_primary == NULL ||
        session->theme == NULL
    ) {
        return;
    }

    for (y = 0; y < session->field_size.height; ++y) {
        unsigned char *row;

        row = session->visual_buffer.pixels + ((size_t)y * (size_t)session->visual_buffer.stride_bytes);
        for (x = 0; x < session->field_size.width; ++x) {
            screensave_color color;
            unsigned int value;

            value = (unsigned int)session->field_primary[(y * session->field_size.width) + x];
            color = plasma_palette_color(session, value);
            row[(x * 4) + 0] = color.blue;
            row[(x * 4) + 1] = color.green;
            row[(x * 4) + 2] = color.red;
            row[(x * 4) + 3] = 255U;
        }
    }
}

void plasma_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_bitmap_view bitmap_view;
    screensave_recti destination_rect;

    if (session == NULL || environment == NULL || environment->renderer == NULL) {
        return;
    }

    screensave_renderer_clear(environment->renderer, plasma_background_color());
    plasma_render_field_to_buffer(session);

    screensave_visual_buffer_get_bitmap_view(&session->visual_buffer, &bitmap_view);
    destination_rect.x = 0;
    destination_rect.y = 0;
    destination_rect.width = session->drawable_size.width;
    destination_rect.height = session->drawable_size.height;
    (void)screensave_renderer_blit_bitmap(environment->renderer, &bitmap_view, &destination_rect);
}
