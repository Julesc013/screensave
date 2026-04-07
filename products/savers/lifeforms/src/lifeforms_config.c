#include "lifeforms_internal.h"
#include "lifeforms_resource.h"
#include "screensave/version.h"

typedef struct lifeforms_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    lifeforms_config *product_config;
} lifeforms_dialog_state;

typedef struct lifeforms_combo_item_tag {
    int value;
    const char *display_name;
} lifeforms_combo_item;

static const lifeforms_combo_item g_lifeforms_rule_items[] = {
    { LIFEFORMS_RULE_CONWAY, "Conway Life" },
    { LIFEFORMS_RULE_HIGHLIFE, "HighLife" }
};

static const lifeforms_combo_item g_lifeforms_density_items[] = {
    { LIFEFORMS_DENSITY_SPARSE, "Sparse" },
    { LIFEFORMS_DENSITY_STANDARD, "Standard" },
    { LIFEFORMS_DENSITY_DENSE, "Dense" }
};

static const lifeforms_combo_item g_lifeforms_seed_items[] = {
    { LIFEFORMS_SEED_SPARSE, "Sparse" },
    { LIFEFORMS_SEED_BALANCED, "Balanced" },
    { LIFEFORMS_SEED_BLOOM, "Bloom" }
};

static const lifeforms_combo_item g_lifeforms_reseed_items[] = {
    { LIFEFORMS_RESEED_PATIENT, "Patient" },
    { LIFEFORMS_RESEED_STANDARD, "Standard" },
    { LIFEFORMS_RESEED_ALERT, "Alert" }
};

static void lifeforms_emit_config_diag(
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
        "lifeforms_config",
        text
    );
}

static lifeforms_config *lifeforms_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(lifeforms_config)) {
        return NULL;
    }

    return (lifeforms_config *)product_config;
}

static const lifeforms_config *lifeforms_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(lifeforms_config)) {
        return NULL;
    }

    return (const lifeforms_config *)product_config;
}

static int lifeforms_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(LIFEFORMS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, LIFEFORMS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int lifeforms_read_flag(HKEY key, const char *value_name, int *value)
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

static int lifeforms_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int lifeforms_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG lifeforms_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG lifeforms_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG lifeforms_write_string(HKEY key, const char *value_name, const char *value)
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

void lifeforms_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    lifeforms_config *config;

    config = lifeforms_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->rule_mode = LIFEFORMS_RULE_CONWAY;
    config->density_mode = LIFEFORMS_DENSITY_STANDARD;
    config->seed_mode = LIFEFORMS_SEED_BALANCED;
    config->reseed_mode = LIFEFORMS_RESEED_STANDARD;
    lifeforms_apply_preset_to_config(LIFEFORMS_DEFAULT_PRESET_KEY, common_config, config);
}

void lifeforms_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    lifeforms_config *config;

    config = lifeforms_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        lifeforms_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = LIFEFORMS_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        lifeforms_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = lifeforms_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : LIFEFORMS_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = LIFEFORMS_DEFAULT_THEME_KEY;
        }
    }

    if (config->rule_mode < LIFEFORMS_RULE_CONWAY || config->rule_mode > LIFEFORMS_RULE_HIGHLIFE) {
        config->rule_mode = LIFEFORMS_RULE_CONWAY;
    }
    if (
        config->density_mode < LIFEFORMS_DENSITY_SPARSE ||
        config->density_mode > LIFEFORMS_DENSITY_DENSE
    ) {
        config->density_mode = LIFEFORMS_DENSITY_STANDARD;
    }
    if (config->seed_mode < LIFEFORMS_SEED_SPARSE || config->seed_mode > LIFEFORMS_SEED_BLOOM) {
        config->seed_mode = LIFEFORMS_SEED_BALANCED;
    }
    if (
        config->reseed_mode < LIFEFORMS_RESEED_PATIENT ||
        config->reseed_mode > LIFEFORMS_RESEED_ALERT
    ) {
        config->reseed_mode = LIFEFORMS_RESEED_STANDARD;
    }
}

