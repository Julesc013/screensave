#include "ecosystems_internal.h"
#include "ecosystems_resource.h"
#include "screensave/version.h"

typedef struct ecosystems_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    ecosystems_config *product_config;
} ecosystems_dialog_state;

typedef struct ecosystems_combo_item_tag {
    int value;
    const char *display_name;
} ecosystems_combo_item;

static const ecosystems_combo_item g_ecosystems_habitat_items[] = {
    { ECOSYSTEMS_HABITAT_AQUARIUM, "Aquarium" },
    { ECOSYSTEMS_HABITAT_AVIARY, "Aviary" },
    { ECOSYSTEMS_HABITAT_FIREFLIES, "Fireflies" }
};

static const ecosystems_combo_item g_ecosystems_density_items[] = {
    { ECOSYSTEMS_DENSITY_SPARSE, "Sparse" },
    { ECOSYSTEMS_DENSITY_STANDARD, "Standard" },
    { ECOSYSTEMS_DENSITY_LUSH, "Lush" }
};

static const ecosystems_combo_item g_ecosystems_activity_items[] = {
    { ECOSYSTEMS_ACTIVITY_CALM, "Calm" },
    { ECOSYSTEMS_ACTIVITY_STANDARD, "Standard" },
    { ECOSYSTEMS_ACTIVITY_LIVELY, "Lively" }
};

static ecosystems_config *ecosystems_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(ecosystems_config)) {
        return NULL;
    }

    return (ecosystems_config *)product_config;
}

static const ecosystems_config *ecosystems_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(ecosystems_config)) {
        return NULL;
    }

    return (const ecosystems_config *)product_config;
}

static int ecosystems_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(ECOSYSTEMS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, ECOSYSTEMS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int ecosystems_read_flag(HKEY key, const char *value_name, int *value)
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

static int ecosystems_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int ecosystems_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG ecosystems_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG ecosystems_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG ecosystems_write_string(HKEY key, const char *value_name, const char *value)
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

void ecosystems_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    ecosystems_config *config;

    config = ecosystems_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->habitat_mode = ECOSYSTEMS_HABITAT_AQUARIUM;
    config->density_mode = ECOSYSTEMS_DENSITY_STANDARD;
    config->activity_mode = ECOSYSTEMS_ACTIVITY_STANDARD;
    ecosystems_apply_preset_to_config(ECOSYSTEMS_DEFAULT_PRESET_KEY, common_config, config);
}

void ecosystems_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    ecosystems_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = ecosystems_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (common_config->preset_key != NULL && ecosystems_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = ECOSYSTEMS_DEFAULT_PRESET_KEY;
    }

    if (common_config->theme_key == NULL || ecosystems_find_theme_descriptor(common_config->theme_key) == NULL) {
        presets = ecosystems_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL
            ? preset_descriptor->theme_key
            : ECOSYSTEMS_DEFAULT_THEME_KEY;
    }

    if (config->habitat_mode < ECOSYSTEMS_HABITAT_AQUARIUM || config->habitat_mode > ECOSYSTEMS_HABITAT_FIREFLIES) {
        config->habitat_mode = ECOSYSTEMS_HABITAT_AQUARIUM;
    }
    if (config->density_mode < ECOSYSTEMS_DENSITY_SPARSE || config->density_mode > ECOSYSTEMS_DENSITY_LUSH) {
        config->density_mode = ECOSYSTEMS_DENSITY_STANDARD;
    }
    if (config->activity_mode < ECOSYSTEMS_ACTIVITY_CALM || config->activity_mode > ECOSYSTEMS_ACTIVITY_LIVELY) {
        config->activity_mode = ECOSYSTEMS_ACTIVITY_STANDARD;
    }
}

