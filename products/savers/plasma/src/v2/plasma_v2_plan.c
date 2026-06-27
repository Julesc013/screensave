#include "plasma_v2_plan.h"

static void plasma_v2_plan_copy_reason(char *dst, ss_u32 dst_size, const char *src)
{
    ss_u32 index;

    if (dst == 0 || dst_size == 0U) {
        return;
    }

    dst[0] = '\0';
    if (src == 0) {
        return;
    }

    index = 0U;
    while (index + 1U < dst_size && src[index] != '\0') {
        dst[index] = src[index];
        ++index;
    }
    dst[index] = '\0';
}

static ss_u32 plasma_v2_plan_dimension_or_one(ss_u32 value)
{
    return value == 0U ? 1U : value;
}

static ss_u32 plasma_v2_plan_min_u32(ss_u32 left, ss_u32 right)
{
    return left < right ? left : right;
}

static ss_u32 plasma_v2_plan_renderer_is_valid(ss_u32 renderer)
{
    return plasma_v2_plan_renderer_token(renderer) != 0 ? SS_V2_TRUE : SS_V2_FALSE;
}

static ss_u32 plasma_v2_plan_renderer_supported(ss_u32 renderer, ss_u32 capability_flags)
{
    if (renderer == PLASMA_V2_RENDERER_SOFTWARE) {
        return (capability_flags & PLASMA_V2_CAP_SOFTWARE_REFERENCE) != 0U ? SS_V2_TRUE : SS_V2_FALSE;
    }
    if (renderer == PLASMA_V2_RENDERER_GDI) {
        return (capability_flags & PLASMA_V2_CAP_GDI) != 0U ? SS_V2_TRUE : SS_V2_FALSE;
    }
    if (renderer == PLASMA_V2_RENDERER_GL11) {
        return (capability_flags & PLASMA_V2_CAP_GL11) != 0U ? SS_V2_TRUE : SS_V2_FALSE;
    }
    return SS_V2_FALSE;
}

static ss_u32 plasma_v2_plan_resolve_renderer(ss_u32 requested, ss_u32 capability_flags, ss_u32 *degradation_flags)
{
    if (plasma_v2_plan_renderer_supported(requested, capability_flags) == SS_V2_TRUE) {
        return requested;
    }

    if (degradation_flags != 0) {
        *degradation_flags |= PLASMA_V2_DEGRADE_RENDERER;
    }

    if ((capability_flags & PLASMA_V2_CAP_GDI) != 0U) {
        return PLASMA_V2_RENDERER_GDI;
    }
    return PLASMA_V2_RENDERER_SOFTWARE;
}

static void plasma_v2_plan_resolve_sizes(plasma_v2_plan *plan)
{
    ss_u32 width;
    ss_u32 height;
    ss_u32 field_width;
    ss_u32 field_height;

    width = plasma_v2_plan_dimension_or_one(plan->drawable_size.width);
    height = plasma_v2_plan_dimension_or_one(plan->drawable_size.height);
    plan->drawable_size.width = width;
    plan->drawable_size.height = height;
    plan->output_size.width = width;
    plan->output_size.height = height;

    field_width = plasma_v2_plan_min_u32(width, (ss_u32)320U);
    field_height = plasma_v2_plan_min_u32(height, (ss_u32)240U);
    if (field_width != width || field_height != height) {
        plan->degradation_flags |= PLASMA_V2_DEGRADE_FIELD_SIZE;
    }
    plan->field_size.width = plasma_v2_plan_dimension_or_one(field_width);
    plan->field_size.height = plasma_v2_plan_dimension_or_one(field_height);
}

const char *plasma_v2_plan_renderer_token(ss_u32 renderer)
{
    if (renderer == PLASMA_V2_RENDERER_SOFTWARE) {
        return "software";
    }
    if (renderer == PLASMA_V2_RENDERER_GDI) {
        return "gdi";
    }
    if (renderer == PLASMA_V2_RENDERER_GL11) {
        return "gl11";
    }
    return 0;
}

void plasma_v2_plan_set_defaults(plasma_v2_plan *plan)
{
    if (plan == 0) {
        return;
    }

    plan->struct_size = (ss_u32)sizeof(*plan);
    plan->schema_version = PLASMA_V2_PLAN_SCHEMA_VERSION;
    plasma_v2_spec_set_defaults(&plan->requested_spec);
    plasma_v2_spec_set_defaults(&plan->resolved_spec);
    plan->drawable_size.width = (ss_u32)320U;
    plan->drawable_size.height = (ss_u32)200U;
    plan->field_size.width = (ss_u32)320U;
    plan->field_size.height = (ss_u32)200U;
    plan->output_size.width = (ss_u32)320U;
    plan->output_size.height = (ss_u32)200U;
    plan->requested_renderer = PLASMA_V2_RENDERER_GDI;
    plan->active_renderer = PLASMA_V2_RENDERER_GDI;
    plan->capability_flags = PLASMA_V2_CAP_SOFTWARE_REFERENCE | PLASMA_V2_CAP_GDI;
    plan->degradation_flags = PLASMA_V2_DEGRADE_NONE;
    plan->base_seed = (ss_u32)104729U;
    plan->stream_seed = (ss_u32)17U;
    plan->use_fixed_point = SS_V2_TRUE;
    plan->use_software_reference = SS_V2_TRUE;
    plan->use_feedback_buffer = SS_V2_TRUE;
    plan->allow_experimental = SS_V2_FALSE;
    plasma_v2_plan_copy_reason(plan->degrade_reason, PLASMA_V2_PLAN_DEGRADE_REASON_LENGTH, "none");
}

