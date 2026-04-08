#include <string.h>

#include "plasma_internal.h"

void plasma_plan_init(plasma_plan *plan)
{
    if (plan == NULL) {
        return;
    }

    ZeroMemory(plan, sizeof(*plan));
    plan->effect_mode = PLASMA_EFFECT_FIRE;
    plan->speed_mode = PLASMA_SPEED_GENTLE;
    plan->resolution_mode = PLASMA_RESOLUTION_STANDARD;
    plan->smoothing_mode = PLASMA_SMOOTHING_SOFT;
    plan->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    plan->seed_policy = PLASMA_PLAN_SEED_POLICY_INHERIT;
    plan->minimum_kind = SCREENSAVE_RENDERER_KIND_GDI;
    plan->preferred_kind = SCREENSAVE_RENDERER_KIND_GL11;
    plan->quality_class = SCREENSAVE_CAPABILITY_QUALITY_SAFE;
    plan->output_family = PLASMA_OUTPUT_FAMILY_RASTER;
    plan->output_mode = PLASMA_OUTPUT_MODE_NATIVE_RASTER;
    plan->sampling_treatment = PLASMA_SAMPLING_TREATMENT_NONE;
    plan->filter_treatment = PLASMA_FILTER_TREATMENT_NONE;
    plan->emulation_treatment = PLASMA_EMULATION_TREATMENT_NONE;
    plan->accent_treatment = PLASMA_ACCENT_TREATMENT_NONE;
    plan->presentation_mode = PLASMA_PRESENTATION_MODE_FLAT;
    plan->classic_execution = 1;
    plasma_advanced_plan_init(plan);
    plasma_modern_plan_init(plan);
    plasma_premium_plan_init(plan);
}

void plasma_plan_bind_renderer_kind(
    plasma_plan *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind requested_kind,
    screensave_renderer_kind active_kind
)
{
    plasma_advanced_bind_plan(plan, module, requested_kind, active_kind);
    plasma_modern_bind_plan(plan, module, requested_kind, active_kind);
    plasma_premium_bind_plan(plan, module, requested_kind, active_kind);
}

int plasma_plan_compile(
    plasma_plan *plan,
    const screensave_saver_module *module,
    const screensave_saver_environment *environment
)
{
    screensave_common_config common_config;
    plasma_config product_config;
    const screensave_config_binding *binding;

    if (plan == NULL || environment == NULL) {
        return 0;
    }

    plasma_plan_init(plan);
    plasma_config_set_defaults(&common_config, &product_config, sizeof(product_config));

    binding = environment->config_binding;
    if (binding != NULL) {
        if (binding->common_config != NULL) {
            common_config = *binding->common_config;
        }
        if (
            binding->product_config != NULL &&
            binding->product_config_size == sizeof(plasma_config)
        ) {
            product_config = *(const plasma_config *)binding->product_config;
        }
    }

    plasma_config_clamp(&common_config, &product_config, sizeof(product_config));
    if (!plasma_selection_resolve(&plan->selection, &common_config, &product_config.selection)) {
        return 0;
    }

    plan->preset_key = plan->selection.selected_preset->preset_key;
    plan->preset = plan->selection.selected_preset->descriptor;
    plan->theme_key = plan->selection.selected_theme->theme_key;
    plan->theme = plan->selection.selected_theme->descriptor;
    plan->effect_mode = product_config.effect_mode;
    plan->speed_mode = product_config.speed_mode;
    plan->resolution_mode = product_config.resolution_mode;
    plan->smoothing_mode = product_config.smoothing_mode;
    plan->detail_level = common_config.detail_level;
    plan->seed_policy = common_config.use_deterministic_seed
        ? PLASMA_PLAN_SEED_POLICY_FIXED
        : PLASMA_PLAN_SEED_POLICY_INHERIT;
    plan->configured_seed = common_config.deterministic_seed;
    plan->base_seed = environment->seed.base_seed;
    plan->stream_seed = environment->seed.stream_seed;
    plan->resolved_rng_seed = environment->seed.base_seed ^ environment->seed.stream_seed;
    plan->deterministic = environment->seed.deterministic;

    if (module != NULL) {
        plan->minimum_kind = module->routing_policy.minimum_kind;
        plan->preferred_kind = module->routing_policy.preferred_kind;
        plan->quality_class = module->routing_policy.quality_class;
    }

    plasma_plan_bind_renderer_kind(
        plan,
        module,
        plasma_resolve_requested_renderer_kind(environment),
        plasma_resolve_renderer_kind(environment)
    );

    return plasma_plan_validate(plan, module);
}

int plasma_plan_validate(
    const plasma_plan *plan,
    const screensave_saver_module *module
)
{
    if (
        plan == NULL ||
        !plasma_content_registry_validate() ||
        !plan->classic_execution ||
        plan->preset_key == NULL ||
        plan->theme_key == NULL ||
        plan->preset == NULL ||
        plan->theme == NULL
    ) {
        return 0;
    }

    if (
        !plasma_selection_state_validate(&plan->selection) ||
        plan->selection.selected_preset == NULL ||
        plan->selection.selected_theme == NULL ||
        plan->selection.selected_preset->descriptor != plan->preset ||
        plan->selection.selected_theme->descriptor != plan->theme
    ) {
        return 0;
    }

    if (
        plan->preset->preset_key == NULL ||
        plan->theme->theme_key == NULL ||
        strcmp(plan->preset->preset_key, plan->preset_key) != 0 ||
        strcmp(plan->theme->theme_key, plan->theme_key) != 0
    ) {
        return 0;
    }

    if (
        plan->effect_mode < PLASMA_EFFECT_PLASMA ||
        plan->effect_mode > PLASMA_EFFECT_INTERFERENCE ||
        plan->speed_mode < PLASMA_SPEED_GENTLE ||
        plan->speed_mode > PLASMA_SPEED_LIVELY ||
        plan->resolution_mode < PLASMA_RESOLUTION_COARSE ||
        plan->resolution_mode > PLASMA_RESOLUTION_FINE ||
        plan->smoothing_mode < PLASMA_SMOOTHING_OFF ||
        plan->smoothing_mode > PLASMA_SMOOTHING_GLOW
    ) {
        return 0;
    }

    if (
        !plasma_output_validate_plan(plan) ||
        !plasma_treatment_validate_plan(plan) ||
        !plasma_presentation_validate_plan(plan) ||
        !plasma_advanced_validate_plan(plan, module) ||
        !plasma_modern_validate_plan(plan, module) ||
        !plasma_premium_validate_plan(plan, module)
    ) {
        return 0;
    }

    if (
        plan->minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        plan->preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        plan->quality_class != SCREENSAVE_CAPABILITY_QUALITY_SAFE
    ) {
        return 0;
    }

    if (module != NULL) {
        if (
            plan->minimum_kind != module->routing_policy.minimum_kind ||
            plan->preferred_kind != module->routing_policy.preferred_kind ||
            plan->quality_class != module->routing_policy.quality_class
        ) {
            return 0;
        }
    }

    return 1;
}
