#include "scr_internal.h"

static int scr_build_registry_path(const screensave_product_identity *product, char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0 || product == NULL || product->product_key == NULL) {
        return 0;
    }

    buffer[0] = '\0';
    if (!scr_append_text(buffer, buffer_size, SCR_SETTINGS_ROOTA)) {
        return 0;
    }

    return scr_append_text(buffer, buffer_size, product->product_key);
}

static void scr_read_flag(HKEY key, const char *value_name, int *value)
{
    DWORD data;
    DWORD type;
    DWORD size;

    data = 0;
    type = 0;
    size = sizeof(data);

    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) == ERROR_SUCCESS && type == REG_DWORD) {
        *value = data != 0;
    }
}

static LONG scr_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

void scr_settings_set_defaults(scr_settings *settings)
{
    if (settings == NULL) {
        return;
    }

    settings->placeholder_visual_enabled = 1;
    settings->diagnostics_overlay_enabled = 0;
}

void scr_settings_load(const screensave_product_identity *product, scr_settings *settings)
{
    char path[260];
    HKEY key;

    if (settings == NULL) {
        return;
    }

    if (!scr_build_registry_path(product, path, sizeof(path))) {
        return;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return;
    }

    scr_read_flag(key, "PlaceholderVisualEnabled", &settings->placeholder_visual_enabled);
    scr_read_flag(key, "DiagnosticsOverlayEnabled", &settings->diagnostics_overlay_enabled);
    RegCloseKey(key);
}

int scr_settings_save(const screensave_product_identity *product, const scr_settings *settings)
{
    char path[260];
    DWORD disposition;
    HKEY key;
    LONG result;

    if (settings == NULL) {
        return 0;
    }

    if (!scr_build_registry_path(product, path, sizeof(path))) {
        return 0;
    }

    result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        path,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &key,
        &disposition
    );
    (void)disposition;

    if (result != ERROR_SUCCESS) {
        return 0;
    }

    result = scr_write_flag(key, "PlaceholderVisualEnabled", settings->placeholder_visual_enabled);
    if (result == ERROR_SUCCESS) {
        result = scr_write_flag(key, "DiagnosticsOverlayEnabled", settings->diagnostics_overlay_enabled);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}
