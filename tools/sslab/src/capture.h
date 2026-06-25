#ifndef SCREENSAVE_SSLAB_CAPTURE_H
#define SCREENSAVE_SSLAB_CAPTURE_H

#include "screensave/private/surface_rgba8.h"

int sslab_write_review_ppm(const screensave_rgba8_surface *surface, const char *path);
int sslab_write_raw_rgba(const screensave_rgba8_surface *surface, const char *path);

#endif