int ecosystems_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    ecosystems_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = ecosystems_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    ecosystems_config_set_defaults(common_config, product_config, product_config_size);
    if (!ecosystems_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        ecosystems_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (ecosystems_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        ecosystems_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (ecosystems_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (ecosystems_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = ecosystems_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (ecosystems_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    (void)ecosystems_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    value_dword = common_config->deterministic_seed;
    if (ecosystems_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)ecosystems_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->habitat_mode;
    if (ecosystems_read_dword(key, "HabitatMode", &value_dword)) {
        config->habitat_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->density_mode;
    if (ecosystems_read_dword(key, "DensityMode", &value_dword)) {
        config->density_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->activity_mode;
    if (ecosystems_read_dword(key, "ActivityMode", &value_dword)) {
        config->activity_mode = (int)value_dword;
    }

    RegCloseKey(key);
    ecosystems_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int ecosystems_config_save(
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
    ecosystems_config safe_product_config;
    const ecosystems_config *config;

    (void)module;
    (void)diagnostics;

    config = ecosystems_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    ecosystems_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!ecosystems_build_registry_path(path, sizeof(path))) {
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

    result = ecosystems_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_dword(key, "HabitatMode", (unsigned long)safe_product_config.habitat_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_dword(key, "DensityMode", (unsigned long)safe_product_config.density_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = ecosystems_write_dword(key, "ActivityMode", (unsigned long)safe_product_config.activity_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static void ecosystems_populate_combo(
    HWND dialog,
    int control_id,
    const ecosystems_combo_item *items,
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

static void ecosystems_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_PRESET, CB_ADDSTRING, 0U, (LPARAM)module->presets[index].display_name);
    }

    SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_THEME, CB_ADDSTRING, 0U, (LPARAM)module->themes[index].display_name);
    }

    ecosystems_populate_combo(dialog, IDC_ECOSYSTEMS_HABITAT, g_ecosystems_habitat_items, 3U);
    ecosystems_populate_combo(dialog, IDC_ECOSYSTEMS_DENSITY, g_ecosystems_density_items, 3U);
    ecosystems_populate_combo(dialog, IDC_ECOSYSTEMS_ACTIVITY, g_ecosystems_activity_items, 3U);
}

static void ecosystems_select_combo_value(HWND dialog, int control_id, LPARAM value)
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

static int ecosystems_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void ecosystems_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const ecosystems_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    ecosystems_select_combo_value(dialog, IDC_ECOSYSTEMS_HABITAT, (LPARAM)product_config->habitat_mode);
    ecosystems_select_combo_value(dialog, IDC_ECOSYSTEMS_DENSITY, (LPARAM)product_config->density_mode);
    ecosystems_select_combo_value(dialog, IDC_ECOSYSTEMS_ACTIVITY, (LPARAM)product_config->activity_mode);
    CheckDlgButton(dialog, IDC_ECOSYSTEMS_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_ECOSYSTEMS_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void ecosystems_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    ecosystems_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_ECOSYSTEMS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.habitat_mode = ECOSYSTEMS_HABITAT_AQUARIUM;
    product_config.density_mode = ECOSYSTEMS_DENSITY_STANDARD;
    product_config.activity_mode = ECOSYSTEMS_ACTIVITY_STANDARD;
    ecosystems_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    ecosystems_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_ECOSYSTEMS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void ecosystems_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    ecosystems_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    ecosystems_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        ecosystems_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_ECOSYSTEMS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->habitat_mode = ecosystems_get_combo_value(dialog, IDC_ECOSYSTEMS_HABITAT, product_config->habitat_mode);
    product_config->density_mode = ecosystems_get_combo_value(dialog, IDC_ECOSYSTEMS_DENSITY, product_config->density_mode);
    product_config->activity_mode = ecosystems_get_combo_value(dialog, IDC_ECOSYSTEMS_ACTIVITY, product_config->activity_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_ECOSYSTEMS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_ECOSYSTEMS_DIAGNOSTICS) == BST_CHECKED;
}

static void ecosystems_initialize_dialog(HWND dialog, ecosystems_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    ecosystems_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Ecosystems\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nAmbient habitats with calmer regroup choreography, clearer habitat families, and preview-safe population pacing.");
    SetDlgItemTextA(dialog, IDC_ECOSYSTEMS_INFO, info);

    ecosystems_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK ecosystems_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    ecosystems_dialog_state *dialog_state;

    dialog_state = (ecosystems_dialog_state *)GetWindowLongPtrA(dialog, DWLP_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (ecosystems_dialog_state *)lParam;
        SetWindowLongPtrA(dialog, DWLP_USER, (LONG_PTR)dialog_state);
        if (dialog_state != NULL) {
            ecosystems_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_ECOSYSTEMS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            ecosystems_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_ECOSYSTEMS_DEFAULTS) {
            ecosystems_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            ecosystems_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            ecosystems_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            ecosystems_config_clamp(
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

INT_PTR ecosystems_config_show_dialog(
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
    ecosystems_dialog_state dialog_state;
    ecosystems_config *config;

    (void)diagnostics;

    config = ecosystems_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_ECOSYSTEMS_CONFIG),
        owner_window,
        ecosystems_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    return result;
}

static int ecosystems_parse_habitat_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "aquarium") == 0) {
        *value_out = ECOSYSTEMS_HABITAT_AQUARIUM;
        return 1;
    }
    if (lstrcmpiA(text, "aviary") == 0) {
        *value_out = ECOSYSTEMS_HABITAT_AVIARY;
        return 1;
    }
    if (lstrcmpiA(text, "fireflies") == 0) {
        *value_out = ECOSYSTEMS_HABITAT_FIREFLIES;
        return 1;
    }
    return 0;
}

static int ecosystems_parse_density_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "sparse") == 0) {
        *value_out = ECOSYSTEMS_DENSITY_SPARSE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = ECOSYSTEMS_DENSITY_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "lush") == 0) {
        *value_out = ECOSYSTEMS_DENSITY_LUSH;
        return 1;
    }
    return 0;
}

static int ecosystems_parse_activity_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "calm") == 0) {
        *value_out = ECOSYSTEMS_ACTIVITY_CALM;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = ECOSYSTEMS_ACTIVITY_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "lively") == 0) {
        *value_out = ECOSYSTEMS_ACTIVITY_LIVELY;
        return 1;
    }
    return 0;
}

