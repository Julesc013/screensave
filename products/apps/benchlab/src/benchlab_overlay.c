#include "benchlab_internal.h"

int anthology_session_build_summary(
    const screensave_saver_session *session,
    char *buffer,
    unsigned int buffer_size
);

static int benchlab_append_text(char *buffer, int buffer_size, const char *text)
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

static int benchlab_append_number(char *buffer, int buffer_size, unsigned long value)
{
    char text[32];

    wsprintfA(text, "%lu", value);
    return benchlab_append_text(buffer, buffer_size, text);
}

static int benchlab_append_hex(char *buffer, int buffer_size, unsigned long value)
{
    char text[32];

    wsprintfA(text, "0x%08lX", value);
    return benchlab_append_text(buffer, buffer_size, text);
}

static void benchlab_build_overlay_text(const benchlab_app *app, char *buffer, int buffer_size)
{
    screensave_renderer_info renderer_info;
    unsigned int index;
    char anthology_summary[512];
    char reason_text[128];
    char status_text[128];

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    buffer[0] = '\0';
    if (app == NULL || app->module == NULL) {
        return;
    }

    benchlab_append_text(buffer, buffer_size, "BenchLab");
    benchlab_append_text(buffer, buffer_size, "\r\nSaver: ");
    benchlab_append_text(buffer, buffer_size, app->module->identity.display_name);
    benchlab_append_text(buffer, buffer_size, "\r\nRuntime: ");
    benchlab_append_text(buffer, buffer_size, screensave_version_get_text());
    benchlab_append_text(buffer, buffer_size, "\r\nRenderer preference: ");
    benchlab_append_text(
        buffer,
        buffer_size,
        screensave_display_renderer_kind(app->requested_renderer_kind)
    );

    if (app->renderer != NULL) {
        screensave_renderer_get_info(app->renderer, &renderer_info);
        benchlab_append_text(buffer, buffer_size, "\r\nActive renderer: ");
        benchlab_append_text(
            buffer,
            buffer_size,
            screensave_display_renderer_kind(renderer_info.active_kind)
        );
        if (renderer_info.selection_reason != NULL) {
            screensave_display_renderer_reason(
                renderer_info.selection_reason,
                reason_text,
                sizeof(reason_text)
            );
            benchlab_append_text(buffer, buffer_size, "\r\nSelection path: ");
            benchlab_append_text(buffer, buffer_size, reason_text);
        }
        if (renderer_info.fallback_reason != NULL) {
            screensave_display_renderer_reason(
                renderer_info.fallback_reason,
                reason_text,
                sizeof(reason_text)
            );
            benchlab_append_text(buffer, buffer_size, "\r\nFallback cause: ");
            benchlab_append_text(buffer, buffer_size, reason_text);
        }
        if (renderer_info.vendor_name != NULL) {
            benchlab_append_text(buffer, buffer_size, "\r\nGL vendor: ");
            benchlab_append_text(buffer, buffer_size, renderer_info.vendor_name);
        }
        if (renderer_info.renderer_name != NULL) {
            benchlab_append_text(buffer, buffer_size, "\r\nGL renderer: ");
            benchlab_append_text(buffer, buffer_size, renderer_info.renderer_name);
        }
        if (renderer_info.version_name != NULL) {
            benchlab_append_text(buffer, buffer_size, "\r\nGL version: ");
            benchlab_append_text(buffer, buffer_size, renderer_info.version_name);
        }
        benchlab_append_text(buffer, buffer_size, "\r\nDrawable: ");
        benchlab_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.width);
        benchlab_append_text(buffer, buffer_size, "x");
        benchlab_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.height);
        if (renderer_info.status_text != NULL) {
            screensave_display_renderer_status(
                renderer_info.status_text,
                status_text,
                sizeof(status_text)
            );
            benchlab_append_text(buffer, buffer_size, "\r\nRenderer status: ");
            benchlab_append_text(buffer, buffer_size, status_text);
        }
    }

    benchlab_append_text(buffer, buffer_size, "\r\nRun mode: ");
    if (app->app_config.deterministic_mode) {
        benchlab_append_text(buffer, buffer_size, "deterministic");
    } else {
        benchlab_append_text(buffer, buffer_size, "session");
    }
    if (app->paused) {
        benchlab_append_text(buffer, buffer_size, " paused");
    } else {
        benchlab_append_text(buffer, buffer_size, " running");
    }

    benchlab_append_text(buffer, buffer_size, "\r\nSeed: ");
    benchlab_append_hex(buffer, buffer_size, app->current_base_seed);
    benchlab_append_text(buffer, buffer_size, "\r\nFrame: ");
    benchlab_append_number(buffer, buffer_size, app->clock.frame_index);
    benchlab_append_text(buffer, buffer_size, " elapsed=");
    benchlab_append_number(buffer, buffer_size, app->clock.elapsed_millis);
    benchlab_append_text(buffer, buffer_size, "ms delta=");
    benchlab_append_number(buffer, buffer_size, app->clock.delta_millis);
    benchlab_append_text(buffer, buffer_size, "ms");
    benchlab_append_text(buffer, buffer_size, "\r\nDetail level: ");
    benchlab_append_text(
        buffer,
        buffer_size,
        screensave_display_detail_level(app->resolved_config.common.detail_level)
    );

    if (app->resolved_config.common.preset_key != NULL) {
        benchlab_append_text(buffer, buffer_size, "\r\nPreset key: ");
        benchlab_append_text(buffer, buffer_size, app->resolved_config.common.preset_key);
    }
    if (app->resolved_config.common.theme_key != NULL) {
        benchlab_append_text(buffer, buffer_size, "\r\nTheme key: ");
        benchlab_append_text(buffer, buffer_size, app->resolved_config.common.theme_key);
    }
    benchlab_append_text(buffer, buffer_size, "\r\nRandomization mode: ");
    benchlab_append_text(
        buffer,
        buffer_size,
        screensave_display_randomization_mode(app->resolved_config.common.randomization_mode)
    );

    if (
        app->session != NULL &&
        app->module != NULL &&
        app->module->identity.product_key != NULL &&
        lstrcmpiA(app->module->identity.product_key, "anthology") == 0 &&
        anthology_session_build_summary(app->session, anthology_summary, sizeof(anthology_summary))
    ) {
        benchlab_append_text(buffer, buffer_size, "\r\n");
        benchlab_append_text(buffer, buffer_size, anthology_summary);
    }

    benchlab_append_text(buffer, buffer_size, "\r\nDiag: emitted=");
    benchlab_append_number(buffer, buffer_size, app->diagnostics.emitted_count);
    benchlab_append_text(buffer, buffer_size, " dropped=");
    benchlab_append_number(buffer, buffer_size, app->diagnostics.dropped_count);

    for (index = 0U; index < 3U; ++index) {
        const char *line;

        line = benchlab_diag_get_recent(&app->diag_buffer, index);
        if (line[0] == '\0') {
            continue;
        }

        benchlab_append_text(buffer, buffer_size, "\r\n");
        benchlab_append_text(buffer, buffer_size, line);
    }

    benchlab_append_text(buffer, buffer_size, "\r\nKeys: F5 restart F6 reseed F7 deterministic F8 pause F9 step");
}

void benchlab_draw_overlay(HDC dc, const RECT *client_rect, const benchlab_app *app)
{
    RECT text_rect;
    char overlay[1536];

    if (dc == NULL || client_rect == NULL || app == NULL) {
        return;
    }

    benchlab_build_overlay_text(app, overlay, sizeof(overlay));
    text_rect = *client_rect;
    text_rect.left += 8;
    text_rect.top += 8;

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(0, 255, 0));
    DrawTextA(dc, overlay, -1, &text_rect, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK);
}
