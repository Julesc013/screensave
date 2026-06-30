#include "plasma_v3_plan.h"

static ss_u32 plasma_v3_mul_overflows(ss_u32 left, ss_u32 right)
{
    if (left == 0U || right == 0U) {
        return SS_V2_FALSE;
    }
    return left > (0xffffffffU / right) ? SS_V2_TRUE : SS_V2_FALSE;
}

static ss_u32 plasma_v3_clamp_dimension(ss_u32 value)
{
    if (value < 16U) {
        return 16U;
    }
    if (value > 4096U) {
        return 4096U;
    }
    return value;
}

void plasma_v3_plan_set_defaults(plasma_v3_plan *plan)
{
    if (plan == 0) {
        return;
    }
    plan->struct_size = (ss_u32)sizeof(*plan);
    plan->schema_version = PLASMA_V3_SCHEMA_VERSION;
    plasma_v3_spec_set_defaults(&plan->requested_spec);
    plasma_v3_spec_set_defaults(&plan->resolved_spec);
    plan->drawable_size.width = 192U;
    plan->drawable_size.height = 108U;
    plan->field_size = plan->drawable_size;
    plan->output_size = plan->drawable_size;
    plan->requested_renderer = PLASMA_V3_RENDERER_GDI;
    plan->active_renderer = PLASMA_V3_RENDERER_GDI;
    plan->capability_flags = PLASMA_V3_CAP_SOFTWARE_REFERENCE | PLASMA_V3_CAP_GDI;
    plan->degradation_flags = PLASMA_V3_DEGRADE_NONE;
    plan->base_seed = 1U;
    plan->frame_delta_millis = 33U;
    plan->use_fixed_point = SS_V2_TRUE;
    plan->use_software_reference = SS_V2_TRUE;
}

static ss_u32 plasma_v3_resolve_renderer(ss_u32 requested, ss_u32 capabilities, ss_u32 *degradation_flags)
{
    if (requested == PLASMA_V3_RENDERER_GL11 && (capabilities & PLASMA_V3_CAP_GL11) != 0U) {
        return PLASMA_V3_RENDERER_GL11;
    }
    if (requested == PLASMA_V3_RENDERER_GDI && (capabilities & PLASMA_V3_CAP_GDI) != 0U) {
        return PLASMA_V3_RENDERER_GDI;
    }
    if ((capabilities & PLASMA_V3_CAP_GDI) != 0U) {
        *degradation_flags |= PLASMA_V3_DEGRADE_RENDERER;
        return PLASMA_V3_RENDERER_GDI;
    }
    *degradation_flags |= PLASMA_V3_DEGRADE_RENDERER;
    return PLASMA_V3_RENDERER_SOFTWARE;
}

ss_u32 plasma_v3_plan_compile(const plasma_v3_plan_request *request, plasma_v3_plan *plan_out)
{
    plasma_v3_spec resolved;
    ss_u32 capabilities;

    if (request == 0 || plan_out == 0 || request->requested_spec == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (request->struct_size < (ss_u32)sizeof(*request)) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    resolved = *request->requested_spec;
    plasma_v3_spec_clamp(&resolved);
    if (plasma_v3_spec_is_valid(&resolved) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plasma_v3_plan_set_defaults(plan_out);
    plan_out->requested_spec = *request->requested_spec;
    plan_out->resolved_spec = resolved;
    plan_out->drawable_size.width = plasma_v3_clamp_dimension(request->drawable_size.width);
    plan_out->drawable_size.height = plasma_v3_clamp_dimension(request->drawable_size.height);
    plan_out->field_size = plan_out->drawable_size;
    plan_out->output_size = plan_out->drawable_size;
    plan_out->requested_renderer = request->requested_renderer;
    capabilities = request->capability_flags;
    if ((capabilities & PLASMA_V3_CAP_SOFTWARE_REFERENCE) == 0U) {
        capabilities |= PLASMA_V3_CAP_SOFTWARE_REFERENCE;
    }
    plan_out->capability_flags = capabilities;
    plan_out->degradation_flags = PLASMA_V3_DEGRADE_NONE;
    plan_out->active_renderer = plasma_v3_resolve_renderer(
        request->requested_renderer,
        capabilities,
        &plan_out->degradation_flags
    );
    plan_out->base_seed = request->base_seed == 0U ? 1U : request->base_seed;
    plan_out->frame_delta_millis = 33U;
    plan_out->use_fixed_point = SS_V2_TRUE;
    plan_out->use_software_reference = SS_V2_TRUE;
    return plasma_v3_plan_is_valid(plan_out) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_FAIL;
}

ss_u32 plasma_v3_plan_required_field_cells(const plasma_v3_plan *plan, ss_u32 *cell_count_out)
{
    if (plan == 0 || cell_count_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_plan_is_valid(plan) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_mul_overflows(plan->field_size.width, plan->field_size.height) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    *cell_count_out = plan->field_size.width * plan->field_size.height;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v3_plan_required_pixel_bytes(const plasma_v3_plan *plan, ss_u32 *byte_count_out)
{
    ss_u32 pixels;

    if (plan == 0 || byte_count_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_plan_is_valid(plan) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_mul_overflows(plan->output_size.width, plan->output_size.height) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    pixels = plan->output_size.width * plan->output_size.height;
    if (plasma_v3_mul_overflows(pixels, PLASMA_V3_PRESENT_PIXEL_BYTES) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    *byte_count_out = pixels * PLASMA_V3_PRESENT_PIXEL_BYTES;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v3_plan_is_valid(const plasma_v3_plan *plan)
{
    if (plan == 0) {
        return SS_V2_FALSE;
    }
    if (plan->struct_size < (ss_u32)sizeof(*plan)) {
        return SS_V2_FALSE;
    }
    if (plan->schema_version != PLASMA_V3_SCHEMA_VERSION) {
        return SS_V2_FALSE;
    }
    if (plasma_v3_spec_is_valid(&plan->resolved_spec) != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plan->field_size.width == 0U || plan->field_size.height == 0U) {
        return SS_V2_FALSE;
    }
    if (plan->output_size.width == 0U || plan->output_size.height == 0U) {
        return SS_V2_FALSE;
    }
    if (plasma_v3_renderer_token(plan->active_renderer) == 0) {
        return SS_V2_FALSE;
    }
    if (plan->use_fixed_point != SS_V2_TRUE || plan->use_software_reference != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    return SS_V2_TRUE;
}