int ecosystems_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const ecosystems_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = ecosystems_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(
            writer->context,
            "product",
            "habitat",
            ecosystems_habitat_mode_name(config->habitat_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "density",
            ecosystems_density_mode_name(config->density_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "activity",
            ecosystems_activity_mode_name(config->activity_mode)
        );
}

int ecosystems_config_import_settings_entry(
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
    ecosystems_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = ecosystems_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "habitat") == 0 || lstrcmpiA(key, "habitat_mode") == 0) {
        return ecosystems_parse_habitat_mode(value, &config->habitat_mode);
    }
    if (lstrcmpiA(key, "density") == 0 || lstrcmpiA(key, "density_mode") == 0) {
        return ecosystems_parse_density_mode(value, &config->density_mode);
    }
    if (lstrcmpiA(key, "activity") == 0 || lstrcmpiA(key, "activity_mode") == 0) {
        return ecosystems_parse_activity_mode(value, &config->activity_mode);
    }

    return 1;
}

void ecosystems_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    ecosystems_config *config;
    ecosystems_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = ecosystems_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x45434F53UL;
    ecosystems_rng_seed(&rng, random_seed ^ 0x45434F53UL);

    roll = ecosystems_rng_range(&rng, 100UL);
    if (roll < 38UL) {
        config->habitat_mode = ECOSYSTEMS_HABITAT_AQUARIUM;
    } else if (roll < 68UL) {
        config->habitat_mode = ECOSYSTEMS_HABITAT_AVIARY;
    } else {
        config->habitat_mode = ECOSYSTEMS_HABITAT_FIREFLIES;
    }

    roll = ecosystems_rng_range(&rng, 100UL);
    if (roll < 30UL) {
        config->density_mode = ECOSYSTEMS_DENSITY_SPARSE;
    } else if (roll < 76UL) {
        config->density_mode = ECOSYSTEMS_DENSITY_STANDARD;
    } else {
        config->density_mode = ECOSYSTEMS_DENSITY_LUSH;
    }

    roll = ecosystems_rng_range(&rng, 100UL);
    if (roll < 40UL) {
        config->activity_mode = ECOSYSTEMS_ACTIVITY_CALM;
    } else if (roll < 82UL) {
        config->activity_mode = ECOSYSTEMS_ACTIVITY_STANDARD;
    } else {
        config->activity_mode = ECOSYSTEMS_ACTIVITY_LIVELY;
    }
}
