#include "mechanize_internal.h"
#include "mechanize_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct mechanize_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    mechanize_config *product_config;
} mechanize_dialog_state;

typedef struct mechanize_combo_item_tag {
    int value;
    const char *display_name;
} mechanize_combo_item;

static const mechanize_combo_item g_mechanize_scene_items[] = {
    { MECHANIZE_SCENE_GEAR_TRAIN, "Gear Train" },
    { MECHANIZE_SCENE_CAM_BANK, "Cam Bank" },
    { MECHANIZE_SCENE_DIAL_ASSEMBLY, "Dial Assembly" }
};

static const mechanize_combo_item g_mechanize_speed_items[] = {
    { MECHANIZE_SPEED_PATIENT, "Patient" },
    { MECHANIZE_SPEED_STANDARD, "Standard" },
    { MECHANIZE_SPEED_BRISK, "Brisk" }
};

static const mechanize_combo_item g_mechanize_density_items[] = {
    { MECHANIZE_DENSITY_SPARSE, "Sparse" },
    { MECHANIZE_DENSITY_STANDARD, "Standard" },
    { MECHANIZE_DENSITY_DENSE, "Dense" }
};

static mechanize_config *mechanize_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(mechanize_config)) {
        return NULL;
    }

    return (mechanize_config *)product_config;
}

static const mechanize_config *mechanize_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(mechanize_config)) {
        return NULL;
    }

    return (const mechanize_config *)product_config;
}

static int mechanize_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(MECHANIZE_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, MECHANIZE_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int mechanize_read_flag(HKEY key, const char *value_name, int *value)
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

static int mechanize_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int mechanize_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG mechanize_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG mechanize_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG mechanize_write_string(HKEY key, const char *value_name, const char *value)
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

void mechanize_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    mechanize_config *config;

    config = mechanize_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->scene_mode = MECHANIZE_SCENE_GEAR_TRAIN;
    config->speed_mode = MECHANIZE_SPEED_STANDARD;
    config->density_mode = MECHANIZE_DENSITY_STANDARD;
    mechanize_apply_preset_to_config(MECHANIZE_DEFAULT_PRESET_KEY, common_config, config);
}

void mechanize_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    mechanize_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = mechanize_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (
        common_config->preset_key != NULL &&
        mechanize_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = MECHANIZE_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        mechanize_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        presets = mechanize_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL
            ? preset_descriptor->theme_key
            : MECHANIZE_DEFAULT_THEME_KEY;
    }

    if (
        config->scene_mode < MECHANIZE_SCENE_GEAR_TRAIN ||
        config->scene_mode > MECHANIZE_SCENE_DIAL_ASSEMBLY
    ) {
        config->scene_mode = MECHANIZE_SCENE_GEAR_TRAIN;
    }
    if (
        config->speed_mode < MECHANIZE_SPEED_PATIENT ||
        config->speed_mode > MECHANIZE_SPEED_BRISK
    ) {
        config->speed_mode = MECHANIZE_SPEED_STANDARD;
    }
    if (
        config->density_mode < MECHANIZE_DENSITY_SPARSE ||
        config->density_mode > MECHANIZE_DENSITY_DENSE
    ) {
        config->density_mode = MECHANIZE_DENSITY_STANDARD;
    }
}

