#include <string.h>

#include "nocturne_internal.h"
#include "nocturne_resource.h"
#include "screensave/version.h"

typedef struct nocturne_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    nocturne_config *product_config;
} nocturne_dialog_state;

typedef struct nocturne_combo_item_tag {
    int value;
    const char *display_name;
} nocturne_combo_item;

static const nocturne_combo_item g_nocturne_motion_items[] = {
    { NOCTURNE_MOTION_NONE, "Pure Black" },
    { NOCTURNE_MOTION_DRIFT_MARK, "Drift Mark" },
    { NOCTURNE_MOTION_QUIET_LINE, "Quiet Line" },
    { NOCTURNE_MOTION_MONOLITH, "Monolith" },
    { NOCTURNE_MOTION_BREATH, "Breath" }
};

static const nocturne_combo_item g_nocturne_fade_items[] = {
    { NOCTURNE_FADE_SLOW, "Slow Fade" },
    { NOCTURNE_FADE_STANDARD, "Standard Fade" },
    { NOCTURNE_FADE_GENTLE, "Gentle Fade" }
};

static const nocturne_combo_item g_nocturne_strength_items[] = {
    { NOCTURNE_STRENGTH_STILL, "Still" },
    { NOCTURNE_STRENGTH_SUBTLE, "Subtle" },
    { NOCTURNE_STRENGTH_SOFT, "Soft" }
};

static void nocturne_emit_config_diag(
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
        "nocturne_config",
        text
    );
}

static nocturne_config *nocturne_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(nocturne_config)) {
        return NULL;
    }

    return (nocturne_config *)product_config;
}

static const nocturne_config *nocturne_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(nocturne_config)) {
        return NULL;
    }

    return (const nocturne_config *)product_config;
}

static int nocturne_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(NOCTURNE_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
           lstrcpyA(buffer, NOCTURNE_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int nocturne_read_flag(HKEY key, const char *value_name, int *value)
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

static int nocturne_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int nocturne_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG nocturne_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG nocturne_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG nocturne_write_string(HKEY key, const char *value_name, const char *value)
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

void nocturne_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    nocturne_config *config;

    config = nocturne_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->motion_mode = NOCTURNE_MOTION_MONOLITH;
    config->fade_speed = NOCTURNE_FADE_STANDARD;
    config->motion_strength = NOCTURNE_STRENGTH_SUBTLE;
    nocturne_apply_preset_to_config(NOCTURNE_DEFAULT_PRESET_KEY, common_config, config);
}

void nocturne_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    nocturne_config *config;

    config = nocturne_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        nocturne_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = NOCTURNE_DEFAULT_PRESET_KEY;
    }

    if (common_config->theme_key == NULL || nocturne_find_theme_descriptor(common_config->theme_key) == NULL) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = nocturne_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL ? preset_descriptor->theme_key : NOCTURNE_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = NOCTURNE_DEFAULT_THEME_KEY;
        }
    }

    if (config->motion_mode < NOCTURNE_MOTION_NONE || config->motion_mode > NOCTURNE_MOTION_BREATH) {
        config->motion_mode = NOCTURNE_MOTION_MONOLITH;
    }
    if (config->fade_speed < NOCTURNE_FADE_SLOW || config->fade_speed > NOCTURNE_FADE_GENTLE) {
        config->fade_speed = NOCTURNE_FADE_STANDARD;
    }
    if (config->motion_strength < NOCTURNE_STRENGTH_STILL || config->motion_strength > NOCTURNE_STRENGTH_SOFT) {
        config->motion_strength = NOCTURNE_STRENGTH_SUBTLE;
    }
}

