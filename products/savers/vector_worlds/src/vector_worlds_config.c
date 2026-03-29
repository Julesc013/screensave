#include "vector_worlds_internal.h"
#include "vector_worlds_resource.h"
#include "screensave/version.h"

typedef struct vector_worlds_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    vector_worlds_config *product_config;
} vector_worlds_dialog_state;

typedef struct vector_worlds_combo_item_tag {
    int value;
    const char *display_name;
} vector_worlds_combo_item;

static const vector_worlds_combo_item g_vector_worlds_scene_items[] = {
    { VECTOR_WORLDS_SCENE_FIELD, "Structure Field" },
    { VECTOR_WORLDS_SCENE_TUNNEL, "Tunnel Flow" },
    { VECTOR_WORLDS_SCENE_TERRAIN, "Terrain Flyover" }
};

static const vector_worlds_combo_item g_vector_worlds_speed_items[] = {
    { VECTOR_WORLDS_SPEED_CALM, "Calm" },
    { VECTOR_WORLDS_SPEED_STANDARD, "Standard" },
    { VECTOR_WORLDS_SPEED_SURGE, "Surge" }
};

static const vector_worlds_combo_item g_vector_worlds_detail_items[] = {
    { VECTOR_WORLDS_DETAIL_LOW, "Low" },
    { VECTOR_WORLDS_DETAIL_STANDARD, "Standard" },
    { VECTOR_WORLDS_DETAIL_HIGH, "High" }
};

static vector_worlds_config *vector_worlds_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(vector_worlds_config)) {
        return NULL;
    }

    return (vector_worlds_config *)product_config;
}

static const vector_worlds_config *vector_worlds_as_const_config(const void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(vector_worlds_config)) {
        return NULL;
    }

    return (const vector_worlds_config *)product_config;
}

static void vector_worlds_emit_config_diag(
    screensave_diag_context *diagnostics,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (diagnostics != NULL) {
        screensave_diag_emit(diagnostics, level, SCREENSAVE_DIAG_DOMAIN_CONFIG, code, "vector_worlds_config", text);
    }
}

static int vector_worlds_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(VECTOR_WORLDS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, VECTOR_WORLDS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int vector_worlds_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int vector_worlds_read_flag(HKEY key, const char *value_name, int *value)
{
    unsigned long data;

    if (value == NULL) {
        return 0;
    }

    data = 0UL;
    if (!vector_worlds_read_dword(key, value_name, &data)) {
        return 0;
    }

    *value = data != 0UL;
    return 1;
}

static int vector_worlds_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG vector_worlds_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG vector_worlds_write_flag(HKEY key, const char *value_name, int value)
{
    return vector_worlds_write_dword(key, value_name, value ? 1UL : 0UL);
}

static LONG vector_worlds_write_string(HKEY key, const char *value_name, const char *value)
{
    if (value == NULL || value[0] == '\0') {
        RegDeleteValueA(key, value_name);
        return ERROR_SUCCESS;
    }

    return RegSetValueExA(key, value_name, 0, REG_SZ, (const BYTE *)value, (DWORD)(lstrlenA(value) + 1));
}

static void vector_worlds_sync_common_detail(screensave_common_config *common_config, const vector_worlds_config *config)
{
    if (common_config != NULL && config != NULL) {
        common_config->detail_level = (screensave_detail_level)config->detail_mode;
    }
}

void vector_worlds_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    vector_worlds_config *config;

    config = vector_worlds_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->scene_mode = VECTOR_WORLDS_SCENE_FIELD;
    config->speed_mode = VECTOR_WORLDS_SPEED_CALM;
    config->detail_mode = VECTOR_WORLDS_DETAIL_STANDARD;
    vector_worlds_apply_preset_to_config(VECTOR_WORLDS_DEFAULT_PRESET_KEY, common_config, config);
}

