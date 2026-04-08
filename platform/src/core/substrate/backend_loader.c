#include <string.h>

#include "screensave/private/backend_loader.h"
#include "screensave/private/backend_kind.h"
#include "screensave/private/backend_registry.h"
#include "screensave/private/renderer_runtime.h"

static void screensave_backend_loader_emit_create_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_RENDERER,
        code,
        "backend_loader",
        text
    );
}

static void screensave_backend_loader_init_selection(
    screensave_backend_selection *selection,
    screensave_renderer_kind requested_kind
)
{
    if (selection == NULL) {
        return;
    }

    memset(selection, 0, sizeof(*selection));
    selection->backend_kind = SCREENSAVE_BACKEND_KIND_UNKNOWN;
    selection->requested_band_ceiling =
        screensave_render_band_ceiling_for_request(requested_kind);
    selection->active_band = SCREENSAVE_RENDER_BAND_UNKNOWN;
    selection->requested_kind = requested_kind;
    selection->active_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
    screensave_backend_caps_init(
        &selection->caps,
        SCREENSAVE_BACKEND_KIND_UNKNOWN,
        SCREENSAVE_RENDER_BAND_UNKNOWN
    );
    screensave_present_path_init(
        &selection->present_path,
        SCREENSAVE_BACKEND_KIND_UNKNOWN,
        SCREENSAVE_RENDER_BAND_UNKNOWN,
        0UL,
        NULL
    );
    screensave_service_seams_init(&selection->service_seams);
}

static const char *screensave_backend_loader_force_selection_reason(
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
)
{
    switch (requested_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return active_kind == SCREENSAVE_RENDERER_KIND_GDI ? "force-gdi" : "force-gdi-fallback-null";

    case SCREENSAVE_RENDERER_KIND_GL11:
        switch (active_kind) {
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "force-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "force-gl11-fallback-gdi";
        case SCREENSAVE_RENDERER_KIND_NULL:
            return "force-gl11-fallback-null";
        default:
            return "force-gl11-fallback";
        }

    case SCREENSAVE_RENDERER_KIND_GL21:
        switch (active_kind) {
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "force-gl21";
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "force-gl21-fallback-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "force-gl21-fallback-gdi";
        case SCREENSAVE_RENDERER_KIND_NULL:
            return "force-gl21-fallback-null";
        default:
            return "force-gl21-fallback";
        }

    case SCREENSAVE_RENDERER_KIND_GL33:
        switch (active_kind) {
        case SCREENSAVE_RENDERER_KIND_GL33:
            return "force-gl33";
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "force-gl33-fallback-gl21";
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "force-gl33-fallback-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "force-gl33-fallback-gdi";
        case SCREENSAVE_RENDERER_KIND_NULL:
            return "force-gl33-fallback-null";
        default:
            return "force-gl33-fallback";
        }

    case SCREENSAVE_RENDERER_KIND_GL46:
        switch (active_kind) {
        case SCREENSAVE_RENDERER_KIND_GL46:
            return "force-gl46";
        case SCREENSAVE_RENDERER_KIND_GL33:
            return "force-gl46-fallback-gl33";
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "force-gl46-fallback-gl21";
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "force-gl46-fallback-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "force-gl46-fallback-gdi";
        case SCREENSAVE_RENDERER_KIND_NULL:
            return "force-gl46-fallback-null";
        default:
            return "force-gl46-fallback";
        }

    case SCREENSAVE_RENDERER_KIND_NULL:
        return "force-null";

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "force-fallback";
    }
}

static const char *screensave_backend_loader_auto_selection_reason(
    screensave_renderer_kind active_kind
)
{
    switch (active_kind) {
    case SCREENSAVE_RENDERER_KIND_GL46:
        return "auto-prefer-gl46";
    case SCREENSAVE_RENDERER_KIND_GL33:
        return "auto-prefer-gl33";
    case SCREENSAVE_RENDERER_KIND_GL21:
        return "auto-fallback-gl21";
    case SCREENSAVE_RENDERER_KIND_GL11:
        return "auto-fallback-gl11";
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "auto-fallback-gdi";
    case SCREENSAVE_RENDERER_KIND_NULL:
        return "auto-fallback-null";
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "auto-fallback";
    }
}

