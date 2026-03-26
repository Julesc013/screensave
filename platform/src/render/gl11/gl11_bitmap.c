#include <stdlib.h>
#include <string.h>

#include "gl11_internal.h"

static unsigned int screensave_gl11_next_power_of_two(unsigned int value)
{
    unsigned int power;

    power = 1U;
    while (power < value) {
        power <<= 1U;
    }

    return power;
}

static int screensave_gl11_bitmap_bytes_per_pixel(unsigned int bits_per_pixel)
{
    switch (bits_per_pixel) {
    case 24U:
        return 3;

    case 32U:
        return 4;

    default:
        return 0;
    }
}

static void screensave_gl11_convert_bitmap_to_rgba(
    const screensave_bitmap_view *bitmap,
    unsigned char *pixels,
    unsigned int texture_width,
    unsigned int texture_height
)
{
    const unsigned char *source_bytes;
    int source_bytes_per_pixel;
    unsigned int y;
    unsigned int x;

    source_bytes = (const unsigned char *)bitmap->pixels;
    source_bytes_per_pixel = screensave_gl11_bitmap_bytes_per_pixel(bitmap->bits_per_pixel);
    memset(pixels, 0, (size_t)texture_width * (size_t)texture_height * 4U);

    for (y = 0U; y < (unsigned int)bitmap->size.height; ++y) {
        const unsigned char *source_row;
        unsigned char *target_row;
        unsigned int source_y;

        source_y = bitmap->origin_top_left
            ? (unsigned int)(bitmap->size.height - 1) - y
            : y;
        source_row = source_bytes + ((size_t)source_y * (size_t)bitmap->stride_bytes);
        target_row = pixels + ((size_t)y * (size_t)texture_width * 4U);

        for (x = 0U; x < (unsigned int)bitmap->size.width; ++x) {
            const unsigned char *source_pixel;
            unsigned char *target_pixel;

            source_pixel = source_row + ((size_t)x * (size_t)source_bytes_per_pixel);
            target_pixel = target_row + ((size_t)x * 4U);
            target_pixel[0] = source_pixel[2];
            target_pixel[1] = source_pixel[1];
            target_pixel[2] = source_pixel[0];
            target_pixel[3] = source_bytes_per_pixel == 4 ? source_pixel[3] : 255U;
        }
    }
}

int screensave_gl11_blit_bitmap_impl(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    screensave_gl11_state *state;
    GLuint texture_id;
    unsigned char *converted_pixels;
    unsigned int texture_width;
    unsigned int texture_height;
    GLfloat destination_x;
    GLfloat destination_y;
    GLfloat destination_width;
    GLfloat destination_height;
    GLfloat texture_u;
    GLfloat texture_v;

    if (!screensave_gl11_state_from_renderer(renderer, &state) || !state->frame_open || bitmap == NULL) {
        return 0;
    }

    if (
        bitmap->pixels == NULL ||
        bitmap->size.width <= 0 ||
        bitmap->size.height <= 0 ||
        screensave_gl11_bitmap_bytes_per_pixel(bitmap->bits_per_pixel) == 0
    ) {
        return 0;
    }

    texture_width = screensave_gl11_next_power_of_two((unsigned int)bitmap->size.width);
    texture_height = screensave_gl11_next_power_of_two((unsigned int)bitmap->size.height);
    converted_pixels = (unsigned char *)malloc((size_t)texture_width * (size_t)texture_height * 4U);
    if (converted_pixels == NULL) {
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6401UL,
            "gl11_bitmap",
            "GL11 bitmap upload memory allocation failed."
        );
        return 0;
    }

    screensave_gl11_convert_bitmap_to_rgba(bitmap, converted_pixels, texture_width, texture_height);

    if (destination_rect != NULL) {
        destination_x = (GLfloat)destination_rect->x;
        destination_y = (GLfloat)destination_rect->y;
        destination_width = (GLfloat)destination_rect->width;
        destination_height = (GLfloat)destination_rect->height;
    } else {
        destination_x = 0.0f;
        destination_y = 0.0f;
        destination_width = (GLfloat)bitmap->size.width;
        destination_height = (GLfloat)bitmap->size.height;
    }

    texture_u = (GLfloat)bitmap->size.width / (GLfloat)texture_width;
    texture_v = (GLfloat)bitmap->size.height / (GLfloat)texture_height;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture_id);
    if (texture_id == 0U) {
        free(converted_pixels);
        screensave_gl11_emit_diag(
            state,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6402UL,
            "gl11_bitmap",
            "glGenTextures failed for a GL11 bitmap blit."
        );
        glDisable(GL_TEXTURE_2D);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        (GLsizei)texture_width,
        (GLsizei)texture_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        converted_pixels
    );

    glColor4ub(255U, 255U, 255U, 255U);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, texture_v);
    glVertex2f(destination_x, destination_y);
    glTexCoord2f(texture_u, texture_v);
    glVertex2f(destination_x + destination_width, destination_y);
    glTexCoord2f(texture_u, 0.0f);
    glVertex2f(destination_x + destination_width, destination_y + destination_height);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(destination_x, destination_y + destination_height);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0U);
    glDeleteTextures(1, &texture_id);
    glDisable(GL_TEXTURE_2D);
    free(converted_pixels);
    return 1;
}
