#include <string.h>

#include "template_saver_internal.h"
#include "template_saver_resource.h"

typedef struct template_saver_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    template_saver_config *product_config;
} template_saver_dialog_state;

typedef struct template_saver_combo_item_tag {
    int value;
    const char *display_name;
} template_saver_combo_item;

static const template_saver_combo_item g_template_saver_motion_items[] = {
    { TEMPLATE_SAVER_MOTION_DRIFT, "Drift" },
    { TEMPLATE_SAVER_MOTION_FOCUS, "Focus" },
    { TEMPLATE_SAVER_MOTION_CALM, "Calm" }
};

static const template_saver_combo_item g_template_saver_accent_items[] = {
    { TEMPLATE_SAVER_ACCENT_FRAME, "Frame" },
    { TEMPLATE_SAVER_ACCENT_PULSE, "Pulse" },
    { TEMPLATE_SAVER_ACCENT_BANDS, "Bands" }
};

static const template_saver_combo_item g_template_saver_spacing_items[] = {
    { TEMPLATE_SAVER_SPACING_TIGHT, "Tight" },
    { TEMPLATE_SAVER_SPACING_BALANCED, "Balanced" },
    { TEMPLATE_SAVER_SPACING_WIDE, "Wide" }
};

static const template_saver_combo_item g_template_saver_detail_items[] = {
    { SCREENSAVE_DETAIL_LEVEL_LOW, "Low" },
    { SCREENSAVE_DETAIL_LEVEL_STANDARD, "Standard" },
    { SCREENSAVE_DETAIL_LEVEL_HIGH, "High" }
};

static template_saver_config *template_saver_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(template_saver_config)) {
        return NULL;
    }

    return (template_saver_config *)product_config;
}

static const template_saver_config *template_saver_as_const_config(
    const void *product_config,
    unsigned int product_config_size
)
{
    if (product_config == NULL || product_config_size != sizeof(template_saver_config)) {
        return NULL;
    }

    return (const template_saver_config *)product_config;
}

static int template_saver_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(TEMPLATE_SAVER_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, TEMPLATE_SAVER_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int template_saver_read_flag(HKEY key, const char *value_name, int *value)
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

static int template_saver_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int template_saver_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG template_saver_write_flag(HKEY key, const char *value_name, int value)
{
    DWORD data;

    data = value ? 1UL : 0UL;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG template_saver_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG template_saver_write_string(HKEY key, const char *value_name, const char *value)
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

static void template_saver_fill_named_combo(
    HWND dialog,
    int control_id,
    const template_saver_combo_item *items,
    unsigned int item_count,
    int selected_value
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < item_count; ++index) {
        SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)items[index].display_name);
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, (LPARAM)items[index].value);
        if (items[index].value == selected_value) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
        }
    }
}

static void template_saver_fill_preset_combo(HWND dialog, const char *selected_key)
{
    const screensave_preset_descriptor *presets;
    unsigned int preset_count;
    unsigned int index;

    presets = template_saver_get_presets(&preset_count);
    SendDlgItemMessageA(dialog, IDC_TEMPLATE_SAVER_PRESET, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < preset_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_TEMPLATE_SAVER_PRESET, CB_ADDSTRING, 0U, (LPARAM)presets[index].display_name);
        SendDlgItemMessageA(
            dialog,
            IDC_TEMPLATE_SAVER_PRESET,
            CB_SETITEMDATA,
            (WPARAM)index,
            (LPARAM)presets[index].preset_key
        );
        if (selected_key != NULL && strcmp(selected_key, presets[index].preset_key) == 0) {
            SendDlgItemMessageA(dialog, IDC_TEMPLATE_SAVER_PRESET, CB_SETCURSEL, (WPARAM)index, 0L);
        }
    }
}

static void template_saver_fill_theme_combo(HWND dialog, const char *selected_key)
{
    const screensave_theme_descriptor *themes;
    unsigned int theme_count;
    unsigned int index;

    themes = template_saver_get_themes(&theme_count);
    SendDlgItemMessageA(dialog, IDC_TEMPLATE_SAVER_THEME, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < theme_count; ++index) {
        SendDlgItemMessageA(dialog, IDC_TEMPLATE_SAVER_THEME, CB_ADDSTRING, 0U, (LPARAM)themes[index].display_name);
        SendDlgItemMessageA(
            dialog,
            IDC_TEMPLATE_SAVER_THEME,
            CB_SETITEMDATA,
            (WPARAM)index,
            (LPARAM)themes[index].theme_key
        );
        if (selected_key != NULL && strcmp(selected_key, themes[index].theme_key) == 0) {
            SendDlgItemMessageA(dialog, IDC_TEMPLATE_SAVER_THEME, CB_SETCURSEL, (WPARAM)index, 0L);
        }
    }
}

