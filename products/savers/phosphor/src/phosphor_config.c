#include "phosphor_internal.h"
#include "phosphor_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct phosphor_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    phosphor_config *product_config;
} phosphor_dialog_state;

typedef struct phosphor_combo_item_tag {
    int value;
    const char *display_name;
} phosphor_combo_item;

static const phosphor_combo_item g_phosphor_curve_items[] = {
    { PHOSPHOR_CURVE_LISSAJOUS, "Lissajous" },
    { PHOSPHOR_CURVE_HARMONOGRAPH, "Harmonograph" },
    { PHOSPHOR_CURVE_DENSE, "Dense Trace" }
};

static const phosphor_combo_item g_phosphor_persistence_items[] = {
    { PHOSPHOR_PERSISTENCE_SHORT, "Short Persistence" },
    { PHOSPHOR_PERSISTENCE_STANDARD, "Standard Persistence" },
    { PHOSPHOR_PERSISTENCE_LONG, "Long Persistence" }
};

static const phosphor_combo_item g_phosphor_drift_items[] = {
    { PHOSPHOR_DRIFT_CALM, "Calm" },
    { PHOSPHOR_DRIFT_STANDARD, "Standard" },
    { PHOSPHOR_DRIFT_WIDE, "Wide" }
};

static const phosphor_combo_item g_phosphor_mirror_items[] = {
    { PHOSPHOR_MIRROR_NONE, "None" },
    { PHOSPHOR_MIRROR_HORIZONTAL, "Horizontal" },
    { PHOSPHOR_MIRROR_QUAD, "Quad" }
};

static void phosphor_emit_config_diag(
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
        "phosphor_config",
        text
    );
}

static phosphor_config *phosphor_as_config(
    void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(phosphor_config)) {
        return NULL;
    }

    return (phosphor_config *)product_config;
}

static const phosphor_config *phosphor_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(phosphor_config)) {
        return NULL;
    }

    return (const phosphor_config *)product_config;
}

static int phosphor_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(PHOSPHOR_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, PHOSPHOR_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int phosphor_read_flag(HKEY key, const char *value_name, int *value)
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

static int phosphor_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int phosphor_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG phosphor_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG phosphor_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG phosphor_write_string(HKEY key, const char *value_name, const char *value)
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

void phosphor_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    phosphor_config *config;

    config = phosphor_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->curve_mode = PHOSPHOR_CURVE_LISSAJOUS;
    config->persistence_mode = PHOSPHOR_PERSISTENCE_STANDARD;
    config->drift_mode = PHOSPHOR_DRIFT_STANDARD;
    config->mirror_mode = PHOSPHOR_MIRROR_NONE;
    phosphor_apply_preset_to_config(PHOSPHOR_DEFAULT_PRESET_KEY, common_config, config);
}

void phosphor_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    phosphor_config *config;

    config = phosphor_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        phosphor_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = PHOSPHOR_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        phosphor_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = phosphor_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : PHOSPHOR_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = PHOSPHOR_DEFAULT_THEME_KEY;
        }
    }

    if (
        config->curve_mode < PHOSPHOR_CURVE_LISSAJOUS ||
        config->curve_mode > PHOSPHOR_CURVE_DENSE
    ) {
        config->curve_mode = PHOSPHOR_CURVE_LISSAJOUS;
    }
    if (
        config->persistence_mode < PHOSPHOR_PERSISTENCE_SHORT ||
        config->persistence_mode > PHOSPHOR_PERSISTENCE_LONG
    ) {
        config->persistence_mode = PHOSPHOR_PERSISTENCE_STANDARD;
    }
    if (
        config->drift_mode < PHOSPHOR_DRIFT_CALM ||
        config->drift_mode > PHOSPHOR_DRIFT_WIDE
    ) {
        config->drift_mode = PHOSPHOR_DRIFT_STANDARD;
    }
    if (
        config->mirror_mode < PHOSPHOR_MIRROR_NONE ||
        config->mirror_mode > PHOSPHOR_MIRROR_QUAD
    ) {
        config->mirror_mode = PHOSPHOR_MIRROR_NONE;
    }
}

