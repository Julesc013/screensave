#include "plasma_internal.h"

screensave_renderer_kind plasma_resolve_renderer_kind(
    const screensave_saver_environment *environment
)
{
    screensave_renderer_info renderer_info;

    if (environment == NULL || environment->renderer == NULL) {
        return SCREENSAVE_RENDERER_KIND_GDI;
    }

    screensave_renderer_get_info(environment->renderer, &renderer_info);
    if (
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GDI ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL11 ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL46
    ) {
        return renderer_info.active_kind;
    }

    return SCREENSAVE_RENDERER_KIND_UNKNOWN;
}

screensave_renderer_kind plasma_resolve_requested_renderer_kind(
    const screensave_saver_environment *environment
)
{
    screensave_renderer_info renderer_info;

    if (environment == NULL || environment->renderer == NULL) {
        return SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }

    screensave_renderer_get_info(environment->renderer, &renderer_info);
    return renderer_info.requested_kind;
}

int plasma_is_lower_band_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GDI ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL11;
}

int plasma_is_advanced_runtime_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

int plasma_is_modern_runtime_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

int plasma_plan_is_lower_band_baseline(const struct plasma_plan_tag *plan)
{
    if (
        plan == NULL ||
        !plan->classic_execution ||
        plan->advanced_enabled ||
        plan->advanced_components != 0UL ||
        plan->modern_enabled ||
        plan->modern_components != 0UL ||
        plan->premium_enabled ||
        plan->premium_components != 0UL ||
        plan->output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan->output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
        plan->filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan->accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT
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

    return 1;
}

int plasma_plan_validate_for_renderer_kind(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind renderer_kind
)
{
    if (
        module == NULL ||
        !plasma_plan_validate(plan, module) ||
        !screensave_saver_supports_renderer_kind(module, renderer_kind)
    ) {
        return 0;
    }

    if (plasma_is_lower_band_kind(renderer_kind)) {
        return plasma_plan_is_lower_band_baseline(plan);
    }

    if (renderer_kind == SCREENSAVE_RENDERER_KIND_GL21) {
        return
            plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 &&
            plan->advanced_enabled &&
            !plan->modern_enabled &&
            !plan->premium_enabled;
    }

    if (renderer_kind == SCREENSAVE_RENDERER_KIND_GL33) {
        return
            plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 &&
            plan->advanced_enabled &&
            plan->modern_enabled &&
            !plan->premium_enabled;
    }

    if (renderer_kind == SCREENSAVE_RENDERER_KIND_GL46) {
        return
            plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL46 &&
            plan->advanced_enabled &&
            plan->modern_enabled &&
            plan->premium_enabled;
    }

    return 0;
}

int plasma_plan_validate_lower_band_baseline(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    return
        plasma_plan_validate_for_renderer_kind(plan, module, SCREENSAVE_RENDERER_KIND_GDI) &&
        plasma_plan_validate_for_renderer_kind(plan, module, SCREENSAVE_RENDERER_KIND_GL11);
}
