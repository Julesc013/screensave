#include <string.h>

#include "gallery_internal.h"
#include "gallery_resource.h"
#include "screensave/version.h"
#include "../../../../platform/src/core/base/saver_registry.h"

typedef struct gallery_dialog_state_tag {
    const screensave_saver_module *module;
    screensave_common_config *common_config;
    gallery_config *product_config;
} gallery_dialog_state;

typedef struct gallery_combo_item_tag {
    int value;
    const char *display_name;
} gallery_combo_item;

static const gallery_combo_item g_gallery_scene_items[] = {
    { GALLERY_SCENE_COMPATIBILITY, "Compatibility Gallery" },
    { GALLERY_SCENE_NEON, "Neon Abstract" },
    { GALLERY_SCENE_SHOWCASE, "Technical Exhibit" }
};

static const gallery_combo_item g_gallery_motion_items[] = {
    { GALLERY_MOTION_CALM, "Calm" },
    { GALLERY_MOTION_STANDARD, "Standard" },
    { GALLERY_MOTION_BRISK, "Brisk" }
};

static const gallery_combo_item g_gallery_detail_items[] = {
    { SCREENSAVE_DETAIL_LEVEL_LOW, "Low" },
    { SCREENSAVE_DETAIL_LEVEL_STANDARD, "Standard" },
    { SCREENSAVE_DETAIL_LEVEL_HIGH, "High" }
};

static gallery_config *gallery_as_config(void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(gallery_config)) {
        return NULL;
    }

    return (gallery_config *)product_config;
}

static const gallery_config *gallery_as_const_config(const void *product_config, unsigned int product_config_size)
{
    if (product_config == NULL || product_config_size != sizeof(gallery_config)) {
        return NULL;
    }

    return (const gallery_config *)product_config;
}

static int gallery_build_registry_path(char *buffer, int buffer_size)
{
    if (buffer == NULL || buffer_size <= 0) {
        return 0;
    }

    buffer[0] = '\0';
    return lstrlenA(GALLERY_PRODUCT_REGISTRY_ROOTA) + 1 <= buffer_size &&
        lstrcpyA(buffer, GALLERY_PRODUCT_REGISTRY_ROOTA) != NULL;
}

static int gallery_read_dword(HKEY key, const char *value_name, unsigned long *value)
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

static int gallery_read_string(HKEY key, const char *value_name, char *buffer, DWORD buffer_size)
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

static LONG gallery_write_dword(HKEY key, const char *value_name, unsigned long value)
{
    DWORD data;

    data = (DWORD)value;
    return RegSetValueExA(key, value_name, 0, REG_DWORD, (const BYTE *)&data, sizeof(data));
}

static LONG gallery_write_string(HKEY key, const char *value_name, const char *value)
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

static void gallery_fill_preset_combo(
    HWND dialog,
    int control_id,
    const screensave_preset_descriptor *presets,
    unsigned int preset_count,
    const char *selected_key
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < preset_count; ++index) {
        SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)presets[index].display_name);
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, (LPARAM)presets[index].preset_key);
        if (selected_key != NULL && presets[index].preset_key != NULL && strcmp(presets[index].preset_key, selected_key) == 0) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
        }
    }
}

static void gallery_fill_theme_combo(
    HWND dialog,
    int control_id,
    const screensave_theme_descriptor *themes,
    unsigned int theme_count,
    const char *selected_key
)
{
    unsigned int index;

    SendDlgItemMessageA(dialog, control_id, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < theme_count; ++index) {
        SendDlgItemMessageA(dialog, control_id, CB_ADDSTRING, 0U, (LPARAM)themes[index].display_name);
        SendDlgItemMessageA(dialog, control_id, CB_SETITEMDATA, (WPARAM)index, (LPARAM)themes[index].theme_key);
        if (selected_key != NULL && themes[index].theme_key != NULL && strcmp(themes[index].theme_key, selected_key) == 0) {
            SendDlgItemMessageA(dialog, control_id, CB_SETCURSEL, (WPARAM)index, 0L);
        }
    }
}

static const char *gallery_selected_preset_key(HWND dialog, int control_id)
{
    LRESULT selection;
    LRESULT item_data;

    selection = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return NULL;
    }

    item_data = SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)selection, 0L);
    return (const char *)item_data;
}

static const char *gallery_selected_theme_key(HWND dialog, int control_id)
{
    return gallery_selected_preset_key(dialog, control_id);
}