int phosphor_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    phosphor_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)diagnostics;

    config = phosphor_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    phosphor_config_set_defaults(common_config, product_config, product_config_size);
    if (!phosphor_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "phosphor"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            phosphor_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (phosphor_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        phosphor_apply_preset_to_config(preset_key, common_config, config);
    }

    if (phosphor_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (phosphor_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = phosphor_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (phosphor_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)phosphor_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (phosphor_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)phosphor_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->curve_mode;
    if (phosphor_read_dword(key, "CurveMode", &value_dword)) {
        config->curve_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->persistence_mode;
    if (phosphor_read_dword(key, "PersistenceMode", &value_dword)) {
        config->persistence_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->drift_mode;
    if (phosphor_read_dword(key, "DriftMode", &value_dword)) {
        config->drift_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->mirror_mode;
    if (phosphor_read_dword(key, "MirrorMode", &value_dword)) {
        config->mirror_mode = (int)value_dword;
    }

    RegCloseKey(key);
    phosphor_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int phosphor_config_save(
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
    phosphor_config safe_product_config;
    screensave_common_config safe_common_config;
    const phosphor_config *config;

    (void)module;

    config = phosphor_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    phosphor_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!phosphor_build_registry_path(path, sizeof(path))) {
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
        phosphor_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6601UL,
            "The Phosphor registry key could not be opened for writing."
        );
        return 0;
    }

    result = phosphor_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_dword(key, "CurveMode", (unsigned long)safe_product_config.curve_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_dword(key, "PersistenceMode", (unsigned long)safe_product_config.persistence_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_dword(key, "DriftMode", (unsigned long)safe_product_config.drift_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = phosphor_write_dword(key, "MirrorMode", (unsigned long)safe_product_config.mirror_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        phosphor_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6602UL,
            "The Phosphor registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT phosphor_add_combo_item(
    HWND dialog,
    int control_id,
    const char *text,
    LPARAM item_data
)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void phosphor_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static int phosphor_get_combo_value(HWND dialog, int control_id, int default_value)
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

static void phosphor_populate_dialog_lists(
    HWND dialog,
    const screensave_saver_module *module
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_PHOSPHOR_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        phosphor_add_combo_item(
            dialog,
            IDC_PHOSPHOR_PRESET,
            module->presets[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_PHOSPHOR_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        phosphor_add_combo_item(
            dialog,
            IDC_PHOSPHOR_THEME,
            module->themes[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_PHOSPHOR_CURVE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_phosphor_curve_items) / sizeof(g_phosphor_curve_items[0])); ++index) {
        phosphor_add_combo_item(
            dialog,
            IDC_PHOSPHOR_CURVE,
            g_phosphor_curve_items[index].display_name,
            (LPARAM)g_phosphor_curve_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_PHOSPHOR_PERSISTENCE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_phosphor_persistence_items) / sizeof(g_phosphor_persistence_items[0])); ++index) {
        phosphor_add_combo_item(
            dialog,
            IDC_PHOSPHOR_PERSISTENCE,
            g_phosphor_persistence_items[index].display_name,
            (LPARAM)g_phosphor_persistence_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_PHOSPHOR_DRIFT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_phosphor_drift_items) / sizeof(g_phosphor_drift_items[0])); ++index) {
        phosphor_add_combo_item(
            dialog,
            IDC_PHOSPHOR_DRIFT,
            g_phosphor_drift_items[index].display_name,
            (LPARAM)g_phosphor_drift_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_PHOSPHOR_MIRROR, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_phosphor_mirror_items) / sizeof(g_phosphor_mirror_items[0])); ++index) {
        phosphor_add_combo_item(
            dialog,
            IDC_PHOSPHOR_MIRROR,
            g_phosphor_mirror_items[index].display_name,
            (LPARAM)g_phosphor_mirror_items[index].value
        );
    }
}

static void phosphor_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const phosphor_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PHOSPHOR_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_PHOSPHOR_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    phosphor_select_combo_value(
        dialog,
        IDC_PHOSPHOR_CURVE,
        (LPARAM)product_config->curve_mode
    );
    phosphor_select_combo_value(
        dialog,
        IDC_PHOSPHOR_PERSISTENCE,
        (LPARAM)product_config->persistence_mode
    );
    phosphor_select_combo_value(
        dialog,
        IDC_PHOSPHOR_DRIFT,
        (LPARAM)product_config->drift_mode
    );
    phosphor_select_combo_value(
        dialog,
        IDC_PHOSPHOR_MIRROR,
        (LPARAM)product_config->mirror_mode
    );

    CheckDlgButton(
        dialog,
        IDC_PHOSPHOR_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_PHOSPHOR_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void phosphor_apply_preset_selection(
    HWND dialog,
    const screensave_saver_module *module
)
{
    LRESULT preset_index;
    phosphor_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_PHOSPHOR_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_PHOSPHOR_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.curve_mode = PHOSPHOR_CURVE_LISSAJOUS;
    product_config.persistence_mode = PHOSPHOR_PERSISTENCE_STANDARD;
    product_config.drift_mode = PHOSPHOR_DRIFT_STANDARD;
    product_config.mirror_mode = PHOSPHOR_MIRROR_NONE;
    phosphor_apply_preset_to_config(
        module->presets[preset_index].preset_key,
        &common_config,
        &product_config
    );
    phosphor_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(
        dialog,
        IDC_PHOSPHOR_DIAGNOSTICS,
        diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void phosphor_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    phosphor_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    phosphor_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_PHOSPHOR_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        phosphor_apply_preset_to_config(
            module->presets[preset_index].preset_key,
            common_config,
            product_config
        );
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_PHOSPHOR_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->curve_mode = phosphor_get_combo_value(
        dialog,
        IDC_PHOSPHOR_CURVE,
        product_config->curve_mode
    );
    product_config->persistence_mode = phosphor_get_combo_value(
        dialog,
        IDC_PHOSPHOR_PERSISTENCE,
        product_config->persistence_mode
    );
    product_config->drift_mode = phosphor_get_combo_value(
        dialog,
        IDC_PHOSPHOR_DRIFT,
        product_config->drift_mode
    );
    product_config->mirror_mode = phosphor_get_combo_value(
        dialog,
        IDC_PHOSPHOR_MIRROR,
        product_config->mirror_mode
    );
    common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_PHOSPHOR_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_PHOSPHOR_DIAGNOSTICS) == BST_CHECKED;
}

static void phosphor_initialize_dialog(
    HWND dialog,
    phosphor_dialog_state *dialog_state
)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    phosphor_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Phosphor\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nVector-style phosphor curves with restrained mirror controls, curated laboratory presets, and calmer long-run choreography.");
    SetDlgItemTextA(dialog, IDC_PHOSPHOR_INFO, info);

    phosphor_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK phosphor_config_dialog_proc(
    HWND dialog,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    phosphor_dialog_state *dialog_state;

    dialog_state = (phosphor_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (phosphor_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            phosphor_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_PHOSPHOR_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            phosphor_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_PHOSPHOR_DEFAULTS) {
            phosphor_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            phosphor_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            phosphor_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            phosphor_config_clamp(
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

INT_PTR phosphor_config_show_dialog(
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
    phosphor_dialog_state dialog_state;
    phosphor_config *config;

    config = phosphor_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        phosphor_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6603UL,
            "The Phosphor dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_PHOSPHOR_CONFIG),
        owner_window,
        phosphor_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        phosphor_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6604UL,
            "The Phosphor config dialog resource could not be loaded."
        );
    }

    return result;
}

static int phosphor_parse_curve_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "lissajous") == 0) {
        *value_out = PHOSPHOR_CURVE_LISSAJOUS;
        return 1;
    }
    if (lstrcmpiA(text, "harmonograph") == 0) {
        *value_out = PHOSPHOR_CURVE_HARMONOGRAPH;
        return 1;
    }
    if (lstrcmpiA(text, "dense") == 0) {
        *value_out = PHOSPHOR_CURVE_DENSE;
        return 1;
    }
    return 0;
}

static int phosphor_parse_persistence_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "short") == 0) {
        *value_out = PHOSPHOR_PERSISTENCE_SHORT;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PHOSPHOR_PERSISTENCE_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "long") == 0) {
        *value_out = PHOSPHOR_PERSISTENCE_LONG;
        return 1;
    }
    return 0;
}

static int phosphor_parse_drift_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "calm") == 0) {
        *value_out = PHOSPHOR_DRIFT_CALM;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = PHOSPHOR_DRIFT_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "wide") == 0) {
        *value_out = PHOSPHOR_DRIFT_WIDE;
        return 1;
    }
    return 0;
}

