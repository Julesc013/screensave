#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../base/ini_file.h"
#include "../base/saver_registry.h"
#include "settings_internal.h"

typedef struct screensave_file_writer_context_tag {
    FILE *file;
    char current_section[64];
} screensave_file_writer_context;

static FILE *screensave_settings_open_file(const char *path, const char *mode)
{
#if defined(_MSC_VER)
    FILE *file;

    if (path == NULL || mode == NULL) {
        return NULL;
    }

    file = NULL;
    if (fopen_s(&file, path, mode) != 0) {
        return NULL;
    }

    return file;
#else
    if (path == NULL || mode == NULL) {
        return NULL;
    }

    return fopen(path, mode);
#endif
}

static void screensave_settings_emit_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    if (diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_CONFIG,
        code,
        origin,
        text
    );
}

static int screensave_text_copy(char *buffer, unsigned int buffer_size, const char *text)
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

static char *screensave_trim_text(char *text)
{
    char *end;

    if (text == NULL) {
        return NULL;
    }

    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }

    end = text + lstrlenA(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

static int screensave_parse_unsigned_long(const char *text, unsigned long *value_out)
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

static int screensave_parse_bool(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "1") == 0 || lstrcmpiA(text, "true") == 0 || lstrcmpiA(text, "yes") == 0) {
        *value_out = 1;
        return 1;
    }

    if (lstrcmpiA(text, "0") == 0 || lstrcmpiA(text, "false") == 0 || lstrcmpiA(text, "no") == 0) {
        *value_out = 0;
        return 1;
    }

    return 0;
}

static int screensave_parse_detail_level(const char *text, screensave_detail_level *level_out)
{
    if (text == NULL || level_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "low") == 0) {
        *level_out = SCREENSAVE_DETAIL_LEVEL_LOW;
        return 1;
    }

    if (lstrcmpiA(text, "high") == 0) {
        *level_out = SCREENSAVE_DETAIL_LEVEL_HIGH;
        return 1;
    }

    if (lstrcmpiA(text, "standard") == 0) {
        *level_out = SCREENSAVE_DETAIL_LEVEL_STANDARD;
        return 1;
    }

    return 0;
}

static int screensave_parse_randomization_mode(
    const char *text,
    screensave_randomization_mode *mode_out
)
{
    if (text == NULL || mode_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "off") == 0) {
        *mode_out = SCREENSAVE_RANDOMIZATION_MODE_OFF;
        return 1;
    }

    if (lstrcmpiA(text, "session") == 0) {
        *mode_out = SCREENSAVE_RANDOMIZATION_MODE_SESSION;
        return 1;
    }

    return 0;
}

static int screensave_parse_seed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "session") == 0) {
        *value_out = 0;
        return 1;
    }

    if (lstrcmpiA(text, "fixed") == 0) {
        *value_out = 1;
        return 1;
    }

    return 0;
}

static int screensave_parse_randomization_scope(const char *text, unsigned long *scope_out)
{
    char buffer[128];
    char *cursor;
    unsigned long scope;

    if (
        text == NULL ||
        scope_out == NULL ||
        !screensave_text_copy(buffer, sizeof(buffer), text)
    ) {
        return 0;
    }

    scope = 0UL;
    cursor = buffer;
    while (*cursor != '\0') {
        char *token;

        token = cursor;
        while (*cursor != '\0' && *cursor != ',') {
            ++cursor;
        }
        if (*cursor == ',') {
            *cursor = '\0';
            ++cursor;
        }

        token = screensave_trim_text(token);
        if (token[0] == '\0' || lstrcmpiA(token, "none") == 0) {
            continue;
        }

        if (lstrcmpiA(token, "preset") == 0) {
            scope |= SCREENSAVE_RANDOMIZATION_SCOPE_PRESET;
        } else if (lstrcmpiA(token, "theme") == 0) {
            scope |= SCREENSAVE_RANDOMIZATION_SCOPE_THEME;
        } else if (lstrcmpiA(token, "detail") == 0) {
            scope |= SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL;
        } else if (lstrcmpiA(token, "product") == 0) {
            scope |= SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT;
        } else {
            return 0;
        }
    }

    *scope_out = scope;
    return 1;
}

