#ifndef SCREENSAVE_SETTINGS_INTERNAL_H
#define SCREENSAVE_SETTINGS_INTERNAL_H

#include "screensave/saver_api.h"

int screensave_settings_load_shared_state(
    const screensave_saver_module *module,
    screensave_common_config *common_config,
    screensave_diag_context *diagnostics
);
int screensave_settings_save_shared_state(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    screensave_diag_context *diagnostics
);

#endif /* SCREENSAVE_SETTINGS_INTERNAL_H */
