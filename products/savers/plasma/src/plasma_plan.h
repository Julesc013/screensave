#ifndef PLASMA_PLAN_H
#define PLASMA_PLAN_H

#include "screensave/saver_api.h"
#include "plasma_content.h"
#include "plasma_output.h"
#include "plasma_selection.h"
#include "plasma_transition.h"
#include "plasma_treatment.h"
#include "plasma_presentation.h"

typedef enum plasma_plan_seed_policy_tag {
    PLASMA_PLAN_SEED_POLICY_INHERIT = 0,
    PLASMA_PLAN_SEED_POLICY_FIXED = 1
} plasma_plan_seed_policy;

typedef struct plasma_plan_tag {
    const char *preset_key;
    const screensave_preset_descriptor *preset;
    const char *theme_key;
    const screensave_theme_descriptor *theme;
    int effect_mode;
    int speed_mode;
    int resolution_mode;
    int smoothing_mode;
    screensave_detail_level detail_level;
    plasma_plan_seed_policy seed_policy;
    unsigned long configured_seed;
    unsigned long base_seed;
    unsigned long stream_seed;
    unsigned long resolved_rng_seed;
    int deterministic;
    plasma_selection_state selection;
    screensave_renderer_kind requested_renderer_kind;
    screensave_renderer_kind active_renderer_kind;
    int advanced_capable;
    int advanced_requested;
    int advanced_enabled;
    int advanced_degraded;
    unsigned long advanced_components;
    unsigned long advanced_degrade_policy;
    int modern_capable;
    int modern_requested;
    int modern_enabled;
    int modern_degraded;
    unsigned long modern_components;
    unsigned long modern_degrade_policy;
    int premium_capable;
    int premium_requested;
    int premium_enabled;
    int premium_degraded;
    unsigned long premium_components;
    unsigned long premium_degrade_policy;
    int transition_requested;
    int transition_enabled;
    plasma_transition_policy transition_policy;
    plasma_transition_fallback_policy transition_fallback_policy;
    plasma_transition_seed_continuity_policy transition_seed_policy;
    unsigned long transition_supported_types;
    unsigned long transition_interval_millis;
    unsigned long transition_duration_millis;
    const plasma_content_journey_entry *journey;
    screensave_renderer_kind minimum_kind;
    screensave_renderer_kind preferred_kind;
    screensave_capability_quality_class quality_class;
    plasma_output_family output_family;
    plasma_output_mode output_mode;
    plasma_sampling_treatment sampling_treatment;
    plasma_filter_treatment filter_treatment;
    plasma_emulation_treatment emulation_treatment;
    plasma_accent_treatment accent_treatment;
    plasma_presentation_mode presentation_mode;
    int classic_execution;
} plasma_plan;

void plasma_plan_init(plasma_plan *plan);
void plasma_plan_bind_renderer_kind(
    plasma_plan *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
);
int plasma_plan_compile(
    plasma_plan *plan,
    const screensave_saver_module *module,
    const screensave_saver_environment *environment
);
int plasma_plan_validate(
    const plasma_plan *plan,
    const screensave_saver_module *module
);

#endif /* PLASMA_PLAN_H */
