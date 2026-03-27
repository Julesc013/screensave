#include "ricochet_internal.h"
#include "ricochet_resource.h"
#include "screensave/version.h"

typedef struct ricochet_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    ricochet_config *product_config;
} ricochet_dialog_state;

typedef struct ricochet_combo_item_tag {
    int value;
    const char *display_name;
} ricochet_combo_item;

static const ricochet_combo_item g_ricochet_object_items[] = {
    { RICOCHET_OBJECT_BLOCK, "Block" },
    { RICOCHET_OBJECT_DISC, "Disc" },
    { RICOCHET_OBJECT_EMBLEM, "Emblem" }
};

static const ricochet_combo_item g_ricochet_count_items[] = {
    { 1, "Single" },
    { 3, "Three" },
    { 5, "Five" }
};

static const ricochet_combo_item g_ricochet_speed_items[] = {
    { RICOCHET_SPEED_CALM, "Calm" },
    { RICOCHET_SPEED_STANDARD, "Standard" },
    { RICOCHET_SPEED_LIVELY, "Lively" }
};

static const ricochet_combo_item g_ricochet_trail_items[] = {
    { RICOCHET_TRAIL_NONE, "None" },
    { RICOCHET_TRAIL_SHORT, "Short" },
    { RICOCHET_TRAIL_PHOSPHOR, "Phosphor" }
};

static void ricochet_emit_config_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
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
        "ricochet_config",
        text
    );
}

static ricochet_config *ricochet_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(ricochet_config)) {
        return NULL;
    }

    return (ricochet_config *)product_config;
}

static const ricochet_config *ricochet_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(ricochet_config)) {
        return NULL;
    }

    return (const ricochet_config *)product_config;
}

static int ricochet_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(RICOCHET_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, RICOCHET_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int ricochet_read_flag(HKEY key, const char *value_name, int *value)
{
    DWORD data;
    DWORD type;
    DWORD size;

    if (value == NULL) {
        return 0;
    }

    data = 0UL;
    type = 0UL;
    size = sizeof(data);
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) != ERROR_SUCCESS || type != REG_DWORD) {
        return 0;
    }

    *value = data != 0UL;
    return 1;
}

static int ricochet_read_dword(HKEY key, const char *value_name, unsigned long *value)
{
    DWORD data;
    DWORD type;
    DWORD size;

    if (value == NULL) {
        return 0;
    }

    data = 0UL;
    type = 0UL;
    size = sizeof(data);
    if (RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)&data, &size) != ERROR_SUCCESS || type != REG_DWORD) {
        return 0;
    }

    *value = (unsigned long)data;
    return 1;
}

static int ricochet_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG ricochet_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG ricochet_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG ricochet_write_string(HKEY key, const char *value_name, const char *value)
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

void ricochet_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    ricochet_config *config;

    config = ricochet_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->object_mode = RICOCHET_OBJECT_BLOCK;
    config->object_count = 1;
    config->speed_mode = RICOCHET_SPEED_STANDARD;
    config->trail_mode = RICOCHET_TRAIL_SHORT;
    ricochet_apply_preset_to_config(RICOCHET_DEFAULT_PRESET_KEY, common_config, config);
}

void ricochet_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    ricochet_config *config;

    config = ricochet_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        ricochet_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = RICOCHET_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        ricochet_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *preset_descriptor;

            preset_descriptor = screensave_find_preset(
                ricochet_get_presets(&preset_count),
                preset_count,
                common_config->preset_key
            );
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : RICOCHET_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = RICOCHET_DEFAULT_THEME_KEY;
        }
    }

    if (config->object_mode < RICOCHET_OBJECT_BLOCK || config->object_mode > RICOCHET_OBJECT_EMBLEM) {
        config->object_mode = RICOCHET_OBJECT_BLOCK;
    }
    if (config->object_count < 1) {
        config->object_count = 1;
    } else if (config->object_count > 5) {
        config->object_count = 5;
    }
    if (config->speed_mode < RICOCHET_SPEED_CALM || config->speed_mode > RICOCHET_SPEED_LIVELY) {
        config->speed_mode = RICOCHET_SPEED_STANDARD;
    }
    if (config->trail_mode < RICOCHET_TRAIL_NONE || config->trail_mode > RICOCHET_TRAIL_PHOSPHOR) {
        config->trail_mode = RICOCHET_TRAIL_SHORT;
    }
}

