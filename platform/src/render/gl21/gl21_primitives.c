#include "gl21_internal.h"

static int screensave_gl21_can_draw(
    screensave_renderer *renderer,
    screensave_gl21_state **state_out
)
{
    screensave_gl21_state *state;

    if (!screensave_gl21_state_from_renderer(renderer, &state)) {
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

static void screensave_gl21_set_color(screensave_color color)
{
    glColor4ub(color.red, color.green, color.blue, color.alpha);
}

void screensave_gl21_clear_impl(screensave_renderer *renderer, screensave_color color)
{
    screensave_gl21_state *state;

    if (!screensave_gl21_can_draw(renderer, &state)) {
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

void screensave_gl21_fill_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gl21_state *state;
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    if (!screensave_gl21_can_draw(renderer, &state) || rect == NULL) {
        return;
    }

    (void)state;
    left = (GLfloat)rect->x;
    top = (GLfloat)rect->y;
    right = (GLfloat)(rect->x + rect->width);
    bottom = (GLfloat)(rect->y + rect->height);

    glDisable(GL_TEXTURE_2D);
    screensave_gl21_set_color(color);
    glBegin(GL_QUADS);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);
    glVertex2f(left, bottom);
    glEnd();
}

void screensave_gl21_draw_frame_rect_impl(
    screensave_renderer *renderer,
    const screensave_recti *rect,
    screensave_color color
)
{
    screensave_gl21_state *state;
    GLfloat left;
    GLfloat top;
    GLfloat right;
    GLfloat bottom;

    if (!screensave_gl21_can_draw(renderer, &state) || rect == NULL) {
        return;
    }

    (void)state;
    left = (GLfloat)rect->x;
    top = (GLfloat)rect->y;
    right = (GLfloat)(rect->x + rect->width);
    bottom = (GLfloat)(rect->y + rect->height);

    glDisable(GL_TEXTURE_2D);
    screensave_gl21_set_color(color);
    glBegin(GL_LINE_LOOP);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);
    glVertex2f(left, bottom);
    glEnd();
}

void screensave_gl21_draw_line_impl(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    screensave_gl21_state *state;

    if (
        !screensave_gl21_can_draw(renderer, &state) ||
        start_point == NULL ||
        end_point == NULL
    ) {
        return;
    }

    (void)state;
    glDisable(GL_TEXTURE_2D);
    screensave_gl21_set_color(color);
    glBegin(GL_LINES);
    glVertex2i(start_point->x, start_point->y);
    glVertex2i(end_point->x, end_point->y);
    glEnd();
}

void screensave_gl21_draw_polyline_impl(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    screensave_gl21_state *state;
    unsigned int index;

    if (
        !screensave_gl21_can_draw(renderer, &state) ||
        points == NULL ||
        point_count < 2U
    ) {
        return;
    }

    (void)state;
    glDisable(GL_TEXTURE_2D);
    screensave_gl21_set_color(color);
    glBegin(GL_LINE_STRIP);
    for (index = 0U; index < point_count; ++index) {
        glVertex2i(points[index].x, points[index].y);
    }
    glEnd();
}

int screensave_gl21_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    screensave_gl21_state *state;
    GLenum pixel_format;
    GLfloat zoom_x;
    GLfloat zoom_y;

    if (
        !screensave_gl21_can_draw(renderer, &state) ||
        bitmap == NULL ||
        destination_rect == NULL ||
        bitmap->pixels == NULL ||
        bitmap->size.width <= 0 ||
        bitmap->size.height <= 0
    ) {
        return 0;
    }

    (void)state;
    if (bitmap->bits_per_pixel == 32U) {
        pixel_format = GL_BGRA;
    } else if (bitmap->bits_per_pixel == 24U) {
        pixel_format = GL_BGR;
    } else {
        return 0;
    }

    glDisable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    zoom_x = (GLfloat)destination_rect->width / (GLfloat)bitmap->size.width;
    zoom_y = (GLfloat)destination_rect->height / (GLfloat)bitmap->size.height;
    if (bitmap->origin_top_left) {
        glRasterPos2i(destination_rect->x, destination_rect->y + destination_rect->height);
        glPixelZoom(zoom_x, -zoom_y);
    } else {
        glRasterPos2i(destination_rect->x, destination_rect->y);
        glPixelZoom(zoom_x, zoom_y);
    }
    glDrawPixels(
        bitmap->size.width,
        bitmap->size.height,
        pixel_format,
        GL_UNSIGNED_BYTE,
        bitmap->pixels
    );
    glPixelZoom(1.0f, 1.0f);
    return 1;
}