static int screensave_scope_to_text(char *buffer, int buffer_size, unsigned long scope)
{
    int used;

    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    used = 0;
    if (scope == 0UL) {
        return screensave_text_copy(buffer, (unsigned int)buffer_size, "none");
    }

    if ((scope & SCREENSAVE_RANDOMIZATION_SCOPE_PRESET) != 0UL) {
        lstrcpyA(buffer + used, "preset");
        used += 6;
    }
    if ((scope & SCREENSAVE_RANDOMIZATION_SCOPE_THEME) != 0UL) {
        lstrcpyA(buffer + used, used > 0 ? ",theme" : "theme");
        used += used > 0 ? 6 : 5;
    }
    if ((scope & SCREENSAVE_RANDOMIZATION_SCOPE_DETAIL) != 0UL) {
        lstrcpyA(buffer + used, used > 0 ? ",detail" : "detail");
        used += used > 0 ? 7 : 6;
    }
    if ((scope & SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT) != 0UL) {
        lstrcpyA(buffer + used, used > 0 ? ",product" : "product");
        used += used > 0 ? 8 : 7;
    }

    return used + 1 <= buffer_size;
}

static int screensave_relative_path_is_safe(const char *path)
{
    const char *cursor;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    if (path[0] == '\\' || path[0] == '/' || strchr(path, ':') != NULL) {
        return 0;
    }

    cursor = path;
    while (*cursor != '\0') {
        if (cursor[0] == '.' && cursor[1] == '.') {
            return 0;
        }
        ++cursor;
    }

    return 1;
}

static int screensave_parse_color_value(const char *text)
{
    int index;

    if (text == NULL || lstrlenA(text) != 7 || text[0] != '#') {
        return 0;
    }

    for (index = 1; index < 7; ++index) {
        if (!isxdigit((unsigned char)text[index])) {
            return 0;
        }
    }

    return 1;
}

static int screensave_join_path(
    const char *directory,
    const char *name,
    char *buffer,
    unsigned int buffer_size
)
{
    unsigned int used;

    if (directory == NULL || name == NULL || buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    if (!screensave_text_copy(buffer, buffer_size, directory)) {
        return 0;
    }

    used = (unsigned int)lstrlenA(buffer);
    if (used > 0U && buffer[used - 1U] != '\\') {
        if (used + 1U >= buffer_size) {
            return 0;
        }
        lstrcatA(buffer, "\\");
    }

    if ((unsigned int)lstrlenA(buffer) + (unsigned int)lstrlenA(name) + 1U > buffer_size) {
        return 0;
    }

    lstrcatA(buffer, name);
    return 1;
}

static int screensave_read_registry_dword(HKEY key, const char *value_name, unsigned long *value_out)
{
    DWORD data;
    DWORD type;
    DWORD size;

    if (key == NULL || value_name == NULL || value_out == NULL) {
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

static int screensave_open_product_key_for_query(const screensave_saver_module *module, HKEY *key_out)
{
    char path[260];
    const char *legacy_product_key;

    if (module == NULL || key_out == NULL) {
        return 0;
    }

    if (
        screensave_saver_registry_build_registry_root(
            module->identity.product_key,
            path,
            (unsigned int)sizeof(path)
        ) &&
        RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, key_out) == ERROR_SUCCESS
    ) {
        return 1;
    }

    legacy_product_key = screensave_saver_registry_legacy_product_key(module->identity.product_key);
    if (
        legacy_product_key == NULL ||
        !screensave_saver_registry_build_registry_root(
            legacy_product_key,
            path,
            (unsigned int)sizeof(path)
        )
    ) {
        return 0;
    }

    return RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, key_out) == ERROR_SUCCESS;
}

static int screensave_open_product_key_for_write(const screensave_saver_module *module, HKEY *key_out)
{
    char path[260];
    DWORD disposition;

    if (
        module == NULL ||
        key_out == NULL ||
        !screensave_saver_registry_build_registry_root(
            module->identity.product_key,
            path,
            (unsigned int)sizeof(path)
        )
    ) {
        return 0;
    }

    return RegCreateKeyExA(
        HKEY_CURRENT_USER,
        path,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        key_out,
        &disposition
    ) == ERROR_SUCCESS;
}

static int screensave_write_registry_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    if (key == NULL || value_name == NULL) {
        return 0;
    }

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data)) == ERROR_SUCCESS;
}

static int screensave_write_section_header(
    screensave_file_writer_context *context,
    const char *section
)
{
    if (context == NULL || context->file == NULL || section == NULL) {
        return 0;
    }

    if (lstrcmpiA(context->current_section, section) != 0) {
        if (context->current_section[0] != '\0') {
            fputs("\n", context->file);
        }
        fprintf(context->file, "[%s]\n", section);
        lstrcpynA(context->current_section, section, sizeof(context->current_section));
    }

    return !ferror(context->file);
}

