#ifndef SCREENSAVE_SCR_ENTRY_H
#define SCREENSAVE_SCR_ENTRY_H

#include <windows.h>

/*
 * Series 03 keeps the public bridge narrow.
 * Saver-facing products only provide identity and hand off to the private host.
 */
typedef struct screensave_product_identity_tag {
    const char *product_key;
    const char *display_name;
} screensave_product_identity;

int screensave_scr_main(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_product_identity *product_identity
);

#endif /* SCREENSAVE_SCR_ENTRY_H */