static const char *screensave_backend_loader_fallback_status_text(
    screensave_renderer_kind active_kind
)
{
    switch (active_kind) {
    case SCREENSAVE_RENDERER_KIND_GL33:
        return "fallback-gl33";
    case SCREENSAVE_RENDERER_KIND_GL21:
        return "fallback-gl21";
    case SCREENSAVE_RENDERER_KIND_GL11:
        return "fallback-gl11";
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "fallback-gdi";
    case SCREENSAVE_RENDERER_KIND_NULL:
        return "fallback-null";
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return NULL;
    }
}

static void screensave_backend_loader_apply_selection(
    screensave_renderer *renderer,
    const screensave_backend_descriptor *descriptor,
    const screensave_backend_request *request,
    const char *fallback_reason,
    screensave_backend_selection *selection
)
{
    screensave_backend_caps captured_caps;
    screensave_present_path present_path;
    screensave_service_seams service_seams;
    screensave_renderer_kind active_kind;
    const char *selection_reason;
    const char *status_text;

    if (renderer == NULL || descriptor == NULL || request == NULL || selection == NULL) {
        return;
    }

    active_kind = screensave_backend_kind_public_kind(descriptor->backend_kind);
    selection->backend_kind = descriptor->backend_kind;
    selection->requested_kind = request->requested_kind;
    selection->active_kind = active_kind;
    selection->requested_band_ceiling =
        screensave_render_band_ceiling_for_request(request->requested_kind);
    selection->active_band = descriptor->band;
    selection_reason =
        request->requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN
            ? screensave_backend_loader_auto_selection_reason(active_kind)
            : screensave_backend_loader_force_selection_reason(
                request->requested_kind,
                active_kind
            );
    selection->selection_reason = selection_reason;
    selection->fallback_reason = fallback_reason;
    status_text = NULL;
    if (
        request->requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN ||
        request->requested_kind != active_kind
    ) {
        status_text = screensave_backend_loader_fallback_status_text(active_kind);
    }
    selection->status_text = status_text;

    screensave_backend_caps_init(
        &captured_caps,
        descriptor->backend_kind,
        descriptor->band
    );
    if (
        descriptor->capture_caps_fn == NULL ||
        !descriptor->capture_caps_fn(renderer, &captured_caps)
    ) {
        screensave_backend_caps_apply_renderer_info(&captured_caps, &renderer->info);
    }

    screensave_present_path_init(
        &present_path,
        descriptor->backend_kind,
        descriptor->band,
        descriptor->descriptor_flags,
        &captured_caps
    );
    screensave_service_seams_bind_private_defaults(&service_seams, descriptor->band);

    selection->caps = captured_caps;
    selection->present_path = present_path;
    selection->service_seams = service_seams;

    renderer->info.requested_kind = request->requested_kind;
    renderer->info.selection_reason = selection_reason;
    renderer->info.fallback_reason = fallback_reason;
    if (status_text != NULL) {
        renderer->info.status_text = status_text;
    }

    screensave_renderer_set_backend_identity(
        renderer,
        descriptor->backend_kind,
        descriptor->band
    );
    screensave_renderer_set_backend_caps(renderer, &captured_caps);
    screensave_renderer_set_present_path(renderer, &present_path);
    screensave_renderer_set_service_seams(renderer, &service_seams);

    if (
        request->diagnostics != NULL &&
        descriptor->backend_kind == SCREENSAVE_BACKEND_KIND_GL46 &&
        request->requested_kind != SCREENSAVE_RENDERER_KIND_GL46
    ) {
        screensave_backend_loader_emit_create_diag(
            request->diagnostics,
            SCREENSAVE_DIAG_LEVEL_INFO,
            4106UL,
            "Renderer selection is using the GL46 premium lane."
        );
    }

    if (
        request->diagnostics != NULL &&
        descriptor->backend_kind == SCREENSAVE_BACKEND_KIND_GL11 &&
        request->requested_kind != SCREENSAVE_RENDERER_KIND_GL11
    ) {
        screensave_backend_loader_emit_create_diag(
            request->diagnostics,
            SCREENSAVE_DIAG_LEVEL_INFO,
            4103UL,
            "Renderer selection is using the GL11 compat lane."
        );
    }

    if (
        request->diagnostics != NULL &&
        descriptor->backend_kind == SCREENSAVE_BACKEND_KIND_GL33 &&
        request->requested_kind != SCREENSAVE_RENDERER_KIND_GL33
    ) {
        screensave_backend_loader_emit_create_diag(
            request->diagnostics,
            SCREENSAVE_DIAG_LEVEL_INFO,
            4105UL,
            "Renderer selection is using the GL33 modern lane."
        );
    }

    if (
        request->diagnostics != NULL &&
        descriptor->backend_kind == SCREENSAVE_BACKEND_KIND_GL21 &&
        request->requested_kind != SCREENSAVE_RENDERER_KIND_GL21
    ) {
        screensave_backend_loader_emit_create_diag(
            request->diagnostics,
            SCREENSAVE_DIAG_LEVEL_INFO,
            4104UL,
            "Renderer selection is using the GL21 advanced lane."
        );
    }

    if (
        request->diagnostics != NULL &&
        descriptor->backend_kind == SCREENSAVE_BACKEND_KIND_GDI &&
        request->requested_kind != SCREENSAVE_RENDERER_KIND_GDI
    ) {
        screensave_backend_loader_emit_create_diag(
            request->diagnostics,
            SCREENSAVE_DIAG_LEVEL_INFO,
            4102UL,
            "Renderer selection landed on the mandatory GDI floor."
        );
    }
}