int lifeforms_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    lifeforms_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = lifeforms_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    lifeforms_config_set_defaults(common_config, product_config, product_config_size);
    if (!lifeforms_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        lifeforms_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (lifeforms_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        lifeforms_apply_preset_to_config(preset_key, common_config, config);
    }

    if (lifeforms_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (lifeforms_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = lifeforms_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (lifeforms_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)lifeforms_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (lifeforms_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)lifeforms_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->rule_mode;
    if (lifeforms_read_dword(key, "RuleMode", &value_dword)) {
        config->rule_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->density_mode;
    if (lifeforms_read_dword(key, "DensityMode", &value_dword)) {
        config->density_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->seed_mode;
    if (lifeforms_read_dword(key, "SeedMode", &value_dword)) {
        config->seed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->reseed_mode;
    if (lifeforms_read_dword(key, "ReseedMode", &value_dword)) {
        config->reseed_mode = (int)value_dword;
    }

    RegCloseKey(key);
    lifeforms_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int lifeforms_config_save(
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
    lifeforms_config safe_product_config;
    screensave_common_config safe_common_config;
    const lifeforms_config *config;

    (void)module;

    config = lifeforms_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        lifeforms_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6801UL,
            "The Lifeforms config could not be saved because the config state was invalid."
        );
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    lifeforms_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!lifeforms_build_registry_path(path, sizeof(path))) {
        lifeforms_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6802UL,
            "The Lifeforms config registry path could not be constructed."
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
        lifeforms_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6803UL,
            "The Lifeforms config registry key could not be opened for writing."
        );
        return 0;
    }

    result = lifeforms_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_dword(key, "RuleMode", (unsigned long)safe_product_config.rule_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_dword(key, "DensityMode", (unsigned long)safe_product_config.density_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_dword(key, "SeedMode", (unsigned long)safe_product_config.seed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = lifeforms_write_dword(key, "ReseedMode", (unsigned long)safe_product_config.reseed_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        lifeforms_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6804UL,
            "The Lifeforms config values could not be written to the registry."
        );
        return 0;
    }

    return 1;
}

static void lifeforms_populate_combo(
    HWND dialog,
    int control_id,
    const lifeforms_combo_item *items,
    unsigned int item_count
)
{
    unsigned int index;
    LRESULT combo_index;

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

static void lifeforms_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_LIFEFORMS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_LIFEFORMS_PRESET,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->presets[index].display_name
        );
    }

    SendDlgItemMessageA(dialog, IDC_LIFEFORMS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        SendDlgItemMessageA(
            dialog,
            IDC_LIFEFORMS_THEME,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->themes[index].display_name
        );
    }

    lifeforms_populate_combo(dialog, IDC_LIFEFORMS_RULE, g_lifeforms_rule_items, 2U);
    lifeforms_populate_combo(dialog, IDC_LIFEFORMS_DENSITY, g_lifeforms_density_items, 3U);
    lifeforms_populate_combo(dialog, IDC_LIFEFORMS_SEED, g_lifeforms_seed_items, 3U);
    lifeforms_populate_combo(dialog, IDC_LIFEFORMS_RESEED, g_lifeforms_reseed_items, 3U);
}

static void lifeforms_select_combo_value(HWND dialog, int control_id, LPARAM value)
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

static int lifeforms_get_combo_value(HWND dialog, int control_id, int fallback_value)
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

static void lifeforms_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const lifeforms_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_LIFEFORMS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_LIFEFORMS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    lifeforms_select_combo_value(dialog, IDC_LIFEFORMS_RULE, (LPARAM)product_config->rule_mode);
    lifeforms_select_combo_value(dialog, IDC_LIFEFORMS_DENSITY, (LPARAM)product_config->density_mode);
    lifeforms_select_combo_value(dialog, IDC_LIFEFORMS_SEED, (LPARAM)product_config->seed_mode);
    lifeforms_select_combo_value(dialog, IDC_LIFEFORMS_RESEED, (LPARAM)product_config->reseed_mode);
    CheckDlgButton(dialog, IDC_LIFEFORMS_DETERMINISTIC, common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(dialog, IDC_LIFEFORMS_DIAGNOSTICS, common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void lifeforms_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    lifeforms_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_LIFEFORMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_LIFEFORMS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.rule_mode = LIFEFORMS_RULE_CONWAY;
    product_config.density_mode = LIFEFORMS_DENSITY_STANDARD;
    product_config.seed_mode = LIFEFORMS_SEED_BALANCED;
    product_config.reseed_mode = LIFEFORMS_RESEED_STANDARD;
    lifeforms_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    lifeforms_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(dialog, IDC_LIFEFORMS_DIAGNOSTICS, diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED);
}

static void lifeforms_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    lifeforms_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    lifeforms_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_LIFEFORMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        lifeforms_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_LIFEFORMS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->rule_mode = lifeforms_get_combo_value(dialog, IDC_LIFEFORMS_RULE, product_config->rule_mode);
    product_config->density_mode = lifeforms_get_combo_value(dialog, IDC_LIFEFORMS_DENSITY, product_config->density_mode);
    product_config->seed_mode = lifeforms_get_combo_value(dialog, IDC_LIFEFORMS_SEED, product_config->seed_mode);
    product_config->reseed_mode = lifeforms_get_combo_value(dialog, IDC_LIFEFORMS_RESEED, product_config->reseed_mode);
    common_config->use_deterministic_seed = IsDlgButtonChecked(dialog, IDC_LIFEFORMS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled = IsDlgButtonChecked(dialog, IDC_LIFEFORMS_DIAGNOSTICS) == BST_CHECKED;
}

static void lifeforms_initialize_dialog(HWND dialog, lifeforms_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    lifeforms_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Lifeforms\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nCurated cellular automata with calmer preview pacing, staged reseeds, and clearer garden-versus-laboratory studies.");
    SetDlgItemTextA(dialog, IDC_LIFEFORMS_INFO, info);

    lifeforms_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK lifeforms_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    lifeforms_dialog_state *dialog_state;

    dialog_state = (lifeforms_dialog_state *)GetWindowLongPtrA(dialog, DWLP_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (lifeforms_dialog_state *)lParam;
        SetWindowLongPtrA(dialog, DWLP_USER, (LONG_PTR)dialog_state);
        if (dialog_state != NULL) {
            lifeforms_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_LIFEFORMS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            lifeforms_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_LIFEFORMS_DEFAULTS) {
            lifeforms_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            lifeforms_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            lifeforms_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            lifeforms_config_clamp(
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

INT_PTR lifeforms_config_show_dialog(
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
    lifeforms_dialog_state dialog_state;
    lifeforms_config *config;

    config = lifeforms_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        lifeforms_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6805UL,
            "The Lifeforms dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_LIFEFORMS_CONFIG),
        owner_window,
        lifeforms_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        lifeforms_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6806UL,
            "The Lifeforms config dialog resource could not be loaded."
        );
    }

    return result;
}

static int lifeforms_parse_rule_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "conway") == 0) {
        *value_out = LIFEFORMS_RULE_CONWAY;
        return 1;
    }
    if (lstrcmpiA(text, "highlife") == 0) {
        *value_out = LIFEFORMS_RULE_HIGHLIFE;
        return 1;
    }
    return 0;
}

static int lifeforms_parse_density_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "sparse") == 0) {
        *value_out = LIFEFORMS_DENSITY_SPARSE;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = LIFEFORMS_DENSITY_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "dense") == 0) {
        *value_out = LIFEFORMS_DENSITY_DENSE;
        return 1;
    }
    return 0;
}

