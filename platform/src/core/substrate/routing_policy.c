#include <string.h>

#include "screensave/private/routing_policy.h"

static int screensave_routing_kind_rank(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return 1;

    case SCREENSAVE_RENDERER_KIND_GL11:
        return 2;

    case SCREENSAVE_RENDERER_KIND_GL21:
        return 3;

    case SCREENSAVE_RENDERER_KIND_GL33:
        return 4;

    case SCREENSAVE_RENDERER_KIND_GL46:
        return 5;

    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return 0;
    }
}

static screensave_renderer_kind screensave_routing_highest_supported_kind(
    const screensave_saver_module *module
)
{
    if (module == NULL) {
        return SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }

    if (screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GL46)) {
        return SCREENSAVE_RENDERER_KIND_GL46;
    }
    if (screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GL33)) {
        return SCREENSAVE_RENDERER_KIND_GL33;
    }
    if (screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GL21)) {
        return SCREENSAVE_RENDERER_KIND_GL21;
    }
    if (screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GL11)) {
        return SCREENSAVE_RENDERER_KIND_GL11;
    }
    if (screensave_saver_supports_renderer_kind(module, SCREENSAVE_RENDERER_KIND_GDI)) {
        return SCREENSAVE_RENDERER_KIND_GDI;
    }

    return SCREENSAVE_RENDERER_KIND_UNKNOWN;
}

static const char *screensave_routing_auto_reason(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "policy-auto-prefer-gdi";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "policy-auto-prefer-gl11";

    case SCREENSAVE_RENDERER_KIND_GL21:
        return "policy-auto-prefer-gl21";

    case SCREENSAVE_RENDERER_KIND_GL33:
        return "policy-auto-prefer-gl33";

    case SCREENSAVE_RENDERER_KIND_GL46:
        return "policy-auto-prefer-gl46";

    case SCREENSAVE_RENDERER_KIND_NULL:
        return "policy-auto-prefer-null";

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "policy-auto";
    }
}

static const char *screensave_routing_request_reason(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "policy-request-gdi";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "policy-request-gl11";

    case SCREENSAVE_RENDERER_KIND_GL21:
        return "policy-request-gl21";

    case SCREENSAVE_RENDERER_KIND_GL33:
        return "policy-request-gl33";

    case SCREENSAVE_RENDERER_KIND_GL46:
        return "policy-request-gl46";

    case SCREENSAVE_RENDERER_KIND_NULL:
        return "policy-request-null";

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "policy-request";
    }
}

static const char *screensave_routing_clamp_reason(
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind effective_kind
)
{
    switch (requested_kind) {
    case SCREENSAVE_RENDERER_KIND_GL11:
        return effective_kind == SCREENSAVE_RENDERER_KIND_GDI
            ? "policy-request-gl11-clamp-gdi"
            : "policy-request-gl11-clamp";

    case SCREENSAVE_RENDERER_KIND_GL21:
        switch (effective_kind) {
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "policy-request-gl21-clamp-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "policy-request-gl21-clamp-gdi";
        default:
            return "policy-request-gl21-clamp";
        }

    case SCREENSAVE_RENDERER_KIND_GL33:
        switch (effective_kind) {
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "policy-request-gl33-clamp-gl21";
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "policy-request-gl33-clamp-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "policy-request-gl33-clamp-gdi";
        default:
            return "policy-request-gl33-clamp";
        }

    case SCREENSAVE_RENDERER_KIND_GL46:
        switch (effective_kind) {
        case SCREENSAVE_RENDERER_KIND_GL33:
            return "policy-request-gl46-clamp-gl33";
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "policy-request-gl46-clamp-gl21";
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "policy-request-gl46-clamp-gl11";
        case SCREENSAVE_RENDERER_KIND_GDI:
            return "policy-request-gl46-clamp-gdi";
        default:
            return "policy-request-gl46-clamp";
        }

    case SCREENSAVE_RENDERER_KIND_GDI:
    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "policy-request-clamp";
    }
}