static int phosphor_parse_mirror_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "none") == 0) {
        *value_out = PHOSPHOR_MIRROR_NONE;
        return 1;
    }
    if (lstrcmpiA(text, "horizontal") == 0) {
        *value_out = PHOSPHOR_MIRROR_HORIZONTAL;
        return 1;
    }
    if (lstrcmpiA(text, "quad") == 0) {
        *value_out = PHOSPHOR_MIRROR_QUAD;
        return 1;
    }
    return 0;
}

int phosphor_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const phosphor_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = phosphor_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "curve", phosphor_curve_mode_name(config->curve_mode)) &&
        writer->write_string(
            writer->context,
            "product",
            "trail",
            phosphor_persistence_mode_name(config->persistence_mode)
        ) &&
        writer->write_string(writer->context, "product", "drift", phosphor_drift_mode_name(config->drift_mode)) &&
        writer->write_string(writer->context, "product", "mirror", phosphor_mirror_mode_name(config->mirror_mode));
}

int phosphor_config_import_settings_entry(
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
    phosphor_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = phosphor_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "curve") == 0 || lstrcmpiA(key, "curve_mode") == 0) {
        return phosphor_parse_curve_mode(value, &config->curve_mode);
    }
    if (
        lstrcmpiA(key, "trail") == 0 ||
        lstrcmpiA(key, "trail_mode") == 0 ||
        lstrcmpiA(key, "persistence") == 0 ||
        lstrcmpiA(key, "persistence_mode") == 0
    ) {
        return phosphor_parse_persistence_mode(value, &config->persistence_mode);
    }
    if (lstrcmpiA(key, "drift") == 0 || lstrcmpiA(key, "drift_mode") == 0) {
        return phosphor_parse_drift_mode(value, &config->drift_mode);
    }
    if (lstrcmpiA(key, "mirror") == 0 || lstrcmpiA(key, "mirror_mode") == 0) {
        return phosphor_parse_mirror_mode(value, &config->mirror_mode);
    }

    return 1;
}