void vector_worlds_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    vector_worlds_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = vector_worlds_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (common_config->preset_key != NULL && vector_worlds_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = VECTOR_WORLDS_DEFAULT_PRESET_KEY;
    }

    if (common_config->theme_key == NULL || vector_worlds_find_theme_descriptor(common_config->theme_key) == NULL) {
        presets = vector_worlds_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL ? preset_descriptor->theme_key : VECTOR_WORLDS_DEFAULT_THEME_KEY;
    }

    if (config->scene_mode < VECTOR_WORLDS_SCENE_FIELD || config->scene_mode > VECTOR_WORLDS_SCENE_TERRAIN) {
        config->scene_mode = VECTOR_WORLDS_SCENE_FIELD;
    }
    if (config->speed_mode < VECTOR_WORLDS_SPEED_CALM || config->speed_mode > VECTOR_WORLDS_SPEED_SURGE) {
        config->speed_mode = VECTOR_WORLDS_SPEED_CALM;
    }
    if (config->detail_mode < VECTOR_WORLDS_DETAIL_LOW || config->detail_mode > VECTOR_WORLDS_DETAIL_HIGH) {
        config->detail_mode = VECTOR_WORLDS_DETAIL_STANDARD;
    }

    vector_worlds_sync_common_detail(common_config, config);
}

int vector_worlds_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    vector_worlds_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = vector_worlds_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    vector_worlds_config_set_defaults(common_config, product_config, product_config_size);
    if (!vector_worlds_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        vector_worlds_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (vector_worlds_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        vector_worlds_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (vector_worlds_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        const screensave_theme_descriptor *theme_descriptor;

        theme_descriptor = vector_worlds_find_theme_descriptor(theme_key);
        if (theme_descriptor != NULL) {
            common_config->theme_key = theme_descriptor->theme_key;
        }
    }

    value_dword = 0UL;
    if (vector_worlds_read_dword(key, "SceneMode", &value_dword)) {
        config->scene_mode = (int)value_dword;
    }
    value_dword = 0UL;
    if (vector_worlds_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = 0UL;
    if (vector_worlds_read_dword(key, "DetailMode", &value_dword)) {
        config->detail_mode = (int)value_dword;
    }
    value_dword = 0UL;
    if (vector_worlds_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    if (vector_worlds_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed)) {
        /* handled by the read helper */
    }
    value_dword = 0UL;
    if (vector_worlds_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    vector_worlds_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    vector_worlds_config_clamp(common_config, product_config, product_config_size);
    RegCloseKey(key);
    return 1;
}

int vector_worlds_config_save(
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
    const vector_worlds_config *config;
    screensave_common_config safe_common_config;
    vector_worlds_config safe_config;

    (void)module;
    (void)diagnostics;

    config = vector_worlds_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_config = *config;
    vector_worlds_config_clamp(&safe_common_config, &safe_config, sizeof(safe_config));
    if (!vector_worlds_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegCreateKeyExA(
        HKEY_CURRENT_USER,
        path,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &key,
        &disposition
    ) != ERROR_SUCCESS) {
        return 0;
    }

    vector_worlds_write_string(key, "PresetKey", safe_common_config.preset_key);
    vector_worlds_write_string(key, "ThemeKey", safe_common_config.theme_key);
    vector_worlds_write_dword(key, "SceneMode", (unsigned long)safe_config.scene_mode);
    vector_worlds_write_dword(key, "SpeedMode", (unsigned long)safe_config.speed_mode);
    vector_worlds_write_dword(key, "DetailMode", (unsigned long)safe_config.detail_mode);
    vector_worlds_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    vector_worlds_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    vector_worlds_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    vector_worlds_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);

    RegCloseKey(key);
    return 1;
}

static void vector_worlds_populate_value_combo(
    HWND dialog,
    int control_id,
    const vector_worlds_combo_item *items,
    unsigned int item_count,
    int selected_value
)
{
    unsigned int index;
    LRESULT selected_index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    selected_index = 0L;
    for (index = 0U; index < item_count; ++index) {
        LRESULT combo_index;

        combo_index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)items[index].display_name);
        if (combo_index != CB_ERR) {
            SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)combo_index, (LPARAM)items[index].value);
            if (items[index].value == selected_value) {
                selected_index = combo_index;
            }
        }
    }

    SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)selected_index, 0L);
}

