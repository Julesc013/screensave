#ifndef SCREENSAVE_SCR_HOST_ENTRY_H
#define SCREENSAVE_SCR_HOST_ENTRY_H

#include "screensave/saver_api.h"

int screensave_scr_main(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_saver_module *module
);

int screensave_scr_main_with_registry(
    HINSTANCE instance,
    HINSTANCE previous_instance,
    LPSTR command_line,
    int show_code,
    const screensave_saver_module *default_module,
    const screensave_saver_module *const *available_modules,
    unsigned int available_module_count
);

#endif /* SCREENSAVE_SCR_HOST_ENTRY_H */
