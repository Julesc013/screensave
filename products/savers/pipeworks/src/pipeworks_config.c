#include "pipeworks_internal.h"
#include "pipeworks_resource.h"
#include "screensave/version.h"

typedef struct pipeworks_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    pipeworks_config *product_config;
} pipeworks_dialog_state;

typedef struct pipeworks_combo_item_tag {
    int value;
    const char *display_name;
} pipeworks_combo_item;

static const pipeworks_combo_item g_pipeworks_density_items[] = {
    { PIPEWORKS_DENSITY_TIGHT, "Tight" },
    { PIPEWORKS_DENSITY_STANDARD, "Standard" },
    { PIPEWORKS_DENSITY_OPEN, "Open" }
};

static const pipeworks_combo_item g_pipeworks_speed_items[] = {
    { PIPEWORKS_SPEED_PATIENT, "Patient" },
    { PIPEWORKS_SPEED_STANDARD, "Standard" },
    { PIPEWORKS_SPEED_BRISK, "Brisk" }
};

static const pipeworks_combo_item g_pipeworks_branch_items[] = {
    { PIPEWORKS_BRANCH_ORDERLY, "Orderly" },
    { PIPEWORKS_BRANCH_BALANCED, "Balanced" },
    { PIPEWORKS_BRANCH_WILD, "Wild" }
};

static const pipeworks_combo_item g_pipeworks_rebuild_items[] = {
    { PIPEWORKS_REBUILD_PATIENT, "Patient" },
    { PIPEWORKS_REBUILD_CYCLE, "Cycle" },
    { PIPEWORKS_REBUILD_FREQUENT, "Frequent" }
};

static void pipeworks_emit_config_diag(
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
        "pipeworks_config",
        text
    );
}

static pipeworks_config *pipeworks_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(pipeworks_config)) {
        return NULL;
    }

    return (pipeworks_config *)product_config;
}

static const pipeworks_config *pipeworks_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(pipeworks_config)) {
        return NULL;
    }

    return (const pipeworks_config *)product_config;
}

static int pipeworks_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(PIPEWORKS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, PIPEWORKS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int pipeworks_read_flag(HKEY key, const char *value_name, int *value)
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

static int pipeworks_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int pipeworks_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG pipeworks_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG pipeworks_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG pipeworks_write_string(HKEY key, const char *value_name, const char *value)
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

void pipeworks_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    pipeworks_config *config;

    config = pipeworks_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->density_mode = PIPEWORKS_DENSITY_STANDARD;
    config->speed_mode = PIPEWORKS_SPEED_STANDARD;
    config->branch_mode = PIPEWORKS_BRANCH_BALANCED;
    config->rebuild_mode = PIPEWORKS_REBUILD_CYCLE;
    pipeworks_apply_preset_to_config(PIPEWORKS_DEFAULT_PRESET_KEY, common_config, config);
}

void pipeworks_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    pipeworks_config *config;

    config = pipeworks_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        pipeworks_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = PIPEWORKS_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        pipeworks_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = pipeworks_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : PIPEWORKS_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = PIPEWORKS_DEFAULT_THEME_KEY;
        }
    }

    if (config->density_mode < PIPEWORKS_DENSITY_TIGHT || config->density_mode > PIPEWORKS_DENSITY_OPEN) {
        config->density_mode = PIPEWORKS_DENSITY_STANDARD;
    }
    if (config->speed_mode < PIPEWORKS_SPEED_PATIENT || config->speed_mode > PIPEWORKS_SPEED_BRISK) {
        config->speed_mode = PIPEWORKS_SPEED_STANDARD;
    }
    if (config->branch_mode < PIPEWORKS_BRANCH_ORDERLY || config->branch_mode > PIPEWORKS_BRANCH_WILD) {
        config->branch_mode = PIPEWORKS_BRANCH_BALANCED;
    }
    if (
        config->rebuild_mode < PIPEWORKS_REBUILD_PATIENT ||
        config->rebuild_mode > PIPEWORKS_REBUILD_FREQUENT
    ) {
        config->rebuild_mode = PIPEWORKS_REBUILD_CYCLE;
    }
}

