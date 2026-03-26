#include "benchlab_internal.h"

static int benchlab_upper_ascii(int character)
{
    if (character >= 'a' && character <= 'z') {
        return character - ('a' - 'A');
    }

    return character;
}

static int benchlab_parse_unsigned_long(const char *text, unsigned long *value_out)
{
    unsigned long value;

    if (text == NULL || text[0] == '\0' || value_out == NULL) {
        return 0;
    }

    value = 0UL;
    while (*text != '\0') {
        if (*text < '0' || *text > '9') {
            return 0;
        }

        value = (value * 10UL) + (unsigned long)(*text - '0');
        ++text;
    }

    *value_out = value;
    return 1;
}

static int benchlab_read_dword(HKEY key, const char *value_name, unsigned long *value_out)
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

static LONG benchlab_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static int benchlab_command_equals(const char *token, const char *expected)
{
    if (token == NULL || expected == NULL) {
        return 0;
    }

    if (token[0] == '/' || token[0] == '-') {
        ++token;
    }

    return lstrcmpiA(token, expected) == 0;
}

static int benchlab_command_starts_with(const char *token, const char *prefix)
{
    const char *cursor;

    if (token == NULL || prefix == NULL) {
        return 0;
    }

    cursor = token;
    if (cursor[0] == '/' || cursor[0] == '-') {
        ++cursor;
    }

    while (*prefix != '\0') {
        if (*cursor == '\0') {
            return 0;
        }
        if (benchlab_upper_ascii(*cursor) != benchlab_upper_ascii(*prefix)) {
            return 0;
        }
        ++cursor;
        ++prefix;
    }

    return 1;
}

void benchlab_app_config_set_defaults(benchlab_app_config *config)
{
    if (config == NULL) {
        return;
    }

    config->client_width = BENCHLAB_DEFAULT_CLIENT_WIDTH;
    config->client_height = BENCHLAB_DEFAULT_CLIENT_HEIGHT;
    config->overlay_enabled = 1;
    config->deterministic_mode = 0;
    config->fixed_seed = BENCHLAB_DEFAULT_FIXED_SEED;
}

void benchlab_app_config_clamp(benchlab_app_config *config)
{
    if (config == NULL) {
        return;
    }

    if (config->client_width < 240) {
        config->client_width = 240;
    }
    if (config->client_height < 180) {
        config->client_height = 180;
    }

    config->overlay_enabled = config->overlay_enabled != 0;
    config->deterministic_mode = config->deterministic_mode != 0;
    if (config->fixed_seed == 0UL) {
        config->fixed_seed = BENCHLAB_DEFAULT_FIXED_SEED;
    }
}

int benchlab_app_config_load(benchlab_app_config *config)
{
    HKEY key;
    unsigned long value;

    if (config == NULL) {
        return 0;
    }

    benchlab_app_config_set_defaults(config);
    if (RegOpenKeyExA(HKEY_CURRENT_USER, BENCHLAB_REGISTRY_ROOTA, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        benchlab_app_config_clamp(config);
        return 1;
    }

    value = (unsigned long)config->client_width;
    if (benchlab_read_dword(key, "ClientWidth", &value)) {
        config->client_width = (int)value;
    }

    value = (unsigned long)config->client_height;
    if (benchlab_read_dword(key, "ClientHeight", &value)) {
        config->client_height = (int)value;
    }

    value = (unsigned long)config->overlay_enabled;
    if (benchlab_read_dword(key, "OverlayEnabled", &value)) {
        config->overlay_enabled = value != 0UL;
    }

    value = (unsigned long)config->deterministic_mode;
    if (benchlab_read_dword(key, "DeterministicMode", &value)) {
        config->deterministic_mode = value != 0UL;
    }

    value = config->fixed_seed;
    if (benchlab_read_dword(key, "FixedSeed", &value)) {
        config->fixed_seed = value;
    }

    RegCloseKey(key);
    benchlab_app_config_clamp(config);
    return 1;
}

int benchlab_app_config_save(const benchlab_app_config *config)
{
    HKEY key;
    DWORD disposition;
    LONG result;
    benchlab_app_config safe_config;

    if (config == NULL) {
        return 0;
    }

    safe_config = *config;
    benchlab_app_config_clamp(&safe_config);

    result = RegCreateKeyExA(
        HKEY_CURRENT_USER,
        BENCHLAB_REGISTRY_ROOTA,
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

    result = benchlab_write_dword(key, "ClientWidth", (unsigned long)safe_config.client_width);
    if (result == ERROR_SUCCESS) {
        result = benchlab_write_dword(key, "ClientHeight", (unsigned long)safe_config.client_height);
    }
    if (result == ERROR_SUCCESS) {
        result = benchlab_write_dword(key, "OverlayEnabled", (unsigned long)safe_config.overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = benchlab_write_dword(key, "DeterministicMode", (unsigned long)safe_config.deterministic_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = benchlab_write_dword(key, "FixedSeed", safe_config.fixed_seed);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

void benchlab_app_config_apply_command_line(benchlab_app *app, LPSTR command_line)
{
    char token[96];
    char *cursor;
    unsigned long value;

    if (app == NULL || command_line == NULL) {
        return;
    }

    cursor = command_line;
    while (*cursor != '\0') {
        int length;

        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor == '\0') {
            break;
        }

        length = 0;
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && length < (int)sizeof(token) - 1) {
            token[length] = *cursor;
            ++length;
            ++cursor;
        }
        token[length] = '\0';

        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t') {
            ++cursor;
        }

        if (benchlab_command_equals(token, "deterministic")) {
            app->app_config.deterministic_mode = 1;
            continue;
        }
        if (benchlab_command_equals(token, "session")) {
            app->app_config.deterministic_mode = 0;
            continue;
        }
        if (benchlab_command_equals(token, "pause")) {
            app->paused = 1;
            continue;
        }
        if (benchlab_command_equals(token, "overlay")) {
            app->app_config.overlay_enabled = 1;
            continue;
        }
        if (benchlab_command_equals(token, "nooverlay")) {
            app->app_config.overlay_enabled = 0;
            continue;
        }
        if (benchlab_command_starts_with(token, "seed:")) {
            if (benchlab_parse_unsigned_long(token + 6, &value)) {
                app->app_config.fixed_seed = value;
                app->app_config.deterministic_mode = 1;
            } else {
                benchlab_emit_app_diag(
                    app,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    7101UL,
                    "BenchLab ignored an invalid /seed: command-line argument."
                );
            }
            continue;
        }
    }

    benchlab_app_config_clamp(&app->app_config);
}
