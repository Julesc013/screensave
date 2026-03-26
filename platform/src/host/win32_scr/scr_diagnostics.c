#include "scr_internal.h"

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

const char *scr_mode_label(scr_run_mode mode)
{
    switch (mode) {
    case SCR_RUN_MODE_SCREEN:
        return "screen saver";
    case SCR_RUN_MODE_PREVIEW:
        return "preview";
    case SCR_RUN_MODE_CONFIG:
    default:
        return "configuration";
    }
}

void scr_build_version_text(const scr_host_context *context, char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    buffer[0] = '\0';
    if (context == NULL) {
        return;
    }

    scr_append_text(buffer, buffer_size, context->product.display_name);
    scr_append_text(buffer, buffer_size, "\r\n");
    scr_append_text(buffer, buffer_size, "Series 03 Win32 host skeleton");
    scr_append_text(buffer, buffer_size, "\r\n");
    scr_append_text(buffer, buffer_size, "Settings are stored per user as a provisional registry scaffold.");
}

void scr_build_overlay_text(const scr_host_context *context, char *buffer, int buffer_size)
{
    DWORD elapsed_millis;

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    buffer[0] = '\0';
    if (context == NULL) {
        return;
    }

    elapsed_millis = GetTickCount() - context->start_tick;

    scr_append_text(buffer, buffer_size, context->product.display_name);
    scr_append_text(buffer, buffer_size, "\r\nMode: ");
    scr_append_text(buffer, buffer_size, scr_mode_label(context->mode));
    scr_append_text(buffer, buffer_size, "\r\nUptime: ");
    scr_append_number(buffer, buffer_size, (unsigned long)(elapsed_millis / 1000UL));
    scr_append_text(buffer, buffer_size, "s");
    scr_append_text(buffer, buffer_size, "\r\nPlaceholder visual: ");
    scr_append_text(buffer, buffer_size, context->settings.placeholder_visual_enabled ? "on" : "off");
}

void scr_show_message_box(HWND owner, const screensave_product_identity *product, const char *text, UINT type)
{
    char title[96];

    title[0] = '\0';
    if (product != NULL && product->display_name != NULL) {
        scr_append_text(title, sizeof(title), product->display_name);
        scr_append_text(title, sizeof(title), " - ScreenSave");
    } else {
        scr_append_text(title, sizeof(title), "ScreenSave");
    }

    MessageBoxA(owner, text, title, type | MB_SETFOREGROUND);
}
