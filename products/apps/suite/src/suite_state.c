#include "suite_internal.h"

static int suite_read_dword(HKEY key, const char *value_name, unsigned long *value_out)
{
    DWORD data;
    DWORD type;
    DWORD size;

    if (value_out == NULL) {
        return 0;
    }

    data = 0UL;
    type = 0UL;
    size = sizeof(data);
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) != ERROR_SUCCESS || type != REG_DWORD) {
        return 0;
    }

    *value_out = (unsigned long)data;
    return 1;
}

static LONG suite_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static int suite_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG suite_write_string(HKEY key, const char *value_name, const char *value)
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

static int suite_is_valid_renderer_request(int renderer_request)
{
    return renderer_request == (int)SCREENSAVE_RENDERER_KIND_UNKNOWN ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GDI ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL11 ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL46;
}

void suite_app_config_set_defaults(suite_app_config *config)
{
    if (config == NULL) {
        return;
    }

    config->client_width = SUITE_DEFAULT_CLIENT_WIDTH;
    config->client_height = SUITE_DEFAULT_CLIENT_HEIGHT;
    config->renderer_request = (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
    lstrcpyA(config->product_key, SUITE_DEFAULT_PRODUCT_KEY);
}

void suite_app_config_clamp(suite_app_config *config)
{
    if (config == NULL) {
        return;
    }

    if (config->client_width < 800) {
        config->client_width = 800;
    }
    if (config->client_height < 520) {
        config->client_height = 520;
    }
    if (!suite_is_valid_renderer_request(config->renderer_request)) {
        config->renderer_request = (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }
    if (suite_find_target_module(config->product_key) == NULL) {
        lstrcpyA(config->product_key, SUITE_DEFAULT_PRODUCT_KEY);
    }
}

int suite_app_config_load(suite_app_config *config)
{
    HKEY key;
    unsigned long value;

    if (config == NULL) {
        return 0;
    }

    suite_app_config_set_defaults(config);
    if (RegOpenKeyExA(HKEY_CURRENT_USER, SUITE_REGISTRY_ROOTA, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        suite_app_config_clamp(config);
        return 1;
    }

    value = (unsigned long)config->client_width;
    if (suite_read_dword(key, "ClientWidth", &value)) {
        config->client_width = (int)value;
    }

    value = (unsigned long)config->client_height;
    if (suite_read_dword(key, "ClientHeight", &value)) {
        config->client_height = (int)value;
    }

    value = (unsigned long)config->renderer_request;
    if (suite_read_dword(key, "RendererRequest", &value)) {
        config->renderer_request = (int)value;
    }

    (void)suite_read_string(key, "ProductKey", config->product_key, sizeof(config->product_key));
    RegCloseKey(key);

    suite_app_config_clamp(config);
    return 1;
}

int suite_app_config_save(const suite_app_config *config)
{
    HKEY key;
    DWORD disposition;
    LONG result;
    suite_app_config safe_config;

    if (config == NULL) {
        return 0;
    }

    safe_config = *config;
    suite_app_config_clamp(&safe_config);

    result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        SUITE_REGISTRY_ROOTA,
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

    result = suite_write_dword(key, "ClientWidth", (unsigned long)safe_config.client_width);
    if (result == ERROR_SUCCESS) {
        result = suite_write_dword(key, "ClientHeight", (unsigned long)safe_config.client_height);
    }
    if (result == ERROR_SUCCESS) {
        result = suite_write_dword(key, "RendererRequest", (unsigned long)safe_config.renderer_request);
    }
    if (result == ERROR_SUCCESS) {
        result = suite_write_string(key, "ProductKey", safe_config.product_key);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}
