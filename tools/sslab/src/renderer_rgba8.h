#ifndef SCREENSAVE_SSLAB_RENDERER_RGBA8_H
#define SCREENSAVE_SSLAB_RENDERER_RGBA8_H

#include "screensave/private/surface_rgba8.h"

typedef struct screensave_renderer_tag screensave_renderer;

struct screensave_renderer_tag {
    screensave_rgba8_surface *surface;
};

void sslab_rgba8_renderer_init(screensave_renderer *renderer, screensave_rgba8_surface *surface);

#endif
