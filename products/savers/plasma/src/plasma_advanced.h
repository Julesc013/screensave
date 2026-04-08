#ifndef PLASMA_ADVANCED_H
#define PLASMA_ADVANCED_H

#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;

#define PLASMA_ADVANCED_COMPONENT_HISTORY_FEEDBACK 0x00000001UL
#define PLASMA_ADVANCED_COMPONENT_DOMAIN_WARP 0x00000002UL
#define PLASMA_ADVANCED_COMPONENT_FLOW_TURBULENCE 0x00000004UL
#define PLASMA_ADVANCED_COMPONENT_GLOW_POST 0x00000008UL

#define PLASMA_ADVANCED_DEGRADE_DROP_HISTORY 0x00000001UL
#define PLASMA_ADVANCED_DEGRADE_DROP_DOMAIN_WARP 0x00000002UL
#define PLASMA_ADVANCED_DEGRADE_DROP_FLOW_TURBULENCE 0x00000004UL
#define PLASMA_ADVANCED_DEGRADE_DROP_GLOW_POST 0x00000008UL

int plasma_is_advanced_kind(screensave_renderer_kind renderer_kind);
void plasma_advanced_plan_init(struct plasma_plan_tag *plan);
void plasma_advanced_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
);
int plasma_advanced_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);
int plasma_advanced_apply_field_effects(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state
);
int plasma_advanced_apply_blur_filter(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
);
int plasma_advanced_apply_overlay_accent(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
);

#endif /* PLASMA_ADVANCED_H */
