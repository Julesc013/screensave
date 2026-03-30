#include "plasma_internal.h"
#include "plasma_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct plasma_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    plasma_config *product_config;
} plasma_dialog_state;

typedef struct plasma_combo_item_tag {
    int value;
    const char *display_name;
} plasma_combo_item;

static const plasma_combo_item g_plasma_effect_items[] = {
    { PLASMA_EFFECT_PLASMA, "Plasma" },
    { PLASMA_EFFECT_FIRE, "Fire" },
    { PLASMA_EFFECT_INTERFERENCE, "Interference Field" }
};

static const plasma_combo_item g_plasma_speed_items[] = {
    { PLASMA_SPEED_GENTLE, "Gentle" },
    { PLASMA_SPEED_STANDARD, "Standard" },
    { PLASMA_SPEED_LIVELY, "Lively" }
};

static const plasma_combo_item g_plasma_resolution_items[] = {
    { PLASMA_RESOLUTION_COARSE, "Coarse" },
    { PLASMA_RESOLUTION_STANDARD, "Standard" },
    { PLASMA_RESOLUTION_FINE, "Fine" }
};

static const plasma_combo_item g_plasma_smoothing_items[] = {
    { PLASMA_SMOOTHING_OFF, "Off" },
    { PLASMA_SMOOTHING_SOFT, "Soft Diffusion" },
    { PLASMA_SMOOTHING_GLOW, "Glow Diffusion" }
};

static void plasma_emit_config_diag(
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
        "plasma_config",
        text
    );
}

static plasma_config *plasma_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(plasma_config)) {
        return NULL;
    }

    return (plasma_config *)product_config;
}

static const plasma_config *plasma_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(plasma_config)) {
        return NULL;
    }

    return (const plasma_config *)product_config;
}

static int plasma_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(PLASMA_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, PLASMA_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int plasma_read_flag(HKEY key, const char *value_name, int *value)
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

static int plasma_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int plasma_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG plasma_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG plasma_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG plasma_write_string(HKEY key, const char *value_name, const char *value)
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

void plasma_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    plasma_config *config;

    config = plasma_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->effect_mode = PLASMA_EFFECT_FIRE;
    config->speed_mode = PLASMA_SPEED_GENTLE;
    config->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    config->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    plasma_apply_preset_to_config(PLASMA_DEFAULT_PRESET_KEY, common_config, config);
}

void plasma_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    plasma_config *config;

    config = plasma_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        plasma_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = PLASMA_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        plasma_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = plasma_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : PLASMA_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = PLASMA_DEFAULT_THEME_KEY;
        }
    }

    if (config->effect_mode < PLASMA_EFFECT_PLASMA || config->effect_mode > PLASMA_EFFECT_INTERFERENCE) {
        config->effect_mode = PLASMA_EFFECT_FIRE;
    }
    if (config->speed_mode < PLASMA_SPEED_GENTLE || config->speed_mode > PLASMA_SPEED_LIVELY) {
        config->speed_mode = PLASMA_SPEED_GENTLE;
    }
    if (
        config->resolution_mode < PLASMA_RESOLUTION_COARSE ||
        config->resolution_mode > PLASMA_RESOLUTION_FINE
    ) {
        config->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    }
    if (config->smoothing_mode < PLASMA_SMOOTHING_OFF || config->smoothing_mode > PLASMA_SMOOTHING_GLOW) {
        config->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    }
}

