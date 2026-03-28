#include "mechanical_dreams_internal.h"
#include "mechanical_dreams_resource.h"
#include "screensave/version.h"

typedef struct mechanical_dreams_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    mechanical_dreams_config *product_config;
} mechanical_dreams_dialog_state;

typedef struct mechanical_dreams_combo_item_tag {
    int value;
    const char *display_name;
} mechanical_dreams_combo_item;

static const mechanical_dreams_combo_item g_mechanical_dreams_scene_items[] = {
    { MECHANICAL_DREAMS_SCENE_GEAR_TRAIN, "Gear Train" },
    { MECHANICAL_DREAMS_SCENE_CAM_BANK, "Cam Bank" },
    { MECHANICAL_DREAMS_SCENE_DIAL_ASSEMBLY, "Dial Assembly" }
};

static const mechanical_dreams_combo_item g_mechanical_dreams_speed_items[] = {
    { MECHANICAL_DREAMS_SPEED_PATIENT, "Patient" },
    { MECHANICAL_DREAMS_SPEED_STANDARD, "Standard" },
    { MECHANICAL_DREAMS_SPEED_BRISK, "Brisk" }
};

static const mechanical_dreams_combo_item g_mechanical_dreams_density_items[] = {
    { MECHANICAL_DREAMS_DENSITY_SPARSE, "Sparse" },
    { MECHANICAL_DREAMS_DENSITY_STANDARD, "Standard" },
    { MECHANICAL_DREAMS_DENSITY_DENSE, "Dense" }
};

static mechanical_dreams_config *mechanical_dreams_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(mechanical_dreams_config)) {
        return NULL;
    }

    return (mechanical_dreams_config *)product_config;
}

static const mechanical_dreams_config *mechanical_dreams_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(mechanical_dreams_config)) {
        return NULL;
    }

    return (const mechanical_dreams_config *)product_config;
}

static int mechanical_dreams_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(MECHANICAL_DREAMS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, MECHANICAL_DREAMS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int mechanical_dreams_read_flag(HKEY key, const char *value_name, int *value)
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

static int mechanical_dreams_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int mechanical_dreams_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG mechanical_dreams_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG mechanical_dreams_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG mechanical_dreams_write_string(HKEY key, const char *value_name, const char *value)
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

void mechanical_dreams_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    mechanical_dreams_config *config;

    config = mechanical_dreams_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->scene_mode = MECHANICAL_DREAMS_SCENE_GEAR_TRAIN;
    config->speed_mode = MECHANICAL_DREAMS_SPEED_STANDARD;
    config->density_mode = MECHANICAL_DREAMS_DENSITY_STANDARD;
    mechanical_dreams_apply_preset_to_config(MECHANICAL_DREAMS_DEFAULT_PRESET_KEY, common_config, config);
}

void mechanical_dreams_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    mechanical_dreams_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = mechanical_dreams_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (
        common_config->preset_key != NULL &&
        mechanical_dreams_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = MECHANICAL_DREAMS_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        mechanical_dreams_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        presets = mechanical_dreams_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL
            ? preset_descriptor->theme_key
            : MECHANICAL_DREAMS_DEFAULT_THEME_KEY;
    }

    if (
        config->scene_mode < MECHANICAL_DREAMS_SCENE_GEAR_TRAIN ||
        config->scene_mode > MECHANICAL_DREAMS_SCENE_DIAL_ASSEMBLY
    ) {
        config->scene_mode = MECHANICAL_DREAMS_SCENE_GEAR_TRAIN;
    }
    if (
        config->speed_mode < MECHANICAL_DREAMS_SPEED_PATIENT ||
        config->speed_mode > MECHANICAL_DREAMS_SPEED_BRISK
    ) {
        config->speed_mode = MECHANICAL_DREAMS_SPEED_STANDARD;
    }
    if (
        config->density_mode < MECHANICAL_DREAMS_DENSITY_SPARSE ||
        config->density_mode > MECHANICAL_DREAMS_DENSITY_DENSE
    ) {
        config->density_mode = MECHANICAL_DREAMS_DENSITY_STANDARD;
    }
}