int mechanize_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    mechanize_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)diagnostics;

    config = mechanize_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    mechanize_config_set_defaults(common_config, product_config, product_config_size);
    if (!mechanize_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "mechanize"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            mechanize_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (mechanize_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        mechanize_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (mechanize_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (mechanize_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = mechanize_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (mechanize_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    (void)mechanize_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    value_dword = common_config->deterministic_seed;
    if (mechanize_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)mechanize_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->scene_mode;
    if (mechanize_read_dword(key, "SceneMode", &value_dword)) {
        config->scene_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (mechanize_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->density_mode;
    if (mechanize_read_dword(key, "DensityMode", &value_dword)) {
        config->density_mode = (int)value_dword;
    }

    RegCloseKey(key);
    mechanize_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int mechanize_config_save(
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
    mechanize_config safe_product_config;
    const mechanize_config *config;

    (void)module;
    (void)diagnostics;

    config = mechanize_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    mechanize_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!mechanize_build_registry_path(path, sizeof(path))) {
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

    result = mechanize_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_flag(
            key,
            "UseDeterministicSeed",
            safe_common_config.use_deterministic_seed
        );
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_dword(key, "SceneMode", (unsigned long)safe_product_config.scene_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = mechanize_write_dword(key, "DensityMode", (unsigned long)safe_product_config.density_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static void mechanize_populate_combo(
    HWND dialog,
    int control_id,
    const mechanize_combo_item *items,
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

static void mechanize_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_MECHANIZE_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_MECHANIZE_PRESET,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->presets[index].display_name
        );
    }

    SendDlgItemMessageA(dialog, IDC_MECHANIZE_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_MECHANIZE_THEME,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->themes[index].display_name
        );
    }

    mechanize_populate_combo(dialog, IDC_MECHANIZE_SCENE, g_mechanize_scene_items, 3U);
    mechanize_populate_combo(dialog, IDC_MECHANIZE_SPEED, g_mechanize_speed_items, 3U);
    mechanize_populate_combo(dialog, IDC_MECHANIZE_DENSITY, g_mechanize_density_items, 3U);
}

static void mechanize_select_combo_value(HWND dialog, int control_id, LPARAM value)
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

static int mechanize_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void mechanize_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const mechanize_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_MECHANIZE_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_MECHANIZE_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    mechanize_select_combo_value(dialog, IDC_MECHANIZE_SCENE, (LPARAM)product_config->scene_mode);
    mechanize_select_combo_value(dialog, IDC_MECHANIZE_SPEED, (LPARAM)product_config->speed_mode);
    mechanize_select_combo_value(dialog, IDC_MECHANIZE_DENSITY, (LPARAM)product_config->density_mode);
    CheckDlgButton(
        dialog,
        IDC_MECHANIZE_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_MECHANIZE_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void mechanize_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    mechanize_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_MECHANIZE_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_MECHANIZE_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.scene_mode = MECHANIZE_SCENE_GEAR_TRAIN;
    product_config.speed_mode = MECHANIZE_SPEED_STANDARD;
    product_config.density_mode = MECHANIZE_DENSITY_STANDARD;
    mechanize_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    mechanize_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_MECHANIZE_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void mechanize_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    mechanize_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    mechanize_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_MECHANIZE_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        mechanize_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_MECHANIZE_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->scene_mode = mechanize_get_combo_value(dialog, IDC_MECHANIZE_SCENE, product_config->scene_mode);
    product_config->speed_mode = mechanize_get_combo_value(dialog, IDC_MECHANIZE_SPEED, product_config->speed_mode);
    product_config->density_mode = mechanize_get_combo_value(dialog, IDC_MECHANIZE_DENSITY, product_config->density_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_MECHANIZE_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_MECHANIZE_DIAGNOSTICS) == BST_CHECKED;
}

static void mechanize_initialize_dialog(
    HWND dialog,
    mechanize_dialog_state *dialog_state
)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    mechanize_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Mechanize\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nCoupled gears, cams, and dials with restrained exhibit choreography, calmer preview pacing, and cleaner scene variants.");
    SetDlgItemTextA(dialog, IDC_MECHANIZE_INFO, info);

    mechanize_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK mechanize_config_dialog_proc(
    HWND dialog,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    mechanize_dialog_state *dialog_state;

    dialog_state = (mechanize_dialog_state *)GetWindowLongPtrA(dialog, DWLP_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (mechanize_dialog_state *)lParam;
        SetWindowLongPtrA(dialog, DWLP_USER, (LONG_PTR)dialog_state);
        if (dialog_state != NULL) {
            mechanize_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_MECHANIZE_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            mechanize_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_MECHANIZE_DEFAULTS) {
            mechanize_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            mechanize_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            mechanize_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            mechanize_config_clamp(
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

INT_PTR mechanize_config_show_dialog(
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
    mechanize_dialog_state dialog_state;
    mechanize_config *config;

    (void)diagnostics;

    config = mechanize_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_MECHANIZE_CONFIG),
        owner_window,
        mechanize_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    return result;
}

static int mechanize_parse_scene_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "gear_train") == 0) {
        *value_out = MECHANIZE_SCENE_GEAR_TRAIN;
        return 1;
    }
    if (lstrcmpiA(text, "cam_bank") == 0) {
        *value_out = MECHANIZE_SCENE_CAM_BANK;
        return 1;
    }
    if (lstrcmpiA(text, "dial_assembly") == 0) {
        *value_out = MECHANIZE_SCENE_DIAL_ASSEMBLY;
        return 1;
    }
    return 0;
}

static int mechanize_parse_speed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "patient") == 0) {
        *value_out = MECHANIZE_SPEED_PATIENT;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = MECHANIZE_SPEED_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "brisk") == 0) {
        *value_out = MECHANIZE_SPEED_BRISK;
        return 1;
    }
    return 0;
}

static int mechanize_parse_density_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "sparse") == 0) {
        *value_out = MECHANIZE_DENSITY_SPARSE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = MECHANIZE_DENSITY_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "dense") == 0) {
        *value_out = MECHANIZE_DENSITY_DENSE;
        return 1;
    }
    return 0;
}