static int screensave_file_writer_write_string(
    void *context,
    const char *section,
    const char *key,
    const char *value
)
{
    screensave_file_writer_context *writer_context;

    writer_context = (screensave_file_writer_context *)context;
    if (
        writer_context == NULL ||
        key == NULL ||
        value == NULL ||
        !screensave_write_section_header(writer_context, section)
    ) {
        return 0;
    }

    fprintf(writer_context->file, "%s=%s\n", key, value);
    return !ferror(writer_context->file);
}

static int screensave_file_writer_write_unsigned_long(
    void *context,
    const char *section,
    const char *key,
    unsigned long value
)
{
    char text[32];

    wsprintfA(text, "%lu", value);
    return screensave_file_writer_write_string(context, section, key, text);
}

int screensave_settings_load_shared_state(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    screensave_diag_context *diagnostics
)
{
    HKEY key;
    unsigned long value;

    if (module == NULL || common_config == NULL) {
        return 0;
    }

    if (!screensave_open_product_key_for_query(module, &key)) {
        return 1;
    }

    value = common_config->schema_version;
    if (screensave_read_registry_dword(key, "SharedSchemaVersion", &value)) {
        common_config->schema_version = value;
    }

    value = (unsigned long)common_config->randomization_mode;
    if (screensave_read_registry_dword(key, "RandomizationMode", &value)) {
        common_config->randomization_mode = (screensave_randomization_mode)value;
    }

    value = common_config->randomization_scope;
    if (screensave_read_registry_dword(key, "RandomizationScope", &value)) {
        common_config->randomization_scope = value;
    }

    RegCloseKey(key);
    screensave_common_config_clamp(common_config);
    if (!screensave_common_config_validate(common_config, &value)) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7601UL,
            "screensave_settings_load_shared_state",
            "Shared settings extension values were clamped after load."
        );
    }

    return 1;
}

int screensave_settings_save_shared_state(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    screensave_diag_context *diagnostics
)
{
    HKEY key;
    screensave_common_config safe_common_config;

    if (module == NULL || common_config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    screensave_common_config_clamp(&safe_common_config);
    if (!screensave_open_product_key_for_write(module, &key)) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7602UL,
            "screensave_settings_save_shared_state",
            "The shared settings extension key could not be opened for writing."
        );
        return 0;
    }

    if (
        !screensave_write_registry_dword(key, "SharedSchemaVersion", safe_common_config.schema_version) ||
        !screensave_write_registry_dword(
            key,
            "RandomizationMode",
            (unsigned long)safe_common_config.randomization_mode
        ) ||
        !screensave_write_registry_dword(
            key,
            "RandomizationScope",
            safe_common_config.randomization_scope
        )
    ) {
        RegCloseKey(key);
        return 0;
    }

    RegCloseKey(key);
    return 1;
}

const char *screensave_settings_file_kind_name(screensave_settings_file_kind kind)
{
    switch (kind) {
    case SCREENSAVE_SETTINGS_FILE_THEME:
        return "theme";

    case SCREENSAVE_SETTINGS_FILE_PRESET:
    default:
        return "preset";
    }
}

const char *screensave_pack_source_kind_name(screensave_pack_source_kind source_kind)
{
    switch (source_kind) {
    case SCREENSAVE_PACK_SOURCE_PORTABLE:
        return "portable";

    case SCREENSAVE_PACK_SOURCE_USER:
        return "user";

    case SCREENSAVE_PACK_SOURCE_BUILTIN:
    default:
        return "builtin";
    }
}

static const screensave_theme_descriptor *screensave_resolve_theme_descriptor(
    const screensave_saver_module *module,
    const screensave_common_config *common_config
)
{
    if (
        module == NULL ||
        common_config == NULL ||
        common_config->theme_key == NULL ||
        common_config->theme_key[0] == '\0'
    ) {
        return NULL;
    }

    return screensave_find_theme(module->themes, module->theme_count, common_config->theme_key);
}

