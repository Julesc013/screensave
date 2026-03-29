#include <string.h>

#include "renderer_private.h"
#include "../../render/gdi/gdi_internal.h"
#include "../../render/gl11/gl11_internal.h"
#include "../../render/gl21/gl21_internal.h"
#include "../../render/gl33/gl33_internal.h"
#include "../../render/gl46/gl46_internal.h"
#include "../../render/null/null_internal.h"

static const screensave_renderer_kind g_screensave_renderer_chain_auto[] = {
    SCREENSAVE_RENDERER_KIND_GL46,
    SCREENSAVE_RENDERER_KIND_GL33,
    SCREENSAVE_RENDERER_KIND_GL21,
    SCREENSAVE_RENDERER_KIND_GL11,
    SCREENSAVE_RENDERER_KIND_GDI,
    SCREENSAVE_RENDERER_KIND_NULL
};

static const screensave_renderer_kind g_screensave_renderer_chain_gdi[] = {
    SCREENSAVE_RENDERER_KIND_GDI,
    SCREENSAVE_RENDERER_KIND_NULL
};

static const screensave_renderer_kind g_screensave_renderer_chain_gl11[] = {
    SCREENSAVE_RENDERER_KIND_GL11,
    SCREENSAVE_RENDERER_KIND_GDI,
    SCREENSAVE_RENDERER_KIND_NULL
};

static const screensave_renderer_kind g_screensave_renderer_chain_gl21[] = {
    SCREENSAVE_RENDERER_KIND_GL21,
    SCREENSAVE_RENDERER_KIND_GL11,
    SCREENSAVE_RENDERER_KIND_GDI,
    SCREENSAVE_RENDERER_KIND_NULL
};

static const screensave_renderer_kind g_screensave_renderer_chain_gl33[] = {
    SCREENSAVE_RENDERER_KIND_GL33,
    SCREENSAVE_RENDERER_KIND_GL21,
    SCREENSAVE_RENDERER_KIND_GL11,
    SCREENSAVE_RENDERER_KIND_GDI,
    SCREENSAVE_RENDERER_KIND_NULL
};

static const screensave_renderer_kind g_screensave_renderer_chain_gl46[] = {
    SCREENSAVE_RENDERER_KIND_GL46,
    SCREENSAVE_RENDERER_KIND_GL33,
    SCREENSAVE_RENDERER_KIND_GL21,
    SCREENSAVE_RENDERER_KIND_GL11,
    SCREENSAVE_RENDERER_KIND_GDI,
    SCREENSAVE_RENDERER_KIND_NULL
};

static const screensave_renderer_kind g_screensave_renderer_chain_null[] = {
    SCREENSAVE_RENDERER_KIND_NULL
};

static void screensave_renderer_emit_create_diag(
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
        "renderer_dispatch",
        text
    );
}

static void screensave_renderer_zero_info(screensave_renderer_info *info)
{
    memset(info, 0, sizeof(*info));
    info->requested_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
    info->active_kind = SCREENSAVE_RENDERER_KIND_UNKNOWN;
}

static void screensave_renderer_apply_selection_info(
    screensave_renderer *renderer,
    screensave_renderer_kind requested_kind,
    const char *selection_reason,
    const char *fallback_reason,
    const char *status_text
)
{
    if (renderer == NULL) {
        return;
    }

    renderer->info.requested_kind = requested_kind;
    renderer->info.selection_reason = selection_reason;
    renderer->info.fallback_reason = fallback_reason;
    if (status_text != NULL) {
        renderer->info.status_text = status_text;
    }
}

