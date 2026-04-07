#ifndef SCREENSAVE_PRIVATE_BACKEND_REGISTRY_H
#define SCREENSAVE_PRIVATE_BACKEND_REGISTRY_H

#include <windows.h>

#include "screensave/diagnostics_api.h"
#include "screensave/renderer_api.h"
#include "screensave/private/backend_caps.h"

#define SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_PLACEHOLDER         0x00000001UL
#define SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_USES_WINDOW_DC      0x00000002UL
#define SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_ACCEPTS_EXTERNAL_DC 0x00000004UL
#define SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_NOOP_PRESENT        0x00000008UL

typedef int (*screensave_backend_create_fn)(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
);
typedef int (*screensave_backend_resize_fn)(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
);
typedef void (*screensave_backend_present_dc_fn)(screensave_renderer *renderer, HDC present_dc);
typedef void (*screensave_backend_clear_present_dc_fn)(screensave_renderer *renderer);
typedef int (*screensave_backend_capture_caps_fn)(
    const screensave_renderer *renderer,
    screensave_backend_caps *caps_out
);

typedef struct screensave_backend_descriptor_tag {
    screensave_backend_kind backend_kind;
    screensave_renderer_kind public_kind;
    screensave_render_band band;
    const char *debug_name;
    unsigned long descriptor_flags;
    screensave_backend_create_fn create_fn;
    screensave_backend_resize_fn resize_fn;
    screensave_backend_present_dc_fn set_present_dc_fn;
    screensave_backend_clear_present_dc_fn clear_present_dc_fn;
    screensave_backend_capture_caps_fn capture_caps_fn;
} screensave_backend_descriptor;

const screensave_backend_descriptor *screensave_backend_registry_get(
    screensave_backend_kind backend_kind
);
const screensave_backend_kind *screensave_backend_registry_chain_for_request(
    screensave_renderer_kind requested_kind,
    unsigned int *count_out
);

#endif /* SCREENSAVE_PRIVATE_BACKEND_REGISTRY_H */