static void gallery_fill_value_combo(
    HWND dialog,
    int control_id,
    const gallery_combo_item *items,
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

static int gallery_selected_value(HWND dialog, int control_id, const gallery_combo_item *items, unsigned int item_count)
{
    LRESULT selection;
    int index;

    selection = SendDlgItemMessageA(dialog, control_id, CB_GETCURSEL, 0U, 0L);
    if (selection == CB_ERR) {
        return items[0].value;
    }

    index = (int)selection;
    if (index < 0 || (unsigned int)index >= item_count) {
        return items[0].value;
    }

    return (int)SendDlgItemMessageA(dialog, control_id, CB_GETITEMDATA, (WPARAM)index, 0L);
}

static void gallery_update_info(HWND dialog, const screensave_saver_module *module)
{
    char info[256];
    char version_text[160];

    if (module == NULL) {
        return;
    }

    lstrcpyA(version_text, screensave_version_get_text());
    info[0] = '\0';
    lstrcpyA(info, module->identity.display_name);
    lstrcatA(info, " Settings\r\n");
    lstrcatA(info, version_text);
    lstrcatA(info, "\r\n");
    lstrcatA(info, module->identity.description);
    SetDlgItemTextA(dialog, IDC_GALLERY_INFO, info);
}

void gallery_config_set_defaults(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    gallery_config *config;

    config = gallery_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_set_defaults(common_config);
    config->scene_mode = GALLERY_SCENE_COMPATIBILITY;
    config->motion_mode = GALLERY_MOTION_STANDARD;
    gallery_apply_preset_to_config(GALLERY_DEFAULT_PRESET_KEY, common_config, config);
}

void gallery_config_clamp(
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size
)
{
    gallery_config *config;
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;

    config = gallery_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return;
    }

    screensave_common_config_clamp(common_config);
    if (common_config->preset_key != NULL && gallery_find_preset_values(common_config->preset_key) == NULL) {
        common_config->preset_key = GALLERY_DEFAULT_PRESET_KEY;
    }

    if (common_config->theme_key == NULL || gallery_find_theme_descriptor(common_config->theme_key) == NULL) {
        presets = gallery_get_presets(&preset_count);
        preset_descriptor = screensave_find_preset(presets, preset_count, common_config->preset_key);
        common_config->theme_key = preset_descriptor != NULL ? preset_descriptor->theme_key : GALLERY_DEFAULT_THEME_KEY;
    }

    if (config->scene_mode < GALLERY_SCENE_COMPATIBILITY || config->scene_mode > GALLERY_SCENE_SHOWCASE) {
        config->scene_mode = GALLERY_SCENE_COMPATIBILITY;
    }
    if (config->motion_mode < GALLERY_MOTION_CALM || config->motion_mode > GALLERY_MOTION_BRISK) {
        config->motion_mode = GALLERY_MOTION_STANDARD;
    }
}

int gallery_config_load(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size,
    screensave_diag_context *diagnostics
)
{
    char path[260];
    HKEY key;
    gallery_config *config;
    unsigned long value_dword;
    char preset_key[64];
    char theme_key[64];

    (void)diagnostics;

    config = gallery_as_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    gallery_config_set_defaults(common_config, product_config, product_config_size);
    if (!gallery_build_registry_path(path, sizeof(path))) {
        return 0;
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        const char *legacy_product_key;

        legacy_product_key = screensave_saver_registry_legacy_product_key(
            module != NULL && module->identity.product_key != NULL ? module->identity.product_key : "gallery"
        );
        if (
            legacy_product_key == NULL ||
            !screensave_saver_registry_build_registry_root(legacy_product_key, path, (unsigned int)sizeof(path)) ||
            RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS
        ) {
            gallery_config_clamp(common_config, product_config, product_config_size);
            return 1;
        }
    }

    preset_key[0] = '\0';
    if (gallery_read_string(key, "PresetKey", preset_key, sizeof(preset_key))) {
        gallery_apply_preset_to_config(preset_key, common_config, config);
    }

    theme_key[0] = '\0';
    if (gallery_read_string(key, "ThemeKey", theme_key, sizeof(theme_key))) {
        if (gallery_find_theme_descriptor(theme_key) != NULL) {
            common_config->theme_key = gallery_find_theme_descriptor(theme_key)->theme_key;
        }
    }

    value_dword = (unsigned long)common_config->detail_level;
    if (gallery_read_dword(key, "DetailLevel", &value_dword)) {
        common_config->detail_level = (screensave_detail_level)value_dword;
    }
    value_dword = (unsigned long)config->scene_mode;
    if (gallery_read_dword(key, "SceneMode", &value_dword)) {
        config->scene_mode = (int)value_dword;
    }
    value_dword = (unsigned long)config->motion_mode;
    if (gallery_read_dword(key, "MotionMode", &value_dword)) {
        config->motion_mode = (int)value_dword;
    }
    value_dword = common_config->use_deterministic_seed;
    if (gallery_read_dword(key, "UseDeterministicSeed", &value_dword)) {
        common_config->use_deterministic_seed = value_dword != 0UL;
    }
    value_dword = common_config->deterministic_seed;
    if (gallery_read_dword(key, "DeterministicSeed", &value_dword)) {
        common_config->deterministic_seed = value_dword;
    }
    value_dword = common_config->diagnostics_overlay_enabled;
    if (gallery_read_dword(key, "DiagnosticsOverlayEnabled", &value_dword)) {
        common_config->diagnostics_overlay_enabled = value_dword != 0UL;
    }

    RegCloseKey(key);
    gallery_config_clamp(common_config, product_config, product_config_size);
    return 1;
}