int screensave_settings_export_file(
    const screensave_saver_module *module,
    const screensave_saver_config_state *config_state,
    screensave_settings_file_kind kind,
    const char *path,
    screensave_diag_context *diagnostics
)
{
    FILE *file;
    screensave_file_writer_context writer_context;
    screensave_settings_writer writer;
    const screensave_saver_config_hooks *config_hooks;

    if (module == NULL || config_state == NULL || path == NULL) {
        return 0;
    }

    file = screensave_settings_open_file(path, "w");
    if (file == NULL) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7603UL,
            "screensave_settings_export_file",
            "The requested settings file could not be opened for writing."
        );
        return 0;
    }

    ZeroMemory(&writer_context, sizeof(writer_context));
    writer_context.file = file;
    ZeroMemory(&writer, sizeof(writer));
    writer.context = &writer_context;
    writer.write_string = screensave_file_writer_write_string;
    writer.write_unsigned_long = screensave_file_writer_write_unsigned_long;

    if (
        !writer.write_string(writer.context, "format", "kind", screensave_settings_file_kind_name(kind)) ||
        !writer.write_unsigned_long(writer.context, "format", "version", SCREENSAVE_SETTINGS_FILE_FORMAT_VERSION) ||
        !writer.write_string(writer.context, "product", "key", module->identity.product_key) ||
        !writer.write_unsigned_long(
            writer.context,
            "product",
            "schema_version",
            config_state->common.schema_version
        )
    ) {
        fclose(file);
        return 0;
    }

    if (kind == SCREENSAVE_SETTINGS_FILE_PRESET) {
        char scope_text[96];

        if (
            !screensave_scope_to_text(scope_text, sizeof(scope_text), config_state->common.randomization_scope) ||
            !writer.write_string(
                writer.context,
                "common",
                "detail_level",
                screensave_detail_level_name(config_state->common.detail_level)
            ) ||
            !writer.write_string(
                writer.context,
                "common",
                "seed_mode",
                screensave_seed_mode_name(&config_state->common)
            ) ||
            !writer.write_unsigned_long(
                writer.context,
                "common",
                "deterministic_seed",
                config_state->common.deterministic_seed
            ) ||
            !writer.write_unsigned_long(
                writer.context,
                "common",
                "diagnostics_overlay_enabled",
                (unsigned long)(config_state->common.diagnostics_overlay_enabled != 0)
            ) ||
            !writer.write_string(
                writer.context,
                "common",
                "randomization_mode",
                screensave_randomization_mode_name(config_state->common.randomization_mode)
            ) ||
            !writer.write_string(writer.context, "common", "randomization_scope", scope_text)
        ) {
            fclose(file);
            return 0;
        }

        if (
            config_state->common.preset_key != NULL &&
            !writer.write_string(writer.context, "common", "preset_key", config_state->common.preset_key)
        ) {
            fclose(file);
            return 0;
        }

        if (
            config_state->common.theme_key != NULL &&
            !writer.write_string(writer.context, "common", "theme_key", config_state->common.theme_key)
        ) {
            fclose(file);
            return 0;
        }
    } else {
        const screensave_theme_descriptor *theme_descriptor;
        char color_text[16];

        theme_descriptor = screensave_resolve_theme_descriptor(module, &config_state->common);
        if (theme_descriptor == NULL) {
            fclose(file);
            screensave_settings_emit_diag(
                diagnostics,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                7604UL,
                "screensave_settings_export_file",
                "The selected theme could not be resolved for export."
            );
            return 0;
        }

        if (
            !writer.write_string(writer.context, "theme", "theme_key", theme_descriptor->theme_key) ||
            !writer.write_string(writer.context, "theme", "display_name", theme_descriptor->display_name) ||
            !writer.write_string(writer.context, "theme", "description", theme_descriptor->description)
        ) {
            fclose(file);
            return 0;
        }

        wsprintfA(
            color_text,
            "#%02X%02X%02X",
            theme_descriptor->primary_color.red,
            theme_descriptor->primary_color.green,
            theme_descriptor->primary_color.blue
        );
        if (!writer.write_string(writer.context, "theme", "primary_color", color_text)) {
            fclose(file);
            return 0;
        }

        wsprintfA(
            color_text,
            "#%02X%02X%02X",
            theme_descriptor->accent_color.red,
            theme_descriptor->accent_color.green,
            theme_descriptor->accent_color.blue
        );
        if (!writer.write_string(writer.context, "theme", "accent_color", color_text)) {
            fclose(file);
            return 0;
        }
    }

    config_hooks = module->config_hooks;
    if (
        kind == SCREENSAVE_SETTINGS_FILE_PRESET &&
        config_hooks != NULL &&
        config_hooks->export_settings_entries != NULL &&
        !config_hooks->export_settings_entries(
            module,
            &config_state->common,
            config_state->product_config,
            config_state->product_config_size,
            kind,
            &writer,
            diagnostics
        )
    ) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

