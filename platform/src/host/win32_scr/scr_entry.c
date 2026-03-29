#include "../../core/base/saver_registry.h"
#include "scr_host_entry.h"
#include "scr_internal.h"

static void scr_show_invalid_argument_message(const scr_host_context *context, const scr_parsed_args *parsed_args)
{
    char message[192];

    message[0] = '\0';

    if (parsed_args->invalid_preview_parent) {
        scr_append_text(message, sizeof(message), "Preview mode requires a valid parent window handle.");
        scr_show_message_box(context->owner_window, context->module, message, MB_OK | MB_ICONERROR);
        return;
    }

    scr_append_text(message, sizeof(message), "Unrecognized argument ");
    if (parsed_args->invalid_argument[0] != '\0') {
        scr_append_text(message, sizeof(message), "'");
        scr_append_text(message, sizeof(message), parsed_args->invalid_argument);
        scr_append_text(message, sizeof(message), "'");
    } else {
        scr_append_text(message, sizeof(message), "for this saver");
    }
    scr_append_text(message, sizeof(message), ". Opening the configuration dialog instead.");
    scr_show_message_box(context->owner_window, context->module, message, MB_OK | MB_ICONINFORMATION);
}

static const screensave_saver_module *scr_resolve_active_module(
    const screensave_saver_module *default_module,
    const screensave_saver_module *const *available_modules,
    unsigned int available_module_count
)
{
    char product_key[64];
    const screensave_saver_module *resolved_module;

    if (
        !screensave_saver_registry_is_valid(available_modules, available_module_count) ||
        !screensave_saver_module_is_valid(default_module)
    ) {
        return NULL;
    }

    resolved_module = default_module;
    if (!scr_load_selected_product_key(product_key, sizeof(product_key))) {
        return resolved_module;
    }

    resolved_module = screensave_saver_registry_find(
        available_modules,
        available_module_count,
        product_key
    );
    if (resolved_module == NULL) {
        resolved_module = default_module;
    }

    return resolved_module;
}

int screensave_scr_main(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_saver_module *module
)
{
    const screensave_saver_module *modules[1];

    modules[0] = module;
    return screensave_scr_main_with_registry(
        instance,
        previous_instance,
        command_line,
        show_code,
        module,
        modules,
        1U
    );
}

int screensave_scr_main_with_registry(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_saver_module *default_module,
    const screensave_saver_module *const *available_modules,
    unsigned int available_module_count
)
{
    INT_PTR dialog_result;
    int parse_result;
    int result;
    scr_host_context context;
    scr_parsed_args parsed_args;
    const screensave_saver_module *active_module;

    active_module = scr_resolve_active_module(default_module, available_modules, available_module_count);
    if (active_module == NULL) {
        MessageBoxA(
            NULL,
            "The saver target did not provide a valid saver registry.",
            "ScreenSave",
            MB_OK | MB_ICONERROR | MB_SETFOREGROUND
        );
        return 1;
    }

    ZeroMemory(&context, sizeof(context));
    context.instance = instance;
    context.previous_instance = previous_instance;
    context.command_line = command_line;
    context.show_code = show_code;
    context.default_module = default_module;
    context.available_modules = available_modules;
    context.available_module_count = available_module_count;
    context.module = active_module;
    context.requested_renderer_kind = scr_load_renderer_request();

    screensave_diag_context_init(
        &context.diagnostics,
        SCREENSAVE_DIAG_LEVEL_INFO
    );

    if (!scr_settings_init(context.module, &context.settings)) {
        scr_show_message_box(
            NULL,
            context.module,
            "The saver configuration state could not be initialized.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }
    if (!scr_settings_init(context.module, &context.resolved_settings)) {
        scr_settings_dispose(&context.settings);
        scr_show_message_box(
            NULL,
            context.module,
            "The session configuration state could not be initialized.",
            MB_OK | MB_ICONERROR
        );
        return 1;
    }

    scr_settings_set_defaults(context.module, &context.settings);
    if (!scr_settings_load(context.module, &context.settings, &context.diagnostics)) {
        scr_emit_host_diagnostic(
            &context,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            2100UL,
            "screensave_scr_main",
            "The saver settings could not be loaded; defaults will be used for this session."
        );
    }
    scr_settings_clamp(context.module, &context.settings);
    screensave_diag_set_minimum_level(
        &context.diagnostics,
        context.settings.common.diagnostics_overlay_enabled ? SCREENSAVE_DIAG_LEVEL_DEBUG : SCREENSAVE_DIAG_LEVEL_INFO
    );
    screensave_config_binding_init(
        &context.config_binding,
        &context.settings.common,
        context.settings.product_config,
        context.settings.product_config_size
    );

    parse_result = scr_parse_command_line(command_line, &parsed_args);
    context.mode = parsed_args.mode;
    context.owner_window = parsed_args.config_owner;
    context.preview_parent = parsed_args.preview_parent;

    if (!parse_result) {
        context.mode = SCREENSAVE_SESSION_MODE_CONFIG;
    }

    if (parsed_args.show_invalid_argument_message) {
        if (parsed_args.invalid_preview_parent) {
            scr_emit_host_diagnostic(
                &context,
                SCREENSAVE_DIAG_LEVEL_WARNING,
                2101UL,
                "screensave_scr_main",
                "Preview mode argument did not provide a valid parent window."
            );
        } else {
            scr_emit_host_diagnostic(
                &context,
                SCREENSAVE_DIAG_LEVEL_INFO,
                2102UL,
                "screensave_scr_main",
                "An invalid saver argument was redirected to the configuration dialog."
            );
        }
        scr_show_invalid_argument_message(&context, &parsed_args);
    }

    if (context.mode == SCREENSAVE_SESSION_MODE_CONFIG) {
        dialog_result = scr_show_config_dialog(&context);
        result = dialog_result == -1 ? 1 : 0;
        scr_settings_dispose(&context.resolved_settings);
        scr_settings_dispose(&context.settings);
        return result;
    }

    result = scr_run_window(&context);
    scr_settings_dispose(&context.resolved_settings);
    scr_settings_dispose(&context.settings);
    return result;
}
