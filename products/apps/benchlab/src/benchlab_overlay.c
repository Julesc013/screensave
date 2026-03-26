#include "benchlab_internal.h"

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
    benchlab_append_text(buffer, buffer_size, "\r\nRequested renderer: ");
    benchlab_append_text(buffer, buffer_size, screensave_renderer_kind_name(app->requested_renderer_kind));

    if (app->renderer != NULL) {
        screensave_renderer_get_info(app->renderer, &renderer_info);
        benchlab_append_text(buffer, buffer_size, "\r\nActive renderer: ");
        benchlab_append_text(buffer, buffer_size, screensave_renderer_kind_name(renderer_info.active_kind));
        benchlab_append_text(buffer, buffer_size, "\r\nDrawable: ");
        benchlab_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.width);
        benchlab_append_text(buffer, buffer_size, "x");
        benchlab_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.height);
        if (renderer_info.status_text != NULL) {
            benchlab_append_text(buffer, buffer_size, "\r\nRender status: ");
            benchlab_append_text(buffer, buffer_size, renderer_info.status_text);
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

    if (app->saver_config.common.preset_key != NULL) {
        benchlab_append_text(buffer, buffer_size, "\r\nPreset: ");
        benchlab_append_text(buffer, buffer_size, app->saver_config.common.preset_key);
    }
    if (app->saver_config.common.theme_key != NULL) {
        benchlab_append_text(buffer, buffer_size, "\r\nTheme: ");
        benchlab_append_text(buffer, buffer_size, app->saver_config.common.theme_key);
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
    SetTextColor(dc, RGB(0, 64, 0));
    OffsetRect(&text_rect, 1, 1);
    DrawTextA(dc, overlay, -1, &text_rect, DT_LEFT | DT_NOPREFIX | DT_TOP);

    OffsetRect(&text_rect, -1, -1);
    SetTextColor(dc, RGB(0, 255, 0));
    DrawTextA(dc, overlay, -1, &text_rect, DT_LEFT | DT_NOPREFIX | DT_TOP);
}