typedef struct screensave_settings_import_context_tag {
    const screensave_saver_module *module;
    screensave_saver_config_state *config_state;
    screensave_settings_file_kind expected_kind;
    screensave_diag_context *diagnostics;
    char declared_kind[32];
    char declared_product_key[64];
    unsigned long declared_version;
    int saw_kind;
    int saw_product_key;
    int saw_version;
    int invalid;
} screensave_settings_import_context;

static int screensave_settings_import_callback(
    void *context,
    const char *section,
    const char *key,
    const char *value
)
{
    screensave_settings_import_context *import_context;
    const screensave_saver_config_hooks *config_hooks;

    import_context = (screensave_settings_import_context *)context;
    if (import_context == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }

    if (lstrcmpiA(section, "format") == 0) {
        if (lstrcmpiA(key, "kind") == 0) {
            import_context->saw_kind = 1;
            return screensave_text_copy(
                import_context->declared_kind,
                sizeof(import_context->declared_kind),
                value
            );
        }

        if (lstrcmpiA(key, "version") == 0) {
            import_context->saw_version = screensave_parse_unsigned_long(
                value,
                &import_context->declared_version
            );
            return import_context->saw_version;
        }

        return 1;
    }

    if (lstrcmpiA(section, "product") == 0) {
        if (lstrcmpiA(key, "key") == 0) {
            import_context->saw_product_key = 1;
            return screensave_text_copy(
                import_context->declared_product_key,
                sizeof(import_context->declared_product_key),
                value
            );
        }

        if (lstrcmpiA(key, "schema_version") == 0) {
            return screensave_parse_unsigned_long(
                value,
                &import_context->config_state->common.schema_version
            );
        }
    }

    if (
        import_context->expected_kind == SCREENSAVE_SETTINGS_FILE_PRESET &&
        lstrcmpiA(section, "common") == 0
    ) {
        if (lstrcmpiA(key, "detail_level") == 0) {
            return screensave_parse_detail_level(
                value,
                &import_context->config_state->common.detail_level
            );
        }

        if (lstrcmpiA(key, "seed_mode") == 0) {
            return screensave_parse_seed_mode(
                value,
                &import_context->config_state->common.use_deterministic_seed
            );
        }

        if (lstrcmpiA(key, "deterministic_seed") == 0) {
            return screensave_parse_unsigned_long(
                value,
                &import_context->config_state->common.deterministic_seed
            );
        }

        if (lstrcmpiA(key, "diagnostics_overlay_enabled") == 0) {
            return screensave_parse_bool(
                value,
                &import_context->config_state->common.diagnostics_overlay_enabled
            );
        }

        if (lstrcmpiA(key, "randomization_mode") == 0) {
            return screensave_parse_randomization_mode(
                value,
                &import_context->config_state->common.randomization_mode
            );
        }

        if (lstrcmpiA(key, "randomization_scope") == 0) {
            return screensave_parse_randomization_scope(
                value,
                &import_context->config_state->common.randomization_scope
            );
        }

        if (lstrcmpiA(key, "preset_key") == 0) {
            config_hooks = import_context->module->config_hooks;
            if (config_hooks != NULL && config_hooks->apply_preset != NULL) {
                config_hooks->apply_preset(
                    import_context->module,
                    value,
                    &import_context->config_state->common,
                    import_context->config_state->product_config,
                    import_context->config_state->product_config_size
                );
            }
            return 1;
        }

        if (lstrcmpiA(key, "theme_key") == 0) {
            const screensave_theme_descriptor *theme_descriptor;

            theme_descriptor = screensave_find_theme(
                import_context->module->themes,
                import_context->module->theme_count,
                value
            );
            if (theme_descriptor != NULL) {
                import_context->config_state->common.theme_key = theme_descriptor->theme_key;
            }
            return 1;
        }
    }

    if (
        import_context->expected_kind == SCREENSAVE_SETTINGS_FILE_THEME &&
        lstrcmpiA(section, "theme") == 0
    ) {
        if (lstrcmpiA(key, "theme_key") == 0) {
            const screensave_theme_descriptor *theme_descriptor;

            theme_descriptor = screensave_find_theme(
                import_context->module->themes,
                import_context->module->theme_count,
                value
            );
            if (theme_descriptor == NULL) {
                import_context->invalid = 1;
                return 0;
            }

            import_context->config_state->common.theme_key = theme_descriptor->theme_key;
            return 1;
        }

        if (
            lstrcmpiA(key, "primary_color") == 0 ||
            lstrcmpiA(key, "accent_color") == 0
        ) {
            return screensave_parse_color_value(value);
        }

        return 1;
    }

    config_hooks = import_context->module->config_hooks;
    if (config_hooks != NULL && config_hooks->import_settings_entry != NULL) {
        return config_hooks->import_settings_entry(
            import_context->module,
            &import_context->config_state->common,
            import_context->config_state->product_config,
            import_context->config_state->product_config_size,
            import_context->expected_kind,
            section,
            key,
            value,
            import_context->diagnostics
        );
    }

    return 1;
}

