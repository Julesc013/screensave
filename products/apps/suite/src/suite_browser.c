#include "suite_internal.h"

static int suite_browser_text_copy(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int text_length;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return 1;
    }

    text_length = (unsigned int)lstrlenA(text);
    if (text_length + 1U > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer, text);
    return 1;
}

static int suite_browser_append_text(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int used;
    unsigned int added;

    if (buffer == NULL || text == NULL || buffer_size == 0U) {
        return 0;
    }

    used = (unsigned int)lstrlenA(buffer);
    added = (unsigned int)lstrlenA(text);
    if (used + added + 1U > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

static void suite_browser_append_line(
    char *buffer,
    unsigned int buffer_size,
    const char *label,
    const char *value
)
{
    if (buffer == NULL || label == NULL) {
        return;
    }

    if (buffer[0] != '\0') {
        suite_browser_append_text(buffer, buffer_size, "\r\n");
    }
    suite_browser_append_text(buffer, buffer_size, label);
    suite_browser_append_text(buffer, buffer_size, ": ");
    suite_browser_append_text(buffer, buffer_size, value != NULL && value[0] != '\0' ? value : "(none)");
}

static void suite_browser_append_flag_line(
    char *buffer,
    unsigned int buffer_size,
    const char *label,
    int enabled
)
{
    suite_browser_append_line(buffer, buffer_size, label, enabled ? "yes" : "no");
}

static void suite_browser_append_unsigned_line(
    char *buffer,
    unsigned int buffer_size,
    const char *label,
    unsigned long value
)
{
    char value_text[32];

    wsprintfA(value_text, "%lu", value);
    suite_browser_append_line(buffer, buffer_size, label, value_text);
}

static const char *suite_browser_renderer_request_label(const suite_app *app)
{
    if (app == NULL) {
        return "Auto";
    }

    return screensave_display_renderer_kind((screensave_renderer_kind)app->app_config.renderer_request);
}

static const char *suite_browser_preset_label(
    const suite_catalog_entry *entry,
    const char *preset_key
)
{
    const screensave_preset_descriptor *preset_descriptor;

    if (
        entry == NULL ||
        entry->module == NULL ||
        preset_key == NULL ||
        preset_key[0] == '\0'
    ) {
        return NULL;
    }

    preset_descriptor = screensave_find_preset(
        entry->module->presets,
        entry->module->preset_count,
        preset_key
    );
    if (
        preset_descriptor != NULL &&
        preset_descriptor->display_name != NULL &&
        preset_descriptor->display_name[0] != '\0'
    ) {
        return preset_descriptor->display_name;
    }

    return preset_key;
}

static const char *suite_browser_theme_label(
    const suite_catalog_entry *entry,
    const char *theme_key
)
{
    const screensave_theme_descriptor *theme_descriptor;

    if (
        entry == NULL ||
        entry->module == NULL ||
        theme_key == NULL ||
        theme_key[0] == '\0'
    ) {
        return NULL;
    }

    theme_descriptor = screensave_find_theme(
        entry->module->themes,
        entry->module->theme_count,
        theme_key
    );
    if (
        theme_descriptor != NULL &&
        theme_descriptor->display_name != NULL &&
        theme_descriptor->display_name[0] != '\0'
    ) {
        return theme_descriptor->display_name;
    }

    return theme_key;
}

static void suite_browser_pack_summary(
    const suite_catalog_entry *entry,
    char *buffer,
    unsigned int buffer_size
)
{
    unsigned int index;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (entry == NULL || entry->pack_count == 0U) {
        suite_browser_text_copy(buffer, buffer_size, "none detected");
        return;
    }

    for (index = 0U; index < entry->pack_count && index < SUITE_PACK_LIMIT; ++index) {
        if (index > 0U) {
            suite_browser_append_text(buffer, buffer_size, ", ");
        }
        if (entry->packs[index].display_name[0] != '\0') {
            suite_browser_append_text(buffer, buffer_size, entry->packs[index].display_name);
        } else {
            suite_browser_append_text(buffer, buffer_size, entry->packs[index].pack_key);
        }
    }
}

void suite_populate_browser(suite_app *app)
{
    unsigned int index;
    LRESULT list_index;

    if (app == NULL || app->browser_window == NULL) {
        return;
    }

    SendMessageA(app->browser_window, LB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < app->catalog_count; ++index) {
        list_index = SendMessageA(
            app->browser_window,
            LB_ADDSTRING,
            0U,
            (LPARAM)app->catalog[index].browser_label
        );
        if (list_index >= 0L) {
            SendMessageA(
                app->browser_window,
                LB_SETITEMDATA,
                (WPARAM)list_index,
                (LPARAM)index
            );
        }
    }

    if (app->selected_index < app->catalog_count) {
        SendMessageA(app->browser_window, LB_SETCURSEL, (WPARAM)app->selected_index, 0L);
    }
}

void suite_draw_info(HDC dc, const RECT *client_rect, const suite_app *app)
{
    const suite_catalog_entry *entry;
    screensave_renderer_info renderer_info;
    char info_text[SUITE_INFO_TEXT_LENGTH];
    char pack_text[256];
    RECT draw_rect;
    HGDIOBJ previous_font;
    const char *summary_text;
    const char *preset_key;
    const char *theme_key;
    const char *selection_reason;
    const char *fallback_reason;
    char selection_text[128];
    char fallback_text[128];
    char status_text[128];

    if (dc == NULL || client_rect == NULL) {
        return;
    }

    FillRect(dc, client_rect, (HBRUSH)(COLOR_BTNFACE + 1));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(16, 16, 16));
    previous_font = SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));

    draw_rect = *client_rect;
    InflateRect(&draw_rect, -8, -8);
    info_text[0] = '\0';

    entry = suite_get_selected_entry(app);
    if (entry == NULL || entry->module == NULL) {
        suite_browser_text_copy(
            info_text,
            sizeof(info_text),
            "Suite browser ready.\r\nNo saver is currently selected."
        );
        DrawTextA(dc, info_text, -1, &draw_rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
        SelectObject(dc, previous_font);
        return;
    }

    summary_text = entry->manifest.summary[0] != '\0'
        ? entry->manifest.summary
        : entry->module->identity.description;
    preset_key = app != NULL && app->working_config.common.preset_key != NULL
        ? app->working_config.common.preset_key
        : entry->manifest.default_preset;
    theme_key = app != NULL && app->working_config.common.theme_key != NULL
        ? app->working_config.common.theme_key
        : entry->manifest.default_theme;

    suite_browser_append_line(info_text, sizeof(info_text), "Product", entry->manifest.name);
    suite_browser_append_line(info_text, sizeof(info_text), "Key", entry->manifest.key);
    suite_browser_append_line(info_text, sizeof(info_text), "Version", entry->manifest.version);
    suite_browser_append_line(info_text, sizeof(info_text), "Role", entry->role_label);
    suite_browser_append_line(info_text, sizeof(info_text), "Family", entry->family_label);
    suite_browser_append_line(info_text, sizeof(info_text), "Summary", summary_text);
    suite_browser_append_line(info_text, sizeof(info_text), "Renderer support", entry->manifest.renderer);
    suite_browser_append_line(info_text, sizeof(info_text), "Minimum renderer lane", entry->manifest.minimum_kind);
    suite_browser_append_line(info_text, sizeof(info_text), "Preferred renderer lane", entry->manifest.preferred_kind);
    suite_browser_append_line(info_text, sizeof(info_text), "Quality class", entry->manifest.quality_class);
    suite_browser_append_flag_line(info_text, sizeof(info_text), "Preview safe", entry->manifest.preview_safe);
    suite_browser_append_flag_line(info_text, sizeof(info_text), "Long-run stable", entry->manifest.long_run_stable);
    if (entry->manifest.degraded_behavior[0] != '\0') {
        suite_browser_append_line(
            info_text,
            sizeof(info_text),
            "Degraded behavior",
            entry->manifest.degraded_behavior
        );
    }
    if (entry->manifest.unsupported_paths[0] != '\0') {
        suite_browser_append_line(
            info_text,
            sizeof(info_text),
            "Unsupported paths",
            entry->manifest.unsupported_paths
        );
    }
    suite_browser_append_flag_line(
        info_text,
        sizeof(info_text),
        "Settings dialog",
        entry->module->config_hooks != NULL && entry->module->config_hooks->show_config_dialog != NULL
    );
    suite_browser_append_unsigned_line(
        info_text,
        sizeof(info_text),
        "Preset count",
        (unsigned long)entry->module->preset_count
    );
    suite_browser_append_unsigned_line(
        info_text,
        sizeof(info_text),
        "Theme count",
        (unsigned long)entry->module->theme_count
    );
    suite_browser_append_line(
        info_text,
        sizeof(info_text),
        "Selected preset",
        suite_browser_preset_label(entry, preset_key)
    );
    suite_browser_append_line(
        info_text,
        sizeof(info_text),
        "Selected theme",
        suite_browser_theme_label(entry, theme_key)
    );
    if (app != NULL) {
        suite_browser_append_line(
            info_text,
            sizeof(info_text),
            "Detail level",
            screensave_display_detail_level(app->working_config.common.detail_level)
        );
        suite_browser_append_line(
            info_text,
            sizeof(info_text),
            "Randomization mode",
            screensave_display_randomization_mode(app->working_config.common.randomization_mode)
        );
    }

    suite_browser_pack_summary(entry, pack_text, sizeof(pack_text));
    suite_browser_append_unsigned_line(info_text, sizeof(info_text), "Pack count", entry->pack_count);
    suite_browser_append_line(info_text, sizeof(info_text), "Packs", pack_text);

    suite_browser_append_line(
        info_text,
        sizeof(info_text),
        ".scr artifact",
        entry->artifact_available ? "available" : "not found locally"
    );
    suite_browser_append_line(
        info_text,
        sizeof(info_text),
        "Artifact path",
        entry->artifact_available ? entry->artifact_path : "module-linked preview only"
    );

    suite_browser_append_line(
        info_text,
        sizeof(info_text),
        "Preview renderer preference",
        suite_browser_renderer_request_label(app)
    );

    selection_reason = "Preview not active";
    fallback_reason = "None";
    if (app != NULL && app->preview_runtime.renderer != NULL) {
        screensave_renderer_get_info(app->preview_runtime.renderer, &renderer_info);
        suite_browser_append_line(
            info_text,
            sizeof(info_text),
            "Active preview renderer",
            screensave_display_renderer_kind(renderer_info.active_kind)
        );
        if (renderer_info.backend_name != NULL && renderer_info.backend_name[0] != '\0') {
            suite_browser_append_line(info_text, sizeof(info_text), "Preview backend", renderer_info.backend_name);
        }
        if (renderer_info.selection_reason != NULL && renderer_info.selection_reason[0] != '\0') {
            screensave_display_renderer_reason(
                renderer_info.selection_reason,
                selection_text,
                sizeof(selection_text)
            );
            selection_reason = selection_text;
        } else {
            selection_reason = "Default selection path";
        }
        if (renderer_info.fallback_reason != NULL && renderer_info.fallback_reason[0] != '\0') {
            screensave_display_renderer_reason(
                renderer_info.fallback_reason,
                fallback_text,
                sizeof(fallback_text)
            );
            fallback_reason = fallback_text;
        } else {
            fallback_reason = "None";
        }
        if (renderer_info.status_text != NULL && renderer_info.status_text[0] != '\0') {
            screensave_display_renderer_status(
                renderer_info.status_text,
                status_text,
                sizeof(status_text)
            );
            suite_browser_append_line(info_text, sizeof(info_text), "Renderer status", status_text);
        }
    } else {
        suite_browser_append_line(info_text, sizeof(info_text), "Active preview renderer", "Not active");
    }
    suite_browser_append_line(info_text, sizeof(info_text), "Selection path", selection_reason);
    suite_browser_append_line(info_text, sizeof(info_text), "Fallback cause", fallback_reason);

    if (app != NULL && app->diagnostics.last_text[0] != '\0') {
        suite_browser_append_line(info_text, sizeof(info_text), "Last diagnostic", app->diagnostics.last_text);
    }

    DrawTextA(dc, info_text, -1, &draw_rect, DT_LEFT | DT_TOP | DT_WORDBREAK);
    SelectObject(dc, previous_font);
}