static void vector_worlds_populate_preset_combo(
    HWND dialog,
    int control_id,
    const screensave_preset_descriptor *descriptors,
    unsigned int descriptor_count,
    const char *selected_key
)
{
    unsigned int index;
    LRESULT selected_index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    selected_index = 0L;
    for (index = 0U; index < descriptor_count; ++index) {
        LRESULT combo_index;

        combo_index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)descriptors[index].display_name);
        if (combo_index != CB_ERR) {
            SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)combo_index, (LPARAM)index);
            if (
                selected_key != NULL &&
                descriptors[index].preset_key != NULL &&
                lstrcmpA(descriptors[index].preset_key, selected_key) == 0
            ) {
                selected_index = combo_index;
            }
        }
    }

    SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)selected_index, 0L);
}

static void vector_worlds_populate_theme_combo(
    HWND dialog,
    int control_id,
    const screensave_theme_descriptor *descriptors,
    unsigned int descriptor_count,
    const char *selected_key
)
{
    unsigned int index;
    LRESULT selected_index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    selected_index = 0L;
    for (index = 0U; index < descriptor_count; ++index) {
        LRESULT combo_index;

        combo_index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)descriptors[index].display_name);
        if (combo_index != CB_ERR) {
            SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)combo_index, (LPARAM)index);
            if (
                selected_key != NULL &&
                descriptors[index].theme_key != NULL &&
                lstrcmpA(descriptors[index].theme_key, selected_key) == 0
            ) {
                selected_index = combo_index;
            }
        }
    }

    SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)selected_index, 0L);
}

static int vector_worlds_combo_value(
    HWND dialog,
    int control_id,
    unsigned int item_count,
    int fallback_value
)
{
    LRESULT selected_index;

    selected_index = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (selected_index == CB_ERR || (unsigned int)selected_index >= item_count) {
        return fallback_value;
    }

    return (int)SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)selected_index, 0L);
}

static void vector_worlds_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const vector_worlds_config *product_config
)
{
    char info[256];
    const screensave_version_info *version_info;

    if (module == NULL) {
        return;
    }

    version_info = screensave_version_get_info();
    vector_worlds_populate_preset_combo(dialog, IDC_VECTOR_WORLDS_PRESET, module->presets, module->preset_count, common_config != NULL ? common_config->preset_key : NULL);
    vector_worlds_populate_theme_combo(dialog, IDC_VECTOR_WORLDS_THEME, module->themes, module->theme_count, common_config != NULL ? common_config->theme_key : NULL);
    vector_worlds_populate_value_combo(dialog, IDC_VECTOR_WORLDS_SCENE, g_vector_worlds_scene_items, (unsigned int)(sizeof(g_vector_worlds_scene_items) / sizeof(g_vector_worlds_scene_items[0])), product_config != NULL ? product_config->scene_mode : VECTOR_WORLDS_SCENE_FIELD);
    vector_worlds_populate_value_combo(dialog, IDC_VECTOR_WORLDS_SPEED, g_vector_worlds_speed_items, (unsigned int)(sizeof(g_vector_worlds_speed_items) / sizeof(g_vector_worlds_speed_items[0])), product_config != NULL ? product_config->speed_mode : VECTOR_WORLDS_SPEED_CALM);
    vector_worlds_populate_value_combo(dialog, IDC_VECTOR_WORLDS_DETAIL, g_vector_worlds_detail_items, (unsigned int)(sizeof(g_vector_worlds_detail_items) / sizeof(g_vector_worlds_detail_items[0])), common_config != NULL ? (int)common_config->detail_level : VECTOR_WORLDS_DETAIL_STANDARD);

    CheckDlgButton(dialog, IDC_VECTOR_WORLDS_DETERMINISTIC, common_config != NULL && common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_VECTOR_WORLDS_DIAGNOSTICS, common_config != NULL && common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);

    info[0] = '\0';
    lstrcpyA(info, "Vector Worlds\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nSoftware-3D wireframe flyovers with curated motion and depth.");
    SetDlgItemTextA(dialog, IDC_VECTOR_WORLDS_INFO, info);
}

