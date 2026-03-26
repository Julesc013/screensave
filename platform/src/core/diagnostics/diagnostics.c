#include <string.h>

#include "screensave/diagnostics_api.h"

static void screensave_diag_copy_text(char *buffer, int buffer_size, const char *text)
{
    int index;

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    index = 0;
    if (text != NULL) {
        while (*text != '\0' && index < buffer_size - 1) {
            buffer[index] = *text;
            ++index;
            ++text;
        }
    }

    buffer[index] = '\0';
}

void screensave_diag_context_init(screensave_diag_context *context, screensave_diag_level minimum_level)
{
    if (context == NULL) {
        return;
    }

    memset(context, 0, sizeof(*context));
    context->minimum_level = minimum_level;
    context->last_level = minimum_level;
    context->last_domain = SCREENSAVE_DIAG_DOMAIN_PLATFORM;
}

void screensave_diag_set_sink(
    screensave_diag_context *context,
    screensave_diag_sink_fn sink,
    void *user_data
)
{
    if (context == NULL) {
        return;
    }

    context->sink = sink;
    context->sink_user_data = user_data;
}

void screensave_diag_set_minimum_level(screensave_diag_context *context, screensave_diag_level minimum_level)
{
    if (context == NULL) {
        return;
    }

    context->minimum_level = minimum_level;
}

int screensave_diag_should_emit(const screensave_diag_context *context, screensave_diag_level level)
{
    if (context == NULL) {
        return 0;
    }

    return level <= context->minimum_level;
}

void screensave_diag_emit(
    screensave_diag_context *context,
    screensave_diag_level level,
    screensave_diag_domain domain,
    unsigned long code,
    const char *origin,
    const char *text
)
{
    screensave_diag_message message;

    if (context == NULL) {
        return;
    }

    if (!screensave_diag_should_emit(context, level)) {
        context->dropped_count += 1UL;
        return;
    }

    context->emitted_count += 1UL;
    context->last_level = level;
    context->last_domain = domain;
    screensave_diag_copy_text(context->last_text, (int)sizeof(context->last_text), text);

    message.level = level;
    message.domain = domain;
    message.code = code;
    message.origin = origin;
    message.text = text;

    if (context->sink != NULL) {
        context->sink(context->sink_user_data, &message);
    }
}

const char *screensave_diag_level_name(screensave_diag_level level)
{
    switch (level) {
    case SCREENSAVE_DIAG_LEVEL_ERROR:
        return "error";

    case SCREENSAVE_DIAG_LEVEL_WARNING:
        return "warning";

    case SCREENSAVE_DIAG_LEVEL_INFO:
        return "info";

    case SCREENSAVE_DIAG_LEVEL_DEBUG:
    default:
        return "debug";
    }
}

const char *screensave_diag_domain_name(screensave_diag_domain domain)
{
    switch (domain) {
    case SCREENSAVE_DIAG_DOMAIN_HOST:
        return "host";

    case SCREENSAVE_DIAG_DOMAIN_CORE:
        return "core";

    case SCREENSAVE_DIAG_DOMAIN_CONFIG:
        return "config";

    case SCREENSAVE_DIAG_DOMAIN_RENDERER:
        return "renderer";

    case SCREENSAVE_DIAG_DOMAIN_SAVER:
        return "saver";

    case SCREENSAVE_DIAG_DOMAIN_APP:
        return "app";

    case SCREENSAVE_DIAG_DOMAIN_PLATFORM:
    default:
        return "platform";
    }
}