int pipeworks_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    pipeworks_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = pipeworks_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    pipeworks_config_set_defaults(common_config, product_config, product_config_size);
    if (!pipeworks_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        pipeworks_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (pipeworks_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        pipeworks_apply_preset_to_config(preset_key, common_config, config);
    }

    if (pipeworks_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (pipeworks_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = pipeworks_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (pipeworks_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)pipeworks_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (pipeworks_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)pipeworks_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->density_mode;
    if (pipeworks_read_dword(key, "DensityMode", &value_dword)) {
        config->density_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (pipeworks_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->branch_mode;
    if (pipeworks_read_dword(key, "BranchMode", &value_dword)) {
        config->branch_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->rebuild_mode;
    if (pipeworks_read_dword(key, "RebuildMode", &value_dword)) {
        config->rebuild_mode = (int)value_dword;
    }

    RegCloseKey(key);
    pipeworks_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int pipeworks_config_save(
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
    pipeworks_config safe_product_config;
    screensave_common_config safe_common_config;
    const pipeworks_config *config;

    (void)module;

    config = pipeworks_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        pipeworks_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6701UL,
            "The Pipeworks config could not be saved because the config state was invalid."
        );
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    pipeworks_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!pipeworks_build_registry_path(path, sizeof(path))) {
        pipeworks_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6702UL,
            "The Pipeworks config registry path could not be constructed."
        );
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
        pipeworks_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6703UL,
            "The Pipeworks config registry key could not be opened for writing."
        );
        return 0;
    }

    result = pipeworks_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_dword(key, "DensityMode", (unsigned long)safe_product_config.density_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_dword(key, "BranchMode", (unsigned long)safe_product_config.branch_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = pipeworks_write_dword(key, "RebuildMode", (unsigned long)safe_product_config.rebuild_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        pipeworks_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6704UL,
            "The Pipeworks config values could not be written to the registry."
        );
        return 0;
    }

    return 1;
}

static void pipeworks_populate_combo(
    HWND dialog,
    int control_id,
    const pipeworks_combo_item *items,
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
            SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)combo_index, (LPARAM)items[index].value);
        }
    }
}

static void pipeworks_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_PIPEWORKS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_PIPEWORKS_PRESET,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->presets[index].display_name
        );
    }

    SendDlgItemMessageA(dialog, IDC_PIPEWORKS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_PIPEWORKS_THEME,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->themes[index].display_name
        );
    }

    pipeworks_populate_combo(dialog, IDC_PIPEWORKS_DENSITY, g_pipeworks_density_items, 3U);
    pipeworks_populate_combo(dialog, IDC_PIPEWORKS_SPEED, g_pipeworks_speed_items, 3U);
    pipeworks_populate_combo(dialog, IDC_PIPEWORKS_BRANCH, g_pipeworks_branch_items, 3U);
    pipeworks_populate_combo(dialog, IDC_PIPEWORKS_REBUILD, g_pipeworks_rebuild_items, 3U);
}

static void pipeworks_select_combo_value(HWND dialog, int control_id, LPARAM value)
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

static int pipeworks_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void pipeworks_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const pipeworks_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PIPEWORKS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PIPEWORKS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    pipeworks_select_combo_value(dialog, IDC_PIPEWORKS_DENSITY, (LPARAM)product_config->density_mode);
    pipeworks_select_combo_value(dialog, IDC_PIPEWORKS_SPEED, (LPARAM)product_config->speed_mode);
    pipeworks_select_combo_value(dialog, IDC_PIPEWORKS_BRANCH, (LPARAM)product_config->branch_mode);
    pipeworks_select_combo_value(dialog, IDC_PIPEWORKS_REBUILD, (LPARAM)product_config->rebuild_mode);
    CheckDlgButton(dialog, IDC_PIPEWORKS_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_PIPEWORKS_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void pipeworks_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    pipeworks_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_PIPEWORKS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_PIPEWORKS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.density_mode = PIPEWORKS_DENSITY_STANDARD;
    product_config.speed_mode = PIPEWORKS_SPEED_STANDARD;
    product_config.branch_mode = PIPEWORKS_BRANCH_BALANCED;
    product_config.rebuild_mode = PIPEWORKS_REBUILD_CYCLE;
    pipeworks_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    pipeworks_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_PIPEWORKS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void pipeworks_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    pipeworks_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    pipeworks_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_PIPEWORKS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        pipeworks_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_PIPEWORKS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->density_mode = pipeworks_get_combo_value(dialog, IDC_PIPEWORKS_DENSITY, product_config->density_mode);
    product_config->speed_mode = pipeworks_get_combo_value(dialog, IDC_PIPEWORKS_SPEED, product_config->speed_mode);
    product_config->branch_mode = pipeworks_get_combo_value(dialog, IDC_PIPEWORKS_BRANCH, product_config->branch_mode);
    product_config->rebuild_mode = pipeworks_get_combo_value(dialog, IDC_PIPEWORKS_REBUILD, product_config->rebuild_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_PIPEWORKS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_PIPEWORKS_DIAGNOSTICS) == BST_CHECKED;
}

static void pipeworks_initialize_dialog(HWND dialog, pipeworks_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    pipeworks_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Pipeworks\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nGrid-grown network construction with curated pulse travel, rebuild cadence, and calmer preview pacing.");
    SetDlgItemTextA(dialog, IDC_PIPEWORKS_INFO, info);

    pipeworks_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK pipeworks_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    pipeworks_dialog_state *dialog_state;

    dialog_state = (pipeworks_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (pipeworks_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            pipeworks_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_PIPEWORKS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            pipeworks_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_PIPEWORKS_DEFAULTS) {
            pipeworks_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            pipeworks_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            pipeworks_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            pipeworks_config_clamp(
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

INT_PTR pipeworks_config_show_dialog(
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
    pipeworks_dialog_state dialog_state;
    pipeworks_config *config;

    config = pipeworks_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        pipeworks_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6705UL,
            "The Pipeworks dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_PIPEWORKS_CONFIG),
        owner_window,
        pipeworks_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        pipeworks_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6706UL,
            "The Pipeworks config dialog resource could not be loaded."
        );
    }

    return result;
}

static int pipeworks_parse_density_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "tight") == 0) {
        *value_out = PIPEWORKS_DENSITY_TIGHT;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PIPEWORKS_DENSITY_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "open") == 0) {
        *value_out = PIPEWORKS_DENSITY_OPEN;
        return 1;
    }
    return 0;
}

