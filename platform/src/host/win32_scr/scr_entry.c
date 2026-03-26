#include "scr_internal.h"

static void scr_show_invalid_argument_message(const scr_host_context *context, const scr_parsed_args *parsed_args)
{
    char message[192];

    message[0] = '\0';

    if (parsed_args->invalid_preview_parent) {
        scr_append_text(message, sizeof(message), "Preview mode requires a valid parent window handle.");
        scr_show_message_box(context->owner_window, &context->product, message, MB_OK | MB_ICONERROR);
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
    scr_show_message_box(context->owner_window, &context->product, message, MB_OK | MB_ICONINFORMATION);
}

int screensave_scr_main(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_product_identity *product_identity
)
{
    INT_PTR dialog_result;
    int parse_result;
    scr_host_context context;
    scr_parsed_args parsed_args;

    if (
        product_identity == NULL ||
        product_identity->product_key == NULL ||
        product_identity->display_name == NULL
    ) {
        MessageBoxA(
            NULL,
            "The saver target did not provide a product identity.",
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
    context.product = *product_identity;

    parse_result = scr_parse_command_line(command_line, &parsed_args);
    context.mode = parsed_args.mode;
    context.owner_window = parsed_args.config_owner;
    context.preview_parent = parsed_args.preview_parent;

    if (!parse_result) {
        scr_show_invalid_argument_message(&context, &parsed_args);
        context.mode = SCR_RUN_MODE_CONFIG;
    }

    if (parsed_args.show_invalid_argument_message) {
        scr_show_invalid_argument_message(&context, &parsed_args);
    }

    scr_settings_set_defaults(&context.settings);
    scr_settings_load(&context.product, &context.settings);

    if (context.mode == SCR_RUN_MODE_CONFIG) {
        dialog_result = scr_show_config_dialog(&context);
        return dialog_result == -1 ? 1 : 0;
    }

    return scr_run_window(&context);
}