int screensave_settings_import_file(
    const screensave_saver_module *module,
    screensave_saver_config_state *config_state,
    screensave_settings_file_kind kind,
    const char *path,
    screensave_diag_context *diagnostics
)
{
    screensave_settings_import_context import_context;
    const char *canonical_product_key;

    if (module == NULL || config_state == NULL || path == NULL) {
        return 0;
    }

    screensave_saver_config_state_set_defaults(module, config_state);

    ZeroMemory(&import_context, sizeof(import_context));
    import_context.module = module;
    import_context.config_state = config_state;
    import_context.expected_kind = kind;
    import_context.diagnostics = diagnostics;

    if (!screensave_ini_parse_file(path, screensave_settings_import_callback, &import_context)) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7605UL,
            "screensave_settings_import_file",
            "The requested settings file could not be parsed."
        );
        return 0;
    }

    if (
        !import_context.saw_kind ||
        !import_context.saw_version ||
        !import_context.saw_product_key ||
        import_context.declared_version != SCREENSAVE_SETTINGS_FILE_FORMAT_VERSION
    ) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7606UL,
            "screensave_settings_import_file",
            "The requested settings file did not match the shared settings header."
        );
        return 0;
    }

    if (lstrcmpiA(import_context.declared_kind, screensave_settings_file_kind_name(kind)) != 0) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7607UL,
            "screensave_settings_import_file",
            "The requested settings file kind did not match the requested import operation."
        );
        return 0;
    }

    canonical_product_key = screensave_saver_registry_canonical_product_key(import_context.declared_product_key);
    if (
        canonical_product_key == NULL ||
        lstrcmpiA(canonical_product_key, module->identity.product_key) != 0
    ) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            7608UL,
            "screensave_settings_import_file",
            "The requested settings file targeted a different saver product."
        );
        return 0;
    }

    screensave_saver_config_state_clamp(module, config_state);
    return !import_context.invalid;
}

void screensave_pack_manifest_set_defaults(screensave_pack_manifest *manifest)
{
    if (manifest == NULL) {
        return;
    }

    ZeroMemory(manifest, sizeof(*manifest));
    manifest->format_version = SCREENSAVE_PACK_MANIFEST_VERSION;
    manifest->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    manifest->source_kind = SCREENSAVE_PACK_SOURCE_BUILTIN;
}

void screensave_pack_manifest_clamp(screensave_pack_manifest *manifest)
{
    if (manifest == NULL) {
        return;
    }

    if (manifest->format_version != SCREENSAVE_PACK_MANIFEST_VERSION) {
        manifest->format_version = SCREENSAVE_PACK_MANIFEST_VERSION;
    }
    if (manifest->schema_version == 0UL) {
        manifest->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    }
    manifest->content_flags &=
        SCREENSAVE_PACK_CONTENT_PRESETS |
        SCREENSAVE_PACK_CONTENT_THEMES |
        SCREENSAVE_PACK_CONTENT_SCENES;
    if (manifest->preset_file_count > SCREENSAVE_PACK_ENTRY_LIMIT) {
        manifest->preset_file_count = SCREENSAVE_PACK_ENTRY_LIMIT;
    }
    if (manifest->theme_file_count > SCREENSAVE_PACK_ENTRY_LIMIT) {
        manifest->theme_file_count = SCREENSAVE_PACK_ENTRY_LIMIT;
    }
    if (manifest->scene_file_count > SCREENSAVE_PACK_ENTRY_LIMIT) {
        manifest->scene_file_count = SCREENSAVE_PACK_ENTRY_LIMIT;
    }
}