static const char *template_saver_selected_key(HWND dialog, int control_id)
{
    LRESULT selection;
    LRESULT item_data;

    selection = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return NULL;
    }

    item_data = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)selection, 0L);
    if (item_data == CB_ERR) {
        return NULL;
    }

    return (const char *)item_data;
}

static int template_saver_selected_value(HWND dialog, int control_id, int fallback_value)
{
    LRESULT selection;
    LRESULT item_data;

    selection = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return fallback_value;
    }

    item_data = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)selection, 0L);
    if (item_data == CB_ERR) {
        return fallback_value;
    }

    return (int)item_data;
}

static void template_saver_sync_dialog(HWND dialog, template_saver_dialog_state *state)
{
    char info_text[256];

    if (state == NULL || state->module == NULL || state->common_config == NULL || state->product_config == NULL) {
        return;
    }

    lstrcpynA(
        info_text,
        "Starter saver template showing the real Settings dialog pattern for product-local config.",
        (int)sizeof(info_text)
    );
    SetDlgItemTextA(dialog, IDC_TEMPLATE_SAVER_INFO, info_text);

    template_saver_fill_preset_combo(dialog, state->common_config->preset_key);
    template_saver_fill_theme_combo(dialog, state->common_config->theme_key);
    template_saver_fill_named_combo(
        dialog,
        IDC_TEMPLATE_SAVER_MOTION,
        g_template_saver_motion_items,
        sizeof(g_template_saver_motion_items) / sizeof(g_template_saver_motion_items[0]),
        state->product_config->motion_mode
    );
    template_saver_fill_named_combo(
        dialog,
        IDC_TEMPLATE_SAVER_ACCENT,
        g_template_saver_accent_items,
        sizeof(g_template_saver_accent_items) / sizeof(g_template_saver_accent_items[0]),
        state->product_config->accent_mode
    );
    template_saver_fill_named_combo(
        dialog,
        IDC_TEMPLATE_SAVER_SPACING,
        g_template_saver_spacing_items,
        sizeof(g_template_saver_spacing_items) / sizeof(g_template_saver_spacing_items[0]),
        state->product_config->spacing_mode
    );
    template_saver_fill_named_combo(
        dialog,
        IDC_TEMPLATE_SAVER_DETAIL,
        g_template_saver_detail_items,
        sizeof(g_template_saver_detail_items) / sizeof(g_template_saver_detail_items[0]),
        state->common_config->detail_level
    );
    CheckDlgButton(
        dialog,
        IDC_TEMPLATE_SAVER_DETERMINISTIC,
        state->common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_TEMPLATE_SAVER_DIAGNOSTICS,
        state->common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void template_saver_apply_dialog(HWND dialog, template_saver_dialog_state *state)
{
    const char *preset_key;
    const char *theme_key;

    if (state == NULL || state->common_config == NULL || state->product_config == NULL) {
        return;
    }

    preset_key = template_saver_selected_key(dialog, IDC_TEMPLATE_SAVER_PRESET);
    if (preset_key != NULL) {
        template_saver_apply_preset_to_config(preset_key, state->common_config, state->product_config);
    }

    theme_key = template_saver_selected_key(dialog, IDC_TEMPLATE_SAVER_THEME);
    if (theme_key != NULL && template_saver_find_theme_descriptor(theme_key) != NULL) {
        state->common_config->theme_key = template_saver_find_theme_descriptor(theme_key)->theme_key;
    }

    state->product_config->motion_mode = template_saver_selected_value(
        dialog,
        IDC_TEMPLATE_SAVER_MOTION,
        state->product_config->motion_mode
    );
    state->product_config->accent_mode = template_saver_selected_value(
        dialog,
        IDC_TEMPLATE_SAVER_ACCENT,
        state->product_config->accent_mode
    );
    state->product_config->spacing_mode = template_saver_selected_value(
        dialog,
        IDC_TEMPLATE_SAVER_SPACING,
        state->product_config->spacing_mode
    );
    state->common_config->detail_level = (screensave_detail_level)template_saver_selected_value(
        dialog,
        IDC_TEMPLATE_SAVER_DETAIL,
        state->common_config->detail_level
    );
    state->common_config->use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_TEMPLATE_SAVER_DETERMINISTIC) == BST_CHECKED;
    state->common_config->diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_TEMPLATE_SAVER_DIAGNOSTICS) == BST_CHECKED;

    template_saver_config_clamp(state->common_config, state->product_config, sizeof(*state->product_config));
}

