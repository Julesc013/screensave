#include "benchlab_internal.h"

static int benchlab_diag_append_text(char *buffer, int buffer_size, const char *text)
{
    int used;
    int added;

    if (buffer == NULL || text == NULL || buffer_size <= 0) {
        return 0;
    }

    used = lstrlenA(buffer);
    added = lstrlenA(text);
    if (used + added + 1 > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

static void benchlab_diag_push_line(benchlab_diag_buffer *buffer, const char *text)
{
    char *slot;

    if (buffer == NULL) {
        return;
    }

    slot = buffer->lines[buffer->next_index];
    slot[0] = '\0';
    if (text != NULL) {
        lstrcpynA(slot, text, BENCHLAB_DIAG_LINE_LENGTH);
    }

    buffer->next_index = (buffer->next_index + 1U) % BENCHLAB_DIAG_LINE_COUNT;
    if (buffer->count < BENCHLAB_DIAG_LINE_COUNT) {
        buffer->count += 1U;
    }
}

static void benchlab_diag_format_line(char *buffer, int buffer_size, const screensave_diag_message *message)
{
    char prefix[64];

    if (buffer == NULL || buffer_size <= 0) {
        return;
    }

    buffer[0] = '\0';
    if (message == NULL) {
        return;
    }

    wsprintfA(
        prefix,
        "%s/%s %lu ",
        screensave_diag_level_name(message->level),
        screensave_diag_domain_name(message->domain),
        message->code
    );
    benchlab_diag_append_text(buffer, buffer_size, prefix);
    if (message->text != NULL && message->text[0] != '\0') {
        benchlab_diag_append_text(buffer, buffer_size, message->text);
    } else if (message->origin != NULL && message->origin[0] != '\0') {
        benchlab_diag_append_text(buffer, buffer_size, message->origin);
    }
}

static void benchlab_diag_sink(void *user_data, const screensave_diag_message *message)
{
    benchlab_app *app;
    char line[BENCHLAB_DIAG_LINE_LENGTH];

    app = (benchlab_app *)user_data;
    if (app == NULL || message == NULL) {
        return;
    }

    benchlab_diag_format_line(line, sizeof(line), message);
    benchlab_diag_push_line(&app->diag_buffer, line);
}

void benchlab_diag_attach(benchlab_app *app)
{
    if (app == NULL) {
        return;
    }

    ZeroMemory(&app->diag_buffer, sizeof(app->diag_buffer));
    screensave_diag_set_sink(&app->diagnostics, benchlab_diag_sink, app);
}

void benchlab_emit_app_diag(
    benchlab_app *app,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (app == NULL) {
        return;
    }

    screensave_diag_emit(
        &app->diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_APP,
        code,
        "benchlab",
        text
    );
}

const char *benchlab_diag_get_recent(const benchlab_diag_buffer *buffer, unsigned int recent_index)
{
    unsigned int index;

    if (buffer == NULL || recent_index >= buffer->count) {
        return "";
    }

    index = (buffer->next_index + BENCHLAB_DIAG_LINE_COUNT - 1U - recent_index) % BENCHLAB_DIAG_LINE_COUNT;
    return buffer->lines[index];
}