int plasma_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    plasma_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)diagnostics;

    config = plasma_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    plasma_config_set_defaults(common_config, product_config, product_config_size);
    if (!plasma_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "plasma"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            plasma_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (plasma_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        plasma_apply_preset_to_config(preset_key, common_config, config);
    }

    if (plasma_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (plasma_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = plasma_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (plasma_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)plasma_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (plasma_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)plasma_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->effect_mode;
    if (plasma_read_dword(key, "EffectMode", &value_dword)) {
        config->effect_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (plasma_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->resolution_mode;
    if (plasma_read_dword(key, "ResolutionMode", &value_dword)) {
        config->resolution_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->smoothing_mode;
    if (plasma_read_dword(key, "SmoothingMode", &value_dword)) {
        config->smoothing_mode = (int)value_dword;
    }

    RegCloseKey(key);
    plasma_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int plasma_config_save(
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
    plasma_config safe_product_config;
    screensave_common_config safe_common_config;
    const plasma_config *config;

    (void)module;

    config = plasma_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    plasma_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!plasma_build_registry_path(path, sizeof(path))) {
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
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6501UL,
            "The Plasma registry key could not be opened for writing."
        );
        return 0;
    }

    result = plasma_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = plasma_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "EffectMode", (unsigned long)safe_product_config.effect_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "ResolutionMode", (unsigned long)safe_product_config.resolution_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = plasma_write_dword(key, "SmoothingMode", (unsigned long)safe_product_config.smoothing_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6502UL,
            "The Plasma registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT plasma_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void plasma_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static int plasma_get_combo_value(HWND dialog, int control_id, int default_value)
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

static void plasma_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_PRESET, module->presets[index].display_name, (LPARAM)index);
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_THEME, module->themes[index].display_name, (LPARAM)index);
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_EFFECT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_effect_items) / sizeof(g_plasma_effect_items[0])); ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_EFFECT, g_plasma_effect_items[index].display_name, (LPARAM)g_plasma_effect_items[index].value);
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_SPEED, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_speed_items) / sizeof(g_plasma_speed_items[0])); ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_SPEED, g_plasma_speed_items[index].display_name, (LPARAM)g_plasma_speed_items[index].value);
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_RESOLUTION, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_resolution_items) / sizeof(g_plasma_resolution_items[0])); ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_RESOLUTION, g_plasma_resolution_items[index].display_name, (LPARAM)g_plasma_resolution_items[index].value);
    }

    SendDlgItemMessageA(dialog, IDC_PLASMA_SMOOTHING, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_plasma_smoothing_items) / sizeof(g_plasma_smoothing_items[0])); ++index) {
        plasma_add_combo_item(dialog, IDC_PLASMA_SMOOTHING, g_plasma_smoothing_items[index].display_name, (LPARAM)g_plasma_smoothing_items[index].value);
    }
}

static void plasma_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const plasma_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PLASMA_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    plasma_select_combo_value(dialog, IDC_PLASMA_EFFECT, (LPARAM)product_config->effect_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_SPEED, (LPARAM)product_config->speed_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_RESOLUTION, (LPARAM)product_config->resolution_mode);
    plasma_select_combo_value(dialog, IDC_PLASMA_SMOOTHING, (LPARAM)product_config->smoothing_mode);

    CheckDlgButton(dialog, IDC_PLASMA_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_PLASMA_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void plasma_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    plasma_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_PLASMA_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.effect_mode = PLASMA_EFFECT_FIRE;
    product_config.speed_mode = PLASMA_SPEED_GENTLE;
    product_config.resolution_mode = PLASMA_RESOLUTION_STANDARD;
    product_config.smoothing_mode = PLASMA_SMOOTHING_SOFT;
    plasma_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    plasma_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_PLASMA_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void plasma_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    plasma_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    plasma_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_PLASMA_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        plasma_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_PLASMA_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->effect_mode = plasma_get_combo_value(dialog, IDC_PLASMA_EFFECT, product_config->effect_mode);
    product_config->speed_mode = plasma_get_combo_value(dialog, IDC_PLASMA_SPEED, product_config->speed_mode);
    product_config->resolution_mode = plasma_get_combo_value(dialog, IDC_PLASMA_RESOLUTION, product_config->resolution_mode);
    product_config->smoothing_mode = plasma_get_combo_value(dialog, IDC_PLASMA_SMOOTHING, product_config->smoothing_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_PLASMA_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_PLASMA_DIAGNOSTICS) == BST_CHECKED;
}

static void plasma_initialize_dialog(HWND dialog, plasma_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    plasma_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Plasma\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nPalette-driven plasma, fire, and interference motion with curated dark-room presets and calmer long-run composition refresh.");
    SetDlgItemTextA(dialog, IDC_PLASMA_INFO, info);

    plasma_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK plasma_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    plasma_dialog_state *dialog_state;

    dialog_state = (plasma_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (plasma_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            plasma_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_PLASMA_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            plasma_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_PLASMA_DEFAULTS) {
            plasma_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            plasma_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            plasma_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            plasma_config_clamp(
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

INT_PTR plasma_config_show_dialog(
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
    plasma_dialog_state dialog_state;
    plasma_config *config;

    config = plasma_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6503UL,
            "The Plasma dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_PLASMA_CONFIG),
        owner_window,
        plasma_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        plasma_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6504UL,
            "The Plasma config dialog resource could not be loaded."
        );
    }

    return result;
}

static int plasma_parse_effect_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "plasma") == 0) {
        *value_out = PLASMA_EFFECT_PLASMA;
        return 1;
    }
    if (lstrcmpiA(text, "fire") == 0) {
        *value_out = PLASMA_EFFECT_FIRE;
        return 1;
    }
    if (lstrcmpiA(text, "interference") == 0) {
        *value_out = PLASMA_EFFECT_INTERFERENCE;
        return 1;
    }
    return 0;
}