static int lifeforms_parse_seed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "sparse") == 0) {
        *value_out = LIFEFORMS_SEED_SPARSE;
        return 1;
    }
    if (lstrcmpiA(text, "balanced") == 0) {
        *value_out = LIFEFORMS_SEED_BALANCED;
        return 1;
    }
    if (lstrcmpiA(text, "bloom") == 0) {
        *value_out = LIFEFORMS_SEED_BLOOM;
        return 1;
    }
    return 0;
}

static int lifeforms_parse_reseed_mode(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL) {
        return 0;
    }
    if (lstrcmpiA(text, "patient") == 0) {
        *value_out = LIFEFORMS_RESEED_PATIENT;
        return 1;
    }
    if (lstrcmpiA(text, "standard") == 0) {
        *value_out = LIFEFORMS_RESEED_STANDARD;
        return 1;
    }
    if (lstrcmpiA(text, "alert") == 0) {
        *value_out = LIFEFORMS_RESEED_ALERT;
        return 1;
    }
    return 0;
}

int lifeforms_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const lifeforms_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = lifeforms_as_const_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(writer->context, "product", "rule", lifeforms_rule_mode_name(config->rule_mode)) &&
        writer->write_string(writer->context, "product", "density", lifeforms_density_mode_name(config->density_mode)) &&
        writer->write_string(writer->context, "product", "seed", lifeforms_seed_mode_name(config->seed_mode)) &&
        writer->write_string(writer->context, "product", "reseed", lifeforms_reseed_mode_name(config->reseed_mode));
}

