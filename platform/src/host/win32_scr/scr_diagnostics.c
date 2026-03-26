#include "scr_internal.h"
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
    scr_append_text(buffer, buffer_size, "Shared diagnostics, common config, the saver/module runtime, and the baseline GDI renderer are active.");
}

void scr_build_overlay_text(const scr_host_context *context, char *buffer, int buffer_size)
{
    screensave_renderer_info renderer_info;

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
    scr_append_text(buffer, buffer_size, "\r\nDetail: ");
    scr_append_text(buffer, buffer_size, screensave_detail_level_name(context->settings.common.detail_level));
    scr_append_text(buffer, buffer_size, "\r\nSeed: ");
    if (context->settings.common.use_deterministic_seed) {
        if (context->settings.common.deterministic_seed != 0UL) {
            scr_append_text(buffer, buffer_size, "fixed");
        } else {
            scr_append_text(buffer, buffer_size, "module-default");
        }
    } else {
        scr_append_text(buffer, buffer_size, "session");
    }
    if (context->settings.common.preset_key != NULL) {
        scr_append_text(buffer, buffer_size, "\r\nPreset: ");
        scr_append_text(buffer, buffer_size, context->settings.common.preset_key);
    }
    if (context->settings.common.theme_key != NULL) {
        scr_append_text(buffer, buffer_size, "\r\nTheme: ");
        scr_append_text(buffer, buffer_size, context->settings.common.theme_key);
    }
    if (context->renderer != NULL) {
        screensave_renderer_get_info(context->renderer, &renderer_info);
        scr_append_text(buffer, buffer_size, "\r\nRenderer: ");
        scr_append_text(buffer, buffer_size, screensave_renderer_kind_name(renderer_info.active_kind));
        if (renderer_info.backend_name != NULL) {
            scr_append_text(buffer, buffer_size, " ");
            scr_append_text(buffer, buffer_size, renderer_info.backend_name);
        }
        scr_append_text(buffer, buffer_size, "\r\nBackbuffer: ");
        scr_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.width);
        scr_append_text(buffer, buffer_size, "x");
        scr_append_number(buffer, buffer_size, (unsigned long)renderer_info.drawable_size.height);
        if (renderer_info.status_text != NULL) {
            scr_append_text(buffer, buffer_size, "\r\nRender status: ");
            scr_append_text(buffer, buffer_size, renderer_info.status_text);
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
