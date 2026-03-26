#include "scr_internal.h"
#include "resource.h"

static scr_host_context *scr_get_dialog_context(HWND dialog)
{
    return (scr_host_context *)GetWindowLongA(dialog, DWL_USER);
}

static void scr_apply_settings_to_dialog(HWND dialog, const scr_settings *settings)
{
    CheckDlgButton(
        dialog,
        IDC_SCR_PLACEHOLDER_VISUAL,
        settings->placeholder_visual_enabled ? BST_CHECKED : BST_UNCHECKED
    );
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

static void scr_read_settings_from_dialog(HWND dialog, scr_settings *settings)
{
    settings->placeholder_visual_enabled =
        IsDlgButtonChecked(dialog, IDC_SCR_PLACEHOLDER_VISUAL) == BST_CHECKED;
    settings->common.use_deterministic_seed =
        IsDlgButtonChecked(dialog, IDC_SCR_DETERMINISTIC_SEED) == BST_CHECKED;
    settings->common.diagnostics_overlay_enabled =
        IsDlgButtonChecked(dialog, IDC_SCR_DIAGNOSTICS) == BST_CHECKED;
}

static void scr_initialize_dialog(HWND dialog, scr_host_context *context)
{
    char title[96];
    char info[256];

    title[0] = '\0';
    scr_append_text(title, sizeof(title), context->module->identity.display_name);
    scr_append_text(title, sizeof(title), " Settings");
    SetWindowTextA(dialog, title);

    scr_build_version_text(context, info, sizeof(info));
    SetDlgItemTextA(dialog, IDC_SCR_INFO, info);
    scr_apply_settings_to_dialog(dialog, &context->settings);
}

static INT_PTR scr_handle_command(HWND dialog, WORD command_id)
{
    scr_host_context *context;
    scr_settings settings;

    context = scr_get_dialog_context(dialog);
    if (context == NULL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    if (command_id == IDC_SCR_RESET_DEFAULTS) {
        scr_settings_set_defaults(&settings);
        scr_apply_settings_to_dialog(dialog, &settings);
        return TRUE;
    }

    if (command_id == IDOK) {
        settings = context->settings;
        scr_read_settings_from_dialog(dialog, &settings);
        if (!scr_settings_save(context->module, &settings)) {
            scr_emit_host_diagnostic(
                context,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                2201UL,
                "scr_config_dialog",
                "Failed to save the shared common-config scaffold."
            );
            scr_show_message_box(dialog, context->module, "Failed to save the shared common-config scaffold.", MB_OK | MB_ICONERROR);
            return TRUE;
        }

        screensave_diag_set_minimum_level(
            &context->diagnostics,
            settings.common.diagnostics_overlay_enabled ? SCREENSAVE_DIAG_LEVEL_DEBUG : SCREENSAVE_DIAG_LEVEL_INFO
        );
        context->settings = settings;
        EndDialog(dialog, IDOK);
        return TRUE;
    }

    if (command_id == IDCANCEL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}

static INT_PTR CALLBACK scr_config_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        SetWindowLongA(dialog, DWL_USER, (LONG)lParam);
        scr_initialize_dialog(dialog, (scr_host_context *)lParam);
        return TRUE;

    case WM_COMMAND:
        return scr_handle_command(dialog, LOWORD(wParam));
    }

    return FALSE;
}

INT_PTR scr_show_config_dialog(scr_host_context *context)
{
    INT_PTR result;

    result = DialogBoxParamA(
        context->instance,
        MAKEINTRESOURCEA(IDD_SCR_CONFIG),
        context->owner_window,
        scr_config_dialog_proc,
        (LPARAM)context
    );

    if (result == -1) {
        scr_show_message_box(
            context->owner_window,
            context->module,
            "The configuration dialog resource could not be loaded.",
            MB_OK | MB_ICONERROR
        );
    }

    return result;
}
