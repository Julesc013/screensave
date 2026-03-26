#include <stdlib.h>

#include "scr_internal.h"

static char scr_lower_char(char value)
{
    if (value >= 'A' && value <= 'Z') {
        return (char)(value - 'A' + 'a');
    }

    return value;
}

static char *scr_skip_space(char *cursor)
{
    while (*cursor == ' ' || *cursor == '\t') {
        cursor++;
    }

    return cursor;
}

static char *scr_skip_separator(char *cursor)
{
    cursor = scr_skip_space(cursor);

    if (*cursor == ':') {
        cursor++;
    }

    return scr_skip_space(cursor);
}

static void scr_copy_invalid_token(const char *cursor, char *buffer, int buffer_size)
{
    int index;

    index = 0;
    if (buffer_size <= 0) {
        return;
    }

    while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && index < buffer_size - 1) {
        buffer[index] = *cursor;
        cursor++;
        index++;
    }

    buffer[index] = '\0';
}

static int scr_try_parse_window_handle(char *cursor, HWND *window_handle)
{
    char *end;
    unsigned long parsed_value;

    if (cursor == NULL || window_handle == NULL || *cursor == '\0') {
        return 0;
    }

    parsed_value = strtoul(cursor, &end, 0);
    if (end == cursor) {
        return 0;
    }

    while (*end == ' ' || *end == '\t') {
        end++;
    }

    if (*end != '\0' || parsed_value == 0UL) {
        return 0;
    }

    *window_handle = (HWND)parsed_value;
    return 1;
}

int scr_parse_command_line(LPSTR command_line, scr_parsed_args *parsed_args)
{
    char *cursor;
    char mode_char;

    if (parsed_args == NULL) {
        return 0;
    }

    ZeroMemory(parsed_args, sizeof(*parsed_args));
    parsed_args->mode = SCREENSAVE_SESSION_MODE_CONFIG;

    if (command_line == NULL) {
        return 1;
    }

    cursor = scr_skip_space(command_line);
    if (*cursor == '\0') {
        return 1;
    }

    if (*cursor != '/' && *cursor != '-') {
        scr_copy_invalid_token(cursor, parsed_args->invalid_argument, sizeof(parsed_args->invalid_argument));
        parsed_args->show_invalid_argument_message = 1;
        return 1;
    }

    cursor++;
    mode_char = scr_lower_char(*cursor);
    if (mode_char == '\0') {
        return 1;
    }

    cursor++;

    if (mode_char == 's') {
        parsed_args->mode = SCREENSAVE_SESSION_MODE_SCREEN;
        return 1;
    }

    if (mode_char == 'p') {
        parsed_args->mode = SCREENSAVE_SESSION_MODE_PREVIEW;
        cursor = scr_skip_separator(cursor);
        if (!scr_try_parse_window_handle(cursor, &parsed_args->preview_parent)) {
            parsed_args->show_invalid_argument_message = 1;
            parsed_args->invalid_preview_parent = 1;
            lstrcpyA(parsed_args->invalid_argument, "/p");
            return 0;
        }

        return 1;
    }

    if (mode_char == 'c' || mode_char == 'a') {
        parsed_args->mode = SCREENSAVE_SESSION_MODE_CONFIG;
        cursor = scr_skip_separator(cursor);
        if (*cursor != '\0') {
            (void)scr_try_parse_window_handle(cursor, &parsed_args->config_owner);
        }

        return 1;
    }

    scr_copy_invalid_token(command_line, parsed_args->invalid_argument, sizeof(parsed_args->invalid_argument));
    parsed_args->show_invalid_argument_message = 1;
    parsed_args->mode = SCREENSAVE_SESSION_MODE_CONFIG;
    return 1;
}
