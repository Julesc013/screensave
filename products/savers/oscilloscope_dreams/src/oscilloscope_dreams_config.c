#include "oscilloscope_dreams_internal.h"
#include "oscilloscope_dreams_resource.h"
#include "screensave/version.h"

typedef struct oscilloscope_dreams_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    oscilloscope_dreams_config *product_config;
} oscilloscope_dreams_dialog_state;

typedef struct oscilloscope_dreams_combo_item_tag {
    int value;
    const char *display_name;
} oscilloscope_dreams_combo_item;

static const oscilloscope_dreams_combo_item g_oscilloscope_dreams_curve_items[] = {
    { OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS, "Lissajous" },
    { OSCILLOSCOPE_DREAMS_CURVE_HARMONOGRAPH, "Harmonograph" },
    { OSCILLOSCOPE_DREAMS_CURVE_DENSE, "Dense Trail" }
};

static const oscilloscope_dreams_combo_item g_oscilloscope_dreams_persistence_items[] = {
    { OSCILLOSCOPE_DREAMS_PERSISTENCE_SHORT, "Short" },
    { OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD, "Standard" },
    { OSCILLOSCOPE_DREAMS_PERSISTENCE_LONG, "Long" }
};

static const oscilloscope_dreams_combo_item g_oscilloscope_dreams_drift_items[] = {
    { OSCILLOSCOPE_DREAMS_DRIFT_CALM, "Calm" },
    { OSCILLOSCOPE_DREAMS_DRIFT_STANDARD, "Standard" },
    { OSCILLOSCOPE_DREAMS_DRIFT_WIDE, "Wide" }
};

static const oscilloscope_dreams_combo_item g_oscilloscope_dreams_mirror_items[] = {
    { OSCILLOSCOPE_DREAMS_MIRROR_NONE, "None" },
    { OSCILLOSCOPE_DREAMS_MIRROR_HORIZONTAL, "Horizontal" },
    { OSCILLOSCOPE_DREAMS_MIRROR_QUAD, "Quad" }
};

static void oscilloscope_dreams_emit_config_diag(
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
        "oscilloscope_dreams_config",
        text
    );
}

static oscilloscope_dreams_config *oscilloscope_dreams_as_config(
    void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(oscilloscope_dreams_config)) {
        return NULL;
    }

    return (oscilloscope_dreams_config *)product_config;
}

static const oscilloscope_dreams_config *oscilloscope_dreams_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(oscilloscope_dreams_config)) {
        return NULL;
    }

    return (const oscilloscope_dreams_config *)product_config;
}

static int oscilloscope_dreams_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(OSCILLOSCOPE_DREAMS_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, OSCILLOSCOPE_DREAMS_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int oscilloscope_dreams_read_flag(HKEY key, const char *value_name, int *value)
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

static int oscilloscope_dreams_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int oscilloscope_dreams_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG oscilloscope_dreams_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG oscilloscope_dreams_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG oscilloscope_dreams_write_string(HKEY key, const char *value_name, const char *value)
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

void oscilloscope_dreams_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    oscilloscope_dreams_config *config;

    config = oscilloscope_dreams_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->curve_mode = OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS;
    config->persistence_mode = OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD;
    config->drift_mode = OSCILLOSCOPE_DREAMS_DRIFT_STANDARD;
    config->mirror_mode = OSCILLOSCOPE_DREAMS_MIRROR_NONE;
    oscilloscope_dreams_apply_preset_to_config(OSCILLOSCOPE_DREAMS_DEFAULT_PRESET_KEY, common_config, config);
}

void oscilloscope_dreams_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    oscilloscope_dreams_config *config;

    config = oscilloscope_dreams_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        oscilloscope_dreams_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = OSCILLOSCOPE_DREAMS_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        oscilloscope_dreams_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = oscilloscope_dreams_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : OSCILLOSCOPE_DREAMS_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = OSCILLOSCOPE_DREAMS_DEFAULT_THEME_KEY;
        }
    }

    if (
        config->curve_mode < OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS ||
        config->curve_mode > OSCILLOSCOPE_DREAMS_CURVE_DENSE
    ) {
        config->curve_mode = OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS;
    }
    if (
        config->persistence_mode < OSCILLOSCOPE_DREAMS_PERSISTENCE_SHORT ||
        config->persistence_mode > OSCILLOSCOPE_DREAMS_PERSISTENCE_LONG
    ) {
        config->persistence_mode = OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD;
    }
    if (
        config->drift_mode < OSCILLOSCOPE_DREAMS_DRIFT_CALM ||
        config->drift_mode > OSCILLOSCOPE_DREAMS_DRIFT_WIDE
    ) {
        config->drift_mode = OSCILLOSCOPE_DREAMS_DRIFT_STANDARD;
    }
    if (
        config->mirror_mode < OSCILLOSCOPE_DREAMS_MIRROR_NONE ||
        config->mirror_mode > OSCILLOSCOPE_DREAMS_MIRROR_QUAD
    ) {
        config->mirror_mode = OSCILLOSCOPE_DREAMS_MIRROR_NONE;
    }
}