static const screensave_renderer_kind *screensave_renderer_chain_for_request(
    screensave_renderer_kind requested_kind,
    unsigned int *count_out
)
{
    if (count_out == NULL) {
        return NULL;
    }

    switch (requested_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        *count_out = sizeof(g_screensave_renderer_chain_gdi) / sizeof(g_screensave_renderer_chain_gdi[0]);
        return g_screensave_renderer_chain_gdi;

    case SCREENSAVE_RENDERER_KIND_GL11:
        *count_out = sizeof(g_screensave_renderer_chain_gl11) / sizeof(g_screensave_renderer_chain_gl11[0]);
        return g_screensave_renderer_chain_gl11;

    case SCREENSAVE_RENDERER_KIND_GL21:
        *count_out = sizeof(g_screensave_renderer_chain_gl21) / sizeof(g_screensave_renderer_chain_gl21[0]);
        return g_screensave_renderer_chain_gl21;

    case SCREENSAVE_RENDERER_KIND_GL33:
        *count_out = sizeof(g_screensave_renderer_chain_gl33) / sizeof(g_screensave_renderer_chain_gl33[0]);
        return g_screensave_renderer_chain_gl33;

    case SCREENSAVE_RENDERER_KIND_GL46:
        *count_out = sizeof(g_screensave_renderer_chain_gl46) / sizeof(g_screensave_renderer_chain_gl46[0]);
        return g_screensave_renderer_chain_gl46;

    case SCREENSAVE_RENDERER_KIND_NULL:
        *count_out = sizeof(g_screensave_renderer_chain_null) / sizeof(g_screensave_renderer_chain_null[0]);
        return g_screensave_renderer_chain_null;

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        *count_out = sizeof(g_screensave_renderer_chain_auto) / sizeof(g_screensave_renderer_chain_auto[0]);
        return g_screensave_renderer_chain_auto;
    }
}