static int plasma_parse_speed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "gentle") == 0) {
        *value_out = PLASMA_SPEED_GENTLE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PLASMA_SPEED_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "lively") == 0) {
        *value_out = PLASMA_SPEED_LIVELY;
        return 1;
    }
    return 0;
}

static int plasma_parse_resolution_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "coarse") == 0) {
        *value_out = PLASMA_RESOLUTION_COARSE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PLASMA_RESOLUTION_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "fine") == 0) {
        *value_out = PLASMA_RESOLUTION_FINE;
        return 1;
    }
    return 0;
}

static int plasma_parse_smoothing_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "off") == 0) {
        *value_out = PLASMA_SMOOTHING_OFF;
        return 1;
    }
    if (lstrcmpiA(text, "soft") == 0) {
        *value_out = PLASMA_SMOOTHING_SOFT;
        return 1;
    }
    if (lstrcmpiA(text, "glow") == 0) {
        *value_out = PLASMA_SMOOTHING_GLOW;
        return 1;
    }
    return 0;
}

int plasma_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const plasma_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = plasma_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "effect", plasma_effect_mode_name(config->effect_mode)) &&
        writer->write_string(writer->context, "product", "speed", plasma_speed_mode_name(config->speed_mode)) &&
        writer->write_string(
            writer->context,
            "product",
            "resolution",
            plasma_resolution_mode_name(config->resolution_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "smoothing",
            plasma_smoothing_mode_name(config->smoothing_mode)
        );
}

int plasma_config_import_settings_entry(
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
    plasma_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = plasma_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "effect_mode") == 0 || lstrcmpiA(key, "effect") == 0) {
        return plasma_parse_effect_mode(value, &config->effect_mode);
    }
    if (lstrcmpiA(key, "speed_mode") == 0 || lstrcmpiA(key, "speed") == 0) {
        return plasma_parse_speed_mode(value, &config->speed_mode);
    }
    if (lstrcmpiA(key, "resolution_mode") == 0 || lstrcmpiA(key, "resolution") == 0) {
        return plasma_parse_resolution_mode(value, &config->resolution_mode);
    }
    if (lstrcmpiA(key, "smoothing_mode") == 0 || lstrcmpiA(key, "smoothing") == 0) {
        return plasma_parse_smoothing_mode(value, &config->smoothing_mode);
    }

    return 1;
}

void plasma_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    plasma_config *config;
    plasma_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = plasma_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x504C4153UL;
    plasma_rng_seed(&rng, random_seed ^ 0x504C4153UL);
    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 36UL) {
        config->effect_mode = PLASMA_EFFECT_PLASMA;
    } else if (roll < 68UL) {
        config->effect_mode = PLASMA_EFFECT_FIRE;
    } else {
        config->effect_mode = PLASMA_EFFECT_INTERFERENCE;
    }

    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 42UL) {
        config->speed_mode = PLASMA_SPEED_GENTLE;
    } else if (roll < 82UL) {
        config->speed_mode = PLASMA_SPEED_STANDARD;
    } else {
        config->speed_mode = PLASMA_SPEED_LIVELY;
    }

    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 28UL) {
        config->resolution_mode = PLASMA_RESOLUTION_COARSE;
    } else if (roll < 72UL) {
        config->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    } else {
        config->resolution_mode = PLASMA_RESOLUTION_FINE;
    }

    roll = plasma_rng_range(&rng, 100UL);
    if (roll < 20UL) {
        config->smoothing_mode = PLASMA_SMOOTHING_OFF;
    } else if (roll < 70UL) {
        config->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    } else {
        config->smoothing_mode = PLASMA_SMOOTHING_GLOW;
    }
}
