#include "stormglass_internal.h"
#include "stormglass_resource.h"
#include "screensave/version.h"

typedef struct stormglass_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    stormglass_config *product_config;
} stormglass_dialog_state;

typedef struct stormglass_combo_item_tag {
    int value;
    const char *display_name;
} stormglass_combo_item;

static const stormglass_combo_item g_stormglass_scene_items[] = {
    { STORMGLASS_SCENE_RAIN, "Rain On Glass" },
    { STORMGLASS_SCENE_FOGGED, "Fogged Pane" },
    { STORMGLASS_SCENE_WINTER, "Winter Pane" }
};

static const stormglass_combo_item g_stormglass_intensity_items[] = {
    { STORMGLASS_INTENSITY_QUIET, "Quiet" },
    { STORMGLASS_INTENSITY_STANDARD, "Standard" },
    { STORMGLASS_INTENSITY_HEAVY, "Heavy" }
};

static const stormglass_combo_item g_stormglass_pane_items[] = {
    { STORMGLASS_PANE_CLEAR, "Clear" },
    { STORMGLASS_PANE_MISTED, "Misted" },
    { STORMGLASS_PANE_CONDENSED, "Condensed" }
};

static stormglass_config *stormglass_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(stormglass_config)) {
        return NULL;
    }

    return (stormglass_config *)product_config;
}

static const stormglass_config *stormglass_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(stormglass_config)) {
        return NULL;
    }

    return (const stormglass_config *)product_config;
}

static int stormglass_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(STORMGLASS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, STORMGLASS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int stormglass_read_flag(HKEY key, const char *value_name, int *value)
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

static int stormglass_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int stormglass_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG stormglass_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG stormglass_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG stormglass_write_string(HKEY key, const char *value_name, const char *value)
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

void stormglass_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    stormglass_config *config;

    config = stormglass_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->scene_mode = STORMGLASS_SCENE_RAIN;
    config->intensity_mode = STORMGLASS_INTENSITY_QUIET;
    config->pane_mode = STORMGLASS_PANE_MISTED;
    stormglass_apply_preset_to_config(STORMGLASS_DEFAULT_PRESET_KEY, common_config, config);
}

void stormglass_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    stormglass_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = stormglass_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (common_config->preset_key != NULL && stormglass_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = STORMGLASS_DEFAULT_PRESET_KEY;
    }

    if (common_config->theme_key == NULL || stormglass_find_theme_descriptor(common_config->theme_key) == NULL) {
        presets = stormglass_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL
            ? preset_descriptor->theme_key
            : STORMGLASS_DEFAULT_THEME_KEY;
    }

    if (config->scene_mode < STORMGLASS_SCENE_RAIN || config->scene_mode > STORMGLASS_SCENE_WINTER) {
        config->scene_mode = STORMGLASS_SCENE_RAIN;
    }
    if (config->intensity_mode < STORMGLASS_INTENSITY_QUIET || config->intensity_mode > STORMGLASS_INTENSITY_HEAVY) {
        config->intensity_mode = STORMGLASS_INTENSITY_QUIET;
    }
    if (config->pane_mode < STORMGLASS_PANE_CLEAR || config->pane_mode > STORMGLASS_PANE_CONDENSED) {
        config->pane_mode = STORMGLASS_PANE_MISTED;
    }
}