int ricochet_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    ricochet_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = ricochet_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    ricochet_config_set_defaults(common_config, product_config, product_config_size);
    if (!ricochet_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        ricochet_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (ricochet_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        ricochet_apply_preset_to_config(preset_key, common_config, config);
    }

    if (ricochet_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (ricochet_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = ricochet_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (ricochet_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)ricochet_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (ricochet_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)ricochet_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->object_mode;
    if (ricochet_read_dword(key, "ObjectMode", &value_dword)) {
        config->object_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->object_count;
    if (ricochet_read_dword(key, "ObjectCount", &value_dword)) {
        config->object_count = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (ricochet_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->trail_mode;
    if (ricochet_read_dword(key, "TrailMode", &value_dword)) {
        config->trail_mode = (int)value_dword;
    }

    RegCloseKey(key);
    ricochet_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int ricochet_config_save(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    DWORD disposition;
    LONG result;
    ricochet_config safe_product_config;
    screensave_common_config safe_common_config;
    const ricochet_config *config;

    (void)module;

    config = ricochet_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    ricochet_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!ricochet_build_registry_path(path, sizeof(path))) {
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
        ricochet_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6301UL,
            "The Ricochet registry key could not be opened for writing."
        );
        return 0;
    }

    result = ricochet_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_dword(key, "ObjectMode", (unsigned long)safe_product_config.object_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_dword(key, "ObjectCount", (unsigned long)safe_product_config.object_count);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ricochet_write_dword(key, "TrailMode", (unsigned long)safe_product_config.trail_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        ricochet_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6302UL,
            "The Ricochet registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT ricochet_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void ricochet_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
{
    LRESULT count;
    LRESULT index;

    count = SendDlgItemMessageA(dialog, control_id, CB_GETCOUNT, 0U, 0L);
    for (index = 0L; index < count; ++index) {
        if (SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L) == item_data) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
            return;
        }
    }
}

static int ricochet_get_combo_value(HWND dialog, int control_id, int default_value)
{
    LRESULT index;
    LRESULT item_data;

    index = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (index == CB_ERR) {
        return default_value;
    }

    item_data = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L);
    if (item_data == CB_ERR) {
        return default_value;
    }

    return (int)item_data;
}

static void ricochet_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_RICOCHET_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        ricochet_add_combo_item(
            dialog,
            IDC_RICOCHET_PRESET,
            module->presets[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_RICOCHET_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        ricochet_add_combo_item(
            dialog,
            IDC_RICOCHET_THEME,
            module->themes[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_RICOCHET_OBJECT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ricochet_object_items) / sizeof(g_ricochet_object_items[0])); ++index) {
        ricochet_add_combo_item(
            dialog,
            IDC_RICOCHET_OBJECT,
            g_ricochet_object_items[index].display_name,
            (LPARAM)g_ricochet_object_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_RICOCHET_COUNT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ricochet_count_items) / sizeof(g_ricochet_count_items[0])); ++index) {
        ricochet_add_combo_item(
            dialog,
            IDC_RICOCHET_COUNT,
            g_ricochet_count_items[index].display_name,
            (LPARAM)g_ricochet_count_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_RICOCHET_SPEED, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ricochet_speed_items) / sizeof(g_ricochet_speed_items[0])); ++index) {
        ricochet_add_combo_item(
            dialog,
            IDC_RICOCHET_SPEED,
            g_ricochet_speed_items[index].display_name,
            (LPARAM)g_ricochet_speed_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_RICOCHET_TRAIL, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ricochet_trail_items) / sizeof(g_ricochet_trail_items[0])); ++index) {
        ricochet_add_combo_item(
            dialog,
            IDC_RICOCHET_TRAIL,
            g_ricochet_trail_items[index].display_name,
            (LPARAM)g_ricochet_trail_items[index].value
        );
    }
}

static void ricochet_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const ricochet_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_RICOCHET_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_RICOCHET_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    ricochet_select_combo_value(dialog, IDC_RICOCHET_OBJECT, (LPARAM)product_config->object_mode);
    ricochet_select_combo_value(dialog, IDC_RICOCHET_COUNT, (LPARAM)product_config->object_count);
    ricochet_select_combo_value(dialog, IDC_RICOCHET_SPEED, (LPARAM)product_config->speed_mode);
    ricochet_select_combo_value(dialog, IDC_RICOCHET_TRAIL, (LPARAM)product_config->trail_mode);

    CheckDlgButton(
        dialog,
        IDC_RICOCHET_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_RICOCHET_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void ricochet_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    ricochet_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_RICOCHET_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_RICOCHET_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.object_mode = RICOCHET_OBJECT_BLOCK;
    product_config.object_count = 1;
    product_config.speed_mode = RICOCHET_SPEED_STANDARD;
    product_config.trail_mode = RICOCHET_TRAIL_SHORT;

    ricochet_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    ricochet_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(
        dialog,
        IDC_RICOCHET_DIAGNOSTICS,
        diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void ricochet_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    ricochet_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    ricochet_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_RICOCHET_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        ricochet_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_RICOCHET_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->object_mode = ricochet_get_combo_value(dialog, IDC_RICOCHET_OBJECT, product_config->object_mode);
    product_config->object_count = ricochet_get_combo_value(dialog, IDC_RICOCHET_COUNT, product_config->object_count);
    product_config->speed_mode = ricochet_get_combo_value(dialog, IDC_RICOCHET_SPEED, product_config->speed_mode);
    product_config->trail_mode = ricochet_get_combo_value(dialog, IDC_RICOCHET_TRAIL, product_config->trail_mode);
    common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_RICOCHET_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_RICOCHET_DIAGNOSTICS) == BST_CHECKED;
}

static void ricochet_initialize_dialog(HWND dialog, ricochet_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    ricochet_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Ricochet\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nPolished bounce motion with restrained trails.");
    SetDlgItemTextA(dialog, IDC_RICOCHET_INFO, info);

    ricochet_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK ricochet_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    ricochet_dialog_state *dialog_state;

    dialog_state = (ricochet_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (ricochet_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            ricochet_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_RICOCHET_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            ricochet_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_RICOCHET_DEFAULTS) {
            ricochet_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            ricochet_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            ricochet_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            ricochet_config_clamp(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            EndDialog(dialog, IDOK);
            return TRUE;
        }

        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(dialog, IDCANCEL);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

INT_PTR ricochet_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    INT_PTR result;
    ricochet_dialog_state dialog_state;
    ricochet_config *config;

    config = ricochet_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        ricochet_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6303UL,
            "The Ricochet dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_RICOCHET_CONFIG),
        owner_window,
        ricochet_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        ricochet_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6304UL,
            "The Ricochet config dialog resource could not be loaded."
        );
    }

    return result;
}
