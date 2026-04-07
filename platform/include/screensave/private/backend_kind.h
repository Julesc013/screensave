#ifndef SCREENSAVE_PRIVATE_BACKEND_KIND_H
#define SCREENSAVE_PRIVATE_BACKEND_KIND_H

#include "screensave/renderer_api.h"

typedef enum screensave_backend_family_tag {
    SCREENSAVE_BACKEND_FAMILY_UNKNOWN = 0,
    SCREENSAVE_BACKEND_FAMILY_GDI = 1,
    SCREENSAVE_BACKEND_FAMILY_OPENGL = 2,
    SCREENSAVE_BACKEND_FAMILY_NULL = 3
} screensave_backend_family;

typedef enum screensave_backend_kind_tag {
    SCREENSAVE_BACKEND_KIND_UNKNOWN = 0,
    SCREENSAVE_BACKEND_KIND_GDI = 1,
    SCREENSAVE_BACKEND_KIND_GL11 = 2,
    SCREENSAVE_BACKEND_KIND_GL21 = 3,
    SCREENSAVE_BACKEND_KIND_GL33 = 4,
    SCREENSAVE_BACKEND_KIND_GL46 = 5,
    SCREENSAVE_BACKEND_KIND_NULL = 6
} screensave_backend_kind;

const char *screensave_backend_kind_name(screensave_backend_kind kind);
screensave_backend_family screensave_backend_family_from_kind(screensave_backend_kind kind);
screensave_renderer_kind screensave_backend_kind_public_kind(screensave_backend_kind kind);

#endif /* SCREENSAVE_PRIVATE_BACKEND_KIND_H */