int mechanize_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const mechanize_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = mechanize_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "scene", mechanize_scene_mode_name(config->scene_mode)) &&
        writer->write_string(writer->context, "product", "speed", mechanize_speed_mode_name(config->speed_mode)) &&
        writer->write_string(writer->context, "product", "density", mechanize_density_mode_name(config->density_mode));
}

int mechanize_config_import_settings_entry(
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
    mechanize_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = mechanize_as_config(product_config, product_config_size);
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
        return mechanize_parse_scene_mode(value, &config->scene_mode);
    }
    if (lstrcmpiA(key, "speed") == 0 || lstrcmpiA(key, "speed_mode") == 0) {
        return mechanize_parse_speed_mode(value, &config->speed_mode);
    }
    if (lstrcmpiA(key, "density") == 0 || lstrcmpiA(key, "density_mode") == 0) {
        return mechanize_parse_density_mode(value, &config->density_mode);
    }

    return 1;
}

void mechanize_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    mechanize_config *config;
    mechanize_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = mechanize_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x4D454342UL;
    mechanize_rng_seed(&rng, random_seed ^ 0x4D454342UL);

    roll = mechanize_rng_range(&rng, 100UL);
    if (roll < 36UL) {
        config->scene_mode = MECHANIZE_SCENE_GEAR_TRAIN;
    } else if (roll < 66UL) {
        config->scene_mode = MECHANIZE_SCENE_CAM_BANK;
    } else {
        config->scene_mode = MECHANIZE_SCENE_DIAL_ASSEMBLY;
    }

    roll = mechanize_rng_range(&rng, 100UL);
    if (roll < 38UL) {
        config->speed_mode = MECHANIZE_SPEED_PATIENT;
    } else if (roll < 80UL) {
        config->speed_mode = MECHANIZE_SPEED_STANDARD;
    } else {
        config->speed_mode = MECHANIZE_SPEED_BRISK;
    }

    roll = mechanize_rng_range(&rng, 100UL);
    if (roll < 28UL) {
        config->density_mode = MECHANIZE_DENSITY_SPARSE;
    } else if (roll < 72UL) {
        config->density_mode = MECHANIZE_DENSITY_STANDARD;
    } else {
        config->density_mode = MECHANIZE_DENSITY_DENSE;
    }
}