int oscilloscope_dreams_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    oscilloscope_dreams_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = oscilloscope_dreams_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    oscilloscope_dreams_config_set_defaults(common_config, product_config, product_config_size);
    if (!oscilloscope_dreams_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        oscilloscope_dreams_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (oscilloscope_dreams_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        oscilloscope_dreams_apply_preset_to_config(preset_key, common_config, config);
    }

    if (oscilloscope_dreams_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (oscilloscope_dreams_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = oscilloscope_dreams_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (oscilloscope_dreams_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)oscilloscope_dreams_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (oscilloscope_dreams_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)oscilloscope_dreams_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->curve_mode;
    if (oscilloscope_dreams_read_dword(key, "CurveMode", &value_dword)) {
        config->curve_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->persistence_mode;
    if (oscilloscope_dreams_read_dword(key, "PersistenceMode", &value_dword)) {
        config->persistence_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->drift_mode;
    if (oscilloscope_dreams_read_dword(key, "DriftMode", &value_dword)) {
        config->drift_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->mirror_mode;
    if (oscilloscope_dreams_read_dword(key, "MirrorMode", &value_dword)) {
        config->mirror_mode = (int)value_dword;
    }

    RegCloseKey(key);
    oscilloscope_dreams_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int oscilloscope_dreams_config_save(
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
    oscilloscope_dreams_config safe_product_config;
    screensave_common_config safe_common_config;
    const oscilloscope_dreams_config *config;

    (void)module;

    config = oscilloscope_dreams_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    oscilloscope_dreams_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!oscilloscope_dreams_build_registry_path(path, sizeof(path))) {
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
        oscilloscope_dreams_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6601UL,
            "The Oscilloscope Dreams registry key could not be opened for writing."
        );
        return 0;
    }

    result = oscilloscope_dreams_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_dword(key, "CurveMode", (unsigned long)safe_product_config.curve_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_dword(key, "PersistenceMode", (unsigned long)safe_product_config.persistence_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_dword(key, "DriftMode", (unsigned long)safe_product_config.drift_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = oscilloscope_dreams_write_dword(key, "MirrorMode", (unsigned long)safe_product_config.mirror_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        oscilloscope_dreams_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6602UL,
            "The Oscilloscope Dreams registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT oscilloscope_dreams_add_combo_item(
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

static void oscilloscope_dreams_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static int oscilloscope_dreams_get_combo_value(HWND dialog, int control_id, int default_value)
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

static void oscilloscope_dreams_populate_dialog_lists(
    HWND dialog,
    const screensave_saver_module *module
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        oscilloscope_dreams_add_combo_item(
            dialog,
            IDC_OSCILLOSCOPE_DREAMS_PRESET,
            module->presets[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        oscilloscope_dreams_add_combo_item(
            dialog,
            IDC_OSCILLOSCOPE_DREAMS_THEME,
            module->themes[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_CURVE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_oscilloscope_dreams_curve_items) / sizeof(g_oscilloscope_dreams_curve_items[0])); ++index) {
        oscilloscope_dreams_add_combo_item(
            dialog,
            IDC_OSCILLOSCOPE_DREAMS_CURVE,
            g_oscilloscope_dreams_curve_items[index].display_name,
            (LPARAM)g_oscilloscope_dreams_curve_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_PERSISTENCE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_oscilloscope_dreams_persistence_items) / sizeof(g_oscilloscope_dreams_persistence_items[0])); ++index) {
        oscilloscope_dreams_add_combo_item(
            dialog,
            IDC_OSCILLOSCOPE_DREAMS_PERSISTENCE,
            g_oscilloscope_dreams_persistence_items[index].display_name,
            (LPARAM)g_oscilloscope_dreams_persistence_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_DRIFT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_oscilloscope_dreams_drift_items) / sizeof(g_oscilloscope_dreams_drift_items[0])); ++index) {
        oscilloscope_dreams_add_combo_item(
            dialog,
            IDC_OSCILLOSCOPE_DREAMS_DRIFT,
            g_oscilloscope_dreams_drift_items[index].display_name,
            (LPARAM)g_oscilloscope_dreams_drift_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_MIRROR, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_oscilloscope_dreams_mirror_items) / sizeof(g_oscilloscope_dreams_mirror_items[0])); ++index) {
        oscilloscope_dreams_add_combo_item(
            dialog,
            IDC_OSCILLOSCOPE_DREAMS_MIRROR,
            g_oscilloscope_dreams_mirror_items[index].display_name,
            (LPARAM)g_oscilloscope_dreams_mirror_items[index].value
        );
    }
}

static void oscilloscope_dreams_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const oscilloscope_dreams_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    oscilloscope_dreams_select_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_CURVE,
        (LPARAM)product_config->curve_mode
    );
    oscilloscope_dreams_select_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_PERSISTENCE,
        (LPARAM)product_config->persistence_mode
    );
    oscilloscope_dreams_select_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_DRIFT,
        (LPARAM)product_config->drift_mode
    );
    oscilloscope_dreams_select_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_MIRROR,
        (LPARAM)product_config->mirror_mode
    );

    CheckDlgButton(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void oscilloscope_dreams_apply_preset_selection(
    HWND dialog,
    const screensave_saver_module *module
)
{
    LRESULT preset_index;
    oscilloscope_dreams_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_OSCILLOSCOPE_DREAMS_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.curve_mode = OSCILLOSCOPE_DREAMS_CURVE_LISSAJOUS;
    product_config.persistence_mode = OSCILLOSCOPE_DREAMS_PERSISTENCE_STANDARD;
    product_config.drift_mode = OSCILLOSCOPE_DREAMS_DRIFT_STANDARD;
    product_config.mirror_mode = OSCILLOSCOPE_DREAMS_MIRROR_NONE;
    oscilloscope_dreams_apply_preset_to_config(
        module->presets[preset_index].preset_key,
        &common_config,
        &product_config
    );
    oscilloscope_dreams_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_DIAGNOSTICS,
        diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void oscilloscope_dreams_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    oscilloscope_dreams_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    oscilloscope_dreams_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        oscilloscope_dreams_apply_preset_to_config(
            module->presets[preset_index].preset_key,
            common_config,
            product_config
        );
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_OSCILLOSCOPE_DREAMS_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->curve_mode = oscilloscope_dreams_get_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_CURVE,
        product_config->curve_mode
    );
    product_config->persistence_mode = oscilloscope_dreams_get_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_PERSISTENCE,
        product_config->persistence_mode
    );
    product_config->drift_mode = oscilloscope_dreams_get_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_DRIFT,
        product_config->drift_mode
    );
    product_config->mirror_mode = oscilloscope_dreams_get_combo_value(
        dialog,
        IDC_OSCILLOSCOPE_DREAMS_MIRROR,
        product_config->mirror_mode
    );
    common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_OSCILLOSCOPE_DREAMS_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_OSCILLOSCOPE_DREAMS_DIAGNOSTICS) == BST_CHECKED;
}

