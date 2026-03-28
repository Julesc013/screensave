#include <stdlib.h>
#include <string.h>

#include "screensave/visual_buffer_api.h"

static int screensave_visual_buffer_valid_size(const screensave_sizei *size)
{
    return size != NULL && size->width > 0 && size->height > 0;
}

static unsigned char *screensave_visual_buffer_row(
    const screensave_visual_buffer *buffer,
    int y
)
{
    return buffer->pixels + ((size_t)y * (size_t)buffer->stride_bytes);
}

static unsigned char screensave_visual_buffer_scale_channel(
    unsigned char channel,
    unsigned int scale
)
{
    return (unsigned char)(((unsigned int)channel * scale) / 255U);
}

int screensave_visual_buffer_init(
    screensave_visual_buffer *buffer,
    const screensave_sizei *size
)
{
    size_t pixel_bytes;

    if (buffer == NULL || !screensave_visual_buffer_valid_size(size)) {
        return 0;
    }

    ZeroMemory(buffer, sizeof(*buffer));
    buffer->size = *size;
    buffer->stride_bytes = size->width * 4;
    pixel_bytes = (size_t)buffer->stride_bytes * (size_t)size->height;
    buffer->pixels = (unsigned char *)malloc(pixel_bytes);
    if (buffer->pixels == NULL) {
        ZeroMemory(buffer, sizeof(*buffer));
        return 0;
    }

    memset(buffer->pixels, 0, pixel_bytes);
    return 1;
}

int screensave_visual_buffer_resize(
    screensave_visual_buffer *buffer,
    const screensave_sizei *size
)
{
    size_t pixel_bytes;
    unsigned char *pixels;

    if (buffer == NULL || !screensave_visual_buffer_valid_size(size)) {
        return 0;
    }

    if (
        buffer->pixels != NULL &&
        buffer->size.width == size->width &&
        buffer->size.height == size->height
    ) {
        return 1;
    }

    pixel_bytes = (size_t)(size->width * 4) * (size_t)size->height;
    pixels = (unsigned char *)malloc(pixel_bytes);
    if (pixels == NULL) {
        return 0;
    }

    memset(pixels, 0, pixel_bytes);
    free(buffer->pixels);
    buffer->pixels = pixels;
    buffer->size = *size;
    buffer->stride_bytes = size->width * 4;
    return 1;
}

void screensave_visual_buffer_dispose(screensave_visual_buffer *buffer)
{
    if (buffer == NULL) {
        return;
    }

    free(buffer->pixels);
    ZeroMemory(buffer, sizeof(*buffer));
}

void screensave_visual_buffer_clear(
    screensave_visual_buffer *buffer,
    screensave_color color
)
{
    int x;
    int y;

    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    for (y = 0; y < buffer->size.height; ++y) {
        unsigned char *row;

        row = screensave_visual_buffer_row(buffer, y);
        for (x = 0; x < buffer->size.width; ++x) {
            row[(x * 4) + 0] = color.blue;
            row[(x * 4) + 1] = color.green;
            row[(x * 4) + 2] = color.red;
            row[(x * 4) + 3] = 255U;
        }
    }
}

void screensave_visual_buffer_decay(
    screensave_visual_buffer *buffer,
    unsigned int keep_scale
)
{
    int x;
    int y;

    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    if (keep_scale > 255U) {
        keep_scale = 255U;
    }

    for (y = 0; y < buffer->size.height; ++y) {
        unsigned char *row;

        row = screensave_visual_buffer_row(buffer, y);
        for (x = 0; x < buffer->size.width; ++x) {
            row[(x * 4) + 0] = screensave_visual_buffer_scale_channel(row[(x * 4) + 0], keep_scale);
            row[(x * 4) + 1] = screensave_visual_buffer_scale_channel(row[(x * 4) + 1], keep_scale);
            row[(x * 4) + 2] = screensave_visual_buffer_scale_channel(row[(x * 4) + 2], keep_scale);
            row[(x * 4) + 3] = 255U;
        }
    }
}