static const char *screensave_renderer_force_selection_reason(
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

static const char *screensave_renderer_auto_selection_reason(screensave_renderer_kind active_kind)
{
    switch (active_kind) {
    case SCREENSAVE_RENDERER_KIND_GL46:
        return "auto-prefer-gl46";
    case SCREENSAVE_RENDERER_KIND_GL33:
        return "auto-fallback-gl33";
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

static const char *screensave_renderer_fallback_status_text(screensave_renderer_kind active_kind)
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

static int screensave_renderer_try_create_backend(
    screensave_renderer_kind kind,
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out,
    const char **failure_reason_out
)
{
    if (renderer_out != NULL) {
        *renderer_out = NULL;
    }
    if (failure_reason_out != NULL) {
        *failure_reason_out = NULL;
    }

    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GL46:
        return screensave_gl46_renderer_create(
            target_window,
            drawable_size,
            diagnostics,
            renderer_out,
            failure_reason_out
        );

    case SCREENSAVE_RENDERER_KIND_GL33:
        return screensave_gl33_renderer_create(
            target_window,
            drawable_size,
            diagnostics,
            renderer_out,
            failure_reason_out
        );

    case SCREENSAVE_RENDERER_KIND_GL21:
        return screensave_gl21_renderer_create(
            target_window,
            drawable_size,
            diagnostics,
            renderer_out,
            failure_reason_out
        );

    case SCREENSAVE_RENDERER_KIND_GL11:
        return screensave_gl11_renderer_create(
            target_window,
            drawable_size,
            diagnostics,
            renderer_out,
            failure_reason_out
        );

    case SCREENSAVE_RENDERER_KIND_GDI:
        if (screensave_gdi_renderer_create(target_window, drawable_size, diagnostics, renderer_out)) {
            return 1;
        }
        if (failure_reason_out != NULL) {
            *failure_reason_out = "gdi-create-failed";
        }
        return 0;

    case SCREENSAVE_RENDERER_KIND_NULL:
        return screensave_null_renderer_create(
            drawable_size,
            diagnostics,
            renderer_out,
            failure_reason_out
        );

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        if (failure_reason_out != NULL) {
            *failure_reason_out = "renderer-kind-invalid";
        }
        return 0;
    }
}

static int screensave_renderer_select_from_chain(
    screensave_renderer_kind requested_kind,
    const screensave_renderer_kind *chain,
    unsigned int chain_count,
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out
)
{
    const char *last_failure_reason;
    const char *attempt_failure_reason;
    const char *selection_reason;
    const char *status_text;
    unsigned int index;
    screensave_renderer_kind active_kind;

    if (renderer_out == NULL || chain == NULL || chain_count == 0U) {
        return 0;
    }

    *renderer_out = NULL;
    last_failure_reason = NULL;

    for (index = 0U; index < chain_count; ++index) {
        active_kind = chain[index];
        attempt_failure_reason = NULL;
        if (
            screensave_renderer_try_create_backend(
                active_kind,
                target_window,
                drawable_size,
                diagnostics,
                renderer_out,
                &attempt_failure_reason
            )
        ) {
            selection_reason =
                requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN
                    ? screensave_renderer_auto_selection_reason(active_kind)
                    : screensave_renderer_force_selection_reason(requested_kind, active_kind);
            status_text = NULL;
            if (
                requested_kind == SCREENSAVE_RENDERER_KIND_UNKNOWN ||
                requested_kind != active_kind
            ) {
                status_text = screensave_renderer_fallback_status_text(active_kind);
            }

            screensave_renderer_apply_selection_info(
                *renderer_out,
                requested_kind,
                selection_reason,
                last_failure_reason,
                status_text
            );
            return 1;
        }

        if (attempt_failure_reason != NULL) {
            last_failure_reason = attempt_failure_reason;
        }
    }

    return 0;
}

void screensave_renderer_init_dispatch(
    screensave_renderer *renderer,
    const screensave_renderer_vtable *vtable,
    void *backend_state,
    const screensave_renderer_info *info
)
{
    if (renderer == NULL) {
        return;
    }

    renderer->vtable = vtable;
    renderer->backend_state = backend_state;
    if (info != NULL) {
        renderer->info = *info;
    } else {
        screensave_renderer_zero_info(&renderer->info);
    }
}

const char *screensave_renderer_kind_name(screensave_renderer_kind kind)
{
    switch (kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return "gdi";

    case SCREENSAVE_RENDERER_KIND_GL11:
        return "gl11";

    case SCREENSAVE_RENDERER_KIND_GL21:
        return "gl21";

    case SCREENSAVE_RENDERER_KIND_GL33:
        return "gl33";

    case SCREENSAVE_RENDERER_KIND_GL46:
        return "gl46";

    case SCREENSAVE_RENDERER_KIND_NULL:
        return "null";

    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return "unknown";
    }
}

int screensave_renderer_has_capability(unsigned long capability_flags, unsigned long capability_flag)
{
    return (capability_flags & capability_flag) == capability_flag;
}

int screensave_renderer_create_for_window(
    screensave_renderer_kind requested_kind,
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_renderer **renderer_out
)
{
    const screensave_renderer_kind *chain;
    unsigned int chain_count;

    if (renderer_out == NULL) {
        return 0;
    }

    *renderer_out = NULL;
    chain = screensave_renderer_chain_for_request(requested_kind, &chain_count);
    if (
        screensave_renderer_select_from_chain(
            requested_kind,
            chain,
            chain_count,
            target_window,
            drawable_size,
            diagnostics,
            renderer_out
        )
    ) {
        return 1;
    }

    screensave_renderer_emit_create_diag(
        diagnostics,
        SCREENSAVE_DIAG_LEVEL_ERROR,
        4101UL,
        "The renderer runtime could not create any renderer backend, including the null safety backend."
    );
    return 0;
}

int screensave_renderer_resize_for_window(screensave_renderer *renderer, const screensave_sizei *drawable_size)
{
    if (renderer == NULL) {
        return 0;
    }

    switch (renderer->info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        return screensave_gdi_renderer_resize(renderer, drawable_size);

    case SCREENSAVE_RENDERER_KIND_GL11:
        return screensave_gl11_renderer_resize(renderer, drawable_size);

    case SCREENSAVE_RENDERER_KIND_GL21:
        return screensave_gl21_renderer_resize(renderer, drawable_size);

    case SCREENSAVE_RENDERER_KIND_NULL:
        return screensave_null_renderer_resize(renderer, drawable_size);

    case SCREENSAVE_RENDERER_KIND_GL33:
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return 0;
    }
}

void screensave_renderer_set_present_dc(screensave_renderer *renderer, HDC present_dc)
{
    if (renderer == NULL) {
        return;
    }

    switch (renderer->info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        screensave_gdi_renderer_set_present_dc(renderer, present_dc);
        break;

    case SCREENSAVE_RENDERER_KIND_GL11:
        screensave_gl11_renderer_set_present_dc(renderer, present_dc);
        break;

    case SCREENSAVE_RENDERER_KIND_GL21:
        screensave_gl21_renderer_set_present_dc(renderer, present_dc);
        break;

    case SCREENSAVE_RENDERER_KIND_NULL:
        screensave_null_renderer_set_present_dc(renderer, present_dc);
        break;

    case SCREENSAVE_RENDERER_KIND_GL33:
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        break;
    }
}

void screensave_renderer_clear_present_dc(screensave_renderer *renderer)
{
    if (renderer == NULL) {
        return;
    }

    switch (renderer->info.active_kind) {
    case SCREENSAVE_RENDERER_KIND_GDI:
        screensave_gdi_renderer_clear_present_dc(renderer);
        break;

    case SCREENSAVE_RENDERER_KIND_GL11:
        screensave_gl11_renderer_clear_present_dc(renderer);
        break;

    case SCREENSAVE_RENDERER_KIND_GL21:
        screensave_gl21_renderer_clear_present_dc(renderer);
        break;

    case SCREENSAVE_RENDERER_KIND_NULL:
        screensave_null_renderer_clear_present_dc(renderer);
        break;

    case SCREENSAVE_RENDERER_KIND_GL33:
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        break;
    }
}

void screensave_renderer_get_info(const screensave_renderer *renderer, screensave_renderer_info *info)
{
    if (info == NULL) {
        return;
    }

    if (renderer == NULL) {
        screensave_renderer_zero_info(info);
        return;
    }

    *info = renderer->info;
}

int screensave_renderer_begin_frame(screensave_renderer *renderer, const screensave_frame_info *frame_info)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->begin_frame == NULL) {
        return 0;
    }

    return renderer->vtable->begin_frame(renderer, frame_info);
}