static void oscilloscope_dreams_initialize_dialog(
    HWND dialog,
    oscilloscope_dreams_dialog_state *dialog_state
)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    oscilloscope_dreams_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Oscilloscope Dreams\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nVector-style phosphor curves with restrained mirror and persistence controls.");
    SetDlgItemTextA(dialog, IDC_OSCILLOSCOPE_DREAMS_INFO, info);

    oscilloscope_dreams_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK oscilloscope_dreams_config_dialog_proc(
    HWND dialog,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    oscilloscope_dreams_dialog_state *dialog_state;

    dialog_state = (oscilloscope_dreams_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (oscilloscope_dreams_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            oscilloscope_dreams_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_OSCILLOSCOPE_DREAMS_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            oscilloscope_dreams_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_OSCILLOSCOPE_DREAMS_DEFAULTS) {
            oscilloscope_dreams_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            oscilloscope_dreams_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            oscilloscope_dreams_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            oscilloscope_dreams_config_clamp(
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

INT_PTR oscilloscope_dreams_config_show_dialog(
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
    oscilloscope_dreams_dialog_state dialog_state;
    oscilloscope_dreams_config *config;

    config = oscilloscope_dreams_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        oscilloscope_dreams_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6603UL,
            "The Oscilloscope Dreams dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_OSCILLOSCOPE_DREAMS_CONFIG),
        owner_window,
        oscilloscope_dreams_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        oscilloscope_dreams_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6604UL,
            "The Oscilloscope Dreams config dialog resource could not be loaded."
        );
    }

    return result;
}