ss_u32 plasma_v2_plan_compile(const plasma_v2_plan_request *request, plasma_v2_plan *plan_out)
{
    plasma_v2_plan plan;

    if (request == 0 || plan_out == 0 || request->requested_spec == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (request->struct_size < (ss_u32)sizeof(*request)) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_plan_renderer_is_valid(request->requested_renderer) == SS_V2_FALSE) {
        return SS_V2_STATUS_UNSUPPORTED;
    }

    plasma_v2_plan_set_defaults(&plan);
    plan.requested_spec = *request->requested_spec;
    plasma_v2_spec_clamp(&plan.requested_spec);
    plan.resolved_spec = *request->requested_spec;
    plasma_v2_spec_clamp(&plan.resolved_spec);
    if (plasma_v2_spec_is_valid(&plan.resolved_spec) == SS_V2_FALSE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plan.drawable_size = request->drawable_size;
    plan.requested_renderer = request->requested_renderer;
    plan.capability_flags = request->capability_flags | PLASMA_V2_CAP_SOFTWARE_REFERENCE;
    plan.base_seed = request->base_seed;
    plan.stream_seed = request->stream_seed;
    plan.allow_experimental = request->allow_experimental == SS_V2_TRUE ? SS_V2_TRUE : SS_V2_FALSE;
    plan.use_fixed_point = SS_V2_TRUE;
    plan.use_software_reference = SS_V2_TRUE;
    plan.use_feedback_buffer = (plan.capability_flags & PLASMA_V2_CAP_FEEDBACK_BUFFER) != 0U ? SS_V2_TRUE : SS_V2_FALSE;
    plan.degradation_flags = PLASMA_V2_DEGRADE_NONE;

    plan.active_renderer = plasma_v2_plan_resolve_renderer(
        plan.requested_renderer,
        plan.capability_flags,
        &plan.degradation_flags
    );
    plasma_v2_plan_resolve_sizes(&plan);
    if (plan.allow_experimental != SS_V2_TRUE) {
        if (plan.resolved_spec.output_kind == PLASMA_V2_OUTPUT_GLYPH) {
            plan.resolved_spec.output_kind = PLASMA_V2_OUTPUT_CONTINUOUS;
            plan.degradation_flags |= PLASMA_V2_DEGRADE_EXPERIMENTAL;
        }
        if (plan.resolved_spec.treatment_kind == PLASMA_V2_TREATMENT_CRT || plan.resolved_spec.treatment_kind == PLASMA_V2_TREATMENT_BLOOM) {
            plan.resolved_spec.treatment_kind = PLASMA_V2_TREATMENT_SOFT;
            plan.degradation_flags |= PLASMA_V2_DEGRADE_EXPERIMENTAL;
        }
    }

    plasma_v2_plan_copy_reason(
        plan.degrade_reason,
        PLASMA_V2_PLAN_DEGRADE_REASON_LENGTH,
        plan.degradation_flags == PLASMA_V2_DEGRADE_NONE ? "none" : "capability_or_stable_scope_degrade"
    );

    if (plasma_v2_plan_is_valid(&plan) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    *plan_out = plan;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_plan_is_valid(const plasma_v2_plan *plan)
{
    if (plan == 0) {
        return SS_V2_FALSE;
    }
    if (plan->struct_size < (ss_u32)sizeof(*plan)) {
        return SS_V2_FALSE;
    }
    if (plan->schema_version != PLASMA_V2_PLAN_SCHEMA_VERSION) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_spec_is_valid(&plan->requested_spec) != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_spec_is_valid(&plan->resolved_spec) != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plan->drawable_size.width == 0U || plan->drawable_size.height == 0U) {
        return SS_V2_FALSE;
    }
    if (plan->field_size.width == 0U || plan->field_size.height == 0U) {
        return SS_V2_FALSE;
    }
    if (plan->output_size.width == 0U || plan->output_size.height == 0U) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_plan_renderer_is_valid(plan->requested_renderer) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_plan_renderer_is_valid(plan->active_renderer) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plan->use_fixed_point != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plan->use_software_reference != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plan->use_feedback_buffer != SS_V2_FALSE && plan->use_feedback_buffer != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plan->allow_experimental != SS_V2_FALSE && plan->allow_experimental != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plan->degrade_reason[0] == '\0') {
        return SS_V2_FALSE;
    }

    return SS_V2_TRUE;
}
