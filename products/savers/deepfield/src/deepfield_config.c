#include "deepfield_internal.h"
#include "deepfield_resource.h"
#include "screensave/version.h"

typedef struct deepfield_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    deepfield_config *product_config;
} deepfield_dialog_state;

typedef struct deepfield_combo_item_tag {
    int value;
    const char *display_name;
} deepfield_combo_item;

static const deepfield_combo_item g_deepfield_scene_items[] = {
    { DEEPFIELD_SCENE_PARALLAX, "Parallax" },
    { DEEPFIELD_SCENE_FLYTHROUGH, "Fly-Through" }
};

static const deepfield_combo_item g_deepfield_density_items[] = {
    { DEEPFIELD_DENSITY_SPARSE, "Sparse" },
    { DEEPFIELD_DENSITY_STANDARD, "Standard" },
    { DEEPFIELD_DENSITY_RICH, "Rich" }
};

static const deepfield_combo_item g_deepfield_speed_items[] = {
    { DEEPFIELD_SPEED_CALM, "Calm" },
    { DEEPFIELD_SPEED_CRUISE, "Cruise" },
    { DEEPFIELD_SPEED_SURGE, "Surge" }
};

static const deepfield_combo_item g_deepfield_camera_items[] = {
    { DEEPFIELD_CAMERA_OBSERVE, "Observe" },
    { DEEPFIELD_CAMERA_DRIFT, "Drift" },
    { DEEPFIELD_CAMERA_ARC, "Arc" }
};

static const deepfield_combo_item g_deepfield_pulse_items[] = {
    { DEEPFIELD_PULSE_NONE, "None" },
    { DEEPFIELD_PULSE_SOFT, "Soft" },
    { DEEPFIELD_PULSE_WARP, "Warp" }
};

static void deepfield_emit_config_diag(
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
        "deepfield_config",
        text
    );
}

static deepfield_config *deepfield_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(deepfield_config)) {
        return NULL;
    }

    return (deepfield_config *)product_config;
}

static const deepfield_config *deepfield_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(deepfield_config)) {
        return NULL;
    }

    return (const deepfield_config *)product_config;
}

static int deepfield_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(DEEPFIELD_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, DEEPFIELD_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int deepfield_read_flag(HKEY key, const char *value_name, int *value)
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

static int deepfield_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int deepfield_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG deepfield_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG deepfield_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG deepfield_write_string(HKEY key, const char *value_name, const char *value)
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

void deepfield_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    deepfield_config *config;

    config = deepfield_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->scene_mode = DEEPFIELD_SCENE_PARALLAX;
    config->density_mode = DEEPFIELD_DENSITY_STANDARD;
    config->speed_mode = DEEPFIELD_SPEED_CALM;
    config->camera_mode = DEEPFIELD_CAMERA_OBSERVE;
    config->pulse_mode = DEEPFIELD_PULSE_NONE;
    deepfield_apply_preset_to_config(DEEPFIELD_DEFAULT_PRESET_KEY, common_config, config);
}