void screensave_renderer_clear(screensave_renderer *renderer, screensave_color color)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->clear == NULL) {
        return;
    }

    renderer->vtable->clear(renderer, color);
}

void screensave_renderer_fill_rect(screensave_renderer *renderer, const screensave_recti *rect, screensave_color color)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->fill_rect == NULL) {
        return;
    }

    renderer->vtable->fill_rect(renderer, rect, color);
}

void screensave_renderer_draw_frame_rect(screensave_renderer *renderer, const screensave_recti *rect, screensave_color color)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->draw_frame_rect == NULL) {
        return;
    }

    renderer->vtable->draw_frame_rect(renderer, rect, color);
}

void screensave_renderer_draw_line(
    screensave_renderer *renderer,
    const screensave_pointi *start_point,
    const screensave_pointi *end_point,
    screensave_color color
)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->draw_line == NULL) {
        return;
    }

    renderer->vtable->draw_line(renderer, start_point, end_point, color);
}

void screensave_renderer_draw_polyline(
    screensave_renderer *renderer,
    const screensave_pointi *points,
    unsigned int point_count,
    screensave_color color
)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->draw_polyline == NULL) {
        return;
    }

    renderer->vtable->draw_polyline(renderer, points, point_count, color);
}

int screensave_renderer_blit_bitmap(
    screensave_renderer *renderer,
    const screensave_bitmap_view *bitmap,
    const screensave_recti *destination_rect
)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->blit_bitmap == NULL) {
        return 0;
    }

    return renderer->vtable->blit_bitmap(renderer, bitmap, destination_rect);
}

int screensave_renderer_end_frame(screensave_renderer *renderer)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->end_frame == NULL) {
        return 0;
    }

    return renderer->vtable->end_frame(renderer);
}

void screensave_renderer_shutdown(screensave_renderer *renderer)
{
    if (renderer == NULL || renderer->vtable == NULL || renderer->vtable->shutdown == NULL) {
        return;
    }

    renderer->vtable->shutdown(renderer);
}