int nocturne_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    nocturne_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = nocturne_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    nocturne_config_set_defaults(common_config, product_config, product_config_size);
    if (!nocturne_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        nocturne_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (nocturne_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        nocturne_apply_preset_to_config(preset_key, common_config, config);
    }

    if (nocturne_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (nocturne_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = nocturne_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (nocturne_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)nocturne_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (nocturne_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)nocturne_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->motion_mode;
    if (nocturne_read_dword(key, "MotionMode", &value_dword)) {
        config->motion_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->fade_speed;
    if (nocturne_read_dword(key, "FadeSpeed", &value_dword)) {
        config->fade_speed = (int)value_dword;
    }
    value_dword = (unsigned long)config->motion_strength;
    if (nocturne_read_dword(key, "MotionStrength", &value_dword)) {
        config->motion_strength = (int)value_dword;
    }

    RegCloseKey(key);
    nocturne_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int nocturne_config_save(
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
    nocturne_config safe_product_config;
    screensave_common_config safe_common_config;
    const nocturne_config *config;

    (void)module;

    config = nocturne_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    nocturne_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!nocturne_build_registry_path(path, sizeof(path))) {
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
        nocturne_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6101UL,
            "The Nocturne registry key could not be opened for writing."
        );
        return 0;
    }

    result = nocturne_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_dword(key, "MotionMode", (unsigned long)safe_product_config.motion_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_dword(key, "FadeSpeed", (unsigned long)safe_product_config.fade_speed);
    }
    if (result == ERROR_SUCCESS) {
        result = nocturne_write_dword(key, "MotionStrength", (unsigned long)safe_product_config.motion_strength);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        nocturne_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6102UL,
            "The Nocturne registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT nocturne_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void nocturne_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static int nocturne_get_combo_value(HWND dialog, int control_id, int default_value)
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

static void nocturne_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_NOCTURNE_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        nocturne_add_combo_item(
            dialog,
            IDC_NOCTURNE_PRESET,
            module->presets[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_NOCTURNE_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        nocturne_add_combo_item(
            dialog,
            IDC_NOCTURNE_THEME,
            module->themes[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_NOCTURNE_MOTION, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_nocturne_motion_items) / sizeof(g_nocturne_motion_items[0])); ++index) {
        nocturne_add_combo_item(
            dialog,
            IDC_NOCTURNE_MOTION,
            g_nocturne_motion_items[index].display_name,
            (LPARAM)g_nocturne_motion_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_NOCTURNE_FADE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_nocturne_fade_items) / sizeof(g_nocturne_fade_items[0])); ++index) {
        nocturne_add_combo_item(
            dialog,
            IDC_NOCTURNE_FADE,
            g_nocturne_fade_items[index].display_name,
            (LPARAM)g_nocturne_fade_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_NOCTURNE_STRENGTH, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_nocturne_strength_items) / sizeof(g_nocturne_strength_items[0])); ++index) {
        nocturne_add_combo_item(
            dialog,
            IDC_NOCTURNE_STRENGTH,
            g_nocturne_strength_items[index].display_name,
            (LPARAM)g_nocturne_strength_items[index].value
        );
    }
}

static void nocturne_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const nocturne_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_NOCTURNE_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_NOCTURNE_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    nocturne_select_combo_value(dialog, IDC_NOCTURNE_MOTION, (LPARAM)product_config->motion_mode);
    nocturne_select_combo_value(dialog, IDC_NOCTURNE_FADE, (LPARAM)product_config->fade_speed);
    nocturne_select_combo_value(dialog, IDC_NOCTURNE_STRENGTH, (LPARAM)product_config->motion_strength);

    CheckDlgButton(
        dialog,
        IDC_NOCTURNE_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_NOCTURNE_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void nocturne_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    nocturne_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_NOCTURNE_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_NOCTURNE_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.motion_mode = NOCTURNE_MOTION_MONOLITH;
    product_config.fade_speed = NOCTURNE_FADE_STANDARD;
    product_config.motion_strength = NOCTURNE_STRENGTH_SUBTLE;

    nocturne_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    nocturne_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(
        dialog,
        IDC_NOCTURNE_DIAGNOSTICS,
        diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void nocturne_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    nocturne_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    nocturne_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_NOCTURNE_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        nocturne_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_NOCTURNE_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->motion_mode = nocturne_get_combo_value(dialog, IDC_NOCTURNE_MOTION, product_config->motion_mode);
    product_config->fade_speed = nocturne_get_combo_value(dialog, IDC_NOCTURNE_FADE, product_config->fade_speed);
    product_config->motion_strength = nocturne_get_combo_value(
        dialog,
        IDC_NOCTURNE_STRENGTH,
        product_config->motion_strength
    );
    common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_NOCTURNE_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_NOCTURNE_DIAGNOSTICS) == BST_CHECKED;
}

static void nocturne_initialize_dialog(HWND dialog, nocturne_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    nocturne_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Nocturne\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nRestrained dark-room saver with curated near-black themes and calm long-run drift.");
    SetDlgItemTextA(dialog, IDC_NOCTURNE_INFO, info);

    nocturne_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK nocturne_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    nocturne_dialog_state *dialog_state;

    dialog_state = (nocturne_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (nocturne_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            nocturne_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_NOCTURNE_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            nocturne_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_NOCTURNE_DEFAULTS) {
            nocturne_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            nocturne_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            nocturne_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            nocturne_config_clamp(
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

INT_PTR nocturne_config_show_dialog(
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
    nocturne_dialog_state dialog_state;
    nocturne_config *config;

    config = nocturne_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        nocturne_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6103UL,
            "The Nocturne dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_NOCTURNE_CONFIG),
        owner_window,
        nocturne_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        nocturne_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6104UL,
            "The Nocturne config dialog resource could not be loaded."
        );
    }

    return result;
}

static int nocturne_parse_motion_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "none") == 0) {
        *value_out = NOCTURNE_MOTION_NONE;
        return 1;
    }
    if (lstrcmpiA(text, "drift_mark") == 0) {
        *value_out = NOCTURNE_MOTION_DRIFT_MARK;
        return 1;
    }
    if (lstrcmpiA(text, "quiet_line") == 0) {
        *value_out = NOCTURNE_MOTION_QUIET_LINE;
        return 1;
    }
    if (lstrcmpiA(text, "monolith") == 0) {
        *value_out = NOCTURNE_MOTION_MONOLITH;
        return 1;
    }
    if (lstrcmpiA(text, "breath") == 0) {
        *value_out = NOCTURNE_MOTION_BREATH;
        return 1;
    }

    return 0;
}

static int nocturne_parse_fade_speed(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "slow") == 0) {
        *value_out = NOCTURNE_FADE_SLOW;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = NOCTURNE_FADE_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "gentle") == 0) {
        *value_out = NOCTURNE_FADE_GENTLE;
        return 1;
    }

    return 0;
}

