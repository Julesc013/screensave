#include "scr_internal.h"

static int scr_build_registry_path(const screensave_saver_module *module, char *buffer, int buffer_size)
{
    if (
        buffer == NULL ||
        buffer_size <= 0 ||
        module == NULL ||
        module->identity.product_key == NULL
    ) {
        return 0;
    }

    buffer[0] = '\0';
    if (!scr_append_text(buffer, buffer_size, SCR_SETTINGS_ROOTA)) {
        return 0;
    }

    return scr_append_text(buffer, buffer_size, module->identity.product_key);
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

static void scr_read_dword(HKEY key, const char *value_name, unsigned long *value)
{
    DWORD data;
    DWORD type;
    DWORD size;

    data = 0;
    type = 0;
    size = sizeof(data);

    if (
        value != NULL &&
        RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) == ERROR_SUCCESS &&
        type == REG_DWORD
    ) {
        *value = (unsigned long)data;
    }
}

static LONG scr_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG scr_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

void scr_settings_set_defaults(scr_settings *settings)
{
    if (settings == NULL) {
        return;
    }

    screensave_common_config_set_defaults(&settings->common);
    settings->validation_scene_enabled = 1;
}

void scr_settings_load(const screensave_saver_module *module, scr_settings *settings)
{
    char path[260];
    HKEY key;
    unsigned long detail_level;

    if (settings == NULL) {
        return;
    }

    if (!scr_build_registry_path(module, path, sizeof(path))) {
        return;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return;
    }

    detail_level = (unsigned long)settings->common.detail_level;
    scr_read_dword(key, "DetailLevel", &detail_level);
    settings->common.detail_level = (screensave_detail_level)detail_level;
    scr_read_flag(key, "DiagnosticsOverlayEnabled", &settings->common.diagnostics_overlay_enabled);
    scr_read_flag(key, "UseDeterministicSeed", &settings->common.use_deterministic_seed);
    scr_read_dword(key, "DeterministicSeed", &settings->common.deterministic_seed);
    scr_read_flag(key, "ValidationSceneEnabled", &settings->validation_scene_enabled);
    RegCloseKey(key);

    screensave_common_config_clamp(&settings->common);
    settings->validation_scene_enabled = settings->validation_scene_enabled != 0;
}

int scr_settings_save(const screensave_saver_module *module, const scr_settings *settings)
{
    char path[260];
    DWORD disposition;
    HKEY key;
    LONG result;
    scr_settings safe_settings;

    if (settings == NULL) {
        return 0;
    }

    safe_settings = *settings;
    screensave_common_config_clamp(&safe_settings.common);
    safe_settings.validation_scene_enabled = safe_settings.validation_scene_enabled != 0;

    if (!scr_build_registry_path(module, path, sizeof(path))) {
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

    result = scr_write_dword(key, "DetailLevel", (unsigned long)safe_settings.common.detail_level);
    if (result == ERROR_SUCCESS) {
        result = scr_write_flag(key, "DiagnosticsOverlayEnabled", safe_settings.common.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = scr_write_flag(key, "UseDeterministicSeed", safe_settings.common.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = scr_write_dword(key, "DeterministicSeed", safe_settings.common.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = scr_write_flag(key, "ValidationSceneEnabled", safe_settings.validation_scene_enabled);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}
