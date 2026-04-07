#include <string.h>

#include "screensave/private/backend_registry.h"
#include "screensave/private/present_path.h"
#include "screensave/private/renderer_runtime.h"

static const char *screensave_present_path_name_for_backend(screensave_backend_kind backend_kind)
{
    switch (backend_kind) {
    case SCREENSAVE_BACKEND_KIND_GDI:
        return "blit-present-dc";

    case SCREENSAVE_BACKEND_KIND_GL11:
    case SCREENSAVE_BACKEND_KIND_GL21:
    case SCREENSAVE_BACKEND_KIND_GL33:
    case SCREENSAVE_BACKEND_KIND_GL46:
        return "context-present-window";

    case SCREENSAVE_BACKEND_KIND_NULL:
        return "noop-present-dc";

    case SCREENSAVE_BACKEND_KIND_UNKNOWN:
    default:
        return "unknown-present";
    }
}

void screensave_present_path_init(
    screensave_present_path *path,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    unsigned long descriptor_flags,
    const screensave_backend_caps *caps
)
{
    if (path == NULL) {
        return;
    }

    memset(path, 0, sizeof(*path));
    path->backend_kind = backend_kind;
    path->active_band = active_band;
    path->path_name = screensave_present_path_name_for_backend(backend_kind);

    if ((descriptor_flags & SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_USES_WINDOW_DC) != 0UL) {
        path->flags |= SCREENSAVE_PRESENT_PATH_FLAG_USES_WINDOW_DC;
    }
    if ((descriptor_flags & SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_ACCEPTS_EXTERNAL_DC) != 0UL) {
        path->flags |= SCREENSAVE_PRESENT_PATH_FLAG_ACCEPTS_EXTERNAL_DC;
    }
    if ((descriptor_flags & SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_NOOP_PRESENT) != 0UL) {
        path->flags |= SCREENSAVE_PRESENT_PATH_FLAG_NOOP_PRESENT;
    }
    if (
        caps != NULL &&
        (caps->state_flags & SCREENSAVE_BACKEND_CAPS_STATE_DOUBLE_BUFFERED) != 0UL
    ) {
        path->flags |= SCREENSAVE_PRESENT_PATH_FLAG_SWAP_BUFFERS;
    }
}

int screensave_present_path_resize_renderer(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
)
{
    const screensave_backend_descriptor *descriptor;

    if (renderer == NULL) {
        return 0;
    }

    descriptor = screensave_backend_registry_get(renderer->backend_kind);
    if (descriptor == NULL || descriptor->resize_fn == NULL) {
        return 0;
    }

    return descriptor->resize_fn(renderer, drawable_size);
}

void screensave_present_path_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    const screensave_backend_descriptor *descriptor;

    if (renderer == NULL) {
        return;
    }

    descriptor = screensave_backend_registry_get(renderer->backend_kind);
    if (descriptor == NULL || descriptor->set_present_dc_fn == NULL) {
        return;
    }

    descriptor->set_present_dc_fn(renderer, present_dc);
}

void screensave_present_path_clear_present_dc(screensave_renderer *renderer)
{
    const screensave_backend_descriptor *descriptor;

    if (renderer == NULL) {
        return;
    }

    descriptor = screensave_backend_registry_get(renderer->backend_kind);
    if (descriptor == NULL || descriptor->clear_present_dc_fn == NULL) {
        return;
    }

    descriptor->clear_present_dc_fn(renderer);
}