void screensave_visual_buffer_add_pixel(
    screensave_visual_buffer *buffer,
    int x,
    int y,
    screensave_color color,
    unsigned int intensity
)
{
    unsigned char *pixel;
    unsigned int add_blue;
    unsigned int add_green;
    unsigned int add_red;
    unsigned int value;

    if (
        buffer == NULL ||
        buffer->pixels == NULL ||
        x < 0 ||
        y < 0 ||
        x >= buffer->size.width ||
        y >= buffer->size.height
    ) {
        return;
    }

    if (intensity > 255U) {
        intensity = 255U;
    }

    pixel = screensave_visual_buffer_row(buffer, y) + ((size_t)x * 4U);
    add_blue = ((unsigned int)color.blue * intensity) / 255U;
    add_green = ((unsigned int)color.green * intensity) / 255U;
    add_red = ((unsigned int)color.red * intensity) / 255U;

    value = (unsigned int)pixel[0] + add_blue;
    pixel[0] = (unsigned char)(value > 255U ? 255U : value);
    value = (unsigned int)pixel[1] + add_green;
    pixel[1] = (unsigned char)(value > 255U ? 255U : value);
    value = (unsigned int)pixel[2] + add_red;
    pixel[2] = (unsigned char)(value > 255U ? 255U : value);
    pixel[3] = 255U;
}

void screensave_visual_buffer_draw_line(
    screensave_visual_buffer *buffer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color,
    unsigned int intensity
)
{
    int x0;
    int y0;
    int x1;
    int y1;
    int delta_x;
    int delta_y;
    int step_x;
    int step_y;
    int error;

    if (
        buffer == NULL ||
        start_point == NULL ||
        end_point == NULL
    ) {
        return;
    }

    x0 = start_point->x;
    y0 = start_point->y;
    x1 = end_point->x;
    y1 = end_point->y;
    delta_x = x1 >= x0 ? x1 - x0 : x0 - x1;
    delta_y = y1 >= y0 ? y1 - y0 : y0 - y1;
    step_x = x0 < x1 ? 1 : -1;
    step_y = y0 < y1 ? 1 : -1;
    error = delta_x - delta_y;

    for (;;) {
        int doubled_error;

        screensave_visual_buffer_add_pixel(buffer, x0, y0, color, intensity);
        if (x0 == x1 && y0 == y1) {
            break;
        }

        doubled_error = error * 2;
        if (doubled_error > -delta_y) {
            error -= delta_y;
            x0 += step_x;
        }
        if (doubled_error < delta_x) {
            error += delta_x;
            y0 += step_y;
        }
    }
}

void screensave_visual_buffer_get_bitmap_view(
    const screensave_visual_buffer *buffer,
    screensave_bitmap_view *bitmap_view
)
{
    if (bitmap_view == NULL) {
        return;
    }

    ZeroMemory(bitmap_view, sizeof(*bitmap_view));
    if (buffer == NULL || buffer->pixels == NULL) {
        return;
    }

    bitmap_view->pixels = buffer->pixels;
    bitmap_view->size = buffer->size;
    bitmap_view->stride_bytes = buffer->stride_bytes;
    bitmap_view->bits_per_pixel = 32U;
    bitmap_view->origin_top_left = 1;
}

screensave_color screensave_color_lerp(
    screensave_color start_color,
    screensave_color end_color,
    unsigned int amount
)
{
    screensave_color color;

    if (amount > 255U) {
        amount = 255U;
    }

    color.red = (unsigned char)(
        ((unsigned int)start_color.red * (255U - amount) + (unsigned int)end_color.red * amount) / 255U
    );
    color.green = (unsigned char)(
        ((unsigned int)start_color.green * (255U - amount) + (unsigned int)end_color.green * amount) / 255U
    );
    color.blue = (unsigned char)(
        ((unsigned int)start_color.blue * (255U - amount) + (unsigned int)end_color.blue * amount) / 255U
    );
    color.alpha = (unsigned char)(
        ((unsigned int)start_color.alpha * (255U - amount) + (unsigned int)end_color.alpha * amount) / 255U
    );
    return color;
}
