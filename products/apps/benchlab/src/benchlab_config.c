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

static int benchlab_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG benchlab_write_string(HKEY key, const char *value_name, const char *value)
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

static const char *benchlab_command_payload(const char *token)
{
    if (token == NULL) {
        return NULL;
    }

    if (token[0] == '/' || token[0] == '-') {
        return token + 1;
    }

    return token;
}

static int benchlab_is_valid_renderer_request(int renderer_request)
{
    return renderer_request == (int)SCREENSAVE_RENDERER_KIND_UNKNOWN ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GDI ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL11 ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_request == (int)SCREENSAVE_RENDERER_KIND_GL46;
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
    config->renderer_request = (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
    config->fixed_seed = BENCHLAB_DEFAULT_FIXED_SEED;
    lstrcpyA(config->product_key, BENCHLAB_DEFAULT_PRODUCT_KEY);
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
    if (!benchlab_is_valid_renderer_request(config->renderer_request)) {
        config->renderer_request = (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }
    if (config->fixed_seed == 0UL) {
        config->fixed_seed = BENCHLAB_DEFAULT_FIXED_SEED;
    }
    if (benchlab_find_target_module(config->product_key) == NULL) {
        lstrcpyA(config->product_key, BENCHLAB_DEFAULT_PRODUCT_KEY);
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

    value = (unsigned long)config->renderer_request;
    if (benchlab_read_dword(key, "RendererRequest", &value)) {
        config->renderer_request = (int)value;
    }

    value = config->fixed_seed;
    if (benchlab_read_dword(key, "FixedSeed", &value)) {
        config->fixed_seed = value;
    }
    (void)benchlab_read_string(key, "ProductKey", config->product_key, sizeof(config->product_key));

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
        result = benchlab_write_dword(key, "RendererRequest", (unsigned long)safe_config.renderer_request);
    }
    if (result == ERROR_SUCCESS) {
        result = benchlab_write_dword(key, "FixedSeed", safe_config.fixed_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = benchlab_write_string(key, "ProductKey", safe_config.product_key);
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
        if (benchlab_command_equals(token, "report")) {
            app->report_mode = 1;
            continue;
        }
        if (benchlab_command_equals(token, "auto")) {
            app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
            continue;
        }
        if (benchlab_command_equals(token, "gdi")) {
            app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GDI;
            continue;
        }
        if (benchlab_command_equals(token, "gl11")) {
            app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL11;
            continue;
        }
        /* Legacy gl_plus aliases migrate to the canonical gl21 request. */
        if (
            benchlab_command_equals(token, "gl21") ||
            benchlab_command_equals(token, "gl_plus") ||
            benchlab_command_equals(token, "glplus")
        ) {
            app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL21;
            continue;
        }
        if (benchlab_command_equals(token, "gl33")) {
            app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL33;
            continue;
        }
        if (benchlab_command_equals(token, "gl46")) {
            app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL46;
            continue;
        }
        if (benchlab_command_starts_with(token, "renderer:")) {
            const char *renderer_name;

            renderer_name = benchlab_command_payload(token) + 9;
            if (lstrcmpiA(renderer_name, "auto") == 0) {
                app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_UNKNOWN;
            } else if (lstrcmpiA(renderer_name, "gdi") == 0) {
                app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GDI;
            } else if (lstrcmpiA(renderer_name, "gl11") == 0) {
                app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL11;
            /* Legacy gl_plus aliases migrate to the canonical gl21 request. */
            } else if (
                lstrcmpiA(renderer_name, "gl21") == 0 ||
                lstrcmpiA(renderer_name, "gl_plus") == 0 ||
                lstrcmpiA(renderer_name, "glplus") == 0
            ) {
                app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL21;
            } else if (lstrcmpiA(renderer_name, "gl33") == 0) {
                app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL33;
            } else if (lstrcmpiA(renderer_name, "gl46") == 0) {
                app->app_config.renderer_request = (int)SCREENSAVE_RENDERER_KIND_GL46;
            } else {
                benchlab_emit_app_diag(
                    app,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    7102UL,
                    "BenchLab ignored an invalid /renderer: command-line argument."
                );
            }
            continue;
        }
        if (benchlab_command_starts_with(token, "seed:")) {
            if (benchlab_parse_unsigned_long(benchlab_command_payload(token) + 5, &value)) {
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
        if (benchlab_command_starts_with(token, "frames:")) {
            if (benchlab_parse_unsigned_long(benchlab_command_payload(token) + 7, &value) && value > 0UL) {
                app->report_frame_count = value;
            } else {
                benchlab_emit_app_diag(
                    app,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    7104UL,
                    "BenchLab ignored an invalid /frames: command-line argument."
                );
            }
            continue;
        }
        if (benchlab_command_starts_with(token, "report:")) {
            const char *report_path;

            report_path = benchlab_command_payload(token) + 7;
            if (report_path[0] != '\0') {
                app->report_mode = 1;
                lstrcpynA(app->report_path, report_path, sizeof(app->report_path));
            } else {
                benchlab_emit_app_diag(
                    app,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    7105UL,
                    "BenchLab ignored an empty /report: command-line argument."
                );
            }
            continue;
        }
        if (benchlab_command_starts_with(token, "saver:") || benchlab_command_starts_with(token, "product:")) {
            const char *product_key;
            const char *payload;

            payload = benchlab_command_payload(token);
            product_key = payload + (benchlab_command_starts_with(token, "saver:") ? 6 : 8);
            if (benchlab_find_target_module(product_key) != NULL) {
                lstrcpynA(app->app_config.product_key, product_key, sizeof(app->app_config.product_key));
            } else {
                benchlab_emit_app_diag(
                    app,
                    SCREENSAVE_DIAG_LEVEL_WARNING,
                    7103UL,
                    "BenchLab ignored an invalid /saver: command-line argument."
                );
            }
            continue;
        }
    }

    benchlab_app_config_clamp(&app->app_config);
}