int stormglass_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    stormglass_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = stormglass_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    stormglass_config_set_defaults(common_config, product_config, product_config_size);
    if (!stormglass_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        stormglass_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (stormglass_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        stormglass_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (stormglass_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (stormglass_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = stormglass_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (stormglass_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    (void)stormglass_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    value_dword = common_config->deterministic_seed;
    if (stormglass_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)stormglass_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->scene_mode;
    if (stormglass_read_dword(key, "SceneMode", &value_dword)) {
        config->scene_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->intensity_mode;
    if (stormglass_read_dword(key, "IntensityMode", &value_dword)) {
        config->intensity_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->pane_mode;
    if (stormglass_read_dword(key, "PaneMode", &value_dword)) {
        config->pane_mode = (int)value_dword;
    }

    RegCloseKey(key);
    stormglass_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int stormglass_config_save(
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
    stormglass_config safe_product_config;
    const stormglass_config *config;

    (void)module;
    (void)diagnostics;

    config = stormglass_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    stormglass_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!stormglass_build_registry_path(path, sizeof(path))) {
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

    result = stormglass_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_dword(key, "SceneMode", (unsigned long)safe_product_config.scene_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_dword(key, "IntensityMode", (unsigned long)safe_product_config.intensity_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = stormglass_write_dword(key, "PaneMode", (unsigned long)safe_product_config.pane_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static void stormglass_populate_combo(HWND dialog, int control_id, const stormglass_combo_item *items, unsigned int item_count)
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

static void stormglass_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_STORMGLASS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_STORMGLASS_PRESET, CB_ADDSTRING, 0U, (LPARAM)module->presets[index].display_name);
    }

    SendDlgItemMessageA(dialog, IDC_STORMGLASS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_STORMGLASS_THEME, CB_ADDSTRING, 0U, (LPARAM)module->themes[index].display_name);
    }

    stormglass_populate_combo(dialog, IDC_STORMGLASS_SCENE, g_stormglass_scene_items, 3U);
    stormglass_populate_combo(dialog, IDC_STORMGLASS_INTENSITY, g_stormglass_intensity_items, 3U);
    stormglass_populate_combo(dialog, IDC_STORMGLASS_PANE, g_stormglass_pane_items, 3U);
}

static void stormglass_select_combo_value(HWND dialog, int control_id, LPARAM value)
{
    LRESULT item_count;
    LRESULT index;

    item_count = SendDlgItemMessageA(dialog, control_id, CB_GETCOUNT, 0U, 0L);
    if (item_count == CB_ERR) {
        return;
    }

    for (index = 0; index < item_count; ++index) {
        if (SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L) == value) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
            return;
        }
    }
}

static int stormglass_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void stormglass_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const stormglass_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_STORMGLASS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_STORMGLASS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    stormglass_select_combo_value(dialog, IDC_STORMGLASS_SCENE, (LPARAM)product_config->scene_mode);
    stormglass_select_combo_value(dialog, IDC_STORMGLASS_INTENSITY, (LPARAM)product_config->intensity_mode);
    stormglass_select_combo_value(dialog, IDC_STORMGLASS_PANE, (LPARAM)product_config->pane_mode);
    CheckDlgButton(dialog, IDC_STORMGLASS_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_STORMGLASS_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void stormglass_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    stormglass_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_STORMGLASS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_STORMGLASS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.scene_mode = STORMGLASS_SCENE_RAIN;
    product_config.intensity_mode = STORMGLASS_INTENSITY_QUIET;
    product_config.pane_mode = STORMGLASS_PANE_MISTED;
    stormglass_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    stormglass_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_STORMGLASS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void stormglass_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    stormglass_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    stormglass_config_set_defaults(common_config, product_config, sizeof(*product_config));
    preset_index = SendDlgItemMessageA(dialog, IDC_STORMGLASS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        stormglass_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_STORMGLASS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->scene_mode = stormglass_get_combo_value(dialog, IDC_STORMGLASS_SCENE, product_config->scene_mode);
    product_config->intensity_mode = stormglass_get_combo_value(dialog, IDC_STORMGLASS_INTENSITY, product_config->intensity_mode);
    product_config->pane_mode = stormglass_get_combo_value(dialog, IDC_STORMGLASS_PANE, product_config->pane_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_STORMGLASS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_STORMGLASS_DIAGNOSTICS) == BST_CHECKED;
}

static void stormglass_initialize_dialog(HWND dialog, stormglass_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    stormglass_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Stormglass\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nCalm pane weather studies with droplets, fog bands, and distant-light atmosphere.");
    SetDlgItemTextA(dialog, IDC_STORMGLASS_INFO, info);

    stormglass_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK stormglass_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    stormglass_dialog_state *dialog_state;

    dialog_state = (stormglass_dialog_state *)GetWindowLongPtrA(dialog, DWLP_USER);
    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (stormglass_dialog_state *)lParam;
        SetWindowLongPtrA(dialog, DWLP_USER, (LONG_PTR)dialog_state);
        if (dialog_state != NULL) {
            stormglass_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;
    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_STORMGLASS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            stormglass_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_STORMGLASS_DEFAULTS) {
            stormglass_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            stormglass_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            stormglass_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            stormglass_config_clamp(
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

INT_PTR stormglass_config_show_dialog(
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
    stormglass_dialog_state dialog_state;
    stormglass_config *config;

    (void)diagnostics;

    config = stormglass_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_STORMGLASS_CONFIG),
        owner_window,
        stormglass_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    return result;
}

static int stormglass_parse_scene_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "rain") == 0 || lstrcmpiA(text, "rain_on_glass") == 0) {
        *value_out = STORMGLASS_SCENE_RAIN;
        return 1;
    }
    if (lstrcmpiA(text, "fogged") == 0 || lstrcmpiA(text, "fogged_pane") == 0) {
        *value_out = STORMGLASS_SCENE_FOGGED;
        return 1;
    }
    if (lstrcmpiA(text, "winter") == 0 || lstrcmpiA(text, "winter_pane") == 0) {
        *value_out = STORMGLASS_SCENE_WINTER;
        return 1;
    }
    return 0;
}

static int stormglass_parse_intensity_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "quiet") == 0) {
        *value_out = STORMGLASS_INTENSITY_QUIET;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = STORMGLASS_INTENSITY_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "heavy") == 0) {
        *value_out = STORMGLASS_INTENSITY_HEAVY;
        return 1;
    }
    return 0;
}

static int stormglass_parse_pane_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "clear") == 0) {
        *value_out = STORMGLASS_PANE_CLEAR;
        return 1;
    }
    if (lstrcmpiA(text, "misted") == 0 || lstrcmpiA(text, "misted_pane") == 0) {
        *value_out = STORMGLASS_PANE_MISTED;
        return 1;
    }
    if (lstrcmpiA(text, "condensed") == 0 || lstrcmpiA(text, "condensed_pane") == 0) {
        *value_out = STORMGLASS_PANE_CONDENSED;
        return 1;
    }
    return 0;
}

