#include "plasma_v3_runtime.h"

#include "field/plasma_v3_field.h"
#include "material/plasma_v3_material.h"
#include "present/plasma_v3_present.h"
#include "treatment/plasma_v3_treatment.h"

static void plasma_v3_clear_u32(ss_u32 *items, ss_u32 count)
{
    ss_u32 index;

    if (items == 0) {
        return;
    }
    for (index = 0U; index < count; ++index) {
        items[index] = 0U;
    }
}

static void plasma_v3_clear_u8(ss_u8 *items, ss_u32 count)
{
    ss_u32 index;

    if (items == 0) {
        return;
    }
    for (index = 0U; index < count; ++index) {
        items[index] = 0U;
    }
}

static ss_u32 plasma_v3_seed_from_plan(const plasma_v3_plan *plan)
{
    ss_u32 seed;

    seed = plan->base_seed ^ (plan->resolved_spec.seed * 1664525U);
    seed ^= plan->resolved_spec.field_scale * 1013904223U;
    if (seed == 0U) {
        seed = 1U;
    }
    return seed;
}

static ss_u32 plasma_v3_next_rng(ss_u32 current)
{
    return (current * 1664525U) + 1013904223U;
}

ss_u32 plasma_v3_runtime_bind(
    const plasma_v3_plan *plan,
    const plasma_v3_runtime_buffers *buffers,
    plasma_v3_runtime *runtime_out
)
{
    ss_u32 required_cells;
    ss_u32 required_bytes;

    if (plan == 0 || buffers == 0 || runtime_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (buffers->struct_size < (ss_u32)sizeof(*buffers)) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    if (plasma_v3_plan_required_field_cells(plan, &required_cells) != SS_V2_STATUS_OK) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_plan_required_pixel_bytes(plan, &required_bytes) != SS_V2_STATUS_OK) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (
        buffers->field_buffer == 0 ||
        buffers->material_buffer == 0 ||
        buffers->treatment_buffer == 0 ||
        buffers->present_buffer == 0
    ) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (
        buffers->field_cell_count < required_cells ||
        buffers->material_byte_count < required_bytes ||
        buffers->treatment_byte_count < required_bytes ||
        buffers->present_byte_count < required_bytes
    ) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    runtime_out->struct_size = (ss_u32)sizeof(*runtime_out);
    runtime_out->resolved_plan = *plan;
    runtime_out->field_buffer = buffers->field_buffer;
    runtime_out->material_buffer = buffers->material_buffer;
    runtime_out->treatment_buffer = buffers->treatment_buffer;
    runtime_out->present_buffer = buffers->present_buffer;
    runtime_out->field_cell_count = required_cells;
    runtime_out->material_byte_count = required_bytes;
    runtime_out->treatment_byte_count = required_bytes;
    runtime_out->present_byte_count = required_bytes;
    runtime_out->frame_index = 0U;
    runtime_out->elapsed_millis = 0U;
    runtime_out->phase0 = 0U;
    runtime_out->phase1 = 0U;
    runtime_out->phase2 = 0U;
    runtime_out->rng_state = 1U;
    return plasma_v3_runtime_reset(runtime_out);
}

ss_u32 plasma_v3_runtime_reset(plasma_v3_runtime *runtime)
{
    if (runtime == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    plasma_v3_clear_u32(runtime->field_buffer, runtime->field_cell_count);
    plasma_v3_clear_u8(runtime->material_buffer, runtime->material_byte_count);
    plasma_v3_clear_u8(runtime->treatment_buffer, runtime->treatment_byte_count);
    plasma_v3_clear_u8(runtime->present_buffer, runtime->present_byte_count);
    runtime->frame_index = 0U;
    runtime->elapsed_millis = 0U;
    runtime->phase0 = runtime->resolved_plan.base_seed & 1023U;
    runtime->phase1 = runtime->resolved_plan.resolved_spec.seed & 1023U;
    runtime->phase2 = runtime->resolved_plan.resolved_spec.palette_shift & 1023U;
    runtime->rng_state = plasma_v3_seed_from_plan(&runtime->resolved_plan);
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v3_runtime_advance(plasma_v3_runtime *runtime, ss_u32 delta_millis)
{
    ss_u32 motion;
    ss_u32 step;

    if (plasma_v3_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    motion = runtime->resolved_plan.resolved_spec.motion + 20U;
    step = ((delta_millis + 1U) * motion) / 96U;
    runtime->elapsed_millis += delta_millis;
    runtime->frame_index += 1U;
    runtime->phase0 = (runtime->phase0 + step + (runtime->resolved_plan.resolved_spec.warp / 8U)) & 1023U;
    runtime->phase1 = (runtime->phase1 + (step * 2U) + (runtime->resolved_plan.resolved_spec.detail / 16U)) & 1023U;
    runtime->phase2 = (runtime->phase2 + (step * 3U) + (runtime->resolved_plan.resolved_spec.palette_shift / 10U)) & 1023U;
    runtime->rng_state = plasma_v3_next_rng(runtime->rng_state);
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v3_runtime_render_frame(plasma_v3_runtime *runtime)
{
    ss_u32 status;

    status = plasma_v3_field_render(runtime);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = plasma_v3_material_map(runtime);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = plasma_v3_treatment_apply(runtime);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return plasma_v3_present_flat(runtime);
}

ss_u32 plasma_v3_runtime_is_valid(const plasma_v3_runtime *runtime)
{
    if (runtime == 0) {
        return SS_V2_FALSE;
    }
    if (runtime->struct_size < (ss_u32)sizeof(*runtime)) {
        return SS_V2_FALSE;
    }
    if (plasma_v3_plan_is_valid(&runtime->resolved_plan) != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (
        runtime->field_buffer == 0 ||
        runtime->material_buffer == 0 ||
        runtime->treatment_buffer == 0 ||
        runtime->present_buffer == 0
    ) {
        return SS_V2_FALSE;
    }
    if (
        runtime->field_cell_count == 0U ||
        runtime->material_byte_count == 0U ||
        runtime->treatment_byte_count == 0U ||
        runtime->present_byte_count == 0U
    ) {
        return SS_V2_FALSE;
    }
    if (runtime->rng_state == 0U) {
        return SS_V2_FALSE;
    }
    return SS_V2_TRUE;
}