int screensave_pack_manifest_validate(
    const screensave_pack_manifest *manifest,
    unsigned long *issue_flags
)
{
    unsigned long flags;
    unsigned int index;

    flags = 0UL;
    if (manifest == NULL) {
        flags = SCREENSAVE_PACK_ISSUE_BAD_FORMAT_VERSION;
    } else {
        if (manifest->format_version != SCREENSAVE_PACK_MANIFEST_VERSION) {
            flags |= SCREENSAVE_PACK_ISSUE_BAD_FORMAT_VERSION;
        }
        if (manifest->pack_key[0] == '\0') {
            flags |= SCREENSAVE_PACK_ISSUE_EMPTY_PACK_KEY;
        }
        if (manifest->product_key[0] == '\0') {
            flags |= SCREENSAVE_PACK_ISSUE_EMPTY_PRODUCT_KEY;
        }
        if (manifest->display_name[0] == '\0') {
            flags |= SCREENSAVE_PACK_ISSUE_EMPTY_DISPLAY_NAME;
        }
        if (manifest->content_flags == 0UL) {
            flags |= SCREENSAVE_PACK_ISSUE_BAD_CONTENT_FLAGS;
        }
        if (
            manifest->preset_file_count == 0U &&
            manifest->theme_file_count == 0U &&
            manifest->scene_file_count == 0U
        ) {
            flags |= SCREENSAVE_PACK_ISSUE_MISSING_ENTRIES;
        }

        for (index = 0U; index < manifest->preset_file_count; ++index) {
            if (!screensave_relative_path_is_safe(manifest->preset_files[index])) {
                flags |= SCREENSAVE_PACK_ISSUE_BAD_ENTRY_PATH;
            }
        }
        for (index = 0U; index < manifest->theme_file_count; ++index) {
            if (!screensave_relative_path_is_safe(manifest->theme_files[index])) {
                flags |= SCREENSAVE_PACK_ISSUE_BAD_ENTRY_PATH;
            }
        }
        for (index = 0U; index < manifest->scene_file_count; ++index) {
            if (!screensave_relative_path_is_safe(manifest->scene_files[index])) {
                flags |= SCREENSAVE_PACK_ISSUE_BAD_ENTRY_PATH;
            }
        }
    }

    if (issue_flags != NULL) {
        *issue_flags = flags;
    }

    return flags == 0UL;
}

typedef struct screensave_pack_load_context_tag {
    screensave_pack_manifest *manifest;
} screensave_pack_load_context;

static int screensave_pack_load_callback(
    void *context,
    const char *section,
    const char *key,
    const char *value
)
{
    screensave_pack_load_context *load_context;
    screensave_pack_manifest *manifest;
    unsigned long parsed_value;

    load_context = (screensave_pack_load_context *)context;
    if (load_context == NULL || load_context->manifest == NULL) {
        return 0;
    }

    manifest = load_context->manifest;
    if (lstrcmpiA(section, "pack") == 0) {
        if (lstrcmpiA(key, "format") == 0) {
            return lstrcmpiA(value, "screensave-pack") == 0;
        }
        if (lstrcmpiA(key, "version") == 0) {
            return screensave_parse_unsigned_long(value, &manifest->format_version);
        }
        if (lstrcmpiA(key, "schema_version") == 0) {
            return screensave_parse_unsigned_long(value, &manifest->schema_version);
        }
        if (lstrcmpiA(key, "pack_key") == 0) {
            return screensave_text_copy(manifest->pack_key, sizeof(manifest->pack_key), value);
        }
        if (lstrcmpiA(key, "product_key") == 0) {
            return screensave_text_copy(manifest->product_key, sizeof(manifest->product_key), value);
        }
        if (lstrcmpiA(key, "display_name") == 0) {
            return screensave_text_copy(manifest->display_name, sizeof(manifest->display_name), value);
        }
        if (lstrcmpiA(key, "description") == 0) {
            return screensave_text_copy(manifest->description, sizeof(manifest->description), value);
        }
        if (lstrcmpiA(key, "content_flags") == 0) {
            if (!screensave_parse_unsigned_long(value, &parsed_value)) {
                return 0;
            }
            manifest->content_flags = parsed_value;
            return 1;
        }
        return 1;
    }

    if (lstrcmpiA(section, "files") == 0) {
        if (strncmp(key, "preset_", 7) == 0) {
            if (manifest->preset_file_count >= SCREENSAVE_PACK_ENTRY_LIMIT) {
                return 0;
            }
            if (!screensave_text_copy(
                    manifest->preset_files[manifest->preset_file_count],
                    SCREENSAVE_PACK_PATH_LENGTH,
                    value
                )) {
                return 0;
            }
            manifest->preset_file_count += 1U;
            manifest->content_flags |= SCREENSAVE_PACK_CONTENT_PRESETS;
            return 1;
        }
        if (strncmp(key, "theme_", 6) == 0) {
            if (manifest->theme_file_count >= SCREENSAVE_PACK_ENTRY_LIMIT) {
                return 0;
            }
            if (!screensave_text_copy(
                    manifest->theme_files[manifest->theme_file_count],
                    SCREENSAVE_PACK_PATH_LENGTH,
                    value
                )) {
                return 0;
            }
            manifest->theme_file_count += 1U;
            manifest->content_flags |= SCREENSAVE_PACK_CONTENT_THEMES;
            return 1;
        }
        if (strncmp(key, "scene_", 6) == 0) {
            if (manifest->scene_file_count >= SCREENSAVE_PACK_ENTRY_LIMIT) {
                return 0;
            }
            if (!screensave_text_copy(
                    manifest->scene_files[manifest->scene_file_count],
                    SCREENSAVE_PACK_PATH_LENGTH,
                    value
                )) {
                return 0;
            }
            manifest->scene_file_count += 1U;
            manifest->content_flags |= SCREENSAVE_PACK_CONTENT_SCENES;
            return 1;
        }
    }

    return 1;
}

