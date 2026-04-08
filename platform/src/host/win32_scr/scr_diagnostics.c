#include "scr_internal.h"
#include "screensave/private/renderer_runtime.h"
#include "screensave/version.h"

int scr_append_text(char *buffer, int buffer_size, const char *text)
{
    int used;
    int added;

    if (buffer == NULL || text == NULL || buffer_size <= 0) {
        return 0;
    }

    used = lstrlenA(buffer);
    added = lstrlenA(text);
    if (used + added + 1 > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

int scr_append_number(char *buffer, int buffer_size, unsigned long value)
{
    char text[32];

    wsprintfA(text, "%lu", value);
    return scr_append_text(buffer, buffer_size, text);
}

void scr_emit_host_diagnostic(
    scr_host_context *context,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    if (context == NULL) {
        return;
    }

    screensave_diag_emit(
        &context->diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_HOST,
        code,
        origin,
        text
    );
}

const char *scr_mode_label(screensave_session_mode mode)
{
    switch (mode) {
    case SCREENSAVE_SESSION_MODE_SCREEN:
        return "screen saver";

    case SCREENSAVE_SESSION_MODE_PREVIEW:
        return "preview";

    case SCREENSAVE_SESSION_MODE_WINDOWED:
        return "windowed";

    case SCREENSAVE_SESSION_MODE_CONFIG:
    default:
        return "configuration";
    }
}

void scr_build_version_text(const scr_host_context *context, char *buffer, int buffer_size)
{
    const screensave_version_info *version_info;

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    buffer[0] = '\0';
    if (context == NULL) {
        return;
    }

    version_info = screensave_version_get_info();

    scr_append_text(buffer, buffer_size, context->module->identity.display_name);
    scr_append_text(buffer, buffer_size, "\r\n");
    scr_append_text(buffer, buffer_size, "ScreenSave ");
    scr_append_text(buffer, buffer_size, version_info->version_text);
    scr_append_text(buffer, buffer_size, "\r\n");
    scr_append_text(buffer, buffer_size, version_info->series_text);
    scr_append_text(buffer, buffer_size, "\r\n");
    scr_append_text(
        buffer,
        buffer_size,
        "Shared diagnostics, common config, the saver/module runtime, the mandatory GDI renderer, the optional GL11 tier, the optional GL21 tier, the optional GL33 modern tier, the optional GL46 premium tier, and the internal null safety fallback are active."
    );
}

void scr_build_overlay_text(const scr_host_context *context, char *buffer, int buffer_size)
{
    screensave_renderer_info renderer_info;
    screensave_service_seams service_seams;
    char reason_text[128];
    char seam_text[192];
    char status_text[128];

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    buffer[0] = '\0';
    if (context == NULL) {
        return;
    }

    scr_append_text(buffer, buffer_size, context->module->identity.display_name);
    scr_append_text(buffer, buffer_size, "\r\nMode: ");
    scr_append_text(buffer, buffer_size, scr_mode_label(context->mode));
    scr_append_text(buffer, buffer_size, "\r\nRuntime: ");
    scr_append_text(buffer, buffer_size, screensave_version_get_text());
    scr_append_text(buffer, buffer_size, "\r\nUptime: ");
    scr_append_number(buffer, buffer_size, context->clock.elapsed_millis / 1000UL);
    scr_append_text(buffer, buffer_size, "s");
    scr_append_text(buffer, buffer_size, "\r\nDetail level: ");
    scr_append_text(
        buffer,
        buffer_size,
        screensave_display_detail_level(context->resolved_settings.common.detail_level)
    );
    scr_append_text(buffer, buffer_size, "\r\nSeed: ");
    if (context->resolved_settings.common.use_deterministic_seed) {
        if (context->resolved_settings.common.deterministic_seed != 0UL) {
            scr_append_text(buffer, buffer_size, "fixed");
        } else {
            scr_append_text(buffer, buffer_size, "module-default");
        }
    } else {
        scr_append_text(buffer, buffer_size, "session");
    }
    scr_append_text(buffer, buffer_size, "\r\nRandomization mode: ");
    scr_append_text(
        buffer,
        buffer_size,
        screensave_display_randomization_mode(context->resolved_settings.common.randomization_mode)
    );
    if (context->resolved_settings.common.preset_key != NULL) {
        scr_append_text(buffer, buffer_size, "\r\nPreset key: ");
        scr_append_text(buffer, buffer_size, context->resolved_settings.common.preset_key);
    }
    if (context->resolved_settings.common.theme_key != NULL) {
        scr_append_text(buffer, buffer_size, "\r\nTheme key: ");
        scr_append_text(buffer, buffer_size, context->resolved_settings.common.theme_key);
    }
    if (context->renderer != NULL) {
        screensave_renderer_get_info(context->renderer, &renderer_info);
        scr_append_text(buffer, buffer_size, "\r\nRenderer preference: ");
        scr_append_text(
            buffer,
            buffer_size,
            screensave_display_renderer_kind(renderer_info.requested_kind)
        );
        if (context->module != NULL) {
            scr_append_text(buffer, buffer_size, "\r\nRouting profile: min ");
            scr_append_text(
                buffer,
                buffer_size,
                screensave_display_renderer_kind(context->module->routing_policy.minimum_kind)
            );
            scr_append_text(buffer, buffer_size, " pref ");
            scr_append_text(
                buffer,
                buffer_size,
                screensave_display_renderer_kind(context->module->routing_policy.preferred_kind)
            );
            scr_append_text(buffer, buffer_size, " quality ");
            scr_append_text(
                buffer,
                buffer_size,
                screensave_capability_quality_name(context->module->routing_policy.quality_class)
            );
        }
        scr_append_text(buffer, buffer_size, "\r\nActive renderer: ");
        scr_append_text(
            buffer,
            buffer_size,
            screensave_display_renderer_kind(renderer_info.active_kind)
        );
        if (renderer_info.backend_name != NULL) {
            scr_append_text(buffer, buffer_size, "\r\nRenderer backend: ");
            scr_append_text(buffer, buffer_size, renderer_info.backend_name);
        }
        if (renderer_info.selection_reason != NULL) {
            screensave_display_renderer_reason(
                renderer_info.selection_reason,
                reason_text,
                sizeof(reason_text)
            );
            scr_append_text(buffer, buffer_size, "\r\nSelection path: ");
            scr_append_text(buffer, buffer_size, reason_text);
        }
        if (renderer_info.fallback_reason != NULL) {
            screensave_display_renderer_reason(
                renderer_info.fallback_reason,
                reason_text,
                sizeof(reason_text)
            );
            scr_append_text(buffer, buffer_size, "\r\nFallback cause: ");
            scr_append_text(buffer, buffer_size, reason_text);
        }
        if (renderer_info.vendor_name != NULL) {
            scr_append_text(buffer, buffer_size, "\r\nBackend vendor: ");
            scr_append_text(buffer, buffer_size, renderer_info.vendor_name);
        }
        if (renderer_info.renderer_name != NULL) {
            scr_append_text(buffer, buffer_size, "\r\nBackend renderer: ");
            scr_append_text(buffer, buffer_size, renderer_info.renderer_name);
        }
        if (renderer_info.version_name != NULL) {
            scr_append_text(buffer, buffer_size, "\r\nBackend detail: ");
            scr_append_text(buffer, buffer_size, renderer_info.version_name);
        }
        scr_append_text(buffer, buffer_size, "\r\nBackbuffer: ");
        scr_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.width);
        scr_append_text(buffer, buffer_size, "x");
        scr_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.height);
        if (renderer_info.status_text != NULL) {
            screensave_display_renderer_status(
                renderer_info.status_text,
                status_text,
                sizeof(status_text)
            );
            scr_append_text(buffer, buffer_size, "\r\nRenderer status: ");
            scr_append_text(buffer, buffer_size, status_text);
        }
        if (
            screensave_renderer_get_private_service_seams(context->renderer, &service_seams) &&
            screensave_service_seams_build_summary(
                &service_seams,
                seam_text,
                sizeof(seam_text)
            )
        ) {
            scr_append_text(buffer, buffer_size, "\r\nService seams: ");
            scr_append_text(buffer, buffer_size, seam_text);
        }
    }
    if (context->session == NULL) {
        scr_append_text(buffer, buffer_size, "\r\nScene: validation fallback");
    }
    if (context->diagnostics.last_text[0] != '\0') {
        scr_append_text(buffer, buffer_size, "\r\nLast diag: ");
        scr_append_text(buffer, buffer_size, screensave_diag_level_name(context->diagnostics.last_level));
        scr_append_text(buffer, buffer_size, "/");
        scr_append_text(buffer, buffer_size, screensave_diag_domain_name(context->diagnostics.last_domain));
        scr_append_text(buffer, buffer_size, " ");
        scr_append_text(buffer, buffer_size, context->diagnostics.last_text);
    }
}

void scr_show_message_box(HWND owner, const screensave_saver_module *module, const char *text, UINT type)
{
    char title[96];

    title[0] = '\0';
    if (module != NULL && module->identity.display_name != NULL) {
        scr_append_text(title, sizeof(title), module->identity.display_name);
        scr_append_text(title, sizeof(title), " - ScreenSave");
    } else {
        scr_append_text(title, sizeof(title), "ScreenSave");
    }

    MessageBoxA(owner, text, title, type | MB_SETFOREGROUND);
}
