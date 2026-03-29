#include "atlas_internal.h"
#include "atlas_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct atlas_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    atlas_config *product_config;
} atlas_dialog_state;

typedef struct atlas_combo_item_tag {
    int value;
    const char *display_name;
} atlas_combo_item;

static const atlas_combo_item g_atlas_mode_items[] = {
    { ATLAS_MODE_ATLAS, "Atlas" },
    { ATLAS_MODE_VOYAGE, "Voyage" },
    { ATLAS_MODE_JULIA, "Julia Study" }
};

static const atlas_combo_item g_atlas_speed_items[] = {
    { ATLAS_SPEED_STILL, "Still" },
    { ATLAS_SPEED_STANDARD, "Standard" },
    { ATLAS_SPEED_BRISK, "Brisk" }
};

static const atlas_combo_item g_atlas_refinement_items[] = {
    { ATLAS_REFINEMENT_DRAFT, "Draft" },
    { ATLAS_REFINEMENT_STANDARD, "Standard" },
    { ATLAS_REFINEMENT_FINE, "Fine" }
};

static atlas_config *atlas_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(atlas_config)) {
        return NULL;
    }

    return (atlas_config *)product_config;
}

static const atlas_config *atlas_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(atlas_config)) {
        return NULL;
    }

    return (const atlas_config *)product_config;
}

static int atlas_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(ATLAS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, ATLAS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int atlas_read_flag(HKEY key, const char *value_name, int *value)
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

static int atlas_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int atlas_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG atlas_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG atlas_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG atlas_write_string(HKEY key, const char *value_name, const char *value)
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

void atlas_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    atlas_config *config;

    config = atlas_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->mode = ATLAS_MODE_ATLAS;
    config->speed_mode = ATLAS_SPEED_STILL;
    config->refinement_mode = ATLAS_REFINEMENT_STANDARD;
    atlas_apply_preset_to_config(ATLAS_DEFAULT_PRESET_KEY, common_config, config);
}

void atlas_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    atlas_config *config;

    config = atlas_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (
        common_config->preset_key != NULL &&
        atlas_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = ATLAS_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        atlas_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = atlas_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : ATLAS_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = ATLAS_DEFAULT_THEME_KEY;
        }
    }

    if (config->mode < ATLAS_MODE_VOYAGE || config->mode > ATLAS_MODE_JULIA) {
        config->mode = ATLAS_MODE_ATLAS;
    }
    if (config->speed_mode < ATLAS_SPEED_STILL || config->speed_mode > ATLAS_SPEED_BRISK) {
        config->speed_mode = ATLAS_SPEED_STILL;
    }
    if (
        config->refinement_mode < ATLAS_REFINEMENT_DRAFT ||
        config->refinement_mode > ATLAS_REFINEMENT_FINE
    ) {
        config->refinement_mode = ATLAS_REFINEMENT_STANDARD;
    }
}