static INT_PTR CALLBACK template_saver_config_dialog_proc(HWND dialog, UINT message, WPARAM w_param, LPARAM l_param)
{
    template_saver_dialog_state *state;

    state = (template_saver_dialog_state *)GetWindowLongPtrA(dialog, GWLP_USERDATA);
    switch (message) {
    case WM_INITDIALOG:
        state = (template_saver_dialog_state *)l_param;
        SetWindowLongPtrA(dialog, GWLP_USERDATA, (LONG_PTR)state);
        template_saver_sync_dialog(dialog, state);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(w_param)) {
        case IDC_TEMPLATE_SAVER_DEFAULTS:
            if (state != NULL) {
                template_saver_config_set_defaults(
                    state->common_config,
                    state->product_config,
                    sizeof(*state->product_config)
                );
                template_saver_sync_dialog(dialog, state);
            }
            return TRUE;

        case IDOK:
            if (state != NULL) {
                template_saver_apply_dialog(dialog, state);
            }
            EndDialog(dialog, IDOK);
            return TRUE;

        case IDCANCEL:
            EndDialog(dialog, IDCANCEL);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

void template_saver_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    template_saver_config *config;

    config = template_saver_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->motion_mode = TEMPLATE_SAVER_MOTION_DRIFT;
    config->accent_mode = TEMPLATE_SAVER_ACCENT_FRAME;
    config->spacing_mode = TEMPLATE_SAVER_SPACING_BALANCED;
    template_saver_apply_preset_to_config(TEMPLATE_SAVER_DEFAULT_PRESET_KEY, common_config, config);
}

void template_saver_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    template_saver_config *config;

    config = template_saver_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);

    if (template_saver_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = TEMPLATE_SAVER_DEFAULT_PRESET_KEY;
    }
    if (common_config->theme_key == NULL || template_saver_find_theme_descriptor(common_config->theme_key) == NULL) {
        common_config->theme_key = TEMPLATE_SAVER_DEFAULT_THEME_KEY;
    }

    if (config->motion_mode < TEMPLATE_SAVER_MOTION_DRIFT || config->motion_mode > TEMPLATE_SAVER_MOTION_CALM) {
        config->motion_mode = TEMPLATE_SAVER_MOTION_DRIFT;
    }
    if (config->accent_mode < TEMPLATE_SAVER_ACCENT_FRAME || config->accent_mode > TEMPLATE_SAVER_ACCENT_BANDS) {
        config->accent_mode = TEMPLATE_SAVER_ACCENT_FRAME;
    }
    if (config->spacing_mode < TEMPLATE_SAVER_SPACING_TIGHT || config->spacing_mode > TEMPLATE_SAVER_SPACING_WIDE) {
        config->spacing_mode = TEMPLATE_SAVER_SPACING_BALANCED;
    }
}

