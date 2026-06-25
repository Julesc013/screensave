#include <stdio.h>

#include "capture.h"

int sslab_write_review_ppm(const screensave_rgba8_surface *surface, const char *path)
{
    FILE *file;
    int x;
    int y;
    const unsigned char *pixel;

    if (surface == NULL || surface->pixels == NULL || path == NULL) {
        return 0;
    }

    file = fopen(path, "w");
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "P3\n%d %d\n255\n", surface->width, surface->height);
    for (y = 0; y < surface->height; ++y) {
        for (x = 0; x < surface->width; ++x) {
            pixel = surface->pixels + (y * surface->stride_bytes) + (x * 4);
            if (x > 0) {
                fprintf(file, " ");
            }
            fprintf(file, "%u %u %u", (unsigned int)pixel[0], (unsigned int)pixel[1], (unsigned int)pixel[2]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

int sslab_write_raw_rgba(const screensave_rgba8_surface *surface, const char *path)
{
    FILE *file;
    unsigned long byte_count;
    size_t written;

    if (surface == NULL || surface->pixels == NULL || path == NULL || surface->width <= 0 || surface->height <= 0) {
        return 0;
    }

    byte_count = (unsigned long)surface->height * (unsigned long)surface->stride_bytes;
    file = fopen(path, "wb");
    if (file == NULL) {
        return 0;
    }

    written = fwrite(surface->pixels, 1U, (size_t)byte_count, file);
    fclose(file);
    return written == (size_t)byte_count;
}
