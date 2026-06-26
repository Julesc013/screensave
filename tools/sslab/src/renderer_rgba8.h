#ifndef SCREENSAVE_SSLAB_RENDERER_RGBA8_H
#define SCREENSAVE_SSLAB_RENDERER_RGBA8_H

#include "screensave/renderer_api.h"
#include "screensave/private/surface_rgba8.h"

#ifdef SSLAB_RGBA8_USE_PLATFORM_RENDERER
#include "screensave/private/renderer_runtime.h"
#else
struct screensave_renderer_tag {
    screensave_rgba8_surface *surface;
};
#endif

void sslab_rgba8_renderer_init(screensave_renderer *renderer, screensave_rgba8_surface *surface);

#endif
