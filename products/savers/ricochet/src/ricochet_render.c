#include "ricochet_internal.h"
#include "screensave/visual_buffer_api.h"

#define RICOCHET_FIXED_ONE 256L

static int ricochet_fixed_to_int(long value)
{
    return (int)(value / RICOCHET_FIXED_ONE);
}

static screensave_color ricochet_scale_color(screensave_color color, unsigned int scale)
{
    color.red = (unsigned char)(((unsigned int)color.red * scale) / 255U);
    color.green = (unsigned char)(((unsigned int)color.green * scale) / 255U);
    color.blue = (unsigned char)(((unsigned int)color.blue * scale) / 255U);
    return color;
}

static void ricochet_draw_disc(
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int size,
    screensave_color fill_color,
    screensave_color outline_color
)
{
    screensave_recti rect;
    int radius;
    int step;
    int span;

    radius = size / 2;
    if (radius < 2) {
        radius = 2;
    }

    for (step = -radius; step <= radius; ++step) {
        span = radius;
        if (step < 0) {
            if (-step > (radius * 3) / 4) {
                span = (radius * 2) / 3;
            } else if (-step > radius / 2) {
                span = (radius * 5) / 6;
            }
        } else {
            if (step > (radius * 3) / 4) {
                span = (radius * 2) / 3;
            } else if (step > radius / 2) {
                span = (radius * 5) / 6;
            }
        }

        rect.x = center_x - span;
        rect.y = center_y + step;
        rect.width = (span * 2) + 1;
        rect.height = 1;
        screensave_renderer_fill_rect(renderer, &rect, fill_color);
    }

    rect.x = center_x - radius;
    rect.y = center_y - radius;
    rect.width = radius * 2;
    rect.height = radius * 2;
    screensave_renderer_draw_frame_rect(renderer, &rect, outline_color);
}

static void ricochet_draw_emblem(
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int size,
    screensave_color fill_color,
    screensave_color outline_color
)
{
    screensave_recti rect;
    screensave_pointi start_point;
    screensave_pointi end_point;
    int arm;

    rect.width = size;
    rect.height = size;
    rect.x = center_x - (size / 2);
    rect.y = center_y - (size / 2);
    screensave_renderer_fill_rect(renderer, &rect, fill_color);
    screensave_renderer_draw_frame_rect(renderer, &rect, outline_color);

    arm = size / 3;
    if (arm < 2) {
        arm = 2;
    }

    start_point.x = center_x - arm;
    start_point.y = center_y;
    end_point.x = center_x + arm;
    end_point.y = center_y;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, outline_color);

    start_point.x = center_x;
    start_point.y = center_y - arm;
    end_point.x = center_x;
    end_point.y = center_y + arm;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, outline_color);
}

static void ricochet_draw_block(
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int size,
    screensave_color fill_color,
    screensave_color outline_color
)
{
    screensave_recti rect;

    rect.width = size;
    rect.height = size;
    rect.x = center_x - (size / 2);
    rect.y = center_y - (size / 2);
    screensave_renderer_fill_rect(renderer, &rect, fill_color);
    screensave_renderer_draw_frame_rect(renderer, &rect, outline_color);
}

static void ricochet_draw_shape(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    int center_x,
    int center_y,
    int size,
    screensave_color fill_color,
    screensave_color outline_color
)
{
    switch (session->config.object_mode) {
    case RICOCHET_OBJECT_DISC:
        ricochet_draw_disc(renderer, center_x, center_y, size, fill_color, outline_color);
        break;

    case RICOCHET_OBJECT_EMBLEM:
        ricochet_draw_emblem(renderer, center_x, center_y, size, fill_color, outline_color);
        break;

    case RICOCHET_OBJECT_BLOCK:
    default:
        ricochet_draw_block(renderer, center_x, center_y, size, fill_color, outline_color);
        break;
    }
}

static void ricochet_render_trails(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    const ricochet_body *body
)
{
    unsigned int age;
    unsigned int index;
    unsigned int scale;
    int trail_limit;
    int center_x;
    int center_y;
    screensave_color fill_color;
    screensave_color outline_color;

    trail_limit = (int)body->trail_count;
    for (age = 0U; age < (unsigned int)trail_limit; ++age) {
        if (body->trail_count == 0U) {
            break;
        }

        index = (body->trail_head + body->trail_count - age - 1U) % body->trail_count;
        scale = 144U;
        if (trail_limit > 1) {
            scale = 28U + (((unsigned int)(trail_limit - age) * 164U) / (unsigned int)trail_limit);
        }
        if (session->config.trail_mode == RICOCHET_TRAIL_PHOSPHOR) {
            scale += 24U;
        }
        if (scale > 232U) {
            scale = 232U;
        }

        fill_color = ricochet_scale_color(session->theme->primary_color, scale);
        outline_color = ricochet_scale_color(session->theme->accent_color, scale);
        center_x = ricochet_fixed_to_int(body->trail_x[index]);
        center_y = ricochet_fixed_to_int(body->trail_y[index]);
        ricochet_draw_shape(
            session,
            renderer,
            center_x,
            center_y,
            body->trail_size[index],
            fill_color,
            outline_color
        );
    }
}

