#include "gdi_internal.h"

int screensave_gdi_state_from_renderer(screensave_renderer *renderer, screensave_gdi_state **state_out)
{
    screensave_gdi_state *state;

    if (state_out != NULL) {
        *state_out = NULL;
    }

    if (renderer == NULL || renderer->backend_state == NULL) {
        return 0;
    }

    state = (screensave_gdi_state *)renderer->backend_state;
    if (state_out != NULL) {
        *state_out = state;
    }

    return 1;
}

void screensave_gdi_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
)
{
    if (renderer == NULL) {
        return;
    }

    renderer->info.active_kind = SCREENSAVE_RENDERER_KIND_GDI;
    renderer->info.capability_flags = SCREENSAVE_GDI_CAPABILITIES;
    renderer->info.backend_name = "gdi";
    renderer->info.status_text = status_text != NULL ? status_text : "ready";

    if (drawable_size != NULL) {
        renderer->info.drawable_size = *drawable_size;
    }
}

void screensave_gdi_emit_diag(
    screensave_gdi_state *state,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    if (state == NULL || state->diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        state->diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_RENDERER,
        code,
        origin,
        text
    );
}

COLORREF screensave_gdi_colorref(screensave_color color)
{
    return RGB(color.red, color.green, color.blue);
}
