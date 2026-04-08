#ifndef PLASMA_PLAN_H
#define PLASMA_PLAN_H

#include "screensave/saver_api.h"
#include "plasma_output.h"
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
