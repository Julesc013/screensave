#ifndef PLASMA_PREMIUM_H
#define PLASMA_PREMIUM_H

#include "screensave/saver_api.h"
#include "screensave/visual_buffer_api.h"
#include "plasma_presentation.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;

#define PLASMA_PREMIUM_COMPONENT_EXTENDED_HISTORY 0x00000001UL
#define PLASMA_PREMIUM_COMPONENT_POST_CHAIN 0x00000002UL
#define PLASMA_PREMIUM_COMPONENT_HEIGHTFIELD_PRESENTATION 0x00000004UL

#define PLASMA_PREMIUM_DEGRADE_DROP_EXTENDED_HISTORY 0x00000001UL
#define PLASMA_PREMIUM_DEGRADE_DROP_POST_CHAIN 0x00000002UL
#define PLASMA_PREMIUM_DEGRADE_DROP_HEIGHTFIELD_PRESENTATION 0x00000004UL

int plasma_is_premium_kind(screensave_renderer_kind renderer_kind);
void plasma_premium_plan_init(struct plasma_plan_tag *plan);
void plasma_premium_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
);
int plasma_premium_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);
int plasma_premium_apply_field_refinement(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state
);
int plasma_premium_apply_post_refinement(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
);
int plasma_premium_prepare_presentation(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
);

#endif /* PLASMA_PREMIUM_H */
