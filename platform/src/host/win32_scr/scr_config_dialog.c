#include <string.h>

#include "scr_internal.h"
#include "resource.h"

typedef struct scr_fallback_dialog_state_tag {
    scr_host_context *context;
    const screensave_saver_module *module;
    scr_settings *settings;
} scr_fallback_dialog_state;

typedef struct scr_selector_dialog_state_tag {
    scr_host_context *context;
    const screensave_saver_module *selected_module;
} scr_selector_dialog_state;

static const screensave_saver_config_hooks *scr_get_config_hooks(const screensave_saver_module *module)
{
    if (module == NULL) {
        return NULL;
    }

    return module->config_hooks;
}

static void scr_apply_fallback_settings_to_dialog(HWND dialog, const scr_settings *settings)
{
    CheckDlgButton(
        dialog,
        IDC_SCR_DETERMINISTIC_SEED,
        settings->common.use_deterministic_seed ? BST_CHECKED : BST_UNCHECKED
    );
    CheckDlgButton(
        dialog,
        IDC_SCR_DIAGNOSTICS,
        settings->common.diagnostics_overlay_enabled ? BST_CHECKED : BST_UNCHECKED
    );
}

static void scr_read_fallback_settings_from_dialog(HWND dialog, scr_settings *settings)
{
    settings->common.use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_SCR_DETERMINISTIC_SEED) == BST_CHECKED;
    settings->common.diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_SCR_DIAGNOSTICS) == BST_CHECKED;
}

static void scr_initialize_fallback_dialog(
    HWND dialog,
    scr_host_context *context,
    const screensave_saver_module *module,
    const scr_settings *settings
)
{
    char title[96];
    char info[256];

    title[0] = '\0';
    scr_append_text(title, sizeof(title), module->identity.display_name);
    scr_append_text(title, sizeof(title), " Settings");
    SetWindowTextA(dialog, title);

    scr_build_version_text(context, info, sizeof(info));
    SetDlgItemTextA(dialog, IDC_SCR_INFO, info);
    scr_apply_fallback_settings_to_dialog(dialog, settings);
}