int stormglass_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const stormglass_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = stormglass_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "scene", stormglass_scene_mode_name(config->scene_mode)) &&
        writer->write_string(writer->context, "product", "intensity", stormglass_intensity_mode_name(config->intensity_mode)) &&
        writer->write_string(writer->context, "product", "pane", stormglass_pane_mode_name(config->pane_mode));
}

int stormglass_config_import_settings_entry(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    const char *section,
    const char *key,
    const char *value,
    screensave_diag_context *diagnostics
)
{
    stormglass_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = stormglass_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "scene") == 0 || lstrcmpiA(key, "scene_mode") == 0) {
        return stormglass_parse_scene_mode(value, &config->scene_mode);
    }
    if (lstrcmpiA(key, "intensity") == 0 || lstrcmpiA(key, "intensity_mode") == 0) {
        return stormglass_parse_intensity_mode(value, &config->intensity_mode);
    }
    if (lstrcmpiA(key, "pane") == 0 || lstrcmpiA(key, "pane_mode") == 0) {
        return stormglass_parse_pane_mode(value, &config->pane_mode);
    }

    return 1;
}

void stormglass_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    stormglass_config *config;
    stormglass_rng_state rng;
    unsigned long random_seed;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = stormglass_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x53746731UL;
    stormglass_rng_seed(&rng, random_seed ^ 0x53746731UL);
    config->scene_mode = (int)stormglass_rng_range(&rng, 3UL);
    config->intensity_mode = (int)stormglass_rng_range(&rng, 3UL);
    config->pane_mode = (int)stormglass_rng_range(&rng, 3UL);
}
