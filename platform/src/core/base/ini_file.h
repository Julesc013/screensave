#ifndef SCREENSAVE_INI_FILE_H
#define SCREENSAVE_INI_FILE_H

typedef int (*screensave_ini_entry_callback)(
    void *context,
    const char *section,
    const char *key,
    const char *value
);

int screensave_ini_parse_file(
    const char *path,
    screensave_ini_entry_callback entry_callback,
    void *callback_context
);

#endif /* SCREENSAVE_INI_FILE_H */