int screensave_pack_manifest_load(
    const char *path,
    screensave_pack_source_kind source_kind,
    screensave_pack_manifest *manifest,
    screensave_diag_context *diagnostics
)
{
    screensave_pack_load_context load_context;
    unsigned long issue_flags;

    if (path == NULL || manifest == NULL) {
        return 0;
    }

    screensave_pack_manifest_set_defaults(manifest);
    manifest->source_kind = source_kind;
    if (!screensave_text_copy(manifest->manifest_path, sizeof(manifest->manifest_path), path)) {
        return 0;
    }

    load_context.manifest = manifest;
    if (!screensave_ini_parse_file(path, screensave_pack_load_callback, &load_context)) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7609UL,
            "screensave_pack_manifest_load",
            "A pack manifest could not be parsed and was ignored."
        );
        return 0;
    }

    screensave_pack_manifest_clamp(manifest);
    if (!screensave_pack_manifest_validate(manifest, &issue_flags)) {
        screensave_settings_emit_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            7610UL,
            "screensave_pack_manifest_load",
            "A pack manifest did not satisfy the shared validation rules and was ignored."
        );
        return 0;
    }

    return 1;
}

int screensave_pack_manifest_discover(
    const char *directory_path,
    screensave_pack_source_kind source_kind,
    const char *product_key_filter,
    screensave_pack_manifest *manifests,
    unsigned int manifest_capacity,
    unsigned int *manifest_count_out,
    screensave_diag_context *diagnostics
)
{
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle;
    char search_pattern[SCREENSAVE_PACK_PATH_LENGTH];
    char pack_directory[SCREENSAVE_PACK_PATH_LENGTH];
    char pack_manifest_path[SCREENSAVE_PACK_PATH_LENGTH];
    unsigned int manifest_count;

    if (manifest_count_out != NULL) {
        *manifest_count_out = 0U;
    }

    if (
        directory_path == NULL ||
        manifests == NULL ||
        manifest_capacity == 0U ||
        !screensave_join_path(directory_path, "*", search_pattern, (unsigned int)sizeof(search_pattern))
    ) {
        return 0;
    }

    find_handle = FindFirstFileA(search_pattern, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE) {
        return 1;
    }

    manifest_count = 0U;
    do {
        const char *canonical_filter;
        const char *canonical_product_key;

        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0U) {
            continue;
        }
        if (lstrcmpA(find_data.cFileName, ".") == 0 || lstrcmpA(find_data.cFileName, "..") == 0) {
            continue;
        }
        if (manifest_count >= manifest_capacity) {
            break;
        }

        if (
            !screensave_join_path(directory_path, find_data.cFileName, pack_directory, (unsigned int)sizeof(pack_directory)) ||
            !screensave_join_path(pack_directory, "pack.ini", pack_manifest_path, (unsigned int)sizeof(pack_manifest_path))
        ) {
            continue;
        }

        if (GetFileAttributesA(pack_manifest_path) == INVALID_FILE_ATTRIBUTES) {
            continue;
        }

        if (!screensave_pack_manifest_load(pack_manifest_path, source_kind, &manifests[manifest_count], diagnostics)) {
            continue;
        }

        canonical_filter = screensave_saver_registry_canonical_product_key(product_key_filter);
        canonical_product_key = screensave_saver_registry_canonical_product_key(manifests[manifest_count].product_key);
        if (
            canonical_filter != NULL &&
            canonical_product_key != NULL &&
            lstrcmpiA(canonical_filter, canonical_product_key) != 0
        ) {
            continue;
        }

        manifest_count += 1U;
    } while (FindNextFileA(find_handle, &find_data) != 0);

    FindClose(find_handle);
    if (manifest_count_out != NULL) {
        *manifest_count_out = manifest_count;
    }

    return 1;
}