int template_saver_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    template_saver_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)module;
    (void)diagnostics;

    config = template_saver_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    template_saver_config_set_defaults(common_config, product_config, product_config_size);
    if (!template_saver_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        template_saver_config_clamp(common_config, product_config, product_config_size);
        return 1;
    }

    preset_key[0] = '\0';
    if (template_saver_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        template_saver_apply_preset_to_config(preset_key, common_config, config);
    }
    if (template_saver_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (template_saver_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = template_saver_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (template_saver_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }

    (void)template_saver_read_flag(key, "UseDeterministicSeed", &common_config->use_deterministic_seed);
    value_dword = common_config->deterministic_seed;
    if (template_saver_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    (void)template_saver_read_flag(key, "DiagnosticsOverlayEnabled", &common_config->diagnostics_overlay_enabled);

    value_dword = (unsigned long)common_config->randomization_mode;
    if (template_saver_read_dword(key, "RandomizationMode", &value_dword)) {
        common_config->randomization_mode = (screensave_randomization_mode)value_dword;
    }
    value_dword = common_config->randomization_scope;
    if (template_saver_read_dword(key, "RandomizationScope", &value_dword)) {
        common_config->randomization_scope = value_dword;
    }

    value_dword = (unsigned long)config->motion_mode;
    if (template_saver_read_dword(key, "MotionMode", &value_dword)) {
        config->motion_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->accent_mode;
    if (template_saver_read_dword(key, "AccentMode", &value_dword)) {
        config->accent_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->spacing_mode;
    if (template_saver_read_dword(key, "SpacingMode", &value_dword)) {
        config->spacing_mode = (int)value_dword;
    }

    RegCloseKey(key);
    template_saver_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int template_saver_config_save(
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
    template_saver_config safe_product_config;
    const template_saver_config *config;

    (void)module;
    (void)diagnostics;

    config = template_saver_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    template_saver_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!template_saver_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (
        RegCreateKeyExA(
            HKEY_CURRENT_USER,
            path,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_SET_VALUE,
            NULL,
            &key,
            &disposition
        ) != ERROR_SUCCESS
    ) {
        return 0;
    }

    (void)disposition;
    result = template_saver_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_flag(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_flag(
            key,
            "DiagnosticsOverlayEnabled",
            safe_common_config.diagnostics_overlay_enabled
        );
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(
            key,
            "RandomizationMode",
            (unsigned long)safe_common_config.randomization_mode
        );
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(key, "RandomizationScope", safe_common_config.randomization_scope);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(key, "MotionMode", (unsigned long)safe_product_config.motion_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(key, "AccentMode", (unsigned long)safe_product_config.accent_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = template_saver_write_dword(key, "SpacingMode", (unsigned long)safe_product_config.spacing_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

INT_PTR template_saver_config_show_dialog(
    HINSTANCE instance,
    HWND owner_window,
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    template_saver_dialog_state state;
    template_saver_config *config;

    (void)diagnostics;

    config = template_saver_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return IDCANCEL;
    }

    state.module = module;
    state.common_config = common_config;
    state.product_config = config;
    return DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_TEMPLATE_SAVER_CONFIG),
        owner_window,
        template_saver_config_dialog_proc,
        (LPARAM)&state
    );
}

int template_saver_config_export_settings_entries(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    const void *product_config,
    unsigned int product_config_size,
    screensave_settings_file_kind kind,
    screensave_settings_writer *writer,
    screensave_diag_context *diagnostics
)
{
    const template_saver_config *config;

    (void)module;
    (void)common_config;
    (void)kind;
    (void)diagnostics;

    config = template_saver_as_const_config(product_config, product_config_size);
    if (config == NULL || writer == NULL || writer->write_string == NULL) {
        return 0;
    }

    return writer->write_string(
        writer->context,
        "product",
        "motion_mode",
        template_saver_motion_mode_name(config->motion_mode)
    ) &&
        writer->write_string(
            writer->context,
            "product",
            "accent_mode",
            template_saver_accent_mode_name(config->accent_mode)
        ) &&
        writer->write_string(
            writer->context,
            "product",
            "spacing_mode",
            template_saver_spacing_mode_name(config->spacing_mode)
        );
}

int template_saver_config_import_settings_entry(
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
    template_saver_config *config;

    (void)module;
    (void)common_config;
    (void)kind;
    (void)diagnostics;

    config = template_saver_as_config(product_config, product_config_size);
    if (config == NULL || section == NULL || key == NULL || value == NULL) {
        return 0;
    }
    if (strcmp(section, "product") != 0) {
        return 1;
    }

    if (strcmp(key, "motion_mode") == 0) {
        config->motion_mode = template_saver_motion_mode_from_name(value, config->motion_mode);
        return 1;
    }
    if (strcmp(key, "accent_mode") == 0) {
        config->accent_mode = template_saver_accent_mode_from_name(value, config->accent_mode);
        return 1;
    }
    if (strcmp(key, "spacing_mode") == 0) {
        config->spacing_mode = template_saver_spacing_mode_from_name(value, config->spacing_mode);
        return 1;
    }

    return 1;
}

void template_saver_config_randomize_settings(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    const screensave_session_seed *seed,
    screensave_diag_context *diagnostics
)
{
    template_saver_config *config;
    unsigned long value;

    (void)module;
    (void)diagnostics;

    config = template_saver_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    value = seed != NULL ? seed->stream_seed : GetTickCount();
    if ((common_config->randomization_scope & SCREENSAVE_RANDOMIZATION_SCOPE_PRODUCT) != 0UL) {
        config->motion_mode = (int)(value % 3UL);
        config->accent_mode = (int)((value / 3UL) % 3UL);
        config->spacing_mode = (int)((value / 9UL) % 3UL);
    }

    template_saver_config_clamp(common_config, product_config, product_config_size);
}