void deepfield_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    deepfield_config *config;

    config = deepfield_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (
        common_config->preset_key != NULL &&
        deepfield_find_preset_values(common_config->preset_key) == NULL
    ) {
        common_config->preset_key = DEEPFIELD_DEFAULT_PRESET_KEY;
    }

    if (
        common_config->theme_key == NULL ||
        deepfield_find_theme_descriptor(common_config->theme_key) == NULL
    ) {
        if (common_config->preset_key != NULL) {
            unsigned int preset_count;
            const screensave_preset_descriptor *presets;
            const screensave_preset_descriptor *preset_descriptor;

            presets = deepfield_get_presets(&preset_count);
            preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
            common_config->theme_key = preset_descriptor != NULL
                ? preset_descriptor->theme_key
                : DEEPFIELD_DEFAULT_THEME_KEY;
        } else {
            common_config->theme_key = DEEPFIELD_DEFAULT_THEME_KEY;
        }
    }

    if (config->scene_mode < DEEPFIELD_SCENE_PARALLAX || config->scene_mode > DEEPFIELD_SCENE_FLYTHROUGH) {
        config->scene_mode = DEEPFIELD_SCENE_PARALLAX;
    }
    if (config->density_mode < DEEPFIELD_DENSITY_SPARSE || config->density_mode > DEEPFIELD_DENSITY_RICH) {
        config->density_mode = DEEPFIELD_DENSITY_STANDARD;
    }
    if (config->speed_mode < DEEPFIELD_SPEED_CALM || config->speed_mode > DEEPFIELD_SPEED_SURGE) {
        config->speed_mode = DEEPFIELD_SPEED_CALM;
    }
    if (config->camera_mode < DEEPFIELD_CAMERA_OBSERVE || config->camera_mode > DEEPFIELD_CAMERA_ARC) {
        config->camera_mode = DEEPFIELD_CAMERA_OBSERVE;
    }
    if (config->pulse_mode < DEEPFIELD_PULSE_NONE || config->pulse_mode > DEEPFIELD_PULSE_WARP) {
        config->pulse_mode = DEEPFIELD_PULSE_NONE;
    }
}

int deepfield_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    deepfield_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = deepfield_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    deepfield_config_set_defaults(common_config, product_config, product_config_size);
    if (!deepfield_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        deepfield_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (deepfield_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        deepfield_apply_preset_to_config(preset_key, common_config, config);
    }

    if (deepfield_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (deepfield_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = deepfield_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (deepfield_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    value_dword = common_config->deterministic_seed;
    (void)deepfield_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    if (deepfield_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)deepfield_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)config->scene_mode;
    if (deepfield_read_dword(key, "SceneMode", &value_dword)) {
        config->scene_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->density_mode;
    if (deepfield_read_dword(key, "DensityMode", &value_dword)) {
        config->density_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->speed_mode;
    if (deepfield_read_dword(key, "SpeedMode", &value_dword)) {
        config->speed_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->camera_mode;
    if (deepfield_read_dword(key, "CameraMode", &value_dword)) {
        config->camera_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->pulse_mode;
    if (deepfield_read_dword(key, "PulseMode", &value_dword)) {
        config->pulse_mode = (int)value_dword;
    }

    RegCloseKey(key);
    deepfield_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int deepfield_config_save(
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
    deepfield_config safe_product_config;
    screensave_common_config safe_common_config;
    const deepfield_config *config;

    (void)module;

    config = deepfield_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    deepfield_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));
    if (!deepfield_build_registry_path(path, sizeof(path))) {
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
        deepfield_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6401UL,
            "The Deepfield registry key could not be opened for writing."
        );
        return 0;
    }

    result = deepfield_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_flag(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_string(key, "PresetKey", safe_common_config.preset_key);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_dword(key, "SceneMode", (unsigned long)safe_product_config.scene_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_dword(key, "DensityMode", (unsigned long)safe_product_config.density_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_dword(key, "SpeedMode", (unsigned long)safe_product_config.speed_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_dword(key, "CameraMode", (unsigned long)safe_product_config.camera_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = deepfield_write_dword(key, "PulseMode", (unsigned long)safe_product_config.pulse_mode);
    }

    RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        deepfield_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6402UL,
            "The Deepfield registry values could not be saved."
        );
        return 0;
    }

    return 1;
}

static LRESULT deepfield_add_combo_item(HWND dialog, int control_id, const char *text, LPARAM item_data)
{
    LRESULT index;

    index = SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)text);
    if (index >= 0L) {
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, item_data);
    }

    return index;
}

static void deepfield_select_combo_value(HWND dialog, int control_id, LPARAM item_data)
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

static int deepfield_get_combo_value(HWND dialog, int control_id, int default_value)
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

static void deepfield_populate_dialog_lists(HWND dialog, const screensave_saver_module *module)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->preset_count; ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_PRESET,
            module->presets[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < module->theme_count; ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_THEME,
            module->themes[index].display_name,
            (LPARAM)index
        );
    }

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_SCENE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_deepfield_scene_items) / sizeof(g_deepfield_scene_items[0])); ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_SCENE,
            g_deepfield_scene_items[index].display_name,
            (LPARAM)g_deepfield_scene_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_DENSITY, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_deepfield_density_items) / sizeof(g_deepfield_density_items[0])); ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_DENSITY,
            g_deepfield_density_items[index].display_name,
            (LPARAM)g_deepfield_density_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_SPEED, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_deepfield_speed_items) / sizeof(g_deepfield_speed_items[0])); ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_SPEED,
            g_deepfield_speed_items[index].display_name,
            (LPARAM)g_deepfield_speed_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_CAMERA, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_deepfield_camera_items) / sizeof(g_deepfield_camera_items[0])); ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_CAMERA,
            g_deepfield_camera_items[index].display_name,
            (LPARAM)g_deepfield_camera_items[index].value
        );
    }

    SendDlgItemMessageA(dialog, IDC_DEEPFIELD_PULSE, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < (unsigned int)(sizeof(g_deepfield_pulse_items) / sizeof(g_deepfield_pulse_items[0])); ++index) {
        deepfield_add_combo_item(
            dialog,
            IDC_DEEPFIELD_PULSE,
            g_deepfield_pulse_items[index].display_name,
            (LPARAM)g_deepfield_pulse_items[index].value
        );
    }
}

