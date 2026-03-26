#include <string.h>

#include "scr_internal.h"
#include "resource.h"

static scr_host_context *scr_get_dialog_context(HWND dialog)
{
    return (scr_host_context *)GetWindowLongA(dialog, DWL_USER);
}

static const screensave_saver_config_hooks *scr_get_config_hooks(const scr_host_context *context)
{
    if (context == NULL || context->module == NULL) {
        return NULL;
    }

    return context->module->config_hooks;
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

static void scr_initialize_fallback_dialog(HWND dialog, scr_host_context *context, const scr_settings *settings)
{
    char title[96];
    char info[256];

    title[0] = '\0';
    scr_append_text(title, sizeof(title), context->module->identity.display_name);
    scr_append_text(title, sizeof(title), " Settings");
    SetWindowTextA(dialog, title);

    scr_build_version_text(context, info, sizeof(info));
    SetDlgItemTextA(dialog, IDC_SCR_INFO, info);
    scr_apply_fallback_settings_to_dialog(dialog, settings);
}

static INT_PTR scr_handle_fallback_command(HWND dialog, WORD command_id, scr_settings *settings)
{
    scr_host_context *context;

    context = scr_get_dialog_context(dialog);
    if (context == NULL) {
        EndDialog(dialog, IDCANCEL);
        return TRUE;
    }

    if (command_id == IDC_SCR_RESET_DEFAULTS) {
        scr_settings_set_defaults(context->module, settings);
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

typedef struct scr_fallback_dialog_state_tag {
    scr_host_context *context;
    scr_settings *settings;
} scr_fallback_dialog_state;

static INT_PTR CALLBACK scr_fallback_dialog_proc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
    scr_fallback_dialog_state *dialog_state;

    dialog_state = (scr_fallback_dialog_state *)GetWindowLongA(dialog, DWL_USER);

    switch (message) {
    case WM_INITDIALOG:
        dialog_state = (scr_fallback_dialog_state *)lParam;
        SetWindowLongA(dialog, DWL_USER, (LONG)dialog_state);
        SetWindowLongA(dialog, DWL_MSGRESULT, 0L);
        if (dialog_state != NULL) {
            scr_initialize_fallback_dialog(dialog, dialog_state->context, dialog_state->settings);
        }
        return TRUE;

    case WM_COMMAND:
        if (dialog_state != NULL) {
            return scr_handle_fallback_command(dialog, LOWORD(wParam), dialog_state->settings);
        }
        break;
    }

    return FALSE;
}

static int scr_copy_settings(scr_settings *target, const scr_settings *source)
{
    if (target == NULL || source == NULL) {
        return 0;
    }

    target->common = source->common;
    if (
        source->product_config != NULL &&
        target->product_config != NULL &&
        source->product_config_size == target->product_config_size &&
        source->product_config_size > 0U
    ) {
        memcpy(target->product_config, source->product_config, source->product_config_size);
    }

    return 1;
}

static void scr_commit_settings(scr_host_context *context, const scr_settings *source)
{
    if (context == NULL || source == NULL) {
        return;
    }

    context->settings.common = source->common;
    if (
        context->settings.product_config != NULL &&
        source->product_config != NULL &&
        context->settings.product_config_size == source->product_config_size &&
        source->product_config_size > 0U
    ) {
        memcpy(
            context->settings.product_config,
            source->product_config,
            source->product_config_size
        );
    }

    screensave_config_binding_init(
        &context->config_binding,
        &context->settings.common,
        context->settings.product_config,
        context->settings.product_config_size
    );
    screensave_diag_set_minimum_level(
        &context->diagnostics,
        context->settings.common.diagnostics_overlay_enabled ? SCREENSAVE_DIAG_LEVEL_DEBUG : SCREENSAVE_DIAG_LEVEL_INFO
    );
}

static INT_PTR scr_show_product_dialog(scr_host_context *context, scr_settings *settings)
{
    const screensave_saver_config_hooks *config_hooks;

    config_hooks = scr_get_config_hooks(context);
    if (config_hooks == NULL || config_hooks->show_config_dialog == NULL) {
        return IDCANCEL;
    }

    return config_hooks->show_config_dialog(
        context->instance,
        context->owner_window,
        context->module,
        &settings->common,
        settings->product_config,
        settings->product_config_size,
        &context->diagnostics
    );
}

static INT_PTR scr_show_fallback_dialog(scr_host_context *context, scr_settings *settings)
{
    INT_PTR result;
    scr_fallback_dialog_state dialog_state;

    dialog_state.context = context;
    dialog_state.settings = settings;

    result = DialogBoxParamA(
        context->instance,
        MAKEINTRESOURCEA(IDD_SCR_CONFIG),
        context->owner_window,
        scr_fallback_dialog_proc,
        (LPARAM)&dialog_state
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

INT_PTR scr_show_config_dialog(scr_host_context *context)
{
    INT_PTR result;
    scr_settings dialog_settings;

    if (context == NULL) {
        return -1;
    }

    if (!scr_settings_init(context->module, &dialog_settings)) {
        scr_show_message_box(
            context->owner_window,
            context->module,
            "The saver configuration state could not be initialized for editing.",
            MB_OK | MB_ICONERROR
        );
        return -1;
    }

    scr_settings_set_defaults(context->module, &dialog_settings);
    scr_copy_settings(&dialog_settings, &context->settings);

    if (scr_get_config_hooks(context) != NULL && scr_get_config_hooks(context)->show_config_dialog != NULL) {
        result = scr_show_product_dialog(context, &dialog_settings);
    } else {
        result = scr_show_fallback_dialog(context, &dialog_settings);
    }

    if (result == -1) {
        scr_show_message_box(
            context->owner_window,
            context->module,
            "The saver configuration dialog could not be loaded.",
            MB_OK | MB_ICONERROR
        );
    }

    if (result == IDOK) {
        scr_settings_clamp(context->module, &dialog_settings);
        if (!scr_settings_save(context->module, &dialog_settings, &context->diagnostics)) {
            scr_emit_host_diagnostic(
                context,
                SCREENSAVE_DIAG_LEVEL_ERROR,
                2201UL,
                "scr_config_dialog",
                "Failed to save the saver settings."
            );
            scr_show_message_box(
                context->owner_window,
                context->module,
                "Failed to save the saver settings.",
                MB_OK | MB_ICONERROR
            );
            result = -1;
        } else {
            scr_commit_settings(context, &dialog_settings);
        }
    }

    scr_settings_dispose(&dialog_settings);
    return result;
}