int lifeforms_config_import_settings_entry(
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
    lifeforms_config *config;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = lifeforms_as_config(product_config, product_config_size);
    if (kind != SCREENSAVE_SETTINGS_FILE_PRESET) {
        return 1;
    }
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (lstrcmpiA(section, "product") != 0) {
        return 1;
    }
    if (lstrcmpiA(key, "rule") == 0 || lstrcmpiA(key, "rule_mode") == 0) {
        return lifeforms_parse_rule_mode(value, &config->rule_mode);
    }
    if (lstrcmpiA(key, "density") == 0 || lstrcmpiA(key, "density_mode") == 0) {
        return lifeforms_parse_density_mode(value, &config->density_mode);
    }
    if (lstrcmpiA(key, "seed") == 0 || lstrcmpiA(key, "seed_mode") == 0) {
        return lifeforms_parse_seed_mode(value, &config->seed_mode);
    }
    if (lstrcmpiA(key, "reseed") == 0 || lstrcmpiA(key, "reseed_mode") == 0) {
        return lifeforms_parse_reseed_mode(value, &config->reseed_mode);
    }

    return 1;
}

void lifeforms_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    lifeforms_config *config;
    lifeforms_rng_state rng;
    unsigned long random_seed;
    unsigned long roll;

    (void)module;
    (void)common_config;
    (void)diagnostics;

    config = lifeforms_as_config(product_config, product_config_size);
    if (config == NULL) {
        return;
    }

    random_seed = seed != NULL ? seed->stream_seed : 0x4C465742UL;
    lifeforms_rng_seed(&rng, random_seed ^ 0x4C465742UL);

    roll = lifeforms_rng_range(&rng, 100UL);
    config->rule_mode = roll < 74UL ? LIFEFORMS_RULE_CONWAY : LIFEFORMS_RULE_HIGHLIFE;

    roll = lifeforms_rng_range(&rng, 100UL);
    if (roll < 28UL) {
        config->density_mode = LIFEFORMS_DENSITY_SPARSE;
    } else if (roll < 72UL) {
        config->density_mode = LIFEFORMS_DENSITY_STANDARD;
    } else {
        config->density_mode = LIFEFORMS_DENSITY_DENSE;
    }

    roll = lifeforms_rng_range(&rng, 100UL);
    if (roll < 24UL) {
        config->seed_mode = LIFEFORMS_SEED_SPARSE;
    } else if (roll < 66UL) {
        config->seed_mode = LIFEFORMS_SEED_BALANCED;
    } else {
        config->seed_mode = LIFEFORMS_SEED_BLOOM;
    }

    roll = lifeforms_rng_range(&rng, 100UL);
    if (roll < 32UL) {
        config->reseed_mode = LIFEFORMS_RESEED_PATIENT;
    } else if (roll < 80UL) {
        config->reseed_mode = LIFEFORMS_RESEED_STANDARD;
    } else {
        config->reseed_mode = LIFEFORMS_RESEED_ALERT;
    }
}
