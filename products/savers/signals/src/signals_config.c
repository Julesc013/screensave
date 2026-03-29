#include "signals_internal.h"
#include "signals_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct signals_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    signals_config *product_config;
} signals_dialog_state;

typedef struct signals_combo_item_tag {
    int value;
    const char *display_name;
} signals_combo_item;

static const signals_combo_item g_signals_layout_items[] = {
    { SIGNALS_LAYOUT_OPERATIONS, "Operations" },
    { SIGNALS_LAYOUT_SPECTRUM, "Spectrum" },
    { SIGNALS_LAYOUT_TELEMETRY, "Telemetry" }
};

static const signals_combo_item g_signals_activity_items[] = {
    { SIGNALS_ACTIVITY_QUIET, "Quiet" },
    { SIGNALS_ACTIVITY_STANDARD, "Standard" },
    { SIGNALS_ACTIVITY_BUSY, "Busy" }
};

static const signals_combo_item g_signals_overlay_items[] = {
    { SIGNALS_OVERLAY_OFF, "Off" },
    { SIGNALS_OVERLAY_GRID, "Grid" },
    { SIGNALS_OVERLAY_SCAN, "Scan" }
};

static signals_config *signals_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(signals_config)) {
        return NULL;
    }

    return (signals_config *)product_config;
}

static const signals_config *signals_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(signals_config)) {
        return NULL;
    }

    return (const signals_config *)product_config;
}

static int signals_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(SIGNALS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, SIGNALS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int signals_read_flag(HKEY key, const char *value_name, int *value)
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

static int signals_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int signals_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG signals_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG signals_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG signals_write_string(HKEY key, const char *value_name, const char *value)
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

void signals_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    signals_config *config;

    config = signals_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->layout_mode = SIGNALS_LAYOUT_OPERATIONS;
    config->activity_mode = SIGNALS_ACTIVITY_STANDARD;
    config->overlay_mode = SIGNALS_OVERLAY_GRID;
    signals_apply_preset_to_config(SIGNALS_DEFAULT_PRESET_KEY, common_config, config);
}

void signals_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    signals_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = signals_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (common_config->preset_key != NULL && signals_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = SIGNALS_DEFAULT_PRESET_KEY;
    }

    if (common_config->theme_key == NULL || signals_find_theme_descriptor(common_config->theme_key) == NULL) {
        presets = signals_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL
            ? preset_descriptor->theme_key
            : SIGNALS_DEFAULT_THEME_KEY;
    }

    if (config->layout_mode < SIGNALS_LAYOUT_OPERATIONS || config->layout_mode > SIGNALS_LAYOUT_TELEMETRY) {
        config->layout_mode = SIGNALS_LAYOUT_OPERATIONS;
    }
    if (config->activity_mode < SIGNALS_ACTIVITY_QUIET || config->activity_mode > SIGNALS_ACTIVITY_BUSY) {
        config->activity_mode = SIGNALS_ACTIVITY_STANDARD;
    }
    if (config->overlay_mode < SIGNALS_OVERLAY_OFF || config->overlay_mode > SIGNALS_OVERLAY_SCAN) {
        config->overlay_mode = SIGNALS_OVERLAY_GRID;
    }
}