static int nocturne_parse_motion_strength(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "still") == 0) {
        *value_out = NOCTURNE_STRENGTH_STILL;
        return 1;
    }
    if (lstrcmpiA(text, "subtle") == 0) {
        *value_out = NOCTURNE_STRENGTH_SUBTLE;
        return 1;
    }
    if (lstrcmpiA(text, "soft") == 0) {
        *value_out = NOCTURNE_STRENGTH_SOFT;
        return 1;
    }

    return 0;
}

int nocturne_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const nocturne_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = nocturne_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "motion", nocturne_motion_mode_name(config->motion_mode)) &&
        writer->write_string(writer->context, "product", "fade", nocturne_fade_speed_name(config->fade_speed)) &&
        writer->write_string(
            writer->context,
            "product",
            "strength",
            nocturne_motion_strength_name(config->motion_strength)
        );
}

int nocturne_config_import_settings_entry(
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
    nocturne_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = nocturne_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "motion_mode") == 0 || lstrcmpiA(key, "motion") == 0) {
        return nocturne_parse_motion_mode(value, &config->motion_mode);
    }
    if (lstrcmpiA(key, "fade_speed") == 0 || lstrcmpiA(key, "fade") == 0) {
        return nocturne_parse_fade_speed(value, &config->fade_speed);
    }
    if (lstrcmpiA(key, "motion_strength") == 0 || lstrcmpiA(key, "strength") == 0) {
        return nocturne_parse_motion_strength(value, &config->motion_strength);
    }

    return 1;
}

void nocturne_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    nocturne_config *config;
    nocturne_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = nocturne_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x4E4F4354UL;
    nocturne_rng_seed(&rng, random_seed ^ 0x4E4F4354UL);

    roll = nocturne_rng_range(&rng, 100UL);
    if (roll < 6UL) {
        config->motion_mode = NOCTURNE_MOTION_NONE;
    } else if (roll < 26UL) {
        config->motion_mode = NOCTURNE_MOTION_DRIFT_MARK;
    } else if (roll < 56UL) {
        config->motion_mode = NOCTURNE_MOTION_QUIET_LINE;
    } else if (roll < 82UL) {
        config->motion_mode = NOCTURNE_MOTION_MONOLITH;
    } else {
        config->motion_mode = NOCTURNE_MOTION_BREATH;
    }

    roll = nocturne_rng_range(&rng, 100UL);
    if (roll < 24UL) {
        config->fade_speed = NOCTURNE_FADE_SLOW;
    } else if (roll < 74UL) {
        config->fade_speed = NOCTURNE_FADE_GENTLE;
    } else {
        config->fade_speed = NOCTURNE_FADE_STANDARD;
    }

    roll = nocturne_rng_range(&rng, 100UL);
    if (roll < 18UL) {
        config->motion_strength = NOCTURNE_STRENGTH_STILL;
    } else if (roll < 76UL) {
        config->motion_strength = NOCTURNE_STRENGTH_SUBTLE;
    } else {
        config->motion_strength = NOCTURNE_STRENGTH_SOFT;
    }
}
