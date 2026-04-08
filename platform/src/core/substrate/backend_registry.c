#include "screensave/private/backend_registry.h"

#include "../../render/gdi/gdi_internal.h"
#include "../../render/gl11/gl11_internal.h"
#include "../../render/gl21/gl21_internal.h"
#include "../../render/gl33/gl33_internal.h"
#include "../../render/gl46/gl46_internal.h"
#include "../../render/null/null_internal.h"

static int screensave_backend_registry_create_gdi(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    if (
        screensave_gdi_renderer_create(
            target_window,
            drawable_size,
            diagnostics,
            renderer_out,
            failure_reason_out
        )
    ) {
        if (failure_reason_out != NULL) {
            *failure_reason_out = NULL;
        }
        return 1;
    }

    if (failure_reason_out != NULL && *failure_reason_out == NULL) {
        *failure_reason_out = "gdi-create-failed";
    }
    return 0;
}

static int screensave_backend_registry_resize_gdi(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
)
{
    return screensave_gdi_renderer_resize(renderer, drawable_size);
}

static void screensave_backend_registry_set_present_dc_gdi(
    screensave_renderer *renderer,
    HDC present_dc
)
{
    screensave_gdi_renderer_set_present_dc(renderer, present_dc);
}

static void screensave_backend_registry_clear_present_dc_gdi(screensave_renderer *renderer)
{
    screensave_gdi_renderer_clear_present_dc(renderer);
}

static int screensave_backend_registry_capture_caps_gdi(
    const screensave_renderer *renderer,
    screensave_backend_caps *caps_out
)
{
    screensave_backend_caps_init(
        caps_out,
        SCREENSAVE_BACKEND_KIND_GDI,
        SCREENSAVE_RENDER_BAND_UNIVERSAL
    );
    screensave_backend_caps_apply_renderer_info(caps_out, &renderer->info);
    return 1;
}

static int screensave_backend_registry_capture_caps_gl11(
    const screensave_renderer *renderer,
    screensave_backend_caps *caps_out
)
{
    screensave_gl11_state *state;

    screensave_backend_caps_init(
        caps_out,
        SCREENSAVE_BACKEND_KIND_GL11,
        SCREENSAVE_RENDER_BAND_COMPAT
    );
    screensave_backend_caps_apply_renderer_info(caps_out, &renderer->info);

    if (!screensave_gl11_state_from_renderer((screensave_renderer *)renderer, &state)) {
        return 0;
    }

    if (state->caps.double_buffered) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_DOUBLE_BUFFERED;
    }
    if (state->caps.support_gdi) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_SUPPORT_GDI;
    }
    if (state->caps.generic_format) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_GENERIC_FORMAT;
    }
    caps_out->rgba_bits = state->caps.rgba_bits;
    caps_out->depth_bits = state->caps.depth_bits;
    caps_out->major_version = state->caps.major_version;
    caps_out->minor_version = state->caps.minor_version;
    if (state->caps.version[0] != '\0') {
        lstrcpynA(caps_out->version, state->caps.version, (int)sizeof(caps_out->version));
    }
    return 1;
}

static int screensave_backend_registry_capture_caps_gl21(
    const screensave_renderer *renderer,
    screensave_backend_caps *caps_out
)
{
    screensave_gl21_state *state;

    screensave_backend_caps_init(
        caps_out,
        SCREENSAVE_BACKEND_KIND_GL21,
        SCREENSAVE_RENDER_BAND_ADVANCED
    );
    screensave_backend_caps_apply_renderer_info(caps_out, &renderer->info);

    if (!screensave_gl21_state_from_renderer((screensave_renderer *)renderer, &state)) {
        return 0;
    }

    if (state->caps.double_buffered) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_DOUBLE_BUFFERED;
    }
    if (state->caps.support_gdi) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_SUPPORT_GDI;
    }
    if (state->caps.generic_format) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_GENERIC_FORMAT;
    }
    if (state->caps.advanced_context) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_ADVANCED_CONTEXT;
    }
    if (state->caps.compatibility_profile) {
        caps_out->state_flags |= SCREENSAVE_BACKEND_CAPS_STATE_COMPAT_PROFILE;
    }
    caps_out->private_capability_flags = state->caps.private_flags;
    caps_out->required_private_capability_flags = state->caps.bundle.required_flags;
    caps_out->preferred_private_capability_flags = state->caps.bundle.preferred_flags;
    caps_out->missing_private_capability_flags = state->caps.bundle.missing_required_flags;
    caps_out->rgba_bits = state->caps.rgba_bits;
    caps_out->depth_bits = state->caps.depth_bits;
    caps_out->major_version = state->caps.major_version;
    caps_out->minor_version = state->caps.minor_version;
    if (state->caps.vendor[0] != '\0') {
        lstrcpynA(caps_out->vendor, state->caps.vendor, (int)sizeof(caps_out->vendor));
    }
    if (state->caps.renderer[0] != '\0') {
        lstrcpynA(caps_out->renderer, state->caps.renderer, (int)sizeof(caps_out->renderer));
    }
    if (state->caps.version[0] != '\0') {
        lstrcpynA(caps_out->version, state->caps.version, (int)sizeof(caps_out->version));
    }
    return 1;
}