int atlas_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    atlas_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)diagnostics;

    config = atlas_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    atlas_config_set_defaults(common_config, product_config, product_config_size);
    if (!atlas_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "atlas"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            atlas_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (atlas_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        atlas_apply_preset_to_config(preset_key, common_config, config);
    }

    if (atlas_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (atlas_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = atlas_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (atlas_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)atlas_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (atlas_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)atlas_read_flag(
        key,
        "DiagnosticsOverlayEnabled",
        &common_config->diagnostics_overlay_enabled
    );

    value_dword = (unsigned long)config->mode;
    if (atlas_read_dword(key, "Mode", &value_dword)) {
        config->mode = (int)value_dword;
    }

    value_dword = (unsigned long)config->speed_mode;
    if (atlas_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }

    value_dword = (unsigned long)config->refinement_mode;
    if (atlas_read_dword(key, "RefinementMode", &value_dword)) {
        config->refinement_mode = (int)value_dword;
    }

    RegCloseKey(key);
    atlas_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int atlas_config_save(
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
    atlas_config safe_product_config;
    const atlas_config *config;

    (void)module;
    (void)diagnostics;

    config = atlas_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    atlas_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!atlas_build_registry_path(path, sizeof(path))) {
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

    result = atlas_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = atlas_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_dword(key, "Mode", (unsigned long)safe_product_config.mode);
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = atlas_write_dword(key, "RefinementMode", (unsigned long)safe_product_config.refinement_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static void atlas_populate_combo(
    HWND dialog,
    int control_id,
    const atlas_combo_item *items,
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

static void atlas_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    if (dialog == NULL || module == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_ATLAS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_ATLAS_PRESET, CB_ADDSTRING, 0U, (LPARAM)module->presets[index].display_name);
    }

    SendDlgItemMessageA(dialog, IDC_ATLAS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_ATLAS_THEME, CB_ADDSTRING, 0U, (LPARAM)module->themes[index].display_name);
    }

    atlas_populate_combo(dialog, IDC_ATLAS_MODE, g_atlas_mode_items, 3U);
    atlas_populate_combo(dialog, IDC_ATLAS_SPEED, g_atlas_speed_items, 3U);
    atlas_populate_combo(dialog, IDC_ATLAS_REFINEMENT, g_atlas_refinement_items, 3U);
}

static void atlas_select_combo_value(HWND dialog, int control_id, LPARAM value)
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

static int atlas_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void atlas_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const atlas_config *product_config
)
{
    unsigned int index;

    if (dialog == NULL || module == NULL || common_config == NULL || product_config == NULL) {
        return;
    }

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_ATLAS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_ATLAS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    atlas_select_combo_value(dialog, IDC_ATLAS_MODE, (LPARAM)product_config->mode);
    atlas_select_combo_value(dialog, IDC_ATLAS_SPEED, (LPARAM)product_config->speed_mode);
    atlas_select_combo_value(dialog, IDC_ATLAS_REFINEMENT, (LPARAM)product_config->refinement_mode);
    CheckDlgButton(dialog, IDC_ATLAS_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_ATLAS_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void atlas_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    atlas_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_ATLAS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_ATLAS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.mode = ATLAS_MODE_ATLAS;
    product_config.speed_mode = ATLAS_SPEED_STILL;
    product_config.refinement_mode = ATLAS_REFINEMENT_STANDARD;
    atlas_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    atlas_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_ATLAS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void atlas_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    atlas_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    atlas_config_set_defaults(common_config, product_config, sizeof(*product_config));
    preset_index = SendDlgItemMessageA(dialog, IDC_ATLAS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        atlas_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_ATLAS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->mode = atlas_get_combo_value(dialog, IDC_ATLAS_MODE, product_config->mode);
    product_config->speed_mode = atlas_get_combo_value(dialog, IDC_ATLAS_SPEED, product_config->speed_mode);
    product_config->refinement_mode = atlas_get_combo_value(dialog, IDC_ATLAS_REFINEMENT, product_config->refinement_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_ATLAS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_ATLAS_DIAGNOSTICS) == BST_CHECKED;
}

static void atlas_initialize_dialog(HWND dialog, atlas_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    atlas_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Atlas\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nCurated Mandelbrot and Julia voyages with staged refinement.");
    SetDlgItemTextA(dialog, IDC_ATLAS_INFO, info);

    atlas_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK atlas_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    atlas_dialog_state *dialog_state;

    dialog_state = (atlas_dialog_state *)GetWindowLongA(dialog, DWL_USER);
    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (atlas_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            atlas_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;
    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_ATLAS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            atlas_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_ATLAS_DEFAULTS) {
            atlas_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            atlas_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            atlas_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            atlas_config_clamp(
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

INT_PTR atlas_config_show_dialog(
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
    atlas_dialog_state dialog_state;
    atlas_config *config;

    (void)diagnostics;

    config = atlas_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_ATLAS_CONFIG),
        owner_window,
        atlas_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    return result;
}

static int atlas_parse_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "voyage") == 0) {
        *value_out = ATLAS_MODE_VOYAGE;
        return 1;
    }
    if (lstrcmpiA(text, "atlas") == 0) {
        *value_out = ATLAS_MODE_ATLAS;
        return 1;
    }
    if (lstrcmpiA(text, "julia") == 0) {
        *value_out = ATLAS_MODE_JULIA;
        return 1;
    }
    return 0;
}

static int atlas_parse_speed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "still") == 0) {
        *value_out = ATLAS_SPEED_STILL;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = ATLAS_SPEED_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "brisk") == 0) {
        *value_out = ATLAS_SPEED_BRISK;
        return 1;
    }
    return 0;
}

static int atlas_parse_refinement_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "draft") == 0) {
        *value_out = ATLAS_REFINEMENT_DRAFT;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = ATLAS_REFINEMENT_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "fine") == 0) {
        *value_out = ATLAS_REFINEMENT_FINE;
        return 1;
    }
    return 0;
}

int atlas_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const atlas_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = atlas_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "mode", atlas_mode_name(config->mode)) &&
        writer->write_string(writer->context, "product", "speed_mode", atlas_speed_mode_name(config->speed_mode)) &&
        writer->write_string(
            writer->context,
            "product",
            "refinement_mode",
            atlas_refinement_mode_name(config->refinement_mode)
        );
}

int atlas_config_import_settings_entry(
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
    atlas_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = atlas_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "mode") == 0) {
        return atlas_parse_mode(value, &config->mode);
    }
    if (lstrcmpiA(key, "speed_mode") == 0) {
        return atlas_parse_speed_mode(value, &config->speed_mode);
    }
    if (lstrcmpiA(key, "refinement_mode") == 0) {
        return atlas_parse_refinement_mode(value, &config->refinement_mode);
    }

    return 1;
}

void atlas_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    atlas_config *config;
    atlas_rng_state rng;
    unsigned long random_seed;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = atlas_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x41544C41UL;
    atlas_rng_seed(&rng, random_seed ^ 0x41544C41UL);
    config->mode = (int)atlas_rng_range(&rng, 3UL);
    config->speed_mode = (int)atlas_rng_range(&rng, 3UL);
    config->refinement_mode = (int)atlas_rng_range(&rng, 3UL);
}