int mechanical_dreams_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    mechanical_dreams_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = mechanical_dreams_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    mechanical_dreams_config_set_defaults(common_config, product_config, product_config_size);
    if (!mechanical_dreams_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        mechanical_dreams_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (mechanical_dreams_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        mechanical_dreams_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (mechanical_dreams_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (mechanical_dreams_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = mechanical_dreams_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (mechanical_dreams_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    (void)mechanical_dreams_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    value_dword = common_config->deterministic_seed;
    if (mechanical_dreams_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)mechanical_dreams_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->scene_mode;
    if (mechanical_dreams_read_dword(key, "SceneMode", &value_dword)) {
        config->scene_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (mechanical_dreams_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->density_mode;
    if (mechanical_dreams_read_dword(key, "DensityMode", &value_dword)) {
        config->density_mode = (int)value_dword;
    }

    RegCloseKey(key);
    mechanical_dreams_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int mechanical_dreams_config_save(
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
    mechanical_dreams_config safe_product_config;
    const mechanical_dreams_config *config;

    (void)module;
    (void)diagnostics;

    config = mechanical_dreams_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    mechanical_dreams_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!mechanical_dreams_build_registry_path(path, sizeof(path))) {
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

    result = mechanical_dreams_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_flag(
            key,
            "UseDeterministicSeed",
            safe_common_config.use_deterministic_seed
        );
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_dword(key, "SceneMode", (unsigned long)safe_product_config.scene_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanical_dreams_write_dword(key, "DensityMode", (unsigned long)safe_product_config.density_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static void mechanical_dreams_populate_combo(
    HWND dialog,
    int control_id,
    const mechanical_dreams_combo_item *items,
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
        combo_index = SendDlgItemMessageA(
            dialog,
            control_id,
            CB_ADDSTRING,
            0U,
            (LPARAM)items[index].display_name
        );
        if (combo_index != CB_ERR && combo_index != CB_ERRSPACE) {
            SendDlgItemMessageA(
                dialog,
                control_id,
                CB_SETITEMDATA,
                (WPARAM)combo_index,
                (LPARAM)items[index].value
            );
        }
    }
}

static void mechanical_dreams_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_MECHANICAL_DREAMS_PRESET,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->presets[index].display_name
        );
    }

    SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_MECHANICAL_DREAMS_THEME,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->themes[index].display_name
        );
    }

    mechanical_dreams_populate_combo(dialog, IDC_MECHANICAL_DREAMS_SCENE, g_mechanical_dreams_scene_items, 3U);
    mechanical_dreams_populate_combo(dialog, IDC_MECHANICAL_DREAMS_SPEED, g_mechanical_dreams_speed_items, 3U);
    mechanical_dreams_populate_combo(dialog, IDC_MECHANICAL_DREAMS_DENSITY, g_mechanical_dreams_density_items, 3U);
}

static void mechanical_dreams_select_combo_value(HWND dialog, int control_id, LPARAM value)
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

static int mechanical_dreams_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void mechanical_dreams_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const mechanical_dreams_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    mechanical_dreams_select_combo_value(dialog, IDC_MECHANICAL_DREAMS_SCENE, (LPARAM)product_config->scene_mode);
    mechanical_dreams_select_combo_value(dialog, IDC_MECHANICAL_DREAMS_SPEED, (LPARAM)product_config->speed_mode);
    mechanical_dreams_select_combo_value(dialog, IDC_MECHANICAL_DREAMS_DENSITY, (LPARAM)product_config->density_mode);
    CheckDlgButton(
        dialog,
        IDC_MECHANICAL_DREAMS_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_MECHANICAL_DREAMS_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void mechanical_dreams_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    mechanical_dreams_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_MECHANICAL_DREAMS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.scene_mode = MECHANICAL_DREAMS_SCENE_GEAR_TRAIN;
    product_config.speed_mode = MECHANICAL_DREAMS_SPEED_STANDARD;
    product_config.density_mode = MECHANICAL_DREAMS_DENSITY_STANDARD;
    mechanical_dreams_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    mechanical_dreams_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_MECHANICAL_DREAMS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void mechanical_dreams_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    mechanical_dreams_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    mechanical_dreams_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        mechanical_dreams_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_MECHANICAL_DREAMS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->scene_mode = mechanical_dreams_get_combo_value(dialog, IDC_MECHANICAL_DREAMS_SCENE, product_config->scene_mode);
    product_config->speed_mode = mechanical_dreams_get_combo_value(dialog, IDC_MECHANICAL_DREAMS_SPEED, product_config->speed_mode);
    product_config->density_mode = mechanical_dreams_get_combo_value(dialog, IDC_MECHANICAL_DREAMS_DENSITY, product_config->density_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_MECHANICAL_DREAMS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_MECHANICAL_DREAMS_DIAGNOSTICS) == BST_CHECKED;
}

static void mechanical_dreams_initialize_dialog(
    HWND dialog,
    mechanical_dreams_dialog_state *dialog_state
)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    mechanical_dreams_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Mechanical Dreams\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nCoupled gears, cams, and dials with restrained mechanical choreography.");
    SetDlgItemTextA(dialog, IDC_MECHANICAL_DREAMS_INFO, info);

    mechanical_dreams_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK mechanical_dreams_config_dialog_proc(
    HWND dialog,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    mechanical_dreams_dialog_state *dialog_state;

    dialog_state = (mechanical_dreams_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (mechanical_dreams_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            mechanical_dreams_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_MECHANICAL_DREAMS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            mechanical_dreams_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_MECHANICAL_DREAMS_DEFAULTS) {
            mechanical_dreams_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            mechanical_dreams_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            mechanical_dreams_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            mechanical_dreams_config_clamp(
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

INT_PTR mechanical_dreams_config_show_dialog(
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
    mechanical_dreams_dialog_state dialog_state;
    mechanical_dreams_config *config;

    (void)diagnostics;

    config = mechanical_dreams_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_MECHANICAL_DREAMS_CONFIG),
        owner_window,
        mechanical_dreams_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    return result;
}