static int screensave_backend_registry_resize_gl11(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
)
{
    return screensave_gl11_renderer_resize(renderer, drawable_size);
}

static void screensave_backend_registry_set_present_dc_gl11(
    screensave_renderer *renderer,
    HDC present_dc
)
{
    screensave_gl11_renderer_set_present_dc(renderer, present_dc);
}

static void screensave_backend_registry_clear_present_dc_gl11(screensave_renderer *renderer)
{
    screensave_gl11_renderer_clear_present_dc(renderer);
}

static int screensave_backend_registry_resize_gl21(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
)
{
    return screensave_gl21_renderer_resize(renderer, drawable_size);
}

static void screensave_backend_registry_set_present_dc_gl21(
    screensave_renderer *renderer,
    HDC present_dc
)
{
    screensave_gl21_renderer_set_present_dc(renderer, present_dc);
}

static void screensave_backend_registry_clear_present_dc_gl21(screensave_renderer *renderer)
{
    screensave_gl21_renderer_clear_present_dc(renderer);
}

static int screensave_backend_registry_resize_null(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
)
{
    return screensave_null_renderer_resize(renderer, drawable_size);
}

static int screensave_backend_registry_create_null(
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    (void)target_window;

    return screensave_null_renderer_create(
        drawable_size,
        diagnostics,
        renderer_out,
        failure_reason_out
    );
}

static void screensave_backend_registry_set_present_dc_null(
    screensave_renderer *renderer,
    HDC present_dc
)
{
    screensave_null_renderer_set_present_dc(renderer, present_dc);
}

static void screensave_backend_registry_clear_present_dc_null(screensave_renderer *renderer)
{
    screensave_null_renderer_clear_present_dc(renderer);
}

static int screensave_backend_registry_capture_caps_null(
    const screensave_renderer *renderer,
    screensave_backend_caps *caps_out
)
{
    screensave_backend_caps_init(
        caps_out,
        SCREENSAVE_BACKEND_KIND_NULL,
        SCREENSAVE_RENDER_BAND_UNKNOWN
    );
    screensave_backend_caps_apply_renderer_info(caps_out, &renderer->info);
    return 1;
}

static const screensave_backend_descriptor g_screensave_backend_descriptors[] = {
    {
        SCREENSAVE_BACKEND_KIND_GDI,
        SCREENSAVE_RENDERER_KIND_GDI,
        SCREENSAVE_RENDER_BAND_UNIVERSAL,
        "gdi",
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_ACCEPTS_EXTERNAL_DC,
        screensave_backend_registry_create_gdi,
        screensave_backend_registry_resize_gdi,
        screensave_backend_registry_set_present_dc_gdi,
        screensave_backend_registry_clear_present_dc_gdi,
        screensave_backend_registry_capture_caps_gdi
    },
    {
        SCREENSAVE_BACKEND_KIND_GL11,
        SCREENSAVE_RENDERER_KIND_GL11,
        SCREENSAVE_RENDER_BAND_COMPAT,
        "gl11",
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_USES_WINDOW_DC,
        screensave_gl11_renderer_create,
        screensave_backend_registry_resize_gl11,
        screensave_backend_registry_set_present_dc_gl11,
        screensave_backend_registry_clear_present_dc_gl11,
        screensave_backend_registry_capture_caps_gl11
    },
    {
        SCREENSAVE_BACKEND_KIND_GL21,
        SCREENSAVE_RENDERER_KIND_GL21,
        SCREENSAVE_RENDER_BAND_ADVANCED,
        "gl21",
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_USES_WINDOW_DC,
        screensave_gl21_renderer_create,
        screensave_backend_registry_resize_gl21,
        screensave_backend_registry_set_present_dc_gl21,
        screensave_backend_registry_clear_present_dc_gl21,
        screensave_backend_registry_capture_caps_gl21
    },
    {
        SCREENSAVE_BACKEND_KIND_GL33,
        SCREENSAVE_RENDERER_KIND_GL33,
        SCREENSAVE_RENDER_BAND_MODERN,
        "gl33",
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_PLACEHOLDER |
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_USES_WINDOW_DC,
        screensave_gl33_renderer_create,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        SCREENSAVE_BACKEND_KIND_GL46,
        SCREENSAVE_RENDERER_KIND_GL46,
        SCREENSAVE_RENDER_BAND_PREMIUM,
        "gl46",
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_PLACEHOLDER |
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_USES_WINDOW_DC,
        screensave_gl46_renderer_create,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        SCREENSAVE_BACKEND_KIND_NULL,
        SCREENSAVE_RENDERER_KIND_NULL,
        SCREENSAVE_RENDER_BAND_UNKNOWN,
        "null",
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_ACCEPTS_EXTERNAL_DC |
        SCREENSAVE_BACKEND_DESCRIPTOR_FLAG_NOOP_PRESENT,
        screensave_backend_registry_create_null,
        screensave_backend_registry_resize_null,
        screensave_backend_registry_set_present_dc_null,
        screensave_backend_registry_clear_present_dc_null,
        screensave_backend_registry_capture_caps_null
    }
};