static INT_PTR scr_handle_fallback_command(
    HWND dialog,
    WORD command_id,
    const screensave_saver_module *module,
    scr_settings *settings
)
{
    if (module == NULL || settings == NULL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    if (command_id == IDC_SCR_RESET_DEFAULTS) {
        scr_settings_set_defaults(module, settings);
        scr_apply_fallback_settings_to_dialog(dialog, settings);
        return TRUE;
    }

    if (command_id == IDOK) {
        scr_read_fallback_settings_from_dialog(dialog, settings);
        EndDialog(dialog, IDOK);
        return TRUE;
    }

    if (command_id == IDCANCEL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

static INT_PTR CALLBACK scr_fallback_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    scr_fallback_dialog_state *dialog_state;

    dialog_state = (scr_fallback_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (scr_fallback_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            scr_initialize_fallback_dialog(
                dialog,
                dialog_state->context,
                dialog_state->module,
                dialog_state->settings
            );
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state != NULL) {
            return scr_handle_fallback_command(
                dialog,
                LOWORD(wParam),
                dialog_state->module,
                dialog_state->settings
            );
        }
        break;
    }

    return FALSE;
}

static INT_PTR scr_show_fallback_dialog(
    scr_host_context *context,
    const screensave_saver_module *module,
    scr_settings *settings,
    HWND owner_window
)
{
    INT_PTR result;
    scr_fallback_dialog_state dialog_state;

    dialog_state.context = context;
    dialog_state.module = module;
    dialog_state.settings = settings;

    result = DialogBoxParamA(
        context->instance,
        MAKEINTRESOURCEA(IDD_SCR_CONFIG),
        owner_window,
        scr_fallback_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        scr_show_message_box(
            context->owner_window,
            module,
            "The configuration dialog resource could not be loaded.",
            MB_OK | MB_ICONERROR
        );
    }

    return result;
}

static int scr_run_module_settings_dialog(
    scr_host_context *context,
    const screensave_saver_module *module,
    HWND owner_window
)
{
    INT_PTR result;
    scr_settings dialog_settings;
    const screensave_saver_config_hooks *config_hooks;

    if (context == NULL || module == NULL) {
        return -1;
    }

    if (!scr_settings_init(module, &dialog_settings)) {
        scr_show_message_box(
            context->owner_window,
            module,
            "The saver configuration state could not be initialized for editing.",
            MB_OK | MB_ICONERROR
        );
        return -1;
    }

    scr_settings_set_defaults(module, &dialog_settings);
    if (!scr_settings_load(module, &dialog_settings, &context->diagnostics)) {
        scr_emit_host_diagnostic(
            context,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            2201UL,
            "scr_run_module_settings_dialog",
            "The saver settings could not be loaded; defaults will be shown for editing."
        );
    }
    scr_settings_clamp(module, &dialog_settings);

    config_hooks = scr_get_config_hooks(module);
    if (config_hooks != NULL && config_hooks->show_config_dialog != NULL) {
        result = config_hooks->show_config_dialog(
            context->instance,
            owner_window,
            module,
            &dialog_settings.common,
            dialog_settings.product_config,
            dialog_settings.product_config_size,
            &context->diagnostics
        );
    } else {
        result = scr_show_fallback_dialog(context, module, &dialog_settings, owner_window);
    }

    if (result == IDOK) {
        scr_settings_clamp(module, &dialog_settings);
        if (!scr_settings_save(module, &dialog_settings, &context->diagnostics)) {
            scr_emit_host_diagnostic(
                context,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                2202UL,
                "scr_run_module_settings_dialog",
                "Failed to save the selected saver settings."
            );
            scr_show_message_box(
                context->owner_window,
                module,
                "Failed to save the selected saver settings.",
                MB_OK | MB_ICONERROR
            );
            result = -1;
        }
    } else if (result == -1) {
        scr_show_message_box(
            context->owner_window,
            module,
            "The saver configuration dialog could not be loaded.",
            MB_OK | MB_ICONERROR
        );
    }

    scr_settings_dispose(&dialog_settings);
    return (int)result;
}

static void scr_selector_update_info(HWND dialog, const scr_selector_dialog_state *dialog_state)
{
    char info[256];
    char version_text[160];

    if (dialog_state == NULL || dialog_state->context == NULL || dialog_state->selected_module == NULL) {
        return;
    }

    scr_build_version_text(dialog_state->context, version_text, sizeof(version_text));

    info[0] = '\0';
    scr_append_text(info, sizeof(info), "Current built-in saver\r\n");
    scr_append_text(info, sizeof(info), dialog_state->selected_module->identity.display_name);
    scr_append_text(info, sizeof(info), "\r\n");
    scr_append_text(info, sizeof(info), version_text);
    scr_append_text(info, sizeof(info), "\r\n");
    scr_append_text(info, sizeof(info), dialog_state->selected_module->identity.description);
    SetDlgItemTextA(dialog, IDC_SCR_PRODUCT_INFO, info);
}

static void scr_selector_populate_combo(HWND dialog, const scr_selector_dialog_state *dialog_state)
{
    unsigned int index;
    const screensave_saver_module *module;

    if (dialog_state == NULL || dialog_state->context == NULL) {
        return;
    }

    SendDlgItemMessageA(dialog, IDC_SCR_PRODUCT, CB_RESETCONTENT, 0U, 0L);
    for (index = 0U; index < dialog_state->context->available_module_count; ++index) {
        module = dialog_state->context->available_modules[index];
        if (module == NULL) {
            continue;
        }

        SendDlgItemMessageA(
            dialog,
            IDC_SCR_PRODUCT,
            CB_ADDSTRING,
            0U,
            (LPARAM)module->identity.display_name
        );
    }
}

static void scr_selector_select_current(HWND dialog, const scr_selector_dialog_state *dialog_state)
{
    unsigned int index;

    if (
        dialog_state == NULL ||
        dialog_state->context == NULL ||
        dialog_state->selected_module == NULL
    ) {
        return;
    }

    for (index = 0U; index < dialog_state->context->available_module_count; ++index) {
        if (dialog_state->context->available_modules[index] == dialog_state->selected_module) {
            SendDlgItemMessageA(dialog, IDC_SCR_PRODUCT, CB_SETCURSEL, (WPARAM)index, 0L);
            return;
        }
    }
}

static void scr_selector_initialize_dialog(HWND dialog, scr_selector_dialog_state *dialog_state)
{
    scr_selector_populate_combo(dialog, dialog_state);
    scr_selector_select_current(dialog, dialog_state);
    scr_selector_update_info(dialog, dialog_state);
}

static void scr_selector_handle_product_change(HWND dialog, scr_selector_dialog_state *dialog_state)
{
    LRESULT selection;

    if (dialog_state == NULL || dialog_state->context == NULL) {
        return;
    }

    selection = SendDlgItemMessageA(dialog, IDC_SCR_PRODUCT, CB_GETCURSEL, 0U, 0L);
    if (
        selection == CB_ERR ||
        (unsigned int)selection >= dialog_state->context->available_module_count
    ) {
        return;
    }

    dialog_state->selected_module = dialog_state->context->available_modules[(unsigned int)selection];
    scr_selector_update_info(dialog, dialog_state);
}

static INT_PTR CALLBACK scr_selector_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    scr_selector_dialog_state *dialog_state;

    dialog_state = (scr_selector_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (scr_selector_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        if (dialog_state != NULL) {
            scr_selector_initialize_dialog(dialog, dialog_state);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state == NULL) {
            return FALSE;
        }

        if (LOWORD(wParam) == IDC_SCR_PRODUCT && HIWORD(wParam) == CBN_SELCHANGE) {
            scr_selector_handle_product_change(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDC_SCR_PRODUCT_SETTINGS) {
            int result;

            result = scr_run_module_settings_dialog(dialog_state->context, dialog_state->selected_module, dialog);
            if (result == -1) {
                return TRUE;
            }

            scr_selector_update_info(dialog, dialog_state);
            return TRUE;
        }

        if (LOWORD(wParam) == IDOK) {
            if (
                dialog_state->selected_module == NULL ||
                !scr_save_selected_product_key(dialog_state->selected_module->identity.product_key)
            ) {
                scr_show_message_box(
                    dialog_state->context->owner_window,
                    dialog_state->selected_module != NULL
                        ? dialog_state->selected_module
                        : dialog_state->context->default_module,
                    "The host could not save the current built-in saver selection.",
                    MB_OK | MB_ICONERROR
                );
                return TRUE;
            }

            dialog_state->context->module = dialog_state->selected_module;
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

static INT_PTR scr_show_selector_dialog(scr_host_context *context)
{
    INT_PTR result;
    scr_selector_dialog_state dialog_state;

    if (context == NULL) {
        return -1;
    }

    dialog_state.context = context;
    dialog_state.selected_module = context->module;

    result = DialogBoxParamA(
        context->instance,
        MAKEINTRESOURCEA(IDD_SCR_PRODUCT_SELECT),
        context->owner_window,
        scr_selector_dialog_proc,
        (LPARAM)&dialog_state
    );

    if (result == -1) {
        scr_show_message_box(
            context->owner_window,
            context->module,
            "The host product-selection dialog could not be loaded.",
            MB_OK | MB_ICONERROR
        );
    }

    return result;
}

INT_PTR scr_show_config_dialog(scr_host_context *context)
{
    if (context == NULL) {
        return -1;
    }

    if (context->available_modules != NULL && context->available_module_count > 1U) {
        return scr_show_selector_dialog(context);
    }

    return scr_run_module_settings_dialog(context, context->module, context->owner_window);
}