int screensave_backend_loader_select_and_create(
    const screensave_backend_request *request,
    screensave_renderer **renderer_out,
    screensave_backend_selection *selection_out
)
{
    const screensave_backend_kind *chain;
    const screensave_backend_descriptor *descriptor;
    const char *attempt_failure_reason;
    const char *last_failure_reason;
    unsigned int chain_count;
    unsigned int index;
    screensave_renderer *renderer;

    if (renderer_out == NULL || request == NULL) {
        return 0;
    }

    *renderer_out = NULL;
    if (selection_out != NULL) {
        screensave_backend_loader_init_selection(
            selection_out,
            request->requested_kind
        );
    }

    chain = screensave_backend_registry_chain_for_request(
        request->requested_kind,
        &chain_count
    );
    if (chain == NULL || chain_count == 0U) {
        return 0;
    }

    last_failure_reason = NULL;
    renderer = NULL;
    for (index = 0U; index < chain_count; ++index) {
        attempt_failure_reason = NULL;
        descriptor = screensave_backend_registry_get(chain[index]);
        if (descriptor == NULL || descriptor->create_fn == NULL) {
            continue;
        }

        if (
            descriptor->create_fn(
                request->target_window,
                &request->drawable_size,
                request->diagnostics,
                &renderer,
                &attempt_failure_reason
            )
        ) {
            if (selection_out != NULL) {
                screensave_backend_loader_apply_selection(
                    renderer,
                    descriptor,
                    request,
                    last_failure_reason,
                    selection_out
                );
            } else {
                screensave_backend_selection selection;

                screensave_backend_loader_init_selection(
                    &selection,
                    request->requested_kind
                );
                screensave_backend_loader_apply_selection(
                    renderer,
                    descriptor,
                    request,
                    last_failure_reason,
                    &selection
                );
            }
            *renderer_out = renderer;
            return 1;
        }

        if (attempt_failure_reason != NULL) {
            last_failure_reason = attempt_failure_reason;
        }
    }

    if (selection_out != NULL) {
        screensave_backend_caps_mark_failure(
            &selection_out->caps,
            last_failure_reason,
            0
        );
    }
    screensave_backend_loader_emit_create_diag(
        request->diagnostics,
        SCREENSAVE_DIAG_LEVEL_ERROR,
        4101UL,
        "The renderer runtime could not create any renderer backend, including the null safety backend."
    );
    return 0;
}