static const screensave_backend_kind g_screensave_backend_chain_auto[] = {
    SCREENSAVE_BACKEND_KIND_GL46,
    SCREENSAVE_BACKEND_KIND_GL33,
    SCREENSAVE_BACKEND_KIND_GL21,
    SCREENSAVE_BACKEND_KIND_GL11,
    SCREENSAVE_BACKEND_KIND_GDI,
    SCREENSAVE_BACKEND_KIND_NULL
};

static const screensave_backend_kind g_screensave_backend_chain_gdi[] = {
    SCREENSAVE_BACKEND_KIND_GDI,
    SCREENSAVE_BACKEND_KIND_NULL
};

static const screensave_backend_kind g_screensave_backend_chain_gl11[] = {
    SCREENSAVE_BACKEND_KIND_GL11,
    SCREENSAVE_BACKEND_KIND_GDI,
    SCREENSAVE_BACKEND_KIND_NULL
};

static const screensave_backend_kind g_screensave_backend_chain_gl21[] = {
    SCREENSAVE_BACKEND_KIND_GL21,
    SCREENSAVE_BACKEND_KIND_GL11,
    SCREENSAVE_BACKEND_KIND_GDI,
    SCREENSAVE_BACKEND_KIND_NULL
};

static const screensave_backend_kind g_screensave_backend_chain_gl33[] = {
    SCREENSAVE_BACKEND_KIND_GL33,
    SCREENSAVE_BACKEND_KIND_GL21,
    SCREENSAVE_BACKEND_KIND_GL11,
    SCREENSAVE_BACKEND_KIND_GDI,
    SCREENSAVE_BACKEND_KIND_NULL
};

static const screensave_backend_kind g_screensave_backend_chain_gl46[] = {
    SCREENSAVE_BACKEND_KIND_GL46,
    SCREENSAVE_BACKEND_KIND_GL33,
    SCREENSAVE_BACKEND_KIND_GL21,
    SCREENSAVE_BACKEND_KIND_GL11,
    SCREENSAVE_BACKEND_KIND_GDI,
    SCREENSAVE_BACKEND_KIND_NULL
};

static const screensave_backend_kind g_screensave_backend_chain_null[] = {
    SCREENSAVE_BACKEND_KIND_NULL
};

const screensave_backend_descriptor *screensave_backend_registry_get(
    screensave_backend_kind backend_kind
)
{
    unsigned int index;

    for (index = 0U; index < sizeof(g_screensave_backend_descriptors) / sizeof(g_screensave_backend_descriptors[0]); ++index) {
        if (g_screensave_backend_descriptors[index].backend_kind == backend_kind) {
            return &g_screensave_backend_descriptors[index];
        }
    }

    return NULL;
}

const screensave_backend_kind *screensave_backend_registry_chain_for_request(
    screensave_renderer_kind requested_kind,
    unsigned int *count_out
)
{
    if (count_out == NULL) {
        return NULL;
    }

    switch (requested_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        *count_out =
            sizeof(g_screensave_backend_chain_gdi) /
            sizeof(g_screensave_backend_chain_gdi[0]);
        return g_screensave_backend_chain_gdi;

    case SCREENSAVE_RENDERER_KIND_GL11:
        *count_out =
            sizeof(g_screensave_backend_chain_gl11) /
            sizeof(g_screensave_backend_chain_gl11[0]);
        return g_screensave_backend_chain_gl11;

    case SCREENSAVE_RENDERER_KIND_GL21:
        *count_out =
            sizeof(g_screensave_backend_chain_gl21) /
            sizeof(g_screensave_backend_chain_gl21[0]);
        return g_screensave_backend_chain_gl21;

    case SCREENSAVE_RENDERER_KIND_GL33:
        *count_out =
            sizeof(g_screensave_backend_chain_gl33) /
            sizeof(g_screensave_backend_chain_gl33[0]);
        return g_screensave_backend_chain_gl33;

    case SCREENSAVE_RENDERER_KIND_GL46:
        *count_out =
            sizeof(g_screensave_backend_chain_gl46) /
            sizeof(g_screensave_backend_chain_gl46[0]);
        return g_screensave_backend_chain_gl46;

    case SCREENSAVE_RENDERER_KIND_NULL:
        *count_out =
            sizeof(g_screensave_backend_chain_null) /
            sizeof(g_screensave_backend_chain_null[0]);
        return g_screensave_backend_chain_null;

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        *count_out =
            sizeof(g_screensave_backend_chain_auto) /
            sizeof(g_screensave_backend_chain_auto[0]);
        return g_screensave_backend_chain_auto;
    }
}