static void vector_worlds_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    screensave_common_config common_config;
    vector_worlds_config product_config;
    int deterministic_enabled;
    int diagnostics_enabled;

    if (module == NULL) {
        return;
    }

    preset_index = SendDlgItemMessageA(dialog, IDC_VECTOR_WORLDS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    deterministic_enabled = IsDlgButtonChecked(dialog, IDC_VECTOR_WORLDS_DETERMINISTIC) == BST_CHECKED;
    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_VECTOR_WORLDS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.scene_mode = VECTOR_WORLDS_SCENE_FIELD;
    product_config.speed_mode = VECTOR_WORLDS_SPEED_CALM;
    product_config.detail_mode = VECTOR_WORLDS_DETAIL_STANDARD;
    vector_worlds_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    vector_worlds_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_VECTOR_WORLDS_DETERMINISTIC, deterministic_enabled ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_VECTOR_WORLDS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void vector_worlds_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    vector_worlds_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    if (module == NULL) {
        return;
    }

    vector_worlds_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_VECTOR_WORLDS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        vector_worlds_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_VECTOR_WORLDS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->scene_mode = vector_worlds_combo_value(dialog, IDC_VECTOR_WORLDS_SCENE, (unsigned int)(sizeof(g_vector_worlds_scene_items) / sizeof(g_vector_worlds_scene_items[0])), product_config->scene_mode);
    product_config->speed_mode = vector_worlds_combo_value(dialog, IDC_VECTOR_WORLDS_SPEED, (unsigned int)(sizeof(g_vector_worlds_speed_items) / sizeof(g_vector_worlds_speed_items[0])), product_config->speed_mode);
    product_config->detail_mode = vector_worlds_combo_value(dialog, IDC_VECTOR_WORLDS_DETAIL, (unsigned int)(sizeof(g_vector_worlds_detail_items) / sizeof(g_vector_worlds_detail_items[0])), product_config->detail_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_VECTOR_WORLDS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_VECTOR_WORLDS_DIAGNOSTICS) == BST_CHECKED;

    vector_worlds_sync_common_detail(common_config, product_config);
}

static INT_PTR CALLBACK vector_worlds_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    vector_worlds_dialog_state *dialog_state;

    dialog_state = (vector_worlds_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (vector_worlds_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            vector_worlds_apply_settings_to_dialog(dialog, dialog_state->module, dialog_state->common_config, dialog_state->product_config);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_VECTOR_WORLDS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            vector_worlds_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_VECTOR_WORLDS_DEFAULTS) {
            vector_worlds_config_set_defaults(dialog_state->common_config, dialog_state->product_config, sizeof(*dialog_state->product_config));
            vector_worlds_apply_settings_to_dialog(dialog, dialog_state->module, dialog_state->common_config, dialog_state->product_config);
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            vector_worlds_read_dialog_settings(dialog, dialog_state->module, dialog_state->common_config, dialog_state->product_config);
            vector_worlds_config_clamp(dialog_state->common_config, dialog_state->product_config, sizeof(*dialog_state->product_config));
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

INT_PTR vector_worlds_config_show_dialog(
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
    vector_worlds_dialog_state dialog_state;
    vector_worlds_config *config;

    config = vector_worlds_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        vector_worlds_emit_config_diag(diagnostics, SCREENSAVE_DIAG_LEVEL_ERROR, 7403UL, "The Vector Worlds dialog could not start because its config state was invalid.");
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_VECTOR_WORLDS_CONFIG), owner_window, vector_worlds_config_dialog_proc, (LPARAM)&dialog_state);
    if (result == -1) {
        vector_worlds_emit_config_diag(diagnostics, SCREENSAVE_DIAG_LEVEL_ERROR, 7404UL, "The Vector Worlds config dialog resource could not be loaded.");
    }

    return result;
}
