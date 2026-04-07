#ifndef SCREENSAVE_PRIVATE_BACKEND_LOADER_H
#define SCREENSAVE_PRIVATE_BACKEND_LOADER_H

#include <windows.h>

#include "screensave/diagnostics_api.h"
#include "screensave/private/backend_caps.h"
#include "screensave/private/present_path.h"
#include "screensave/private/service_seams.h"

typedef struct screensave_backend_request_tag {
    screensave_renderer_kind requested_kind;
    HWND target_window;
    screensave_sizei drawable_size;
    screensave_diag_context *diagnostics;
} screensave_backend_request;

typedef struct screensave_backend_selection_tag {
    screensave_backend_kind backend_kind;
    screensave_render_band requested_band_ceiling;
    screensave_render_band active_band;
    screensave_renderer_kind requested_kind;
    screensave_renderer_kind active_kind;
    const char *selection_reason;
    const char *fallback_reason;
    const char *status_text;
    screensave_backend_caps caps;
    screensave_present_path present_path;
    screensave_service_seams service_seams;
} screensave_backend_selection;

int screensave_backend_loader_select_and_create(
    const screensave_backend_request *request,
    screensave_renderer **renderer_out,
    screensave_backend_selection *selection_out
);

#endif /* SCREENSAVE_PRIVATE_BACKEND_LOADER_H */
