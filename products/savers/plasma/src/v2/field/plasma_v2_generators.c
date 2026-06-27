#include "plasma_v2_generators.h"
#include "plasma_v2_sources.h"

static ss_u32 plasma_v2_generator_store(plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y, ss_u32 value)
{
    ss_u32 offset;

    offset = (y * runtime->resolved_plan.field_size.width) + x;
    if (offset >= runtime->field_cell_count) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    runtime->field_a[offset] = value > 1000U ? 1000U : value;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_generate_classic(plasma_v2_runtime *runtime)
{
    ss_u32 x;
    ss_u32 y;
    ss_u32 width;
    ss_u32 height;
    ss_u32 value;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    width = runtime->resolved_plan.field_size.width;
    height = runtime->resolved_plan.field_size.height;
    for (y = 0U; y < height; ++y) {
        for (x = 0U; x < width; ++x) {
            value = plasma_v2_source_wave(runtime, x, y, 0U);
            value += plasma_v2_source_wave(runtime, y, x, 1U);
            value += plasma_v2_source_noise(runtime, x >> 1U, y >> 1U, 2U);
            if (plasma_v2_generator_store(runtime, x, y, value / 3U) != SS_V2_STATUS_OK) {
                return SS_V2_STATUS_BAD_SIZE;
            }
        }
    }
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_generate_interference(plasma_v2_runtime *runtime)
{
    ss_u32 x;
    ss_u32 y;
    ss_u32 width;
    ss_u32 height;
    ss_u32 value;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    width = runtime->resolved_plan.field_size.width;
    height = runtime->resolved_plan.field_size.height;
    for (y = 0U; y < height; ++y) {
        for (x = 0U; x < width; ++x) {
            value = plasma_v2_source_wave(runtime, x, y, 3U);
            value += plasma_v2_source_radial(runtime, x, y);
            value += plasma_v2_source_wave(runtime, x + y, y, 4U);
            if (plasma_v2_generator_store(runtime, x, y, value / 3U) != SS_V2_STATUS_OK) {
                return SS_V2_STATUS_BAD_SIZE;
            }
        }
    }
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_generate_fire(plasma_v2_runtime *runtime)
{
    ss_u32 x;
    ss_u32 y;
    ss_u32 width;
    ss_u32 height;
    ss_u32 vertical;
    ss_u32 noise;
    ss_u32 value;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    width = runtime->resolved_plan.field_size.width;
    height = runtime->resolved_plan.field_size.height == 0U ? 1U : runtime->resolved_plan.field_size.height;
    for (y = 0U; y < height; ++y) {
        for (x = 0U; x < width; ++x) {
            vertical = ((height - 1U - y) * 1000U) / height;
            noise = plasma_v2_source_noise(runtime, x, y + runtime->phase2, 5U);
            value = ((vertical * 3U) + noise + plasma_v2_source_wave(runtime, x, y, 6U)) / 5U;
            if (plasma_v2_generator_store(runtime, x, y, value) != SS_V2_STATUS_OK) {
                return SS_V2_STATUS_BAD_SIZE;
            }
        }
    }
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_generate_field(plasma_v2_runtime *runtime)
{
    ss_u32 family;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    family = runtime->resolved_plan.resolved_spec.field_family;
    if (family == PLASMA_V2_FIELD_INTERFERENCE) {
        return plasma_v2_generate_interference(runtime);
    }
    if (family == PLASMA_V2_FIELD_FIRE) {
        return plasma_v2_generate_fire(runtime);
    }
    return plasma_v2_generate_classic(runtime);
}
