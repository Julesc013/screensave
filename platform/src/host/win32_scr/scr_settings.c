#include "scr_internal.h"

static int scr_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
{
    DWORD type;
    DWORD size;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    type = 0UL;
    size = buffer_size;
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)buffer, &size) != ERROR_SUCCESS || type != REG_SZ) {
        buffer[0] = '\0';
        return 0;
    }

    buffer[buffer_size - 1U] = '\0';
    return buffer[0] != '\0';
}

static LONG scr_write_string(HKEY key, const char *value_name, const char *value)
{
    if (value == NULL || value[0] == '\0') {
        RegDeleteValueA(key, value_name);
        return ERROR_SUCCESS;
    }

    return RegSetValueExA(
        key,
        value_name,
        0,
        REG_SZ,
        (const BYTE *)value,
        (DWORD)(lstrlenA(value) + 1)
    );
}

int scr_settings_init(const screensave_saver_module *module, scr_settings *settings)
{
    return screensave_saver_config_state_init(module, settings);
}

void scr_settings_dispose(scr_settings *settings)
{
    screensave_saver_config_state_dispose(settings);
}

void scr_settings_set_defaults(const screensave_saver_module *module, scr_settings *settings)
{
    screensave_saver_config_state_set_defaults(module, settings);
}

void scr_settings_clamp(const screensave_saver_module *module, scr_settings *settings)
{
    screensave_saver_config_state_clamp(module, settings);
}

int scr_settings_load(
    const screensave_saver_module *module,
    scr_settings *settings,
    screensave_diag_context *diagnostics
)
{
    return screensave_saver_config_state_load(module, settings, diagnostics);
}

int scr_settings_save(
    const screensave_saver_module *module,
    const scr_settings *settings,
    screensave_diag_context *diagnostics
)
{
    return screensave_saver_config_state_save(module, settings, diagnostics);
}

int scr_load_selected_product_key(char *buffer, unsigned int buffer_size)
{
    HKEY key;
    int result;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (RegOpenKeyExA(HKEY_CURRENT_USER, SCR_HOST_REGISTRY_ROOTA, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return 0;
    }

    result = scr_read_string(key, "CurrentProductKey", buffer, (DWORD)buffer_size);
    RegCloseKey(key);
    return result;
}

int scr_save_selected_product_key(const char *product_key)
{
    HKEY key;
    DWORD disposition;
    LONG result;

    result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        SCR_HOST_REGISTRY_ROOTA,
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

    result = scr_write_string(key, "CurrentProductKey", product_key);
    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}