int gallery_config_save(
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
    gallery_config safe_product_config;
    const gallery_config *config;

    (void)module;
    (void)diagnostics;

    config = gallery_as_const_config(product_config, product_config_size);
    if (common_config == NULL || config == NULL) {
        return 0;
    }

    safe_common_config = *common_config;
    safe_product_config = *config;
    gallery_config_clamp(&safe_common_config, &safe_product_config, sizeof(safe_product_config));

    if (!gallery_build_registry_path(path, sizeof(path))) {
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

    result = gallery_write_string(key, "PresetKey", safe_common_config.preset_key);
    if (result == ERROR_SUCCESS) {
        result = gallery_write_string(key, "ThemeKey", safe_common_config.theme_key);
    }
    if (result == ERROR_SUCCESS) {
        result = gallery_write_dword(key, "DetailLevel", (unsigned long)safe_common_config.detail_level);
    }
    if (result == ERROR_SUCCESS) {
        result = gallery_write_dword(key, "UseDeterministicSeed", safe_common_config.use_deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = gallery_write_dword(key, "DeterministicSeed", safe_common_config.deterministic_seed);
    }
    if (result == ERROR_SUCCESS) {
        result = gallery_write_dword(key, "DiagnosticsOverlayEnabled", safe_common_config.diagnostics_overlay_enabled);
    }
    if (result == ERROR_SUCCESS) {
        result = gallery_write_dword(key, "SceneMode", (unsigned long)safe_product_config.scene_mode);
    }
    if (result == ERROR_SUCCESS) {
        result = gallery_write_dword(key, "MotionMode", (unsigned long)safe_product_config.motion_mode);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

static INT_PTR gallery_handle_command(HWND dialog, WORD command_id, gallery_dialog_state *dialog_state)
{
    const char *preset_key;

    if (dialog_state == NULL || dialog_state->common_config == NULL || dialog_state->product_config == NULL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    if (command_id == IDC_GALLERY_DEFAULTS) {
        gallery_config_set_defaults(
            dialog_state->common_config,
            dialog_state->product_config,
            sizeof(*dialog_state->product_config)
        );
        gallery_fill_preset_combo(
            dialog,
            IDC_GALLERY_PRESET,
            g_gallery_presets,
            GALLERY_PRESET_COUNT,
            dialog_state->common_config->preset_key
        );
        gallery_fill_theme_combo(
            dialog,
            IDC_GALLERY_THEME,
            g_gallery_themes,
            GALLERY_THEME_COUNT,
            dialog_state->common_config->theme_key
        );
        gallery_fill_value_combo(
            dialog,
            IDC_GALLERY_SCENE,
            g_gallery_scene_items,
            (unsigned int)(sizeof(g_gallery_scene_items) / sizeof(g_gallery_scene_items[0])),
            dialog_state->product_config->scene_mode
        );
        gallery_fill_value_combo(
            dialog,
            IDC_GALLERY_MOTION,
            g_gallery_motion_items,
            (unsigned int)(sizeof(g_gallery_motion_items) / sizeof(g_gallery_motion_items[0])),
            dialog_state->product_config->motion_mode
        );
        gallery_fill_value_combo(
            dialog,
            IDC_GALLERY_DETAIL,
            g_gallery_detail_items,
            (unsigned int)(sizeof(g_gallery_detail_items) / sizeof(g_gallery_detail_items[0])),
            (int)dialog_state->common_config->detail_level
        );
        CheckDlgButton(
            dialog,
            IDC_GALLERY_DETERMINISTIC,
            dialog_state->common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
        );
        CheckDlgButton(
            dialog,
            IDC_GALLERY_DIAGNOSTICS,
            dialog_state->common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
        );
        return TRUE;
    }

    if (command_id == IDOK) {
        preset_key = gallery_selected_preset_key(dialog, IDC_GALLERY_PRESET);
        if (preset_key != NULL) {
            gallery_apply_preset_to_config(preset_key, dialog_state->common_config, dialog_state->product_config);
        }

        dialog_state->common_config->detail_level = (screensave_detail_level)gallery_selected_value(
            dialog,
            IDC_GALLERY_DETAIL,
            g_gallery_detail_items,
            (unsigned int)(sizeof(g_gallery_detail_items) / sizeof(g_gallery_detail_items[0]))
        );
        dialog_state->common_config->use_deterministic_seed =
            IsDlgButtonChecked(dialog, IDC_GALLERY_DETERMINISTIC) == BST_CHECKED;
        dialog_state->common_config->diagnostics_overlay_enabled =
            IsDlgButtonChecked(dialog, IDC_GALLERY_DIAGNOSTICS) == BST_CHECKED;
        preset_key = gallery_selected_theme_key(dialog, IDC_GALLERY_THEME);
        if (preset_key != NULL && gallery_find_theme_descriptor(preset_key) != NULL) {
            dialog_state->common_config->theme_key = preset_key;
        }
        dialog_state->product_config->scene_mode = gallery_selected_value(
            dialog,
            IDC_GALLERY_SCENE,
            g_gallery_scene_items,
            (unsigned int)(sizeof(g_gallery_scene_items) / sizeof(g_gallery_scene_items[0]))
        );
        dialog_state->product_config->motion_mode = gallery_selected_value(
            dialog,
            IDC_GALLERY_MOTION,
            g_gallery_motion_items,
            (unsigned int)(sizeof(g_gallery_motion_items) / sizeof(g_gallery_motion_items[0]))
        );
        EndDialog(dialog, IDOK);
        return TRUE;
    }

    if (command_id == IDCANCEL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

static INT_PTR CALLBACK gallery_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    gallery_dialog_state *dialog_state;

    dialog_state = (gallery_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (gallery_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            gallery_update_info(dialog, dialog_state->module);
            gallery_fill_preset_combo(
                dialog,
                IDC_GALLERY_PRESET,
                g_gallery_presets,
                GALLERY_PRESET_COUNT,
                dialog_state->common_config->preset_key
            );
            gallery_fill_theme_combo(
                dialog,
                IDC_GALLERY_THEME,
                g_gallery_themes,
                GALLERY_THEME_COUNT,
                dialog_state->common_config->theme_key
            );
            gallery_fill_value_combo(
                dialog,
                IDC_GALLERY_SCENE,
                g_gallery_scene_items,
                (unsigned int)(sizeof(g_gallery_scene_items) / sizeof(g_gallery_scene_items[0])),
                dialog_state->product_config->scene_mode
            );
            gallery_fill_value_combo(
                dialog,
                IDC_GALLERY_MOTION,
                g_gallery_motion_items,
                (unsigned int)(sizeof(g_gallery_motion_items) / sizeof(g_gallery_motion_items[0])),
                dialog_state->product_config->motion_mode
            );
            gallery_fill_value_combo(
                dialog,
                IDC_GALLERY_DETAIL,
                g_gallery_detail_items,
                (unsigned int)(sizeof(g_gallery_detail_items) / sizeof(g_gallery_detail_items[0])),
                (int)dialog_state->common_config->detail_level
            );
            CheckDlgButton(
                dialog,
                IDC_GALLERY_DETERMINISTIC,
                dialog_state->common_config->use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
            );
            CheckDlgButton(
                dialog,
                IDC_GALLERY_DIAGNOSTICS,
                dialog_state->common_config->diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
            );
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state != NULL) {
            return gallery_handle_command(dialog, LOWORD(wParam), dialog_state);
        }
        break;
    }

    return FALSE;
}

INT_PTR gallery_config_show_dialog(
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
    gallery_dialog_state dialog_state;
    gallery_config *config;

    (void)diagnostics;

    config = gallery_as_config(product_config, product_config_size);
    if (module == NULL || common_config == NULL || config == NULL) {
        return -1;
    }

    dialog_state.module = module;
    dialog_state.common_config = common_config;
    dialog_state.product_config = config;
    result = DialogBoxParamA(
        instance,
        MAKEINTRESOURCEA(IDD_GALLERY_CONFIG),
        owner_window,
        gallery_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        MessageBoxA(
            owner_window,
            "The Gallery configuration dialog resource could not be loaded.",
            module->identity.display_name,
            MB_OK | MB_ICONERROR
        );
    }

    if (result == IDOK) {
        gallery_config_clamp(common_config, product_config, product_config_size);
    }

    return result;
}
