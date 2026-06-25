#include <stdlib.h>
#include <string.h>

#include "screensave/private/surface_rgba8.h"

int screensave_rgba8_surface_init(screensave_rgba8_surface *surface, int width, int height)
{
    unsigned long byte_count;

    if (surface == NULL || width <= 0 || height <= 0) {
        return 0;
    }

    surface->width = 0;
    surface->height = 0;
    surface->stride_bytes = 0;
    surface->pixels = NULL;

    byte_count = (unsigned long)width * (unsigned long)height * 4UL;
    if (byte_count == 0UL || byte_count > 0x7FFFFFFFUL) {
        return 0;
    }

    surface->pixels = (unsigned char *)malloc((size_t)byte_count);
    if (surface->pixels == NULL) {
        return 0;
    }

    surface->width = width;
    surface->height = height;
    surface->stride_bytes = width * 4;
    memset(surface->pixels, 0, (size_t)byte_count);
    return 1;
}

void screensave_rgba8_surface_dispose(screensave_rgba8_surface *surface)
{
    if (surface == NULL) {
        return;
    }

    if (surface->pixels != NULL) {
        free(surface->pixels);
    }
    surface->width = 0;
    surface->height = 0;
    surface->stride_bytes = 0;
    surface->pixels = NULL;
}

void screensave_rgba8_surface_set_pixel(
    screensave_rgba8_surface *surface,
    int x,
    int y,
    screensave_color color
)
{
    unsigned char *pixel;

    if (
        surface == NULL ||
        surface->pixels == NULL ||
        x < 0 ||
        y < 0 ||
        x >= surface->width ||
        y >= surface->height
    ) {
        return;
    }

    pixel = surface->pixels + (y * surface->stride_bytes) + (x * 4);
    pixel[0] = color.red;
    pixel[1] = color.green;
    pixel[2] = color.blue;
    pixel[3] = color.alpha;
}

void screensave_rgba8_surface_clear(screensave_rgba8_surface *surface, screensave_color color)
{
    int x;
    int y;

    if (surface == NULL || surface->pixels == NULL) {
        return;
    }

    for (y = 0; y < surface->height; ++y) {
        for (x = 0; x < surface->width; ++x) {
            screensave_rgba8_surface_set_pixel(surface, x, y, color);
        }
    }
}

unsigned long screensave_rgba8_surface_checksum(const screensave_rgba8_surface *surface)
{
    unsigned long hash;
    unsigned long index;
    unsigned long byte_count;

    if (surface == NULL || surface->pixels == NULL || surface->width <= 0 || surface->height <= 0) {
        return 0UL;
    }

    hash = 2166136261UL;
    byte_count = (unsigned long)surface->height * (unsigned long)surface->stride_bytes;
    for (index = 0UL; index < byte_count; ++index) {
        hash ^= (unsigned long)surface->pixels[index];
        hash *= 16777619UL;
    }

    return hash;
}
