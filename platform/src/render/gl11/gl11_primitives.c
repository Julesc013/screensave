#include "gl11_internal.h"

static int screensave_gl11_can_draw(
    screensave_renderer *renderer,
    screensave_gl11_state **state_out
)
{
    screensave_gl11_state *state;

    if (!screensave_gl11_state_from_renderer(renderer, &state)) {
        return 0;
    }

    if (!state->frame_open) {
        return 0;
    }

    if (state_out != NULL) {
        *state_out = state;
    }
    return 1;
}

static void screensave_gl11_set_color(screensave_color color)
{
    glColor4ub(color.red, color.green, color.blue, color.alpha);
}

void screensave_gl11_clear_impl(screensave_renderer *renderer, screensave_color color)
{
    screensave_gl11_state *state;

    if (!screensave_gl11_can_draw(renderer, &state)) {
        return;
    }

    (void)state;
    glDisable(GL_TEXTURE_2D);
    glClearColor(
        (GLclampf)color.red / 255.0f,
        (GLclampf)color.green / 255.0f,
        (GLclampf)color.blue / 255.0f,
        (GLclampf)color.alpha / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
}

void screensave_gl11_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gl11_state *state;
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    if (!screensave_gl11_can_draw(renderer, &state) || rect == NULL) {
        return;
    }

    (void)state;
    left = (GLfloat)rect->x;
    top = (GLfloat)rect->y;
    right = (GLfloat)(rect->x + rect->width);
    bottom = (GLfloat)(rect->y + rect->height);

    glDisable(GL_TEXTURE_2D);
    screensave_gl11_set_color(color);
    glBegin(GL_QUADS);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);
    glVertex2f(left, bottom);
    glEnd();
}

void screensave_gl11_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gl11_state *state;
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    if (!screensave_gl11_can_draw(renderer, &state) || rect == NULL) {
        return;
    }

    (void)state;
    left = (GLfloat)rect->x;
    top = (GLfloat)rect->y;
    right = (GLfloat)(rect->x + rect->width);
    bottom = (GLfloat)(rect->y + rect->height);

    glDisable(GL_TEXTURE_2D);
    screensave_gl11_set_color(color);
    glBegin(GL_LINE_LOOP);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);
    glVertex2f(left, bottom);
    glEnd();
}

void screensave_gl11_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    screensave_gl11_state *state;

    if (
        !screensave_gl11_can_draw(renderer, &state) ||
        start_point == NULL ||
        end_point == NULL
    ) {
        return;
    }

    (void)state;
    glDisable(GL_TEXTURE_2D);
    screensave_gl11_set_color(color);
    glBegin(GL_LINES);
    glVertex2i(start_point->x, start_point->y);
    glVertex2i(end_point->x, end_point->y);
    glEnd();
}

void screensave_gl11_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    screensave_gl11_state *state;
    unsigned int index;

    if (
        !screensave_gl11_can_draw(renderer, &state) ||
        points == NULL ||
        point_count < 2U
    ) {
        return;
    }

    (void)state;
    glDisable(GL_TEXTURE_2D);
    screensave_gl11_set_color(color);
    glBegin(GL_LINE_STRIP);
    for (index = 0U; index < point_count; ++index) {
        glVertex2i(points[index].x, points[index].y);
    }
    glEnd();
}