static const char *screensave_routing_raise_reason(
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind effective_kind
)
{
    switch (requested_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        switch (effective_kind) {
        case SCREENSAVE_RENDERER_KIND_GL11:
            return "policy-request-gdi-raise-gl11";
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "policy-request-gdi-raise-gl21";
        case SCREENSAVE_RENDERER_KIND_GL33:
            return "policy-request-gdi-raise-gl33";
        case SCREENSAVE_RENDERER_KIND_GL46:
            return "policy-request-gdi-raise-gl46";
        default:
            return "policy-request-gdi-raise";
        }

    case SCREENSAVE_RENDERER_KIND_GL11:
        switch (effective_kind) {
        case SCREENSAVE_RENDERER_KIND_GL21:
            return "policy-request-gl11-raise-gl21";
        case SCREENSAVE_RENDERER_KIND_GL33:
            return "policy-request-gl11-raise-gl33";
        case SCREENSAVE_RENDERER_KIND_GL46:
            return "policy-request-gl11-raise-gl46";
        default:
            return "policy-request-gl11-raise";
        }

    case SCREENSAVE_RENDERER_KIND_GL21:
        switch (effective_kind) {
        case SCREENSAVE_RENDERER_KIND_GL33:
            return "policy-request-gl21-raise-gl33";
        case SCREENSAVE_RENDERER_KIND_GL46:
            return "policy-request-gl21-raise-gl46";
        default:
            return "policy-request-gl21-raise";
        }

    case SCREENSAVE_RENDERER_KIND_GL33:
        return effective_kind == SCREENSAVE_RENDERER_KIND_GL46
            ? "policy-request-gl33-raise-gl46"
            : "policy-request-gl33-raise";

    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "policy-request-raise";
    }
}

static void screensave_routing_emit_diag(
    screensave_diag_context *diagnostics,
    unsigned long code,
    const char *module_key,
    const char *text
)
{
    char message[192];

    if (diagnostics == NULL || text == NULL) {
        return;
    }

    message[0] = '\0';
    if (module_key != NULL && module_key[0] != '\0') {
        wsprintfA(message, "%s (%s)", text, module_key);
    } else {
        lstrcpynA(message, text, sizeof(message));
    }

    screensave_diag_emit(
        diagnostics,
        SCREENSAVE_DIAG_LEVEL_INFO,
        SCREENSAVE_DIAG_DOMAIN_RENDERER,
        code,
        "routing_policy",
        message
    );
}

int screensave_routing_prepare_backend_request(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    screensave_renderer_kind requested_kind,
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_backend_request *request_out
)
{
    screensave_renderer_kind effective_kind;
    screensave_renderer_kind preferred_kind;
    screensave_renderer_kind minimum_kind;
    screensave_renderer_kind highest_kind;
    const char *policy_reason;

    if (request_out == NULL) {
        return 0;
    }

    (void)common_config;

    memset(request_out, 0, sizeof(*request_out));
    request_out->requested_kind = requested_kind;
    request_out->effective_kind = requested_kind;
    request_out->target_window = target_window;
    request_out->diagnostics = diagnostics;
    if (drawable_size != NULL) {
        request_out->drawable_size = *drawable_size;
    } else {
        request_out->drawable_size.width = 0;
        request_out->drawable_size.height = 0;
    }

    if (module == NULL || !screensave_saver_module_is_valid(module)) {
        return 1;
    }

    minimum_kind = module->routing_policy.minimum_kind;
    preferred_kind = module->routing_policy.preferred_kind;
    highest_kind = screensave_routing_highest_supported_kind(module);
    effective_kind = requested_kind;
    policy_reason = NULL;

    if (requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN) {
        effective_kind = preferred_kind;
        policy_reason = screensave_routing_auto_reason(preferred_kind);
        screensave_routing_emit_diag(
            diagnostics,
            4110UL,
            module->identity.product_key,
            "Routing policy applied the saver-preferred renderer lane."
        );
    } else if (screensave_routing_kind_rank(requested_kind) == 0) {
        effective_kind = requested_kind;
        policy_reason = screensave_routing_request_reason(requested_kind);
    } else if (screensave_routing_kind_rank(requested_kind) > screensave_routing_kind_rank(highest_kind)) {
        effective_kind = highest_kind;
        policy_reason = screensave_routing_clamp_reason(requested_kind, effective_kind);
        screensave_routing_emit_diag(
            diagnostics,
            4111UL,
            module->identity.product_key,
            "Routing policy clamped the requested renderer to the saver's declared supported ceiling."
        );
    } else if (screensave_routing_kind_rank(requested_kind) < screensave_routing_kind_rank(minimum_kind)) {
        effective_kind = minimum_kind;
        policy_reason = screensave_routing_raise_reason(requested_kind, effective_kind);
        screensave_routing_emit_diag(
            diagnostics,
            4112UL,
            module->identity.product_key,
            "Routing policy raised the requested renderer to the saver's declared supported floor."
        );
    } else {
        policy_reason = screensave_routing_request_reason(requested_kind);
    }

    request_out->effective_kind = effective_kind;
    request_out->minimum_kind = minimum_kind;
    request_out->preferred_kind = preferred_kind;
    request_out->quality_class = module->routing_policy.quality_class;
    request_out->policy_reason = policy_reason;

    return 1;
}
