#include "lifeforms_internal.h"

static screensave_color lifeforms_scale_color(screensave_color color, unsigned int scale)
{
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static screensave_color lifeforms_lerp_color(
    screensave_color left_color,
    screensave_color right_color,
    unsigned int amount
)
{
    screensave_color color;

    color.red = (unsigned char)(
        left_color.red + (((int)right_color.red - (int)left_color.red) * (int)amount) / 255
    );
    color.green = (unsigned char)(
        left_color.green + (((int)right_color.green - (int)left_color.green) * (int)amount) / 255
    );
    color.blue = (unsigned char)(
        left_color.blue + (((int)right_color.blue - (int)left_color.blue) * (int)amount) / 255
    );
    color.alpha = 255;
    return color;
}

static screensave_color lifeforms_background_color(
    const screensave_saver_session *session
)
{
    screensave_color color;

    color.red = 6;
    color.green = 8;
    color.blue = 10;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL || session->theme->theme_key == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "laboratory_age") == 0) {
        color.red = 8;
        color.green = 16;
        color.blue = 24;
    } else if (lstrcmpiA(session->theme->theme_key, "amber_phosphor") == 0) {
        color.red = 12;
        color.green = 8;
        color.blue = 4;
    } else if (lstrcmpiA(session->theme->theme_key, "green_phosphor") == 0) {
        color.red = 2;
        color.green = 8;
        color.blue = 3;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_quiet") == 0) {
        color.red = 20;
        color.green = 22;
        color.blue = 24;
    }

    return color;
}

static screensave_color lifeforms_cell_color(
    const screensave_saver_session *session,
    unsigned char age_value
)
{
    screensave_color base_color;
    screensave_color accent_color;
    unsigned int age_mix;

    if (session == NULL || session->theme == NULL) {
        base_color.red = 255;
        base_color.green = 255;
        base_color.blue = 255;
        base_color.alpha = 255;
        return base_color;
    }

    base_color = session->theme->primary_color;
    accent_color = session->theme->accent_color;
    age_mix = age_value;
    if (age_mix > 208U) {
        age_mix = 208U;
    }

    if (lstrcmpiA(session->theme->theme_key, "classic_mono") == 0) {
        base_color = lifeforms_scale_color(base_color, 180U);
        accent_color = session->theme->accent_color;
    } else if (lstrcmpiA(session->theme->theme_key, "museum_quiet") == 0) {
        base_color = lifeforms_scale_color(base_color, 172U);
        accent_color = lifeforms_scale_color(accent_color, 212U);
    }

    return lifeforms_lerp_color(base_color, accent_color, age_mix);
}

void lifeforms_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;
    int offset_x;
    int offset_y;
    int inset;
    int x;
    int y;

    if (session == NULL || environment == NULL || environment->renderer == NULL) {
        return;
    }

    background = lifeforms_background_color(session);
    offset_x = (session->drawable_size.width - (session->grid_size.width * session->cell_size)) / 2;
    offset_y = (session->drawable_size.height - (session->grid_size.height * session->cell_size)) / 2;
    inset = session->cell_size > 4 ? 1 : 0;

    screensave_renderer_clear(environment->renderer, background);

    for (y = 0; y < session->grid_size.height; ++y) {
        for (x = 0; x < session->grid_size.width; ++x) {
            unsigned int index;
            screensave_recti rect;

            index = (unsigned int)(y * session->current_cells.stride_cells + x);
            if (session->current_cells.cells == NULL || session->current_cells.cells[index] == 0U) {
                continue;
            }

            rect.x = offset_x + (x * session->cell_size) + inset;
            rect.y = offset_y + (y * session->cell_size) + inset;
            rect.width = session->cell_size - (inset * 2);
            rect.height = session->cell_size - (inset * 2);
            if (rect.width < 1) {
                rect.width = 1;
            }
            if (rect.height < 1) {
                rect.height = 1;
            }

            screensave_renderer_fill_rect(
                environment->renderer,
                &rect,
                lifeforms_cell_color(session, session->ages.cells != NULL ? session->ages.cells[index] : 0U)
            );
        }
    }
}
