#include "screensave/private/backend_kind.h"

const char *screensave_backend_kind_name(screensave_backend_kind kind)
{
    switch (kind) {
    case SCREENSAVE_BACKEND_KIND_GDI:
        return "gdi";

    case SCREENSAVE_BACKEND_KIND_GL11:
        return "gl11";

    case SCREENSAVE_BACKEND_KIND_GL21:
        return "gl21";

    case SCREENSAVE_BACKEND_KIND_GL33:
        return "gl33";

    case SCREENSAVE_BACKEND_KIND_GL46:
        return "gl46";

    case SCREENSAVE_BACKEND_KIND_NULL:
        return "null";

    case SCREENSAVE_BACKEND_KIND_UNKNOWN:
    default:
        return "unknown";
    }
}

screensave_backend_family screensave_backend_family_from_kind(screensave_backend_kind kind)
{
    switch (kind) {
    case SCREENSAVE_BACKEND_KIND_GDI:
        return SCREENSAVE_BACKEND_FAMILY_GDI;

    case SCREENSAVE_BACKEND_KIND_GL11:
    case SCREENSAVE_BACKEND_KIND_GL21:
    case SCREENSAVE_BACKEND_KIND_GL33:
    case SCREENSAVE_BACKEND_KIND_GL46:
        return SCREENSAVE_BACKEND_FAMILY_OPENGL;

    case SCREENSAVE_BACKEND_KIND_NULL:
        return SCREENSAVE_BACKEND_FAMILY_NULL;

    case SCREENSAVE_BACKEND_KIND_UNKNOWN:
    default:
        return SCREENSAVE_BACKEND_FAMILY_UNKNOWN;
    }
}

screensave_renderer_kind screensave_backend_kind_public_kind(screensave_backend_kind kind)
{
    switch (kind) {
    case SCREENSAVE_BACKEND_KIND_GDI:
        return SCREENSAVE_RENDERER_KIND_GDI;

    case SCREENSAVE_BACKEND_KIND_GL11:
        return SCREENSAVE_RENDERER_KIND_GL11;

    case SCREENSAVE_BACKEND_KIND_GL21:
        return SCREENSAVE_RENDERER_KIND_GL21;

    case SCREENSAVE_BACKEND_KIND_GL33:
        return SCREENSAVE_RENDERER_KIND_GL33;

    case SCREENSAVE_BACKEND_KIND_GL46:
        return SCREENSAVE_RENDERER_KIND_GL46;

    case SCREENSAVE_BACKEND_KIND_NULL:
        return SCREENSAVE_RENDERER_KIND_NULL;

    case SCREENSAVE_BACKEND_KIND_UNKNOWN:
    default:
        return SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }
}