static int pipeworks_parse_speed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "patient") == 0) {
        *value_out = PIPEWORKS_SPEED_PATIENT;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PIPEWORKS_SPEED_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "brisk") == 0) {
        *value_out = PIPEWORKS_SPEED_BRISK;
        return 1;
    }
    return 0;
}

static int pipeworks_parse_branch_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "orderly") == 0) {
        *value_out = PIPEWORKS_BRANCH_ORDERLY;
        return 1;
    }
    if (lstrcmpiA(text, "balanced") == 0) {
        *value_out = PIPEWORKS_BRANCH_BALANCED;
        return 1;
    }
    if (lstrcmpiA(text, "wild") == 0) {
        *value_out = PIPEWORKS_BRANCH_WILD;
        return 1;
    }
    return 0;
}

static int pipeworks_parse_rebuild_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "patient") == 0) {
        *value_out = PIPEWORKS_REBUILD_PATIENT;
        return 1;
    }
    if (lstrcmpiA(text, "cycle") == 0) {
        *value_out = PIPEWORKS_REBUILD_CYCLE;
        return 1;
    }
    if (lstrcmpiA(text, "frequent") == 0) {
        *value_out = PIPEWORKS_REBUILD_FREQUENT;
        return 1;
    }
    return 0;
}

int pipeworks_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const pipeworks_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = pipeworks_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "density", pipeworks_density_mode_name(config->density_mode)) &&
        writer->write_string(writer->context, "product", "speed", pipeworks_speed_mode_name(config->speed_mode)) &&
        writer->write_string(writer->context, "product", "branch", pipeworks_branch_mode_name(config->branch_mode)) &&
        writer->write_string(writer->context, "product", "rebuild", pipeworks_rebuild_mode_name(config->rebuild_mode));
}

int pipeworks_config_import_settings_entry(
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
    pipeworks_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = pipeworks_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "density") == 0 || lstrcmpiA(key, "density_mode") == 0) {
        return pipeworks_parse_density_mode(value, &config->density_mode);
    }
    if (lstrcmpiA(key, "speed") == 0 || lstrcmpiA(key, "speed_mode") == 0) {
        return pipeworks_parse_speed_mode(value, &config->speed_mode);
    }
    if (lstrcmpiA(key, "branch") == 0 || lstrcmpiA(key, "branch_mode") == 0) {
        return pipeworks_parse_branch_mode(value, &config->branch_mode);
    }
    if (lstrcmpiA(key, "rebuild") == 0 || lstrcmpiA(key, "rebuild_mode") == 0) {
        return pipeworks_parse_rebuild_mode(value, &config->rebuild_mode);
    }

    return 1;
}

void pipeworks_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    pipeworks_config *config;
    pipeworks_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = pipeworks_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x50775042UL;
    pipeworks_rng_seed(&rng, random_seed ^ 0x50775042UL);

    roll = pipeworks_rng_range(&rng, 100UL);
    if (roll < 30UL) {
        config->density_mode = PIPEWORKS_DENSITY_TIGHT;
    } else if (roll < 74UL) {
        config->density_mode = PIPEWORKS_DENSITY_STANDARD;
    } else {
        config->density_mode = PIPEWORKS_DENSITY_OPEN;
    }

    roll = pipeworks_rng_range(&rng, 100UL);
    if (roll < 34UL) {
        config->speed_mode = PIPEWORKS_SPEED_PATIENT;
    } else if (roll < 80UL) {
        config->speed_mode = PIPEWORKS_SPEED_STANDARD;
    } else {
        config->speed_mode = PIPEWORKS_SPEED_BRISK;
    }

    roll = pipeworks_rng_range(&rng, 100UL);
    if (roll < 24UL) {
        config->branch_mode = PIPEWORKS_BRANCH_ORDERLY;
    } else if (roll < 76UL) {
        config->branch_mode = PIPEWORKS_BRANCH_BALANCED;
    } else {
        config->branch_mode = PIPEWORKS_BRANCH_WILD;
    }

    roll = pipeworks_rng_range(&rng, 100UL);
    if (roll < 28UL) {
        config->rebuild_mode = PIPEWORKS_REBUILD_PATIENT;
    } else if (roll < 78UL) {
        config->rebuild_mode = PIPEWORKS_REBUILD_CYCLE;
    } else {
        config->rebuild_mode = PIPEWORKS_REBUILD_FREQUENT;
    }
}
