#include "ember_internal.h"
#include "ember_resource.h"
#include "screensave/version.h"

typedef struct ember_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    ember_config *product_config;
} ember_dialog_state;

typedef struct ember_combo_item_tag {
    int value;
    const char *display_name;
} ember_combo_item;

static const ember_combo_item g_ember_effect_items[] = {
    { EMBER_EFFECT_PLASMA, "Plasma" },
    { EMBER_EFFECT_FIRE, "Fire" },
    { EMBER_EFFECT_INTERFERENCE, "Interference" }
};

static const ember_combo_item g_ember_speed_items[] = {
    { EMBER_SPEED_GENTLE, "Gentle" },
    { EMBER_SPEED_STANDARD, "Standard" },
    { EMBER_SPEED_LIVELY, "Lively" }
};

static const ember_combo_item g_ember_resolution_items[] = {
    { EMBER_RESOLUTION_COARSE, "Coarse" },
    { EMBER_RESOLUTION_STANDARD, "Standard" },
    { EMBER_RESOLUTION_FINE, "Fine" }
};

static const ember_combo_item g_ember_smoothing_items[] = {
    { EMBER_SMOOTHING_OFF, "Off" },
    { EMBER_SMOOTHING_SOFT, "Soft" },
    { EMBER_SMOOTHING_GLOW, "Glow" }
};

static void ember_emit_config_diag(
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
        "ember_config",
        text
    );
}

static ember_config *ember_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(ember_config)) {
        return NULL;
    }

    return (ember_config *)product_config;
}

static const ember_config *ember_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(ember_config)) {
        return NULL;
    }

    return (const ember_config *)product_config;
}

static int ember_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(EMBER_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, EMBER_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int ember_read_flag(HKEY key, const char *value_name, int *value)
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

static int ember_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int ember_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG ember_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG ember_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG ember_write_string(HKEY key, const char *value_name, const char *value)
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

void ember_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    ember_config *config;

    config = ember_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->effect_mode = EMBER_EFFECT_FIRE;
    config->speed_mode = EMBER_SPEED_GENTLE;
    config->resolution_mode = EMBER_RESOLUTION_STANDARD;
    config->smoothing_mode = EMBER_SMOOTHING_SOFT;
    ember_apply_preset_to_config(EMBER_DEFAULT_PRESET_KEY, common_config, config);
}

void ember_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    ember_config *config;

    config = ember_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        ember_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = EMBER_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        ember_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = ember_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : EMBER_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = EMBER_DEFAULT_THEME_KEY;
        }
    }

    if (config->effect_mode < EMBER_EFFECT_PLASMA || config->effect_mode > EMBER_EFFECT_INTERFERENCE) {
        config->effect_mode = EMBER_EFFECT_FIRE;
    }
    if (config->speed_mode < EMBER_SPEED_GENTLE || config->speed_mode > EMBER_SPEED_LIVELY) {
        config->speed_mode = EMBER_SPEED_GENTLE;
    }
    if (
        config->resolution_mode < EMBER_RESOLUTION_COARSE ||
        config->resolution_mode > EMBER_RESOLUTION_FINE
    ) {
        config->resolution_mode = EMBER_RESOLUTION_STANDARD;
    }
    if (config->smoothing_mode < EMBER_SMOOTHING_OFF || config->smoothing_mode > EMBER_SMOOTHING_GLOW) {
        config->smoothing_mode = EMBER_SMOOTHING_SOFT;
    }
}

