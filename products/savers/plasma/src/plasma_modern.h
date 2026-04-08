#ifndef PLASMA_MODERN_H
#define PLASMA_MODERN_H

#include "screensave/saver_api.h"
#include "plasma_presentation.h"

struct plasma_plan_tag;
struct plasma_execution_state_tag;

#define PLASMA_MODERN_COMPONENT_REFINED_FIELD 0x00000001UL
#define PLASMA_MODERN_COMPONENT_REFINED_FILTER 0x00000002UL
#define PLASMA_MODERN_COMPONENT_PRESENTATION_BUFFER 0x00000004UL

#define PLASMA_MODERN_DEGRADE_DROP_REFINED_FIELD 0x00000001UL
#define PLASMA_MODERN_DEGRADE_DROP_REFINED_FILTER 0x00000002UL
#define PLASMA_MODERN_DEGRADE_DROP_PRESENTATION_BUFFER 0x00000004UL

int plasma_is_modern_kind(screensave_renderer_kind renderer_kind);
void plasma_modern_plan_init(struct plasma_plan_tag *plan);
void plasma_modern_bind_plan(
    struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
);
int plasma_modern_validate_plan(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
);
int plasma_modern_apply_field_refinement(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state
);
int plasma_modern_apply_filter_refinement(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
);
int plasma_modern_prepare_presentation(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_treated_frame *treated_frame,
    plasma_presentation_target *target_out
);

#endif /* PLASMA_MODERN_H */
