#include "gl21_internal.h"

int screensave_gl21_state_from_renderer(screensave_renderer *renderer, screensave_gl21_state **state_out)
{
    screensave_gl21_state *state;

    if (state_out != NULL) {
        *state_out = NULL;
    }

    if (renderer == NULL || renderer->backend_state == NULL) {
        return 0;
    }

    state = (screensave_gl21_state *)renderer->backend_state;
    if (state_out != NULL) {
        *state_out = state;
    }

    return 1;
}

void screensave_gl21_update_renderer_info(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size,
    const char *status_text
)
{
    screensave_gl21_state *state;

    if (!screensave_gl21_state_from_renderer(renderer, &state)) {
        return;
    }

    renderer->info.active_kind = SCREENSAVE_RENDERER_KIND_GL21;
    renderer->info.capability_flags = SCREENSAVE_GL21_CAPABILITIES;
    renderer->info.backend_name = "gl21";
    renderer->info.status_text = status_text != NULL ? status_text : "ready";
    renderer->info.vendor_name = state->caps.vendor[0] != '\0' ? state->caps.vendor : NULL;
    renderer->info.renderer_name = state->caps.renderer[0] != '\0' ? state->caps.renderer : NULL;
    renderer->info.version_name = state->caps.version[0] != '\0' ? state->caps.version : NULL;

    if (drawable_size != NULL) {
        renderer->info.drawable_size = *drawable_size;
    }
}

void screensave_gl21_emit_diag(
    screensave_gl21_state *state,
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