void phosphor_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    phosphor_config *config;
    phosphor_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = phosphor_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x50484F53UL;
    phosphor_rng_seed(&rng, random_seed ^ 0x50484F53UL);

    roll = phosphor_rng_range(&rng, 100UL);
    if (roll < 36UL) {
        config->curve_mode = PHOSPHOR_CURVE_LISSAJOUS;
    } else if (roll < 72UL) {
        config->curve_mode = PHOSPHOR_CURVE_HARMONOGRAPH;
    } else {
        config->curve_mode = PHOSPHOR_CURVE_DENSE;
    }

    roll = phosphor_rng_range(&rng, 100UL);
    if (roll < 24UL) {
        config->persistence_mode = PHOSPHOR_PERSISTENCE_SHORT;
    } else if (roll < 68UL) {
        config->persistence_mode = PHOSPHOR_PERSISTENCE_STANDARD;
    } else {
        config->persistence_mode = PHOSPHOR_PERSISTENCE_LONG;
    }

    roll = phosphor_rng_range(&rng, 100UL);
    if (roll < 34UL) {
        config->drift_mode = PHOSPHOR_DRIFT_CALM;
    } else if (roll < 78UL) {
        config->drift_mode = PHOSPHOR_DRIFT_STANDARD;
    } else {
        config->drift_mode = PHOSPHOR_DRIFT_WIDE;
    }

    roll = phosphor_rng_range(&rng, 100UL);
    if (roll < 32UL) {
        config->mirror_mode = PHOSPHOR_MIRROR_NONE;
    } else if (roll < 68UL) {
        config->mirror_mode = PHOSPHOR_MIRROR_HORIZONTAL;
    } else {
        config->mirror_mode = PHOSPHOR_MIRROR_QUAD;
    }
}