int ember_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    ember_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = ember_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    ember_config_set_defaults(common_config, product_config, product_config_size);
    if (!ember_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        ember_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (ember_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        ember_apply_preset_to_config(preset_key, common_config, config);
    }

    if (ember_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (ember_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = ember_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (ember_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)ember_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (ember_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)ember_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->effect_mode;
    if (ember_read_dword(key, "EffectMode", &value_dword)) {
        config->effect_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (ember_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->resolution_mode;
    if (ember_read_dword(key, "ResolutionMode", &value_dword)) {
        config->resolution_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->smoothing_mode;
    if (ember_read_dword(key, "SmoothingMode", &value_dword)) {
        config->smoothing_mode = (int)value_dword;
    }

    RegCloseKey(key);
    ember_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int ember_config_save(
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
    ember_config safe_product_config;
    screensave_common_config safe_common_config;
    const ember_config *config;

    (void)module;

    config = ember_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    ember_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!ember_build_registry_path(path, sizeof(path))) {
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
        ember_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6501UL,
            "The Ember registry key could not be opened for writing."
        );
        return 0;
    }

    result = ember_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = ember_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_dword(key, "EffectMode", (unsigned long)safe_product_config.effect_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_dword(key, "ResolutionMode", (unsigned long)safe_product_config.resolution_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ember_write_dword(key, "SmoothingMode", (unsigned long)safe_product_config.smoothing_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        ember_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6502UL,
            "The Ember registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT ember_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void ember_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static int ember_get_combo_value(HWND dialog, int control_id, int default_value)
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

static void ember_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_EMBER_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        ember_add_combo_item(dialog, IDC_EMBER_PRESET, module->presets[index].display_name, (LPARAM)index);
    }

    SendDlgItemMessageA(dialog, IDC_EMBER_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        ember_add_combo_item(dialog, IDC_EMBER_THEME, module->themes[index].display_name, (LPARAM)index);
    }

    SendDlgItemMessageA(dialog, IDC_EMBER_EFFECT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ember_effect_items) / sizeof(g_ember_effect_items[0])); ++index) {
        ember_add_combo_item(dialog, IDC_EMBER_EFFECT, g_ember_effect_items[index].display_name, (LPARAM)g_ember_effect_items[index].value);
    }

    SendDlgItemMessageA(dialog, IDC_EMBER_SPEED, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ember_speed_items) / sizeof(g_ember_speed_items[0])); ++index) {
        ember_add_combo_item(dialog, IDC_EMBER_SPEED, g_ember_speed_items[index].display_name, (LPARAM)g_ember_speed_items[index].value);
    }

    SendDlgItemMessageA(dialog, IDC_EMBER_RESOLUTION, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ember_resolution_items) / sizeof(g_ember_resolution_items[0])); ++index) {
        ember_add_combo_item(dialog, IDC_EMBER_RESOLUTION, g_ember_resolution_items[index].display_name, (LPARAM)g_ember_resolution_items[index].value);
    }

    SendDlgItemMessageA(dialog, IDC_EMBER_SMOOTHING, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_ember_smoothing_items) / sizeof(g_ember_smoothing_items[0])); ++index) {
        ember_add_combo_item(dialog, IDC_EMBER_SMOOTHING, g_ember_smoothing_items[index].display_name, (LPARAM)g_ember_smoothing_items[index].value);
    }
}

static void ember_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const ember_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_EMBER_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_EMBER_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    ember_select_combo_value(dialog, IDC_EMBER_EFFECT, (LPARAM)product_config->effect_mode);
    ember_select_combo_value(dialog, IDC_EMBER_SPEED, (LPARAM)product_config->speed_mode);
    ember_select_combo_value(dialog, IDC_EMBER_RESOLUTION, (LPARAM)product_config->resolution_mode);
    ember_select_combo_value(dialog, IDC_EMBER_SMOOTHING, (LPARAM)product_config->smoothing_mode);

    CheckDlgButton(dialog, IDC_EMBER_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_EMBER_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void ember_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    ember_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_EMBER_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_EMBER_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.effect_mode = EMBER_EFFECT_FIRE;
    product_config.speed_mode = EMBER_SPEED_GENTLE;
    product_config.resolution_mode = EMBER_RESOLUTION_STANDARD;
    product_config.smoothing_mode = EMBER_SMOOTHING_SOFT;
    ember_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    ember_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_EMBER_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void ember_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    ember_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    ember_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_EMBER_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        ember_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_EMBER_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->effect_mode = ember_get_combo_value(dialog, IDC_EMBER_EFFECT, product_config->effect_mode);
    product_config->speed_mode = ember_get_combo_value(dialog, IDC_EMBER_SPEED, product_config->speed_mode);
    product_config->resolution_mode = ember_get_combo_value(dialog, IDC_EMBER_RESOLUTION, product_config->resolution_mode);
    product_config->smoothing_mode = ember_get_combo_value(dialog, IDC_EMBER_SMOOTHING, product_config->smoothing_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_EMBER_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_EMBER_DIAGNOSTICS) == BST_CHECKED;
}

static void ember_initialize_dialog(HWND dialog, ember_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    ember_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Ember\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nPalette-driven plasma, fire, and interference motion.");
    SetDlgItemTextA(dialog, IDC_EMBER_INFO, info);

    ember_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK ember_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    ember_dialog_state *dialog_state;

    dialog_state = (ember_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (ember_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            ember_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_EMBER_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            ember_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_EMBER_DEFAULTS) {
            ember_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            ember_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            ember_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            ember_config_clamp(
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

INT_PTR ember_config_show_dialog(
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
    ember_dialog_state dialog_state;
    ember_config *config;

    config = ember_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        ember_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6503UL,
            "The Ember dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_EMBER_CONFIG),
        owner_window,
        ember_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        ember_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6504UL,
            "The Ember config dialog resource could not be loaded."
        );
    }

    return result;
}
