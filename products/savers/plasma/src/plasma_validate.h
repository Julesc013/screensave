#ifndef PLASMA_VALIDATE_H
#define PLASMA_VALIDATE_H

#include "screensave/saver_api.h"

struct plasma_plan_tag;

screensave_renderer_kind plasma_resolve_renderer_kind(
    const screensave_saver_environment *environment
);
screensave_renderer_kind plasma_resolve_requested_renderer_kind(
    const screensave_saver_environment *environment
);
int plasma_is_lower_band_kind(screensave_renderer_kind renderer_kind);
int plasma_plan_is_lower_band_baseline(const struct plasma_plan_tag *plan);
int plasma_plan_validate_for_renderer_kind(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind renderer_kind
);
int plasma_plan_validate_lower_band_baseline(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);

#endif /* PLASMA_VALIDATE_H */
