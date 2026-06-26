#include "screensave/v2/diagnostics.h"

#include "screensave/v2/internal/validate.h"

void ss_v2_diag_context_init(ss_v2_diag_context *context, ss_u32 minimum_level)
{
    if (context == 0) {
        return;
    }
    context->struct_size = (ss_u32)sizeof(*context);
    context->abi_version = SS_V2_ABI_VERSION;
    context->sink = 0;
    context->user_data = 0;
    context->minimum_level = minimum_level;
}

ss_u32 ss_v2_diag_message_is_valid(const ss_v2_diag_message *message)
{
    ss_u32 status;

    status = ss_v2_check_prefix(message, (ss_u32)sizeof(*message), (ss_u32)sizeof(*message), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (message->origin == 0 || message->text == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return SS_V2_STATUS_OK;
}

void ss_v2_diag_emit(
    ss_v2_diag_context *context,
    ss_u32 level,
    ss_u32 domain,
    ss_u32 code,
    const char *origin,
    const char *text
)
{
    ss_v2_diag_message message;

    if (context == 0 || context->sink == 0 || level > context->minimum_level) {
        return;
    }
    message.struct_size = (ss_u32)sizeof(message);
    message.abi_version = SS_V2_ABI_VERSION;
    message.level = level;
    message.domain = domain;
    message.code = code;
    message.origin = origin;
    message.text = text;
    if (ss_v2_diag_message_is_valid(&message) == SS_V2_STATUS_OK) {
        context->sink(context->user_data, &message);
    }
}
