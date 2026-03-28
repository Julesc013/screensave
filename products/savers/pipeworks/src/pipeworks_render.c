#include "pipeworks_internal.h"

static screensave_color pipeworks_scale_color(screensave_color color, unsigned int scale)
{
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static screensave_color pipeworks_lerp_color(
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

static screensave_color pipeworks_background_color(
    const screensave_saver_session *session
)
{
    screensave_color color;

    color.red = 10;
    color.green = 14;
    color.blue = 18;
    color.alpha = 255;
    if (session == NULL || session->theme == NULL || session->theme->theme_key == NULL) {
        return color;
    }

    if (lstrcmpiA(session->theme->theme_key, "clean_workstation") == 0) {
        color.red = 232;
        color.green = 236;
        color.blue = 240;
    } else if (lstrcmpiA(session->theme->theme_key, "industrial_pipes") == 0) {
        color.red = 20;
        color.green = 22;
        color.blue = 24;
    } else if (lstrcmpiA(session->theme->theme_key, "phosphor_grid") == 0) {
        color.red = 2;
        color.green = 10;
        color.blue = 4;
    } else if (lstrcmpiA(session->theme->theme_key, "quiet_night") == 0) {
        color.red = 8;
        color.green = 12;
        color.blue = 20;
    }

    return color;
}

static void pipeworks_fill_rect_if_visible(
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

void pipeworks_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;
    screensave_color pipe_color;
    screensave_color glow_color;
    int offset_x;
    int offset_y;
    int thickness;
    int joint_size;
    int x;
    int y;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    background = pipeworks_background_color(session);
    pipe_color = session->theme->primary_color;
    glow_color = session->theme->accent_color;
    offset_x = (session->drawable_size.width - (session->grid_size.width * session->cell_size)) / 2;
    offset_y = (session->drawable_size.height - (session->grid_size.height * session->cell_size)) / 2;
    thickness = session->cell_size / 3;
    if (thickness < 2) {
        thickness = 2;
    }
    joint_size = thickness + 2;
    if (joint_size > session->cell_size) {
        joint_size = session->cell_size;
    }

    screensave_renderer_clear(environment->renderer, background);

    for (y = 0; y < session->grid_size.height; ++y) {
        for (x = 0; x < session->grid_size.width; ++x) {
            unsigned int index;
            unsigned char cell_mask;
            unsigned char glow_value;
            int cell_left;
            int cell_top;
            int center_x;
            int center_y;
            screensave_color base_color;
            screensave_color active_color;
            unsigned int glow_mix;

            index = (unsigned int)(y * session->cells.stride_cells + x);
            cell_mask = session->cells.cells != NULL ? session->cells.cells[index] : 0U;
            if (cell_mask == 0U) {
                continue;
            }

            glow_value = session->glow.cells != NULL ? session->glow.cells[index] : 0U;
            glow_mix = ((unsigned int)glow_value * 7U) / 8U;
            base_color = pipeworks_lerp_color(background, pipe_color, 208U);
            active_color = pipeworks_lerp_color(base_color, glow_color, glow_mix);

            cell_left = offset_x + (x * session->cell_size);
            cell_top = offset_y + (y * session->cell_size);
            center_x = cell_left + (session->cell_size / 2);
            center_y = cell_top + (session->cell_size / 2);

            pipeworks_fill_rect_if_visible(
                environment->renderer,
                center_x - (joint_size / 2),
                center_y - (joint_size / 2),
                joint_size,
                joint_size,
                active_color
            );

            if ((cell_mask & PIPEWORKS_CELL_NORTH) != 0U) {
                pipeworks_fill_rect_if_visible(
                    environment->renderer,
                    center_x - (thickness / 2),
                    cell_top,
                    thickness,
                    (session->cell_size / 2) + 1,
                    active_color
                );
            }
            if ((cell_mask & PIPEWORKS_CELL_EAST) != 0U) {
                pipeworks_fill_rect_if_visible(
                    environment->renderer,
                    center_x,
                    center_y - (thickness / 2),
                    (session->cell_size / 2) + 1,
                    thickness,
                    active_color
                );
            }
            if ((cell_mask & PIPEWORKS_CELL_SOUTH) != 0U) {
                pipeworks_fill_rect_if_visible(
                    environment->renderer,
                    center_x - (thickness / 2),
                    center_y,
                    thickness,
                    (session->cell_size / 2) + 1,
                    active_color
                );
            }
            if ((cell_mask & PIPEWORKS_CELL_WEST) != 0U) {
                pipeworks_fill_rect_if_visible(
                    environment->renderer,
                    cell_left,
                    center_y - (thickness / 2),
                    (session->cell_size / 2) + 1,
                    thickness,
                    active_color
                );
            }

            if (glow_value > 32U) {
                screensave_color highlight_color;
                int inner_size;

                highlight_color = pipeworks_scale_color(glow_color, 255U);
                highlight_color = pipeworks_lerp_color(active_color, highlight_color, 120U);
                inner_size = joint_size / 2;
                if (inner_size < 2) {
                    inner_size = 2;
                }

                pipeworks_fill_rect_if_visible(
                    environment->renderer,
                    center_x - (inner_size / 2),
                    center_y - (inner_size / 2),
                    inner_size,
                    inner_size,
                    highlight_color
                );
            }
        }
    }
}
