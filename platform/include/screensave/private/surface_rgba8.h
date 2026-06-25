#ifndef SCREENSAVE_PRIVATE_SURFACE_RGBA8_H
#define SCREENSAVE_PRIVATE_SURFACE_RGBA8_H

#include "screensave/types.h"

typedef struct screensave_rgba8_surface_tag {
    int width;
    int height;
    int stride_bytes;
    unsigned char *pixels;
} screensave_rgba8_surface;

int screensave_rgba8_surface_init(screensave_rgba8_surface *surface, int width, int height);
void screensave_rgba8_surface_dispose(screensave_rgba8_surface *surface);
void screensave_rgba8_surface_clear(screensave_rgba8_surface *surface, screensave_color color);
void screensave_rgba8_surface_set_pixel(
    screensave_rgba8_surface *surface,
    int x,
    int y,
    screensave_color color
);
unsigned long screensave_rgba8_surface_checksum(const screensave_rgba8_surface *surface);

#endif /* SCREENSAVE_PRIVATE_SURFACE_RGBA8_H */