int signals_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    signals_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)diagnostics;

    config = signals_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    signals_config_set_defaults(common_config, product_config, product_config_size);
    if (!signals_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "signals"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            signals_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (signals_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        signals_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (signals_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (signals_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = signals_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (signals_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    (void)signals_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    value_dword = common_config->deterministic_seed;
    if (signals_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)signals_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->layout_mode;
    if (signals_read_dword(key, "LayoutMode", &value_dword)) {
        config->layout_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->activity_mode;
    if (signals_read_dword(key, "ActivityMode", &value_dword)) {
        config->activity_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->overlay_mode;
    if (signals_read_dword(key, "OverlayMode", &value_dword)) {
        config->overlay_mode = (int)value_dword;
    }

    RegCloseKey(key);
    signals_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int signals_config_save(
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
    screensave_common_config safe_common_config;
    signals_config safe_product_config;
    const signals_config *config;

    (void)module;
    (void)diagnostics;

    config = signals_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    signals_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!signals_build_registry_path(path, sizeof(path))) {
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

    result = signals_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = signals_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_dword(key, "LayoutMode", (unsigned long)safe_product_config.layout_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_dword(key, "ActivityMode", (unsigned long)safe_product_config.activity_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = signals_write_dword(key, "OverlayMode", (unsigned long)safe_product_config.overlay_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static void signals_populate_combo(
    HWND dialog,
    int control_id,
    const signals_combo_item *items,
    unsigned int item_count
)
{
    unsigned int index;
    LRESULT combo_index;

    if (dialog == NULL || items == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < item_count; ++index) {
        combo_index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)items[index].display_name);
        if (combo_index != CB_ERR && combo_index != CB_ERRSPACE) {
            SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)combo_index, (LPARAM)items[index].value);
        }
    }
}

static void signals_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_SIGNALS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_SIGNALS_PRESET, CB_ADDSTRING, 0U, (LPARAM)module->presets[index].display_name);
    }

    SendDlgItemMessageA(dialog, IDC_SIGNALS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_SIGNALS_THEME, CB_ADDSTRING, 0U, (LPARAM)module->themes[index].display_name);
    }

    signals_populate_combo(dialog, IDC_SIGNALS_LAYOUT, g_signals_layout_items, 3U);
    signals_populate_combo(dialog, IDC_SIGNALS_ACTIVITY, g_signals_activity_items, 3U);
    signals_populate_combo(dialog, IDC_SIGNALS_OVERLAY, g_signals_overlay_items, 3U);
}

static void signals_select_combo_value(HWND dialog, int control_id, LPARAM value)
{
    LRESULT item_count;
    LRESULT index;

    item_count = SendDlgItemMessageA(dialog, control_id, CB_GETCOUNT, 0U, 0L);
    if (item_count == CB_ERR) {
        return;
    }

    for (index = 0L; index < item_count; ++index) {
        if (SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L) == value) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
            return;
        }
    }
}

static int signals_get_combo_value(HWND dialog, int control_id, int fallback_value)
{
    LRESULT selection;
    LRESULT value;

    selection = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return fallback_value;
    }

    value = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)selection, 0L);
    if (value == CB_ERR) {
        return fallback_value;
    }

    return (int)value;
}

static void signals_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const signals_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_SIGNALS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_SIGNALS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    signals_select_combo_value(dialog, IDC_SIGNALS_LAYOUT, (LPARAM)product_config->layout_mode);
    signals_select_combo_value(dialog, IDC_SIGNALS_ACTIVITY, (LPARAM)product_config->activity_mode);
    signals_select_combo_value(dialog, IDC_SIGNALS_OVERLAY, (LPARAM)product_config->overlay_mode);
    CheckDlgButton(dialog, IDC_SIGNALS_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_SIGNALS_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void signals_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    signals_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_SIGNALS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_SIGNALS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.layout_mode = SIGNALS_LAYOUT_OPERATIONS;
    product_config.activity_mode = SIGNALS_ACTIVITY_STANDARD;
    product_config.overlay_mode = SIGNALS_OVERLAY_GRID;
    signals_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    signals_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_SIGNALS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void signals_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    signals_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    signals_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_SIGNALS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        signals_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_SIGNALS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->layout_mode = signals_get_combo_value(dialog, IDC_SIGNALS_LAYOUT, product_config->layout_mode);
    product_config->activity_mode = signals_get_combo_value(dialog, IDC_SIGNALS_ACTIVITY, product_config->activity_mode);
    product_config->overlay_mode = signals_get_combo_value(dialog, IDC_SIGNALS_OVERLAY, product_config->overlay_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_SIGNALS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_SIGNALS_DIAGNOSTICS) == BST_CHECKED;
}

static void signals_initialize_dialog(HWND dialog, signals_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    signals_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Signals\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nSynthetic scopes, counters, and status panes with restrained system cadence.");
    SetDlgItemTextA(dialog, IDC_SIGNALS_INFO, info);

    signals_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK signals_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    signals_dialog_state *dialog_state;

    dialog_state = (signals_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (signals_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            signals_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_SIGNALS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            signals_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_SIGNALS_DEFAULTS) {
            signals_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            signals_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            signals_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            signals_config_clamp(
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

INT_PTR signals_config_show_dialog(
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
    signals_dialog_state dialog_state;
    signals_config *config;

    (void)diagnostics;

    config = signals_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_SIGNALS_CONFIG),
        owner_window,
        signals_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    return result;
}