static void deepfield_apply_settings_to_dialog(
    HWND dialog,
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const deepfield_config *product_config
)
{
    unsigned int index;

    if (common_config->preset_key != NULL) {
        for (index = 0U; index < module->preset_count; ++index) {
            if (lstrcmpiA(module->presets[index].preset_key, common_config->preset_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_DEEPFIELD_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    if (common_config->theme_key != NULL) {
        for (index = 0U; index < module->theme_count; ++index) {
            if (lstrcmpiA(module->themes[index].theme_key, common_config->theme_key) == 0) {
                SendDlgItemMessageA(dialog, IDC_DEEPFIELD_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
                break;
            }
        }
    }

    deepfield_select_combo_value(dialog, IDC_DEEPFIELD_SCENE, (LPARAM)product_config->scene_mode);
    deepfield_select_combo_value(dialog, IDC_DEEPFIELD_DENSITY, (LPARAM)product_config->density_mode);
    deepfield_select_combo_value(dialog, IDC_DEEPFIELD_SPEED, (LPARAM)product_config->speed_mode);
    deepfield_select_combo_value(dialog, IDC_DEEPFIELD_CAMERA, (LPARAM)product_config->camera_mode);
    deepfield_select_combo_value(dialog, IDC_DEEPFIELD_PULSE, (LPARAM)product_config->pulse_mode);

    CheckDlgButton(
        dialog,
        IDC_DEEPFIELD_DETERMINISTIC,
        common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_DEEPFIELD_DIAGNOSTICS,
        common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void deepfield_apply_preset_selection(HWND dialog, const screensave_saver_module *module)
{
    LRESULT preset_index;
    deepfield_config product_config;
    screensave_common_config common_config;
    int diagnostics_enabled;

    preset_index = SendDlgItemMessageA(dialog, IDC_DEEPFIELD_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index == CB_ERR || (unsigned int)preset_index >= module->preset_count) {
        return;
    }

    diagnostics_enabled = IsDlgButtonChecked(dialog, IDC_DEEPFIELD_DIAGNOSTICS) == BST_CHECKED;
    screensave_common_config_set_defaults(&common_config);
    product_config.scene_mode = DEEPFIELD_SCENE_PARALLAX;
    product_config.density_mode = DEEPFIELD_DENSITY_STANDARD;
    product_config.speed_mode = DEEPFIELD_SPEED_CALM;
    product_config.camera_mode = DEEPFIELD_CAMERA_OBSERVE;
    product_config.pulse_mode = DEEPFIELD_PULSE_NONE;

    deepfield_apply_preset_to_config(module->presets[preset_index].preset_key, &common_config, &product_config);
    deepfield_apply_settings_to_dialog(dialog, module, &common_config, &product_config);
    CheckDlgButton(
        dialog,
        IDC_DEEPFIELD_DIAGNOSTICS,
        diagnostics_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void deepfield_read_dialog_settings(
    HWND dialog,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    deepfield_config *product_config
)
{
    LRESULT preset_index;
    LRESULT theme_index;

    deepfield_config_set_defaults(common_config, product_config, sizeof(*product_config));

    preset_index = SendDlgItemMessageA(dialog, IDC_DEEPFIELD_PRESET, CB_GETCURSEL, 0U, 0L);
    if (preset_index != CB_ERR && (unsigned int)preset_index < module->preset_count) {
        deepfield_apply_preset_to_config(module->presets[preset_index].preset_key, common_config, product_config);
    } else {
        common_config->preset_key = NULL;
    }

    theme_index = SendDlgItemMessageA(dialog, IDC_DEEPFIELD_THEME, CB_GETCURSEL, 0U, 0L);
    if (theme_index != CB_ERR && (unsigned int)theme_index < module->theme_count) {
        common_config->theme_key = module->themes[theme_index].theme_key;
    }

    product_config->scene_mode = deepfield_get_combo_value(dialog, IDC_DEEPFIELD_SCENE, product_config->scene_mode);
    product_config->density_mode = deepfield_get_combo_value(dialog, IDC_DEEPFIELD_DENSITY, product_config->density_mode);
    product_config->speed_mode = deepfield_get_combo_value(dialog, IDC_DEEPFIELD_SPEED, product_config->speed_mode);
    product_config->camera_mode = deepfield_get_combo_value(dialog, IDC_DEEPFIELD_CAMERA, product_config->camera_mode);
    product_config->pulse_mode = deepfield_get_combo_value(dialog, IDC_DEEPFIELD_PULSE, product_config->pulse_mode);
    common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_DEEPFIELD_DETERMINISTIC) == BST_CHECKED;
    common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_DEEPFIELD_DIAGNOSTICS) == BST_CHECKED;
}

static void deepfield_initialize_dialog(HWND dialog, deepfield_dialog_state *dialog_state)
{
    char info[256];
    const screensave_version_info *version_info;

    version_info = screensave_version_get_info();
    deepfield_populate_dialog_lists(dialog, dialog_state->module);

    info[0] = '\0';
    lstrcpyA(info, "Deepfield\r\n");
    lstrcatA(info, version_info->version_text);
    lstrcatA(info, "\r\nAtmospheric drift and fly-through motion with restrained depth cues.");
    SetDlgItemTextA(dialog, IDC_DEEPFIELD_INFO, info);

    deepfield_apply_settings_to_dialog(
        dialog,
        dialog_state->module,
        dialog_state->common_config,
        dialog_state->product_config
    );
}

static INT_PTR CALLBACK deepfield_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    deepfield_dialog_state *dialog_state;

    dialog_state = (deepfield_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (deepfield_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            deepfield_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_DEEPFIELD_PRESET && HIWORD(wParam) == CBN_SELCHANGE) {
            deepfield_apply_preset_selection(dialog, dialog_state->module);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_DEEPFIELD_DEFAULTS) {
            deepfield_config_set_defaults(
                dialog_state->common_config,
                dialog_state->product_config,
                sizeof(*dialog_state->product_config)
            );
            deepfield_apply_settings_to_dialog(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            deepfield_read_dialog_settings(
                dialog,
                dialog_state->module,
                dialog_state->common_config,
                dialog_state->product_config
            );
            deepfield_config_clamp(
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

INT_PTR deepfield_config_show_dialog(
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
    deepfield_dialog_state dialog_state;
    deepfield_config *config;

    config = deepfield_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        deepfield_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6403UL,
            "The Deepfield dialog could not start because its config state was invalid."
        );
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;

    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_DEEPFIELD_CONFIG),
        owner_window,
        deepfield_config_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        deepfield_emit_config_diag(
            diagnostics,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6404UL,
            "The Deepfield config dialog resource could not be loaded."
        );
    }

    return result;
}
