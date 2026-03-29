#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "ini_file.h"

static int screensave_ini_text_copy(char *buffer, unsigned int buffer_size, const char *text)
{
    size_t text_length;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return 1;
    }

    text_length = strlen(text);
    if (text_length + 1U > (size_t)buffer_size) {
        return 0;
    }

    strcpy(buffer, text);
    return 1;
}

static char *screensave_ini_trim_text(char *text)
{
    char *end;

    if (text == NULL) {
        return NULL;
    }

    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }

    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }

    *end = '\0';
    return text;
}

int screensave_ini_parse_file(
    const char *path,
    screensave_ini_entry_callback entry_callback,
    void *callback_context
)
{
    FILE *file;
    char line[512];
    char current_section[64];

    if (path == NULL || entry_callback == NULL) {
        return 0;
    }

    file = fopen(path, "r");
    if (file == NULL) {
        return 0;
    }

    current_section[0] = '\0';
    while (fgets(line, sizeof(line), file) != NULL) {
        char *cursor;
        char *separator;

        cursor = screensave_ini_trim_text(line);
        if (
            cursor == NULL ||
            cursor[0] == '\0' ||
            cursor[0] == ';' ||
            cursor[0] == '#'
        ) {
            continue;
        }

        if (cursor[0] == '[') {
            char *end;

            end = strchr(cursor, ']');
            if (end == NULL) {
                fclose(file);
                return 0;
            }

            *end = '\0';
            if (!screensave_ini_text_copy(current_section, sizeof(current_section), cursor + 1)) {
                fclose(file);
                return 0;
            }
            continue;
        }

        separator = strchr(cursor, '=');
        if (separator == NULL) {
            fclose(file);
            return 0;
        }

        *separator = '\0';
        if (
            !entry_callback(
                callback_context,
                current_section,
                screensave_ini_trim_text(cursor),
                screensave_ini_trim_text(separator + 1)
            )
        ) {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}
