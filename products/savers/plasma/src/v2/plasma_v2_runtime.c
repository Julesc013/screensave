#include "plasma_v2_runtime.h"

static ss_u32 plasma_v2_runtime_mul_overflows(ss_u32 left, ss_u32 right)
{
    if (left == 0U || right == 0U) {
        return SS_V2_FALSE;
    }
    return left > (0xffffffffU / right) ? SS_V2_TRUE : SS_V2_FALSE;
}

static void plasma_v2_runtime_clear_u32(ss_u32 *items, ss_u32 count)
{
    ss_u32 index;

    if (items == 0) {
        return;
    }

    for (index = 0U; index < count; ++index) {
        items[index] = 0U;
    }
}

static void plasma_v2_runtime_clear_u8(ss_u8 *items, ss_u32 count)
{
    ss_u32 index;

    if (items == 0) {
        return;
    }

    for (index = 0U; index < count; ++index) {
        items[index] = 0U;
    }
}

static ss_u32 plasma_v2_runtime_seed_from_plan(const plasma_v2_plan *plan)
{
    ss_u32 seed;

    seed = plan->base_seed ^ (plan->stream_seed * (ss_u32)1664525U);
    seed ^= plan->resolved_spec.seed * (ss_u32)1013904223U;
    if (seed == 0U) {
        seed = (ss_u32)1U;
    }
    return seed;
}

static ss_u32 plasma_v2_runtime_next_rng(ss_u32 current)
{
    return (current * (ss_u32)1664525U) + (ss_u32)1013904223U;
}

static ss_u32 plasma_v2_runtime_phase_step(const plasma_v2_runtime *runtime, ss_u32 delta_millis)
{
    ss_u32 speed;

    speed = runtime->resolved_plan.resolved_spec.speed + (ss_u32)1U;
    return ((delta_millis + (ss_u32)1U) * speed) / (ss_u32)64U;
}

ss_u32 plasma_v2_runtime_required_field_cells(const plasma_v2_plan *plan, ss_u32 *cell_count_out)
{
    ss_u32 width;
    ss_u32 height;

    if (plan == 0 || cell_count_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_plan_is_valid(plan) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    width = plan->field_size.width;
    height = plan->field_size.height;
    if (plasma_v2_runtime_mul_overflows(width, height) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    *cell_count_out = width * height;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_runtime_required_pixel_bytes(const plasma_v2_plan *plan, ss_u32 *byte_count_out)
{
    ss_u32 pixels;

    if (plan == 0 || byte_count_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_plan_is_valid(plan) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_runtime_mul_overflows(plan->output_size.width, plan->output_size.height) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    pixels = plan->output_size.width * plan->output_size.height;
    if (plasma_v2_runtime_mul_overflows(pixels, PLASMA_V2_PRESENT_PIXEL_BYTES) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    *byte_count_out = pixels * PLASMA_V2_PRESENT_PIXEL_BYTES;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_runtime_reset(plasma_v2_runtime *runtime)
{
    if (runtime == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plasma_v2_runtime_clear_u32(runtime->field_a, runtime->field_cell_count);
    plasma_v2_runtime_clear_u32(runtime->field_b, runtime->field_cell_count);
    plasma_v2_runtime_clear_u32(runtime->field_history, runtime->field_cell_count);
    plasma_v2_runtime_clear_u8(runtime->material_buffer, runtime->material_byte_count);
    plasma_v2_runtime_clear_u8(runtime->treatment_buffer, runtime->treatment_byte_count);
    plasma_v2_runtime_clear_u8(runtime->present_buffer, runtime->present_byte_count);
    runtime->frame_index = 0U;
    runtime->elapsed_millis = 0U;
    runtime->phase0 = runtime->resolved_plan.base_seed & 255U;
    runtime->phase1 = runtime->resolved_plan.stream_seed & 255U;
    runtime->phase2 = runtime->resolved_plan.resolved_spec.seed & 255U;
    runtime->rng_state = plasma_v2_runtime_seed_from_plan(&runtime->resolved_plan);
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_runtime_bind(
    const plasma_v2_plan *plan,
    const plasma_v2_runtime_buffers *buffers,
    plasma_v2_runtime *runtime_out
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
    if (plasma_v2_runtime_required_field_cells(plan, &required_cells) != SS_V2_STATUS_OK) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_runtime_required_pixel_bytes(plan, &required_bytes) != SS_V2_STATUS_OK) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (
        buffers->field_a == 0 ||
        buffers->field_b == 0 ||
        buffers->field_history == 0 ||
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
    runtime_out->field_a = buffers->field_a;
    runtime_out->field_b = buffers->field_b;
    runtime_out->field_history = buffers->field_history;
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
    return plasma_v2_runtime_reset(runtime_out);
}

ss_u32 plasma_v2_runtime_advance(plasma_v2_runtime *runtime, ss_u32 delta_millis)
{
    ss_u32 step;

    if (runtime == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    step = plasma_v2_runtime_phase_step(runtime, delta_millis);
    runtime->elapsed_millis += delta_millis;
    runtime->frame_index += 1U;
    runtime->phase0 = (runtime->phase0 + step + runtime->resolved_plan.resolved_spec.warp) & 1023U;
    runtime->phase1 = (runtime->phase1 + (step * 3U) + runtime->resolved_plan.resolved_spec.complexity) & 1023U;
    runtime->phase2 = (runtime->phase2 + (step * 5U) + runtime->resolved_plan.resolved_spec.feedback) & 1023U;
    runtime->rng_state = plasma_v2_runtime_next_rng(runtime->rng_state);
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_runtime_is_valid(const plasma_v2_runtime *runtime)
{
    if (runtime == 0) {
        return SS_V2_FALSE;
    }
    if (runtime->struct_size < (ss_u32)sizeof(*runtime)) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_plan_is_valid(&runtime->resolved_plan) != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (
        runtime->field_a == 0 ||
        runtime->field_b == 0 ||
        runtime->field_history == 0 ||
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