static void ricochet_render_corner_celebration(
    screensave_saver_session *session,
    screensave_renderer *renderer,
    screensave_color accent_color
)
{
    screensave_pointi start_point;
    screensave_pointi end_point;
    int inset;
    int arm;

    if (
        session == NULL ||
        renderer == NULL ||
        session->celebration_timer <= 0 ||
        session->preview_mode
    ) {
        return;
    }

    inset = 10;
    arm = 16;

    start_point.x = inset;
    start_point.y = inset + arm;
    end_point.x = inset;
    end_point.y = inset;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);
    end_point.x = inset + arm;
    end_point.y = inset;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);

    start_point.x = session->drawable_size.width - inset - 1;
    start_point.y = inset + arm;
    end_point.x = session->drawable_size.width - inset - 1;
    end_point.y = inset;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);
    end_point.x = session->drawable_size.width - inset - arm - 1;
    end_point.y = inset;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);

    start_point.x = inset;
    start_point.y = session->drawable_size.height - inset - arm - 1;
    end_point.x = inset;
    end_point.y = session->drawable_size.height - inset - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);
    end_point.x = inset + arm;
    end_point.y = session->drawable_size.height - inset - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);

    start_point.x = session->drawable_size.width - inset - 1;
    start_point.y = session->drawable_size.height - inset - arm - 1;
    end_point.x = session->drawable_size.width - inset - 1;
    end_point.y = session->drawable_size.height - inset - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);
    end_point.x = session->drawable_size.width - inset - arm - 1;
    end_point.y = session->drawable_size.height - inset - 1;
    screensave_renderer_draw_line(renderer, &start_point, &end_point, accent_color);
}

void ricochet_render_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_color background;
    screensave_color fill_color;
    screensave_color outline_color;
    screensave_color shadow_color;
    screensave_recti frame_rect;
    unsigned int index;
    int center_x;
    int center_y;

    if (session == NULL || environment == NULL || environment->renderer == NULL || session->theme == NULL) {
        return;
    }

    background.red = 0;
    background.green = 0;
    background.blue = 0;
    background.alpha = 255;
    screensave_renderer_clear(environment->renderer, background);

    fill_color = session->theme->primary_color;
    outline_color = session->theme->accent_color;
    shadow_color = ricochet_scale_color(session->theme->accent_color, 48U);

    if (session->config.trail_mode != RICOCHET_TRAIL_NONE) {
        for (index = 0U; index < session->object_count; ++index) {
            ricochet_render_trails(session, environment->renderer, &session->bodies[index]);
        }
    }

    for (index = 0U; index < session->object_count; ++index) {
        center_x = ricochet_fixed_to_int(session->bodies[index].x);
        center_y = ricochet_fixed_to_int(session->bodies[index].y);

        if (session->bodies[index].flash_timer > 0) {
            fill_color = screensave_color_lerp(session->theme->primary_color, outline_color, 92U);
            outline_color = screensave_color_lerp(session->theme->accent_color, fill_color, 128U);
        } else {
            fill_color = session->theme->primary_color;
            outline_color = session->theme->accent_color;
        }

        if (!session->preview_mode) {
            ricochet_draw_shape(
                session,
                environment->renderer,
                center_x + 1,
                center_y + 1,
                session->bodies[index].size,
                shadow_color,
                shadow_color
            );
        }

        ricochet_draw_shape(
            session,
            environment->renderer,
            center_x,
            center_y,
            session->bodies[index].size,
            fill_color,
            outline_color
        );
    }

    if (session->celebration_timer > 0 && !session->preview_mode) {
        frame_rect.x = 8;
        frame_rect.y = 8;
        frame_rect.width = session->drawable_size.width - 16;
        frame_rect.height = session->drawable_size.height - 16;
        if (frame_rect.width > 0 && frame_rect.height > 0) {
            screensave_renderer_draw_frame_rect(
                environment->renderer,
                &frame_rect,
                ricochet_scale_color(session->theme->accent_color, 104U)
            );
        }
        ricochet_render_corner_celebration(
            session,
            environment->renderer,
            ricochet_scale_color(session->theme->accent_color, 188U)
        );
    }
}
