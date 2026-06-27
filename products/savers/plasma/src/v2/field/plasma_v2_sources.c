#include "plasma_v2_sources.h"

static ss_u32 plasma_v2_source_tri(ss_u32 value)
{
    value = value & 1023U;
    if (value < 512U) {
        return (value * 1000U) / 511U;
    }
    return ((1023U - value) * 1000U) / 511U;
}

static ss_u32 plasma_v2_source_hash(ss_u32 value)
{
    value ^= value >> 16U;
    value *= 2246822519U;
    value ^= value >> 13U;
    value *= 3266489917U;
    value ^= value >> 16U;
    return value;
}

ss_u32 plasma_v2_source_wave(const plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y, ss_u32 stream)
{
    ss_u32 scale;
    ss_u32 phase;
    ss_u32 value;

    if (runtime == 0) {
        return 0U;
    }
    scale = runtime->resolved_plan.resolved_spec.scale + 1U;
    phase = runtime->phase0 + (stream * 173U);
    value = ((x * scale) / 32U) + ((y * (scale + 89U)) / 48U) + phase;
    return plasma_v2_source_tri(value);
}

ss_u32 plasma_v2_source_radial(const plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y)
{
    ss_u32 width;
    ss_u32 height;
    ss_i32 dx;
    ss_i32 dy;
    ss_u32 distance;

    if (runtime == 0) {
        return 0U;
    }
    width = runtime->resolved_plan.field_size.width == 0U ? 1U : runtime->resolved_plan.field_size.width;
    height = runtime->resolved_plan.field_size.height == 0U ? 1U : runtime->resolved_plan.field_size.height;
    dx = (ss_i32)((x * 512U) / width) - (ss_i32)256;
    dy = (ss_i32)((y * 512U) / height) - (ss_i32)256;
    distance = (ss_u32)(((dx * dx) + (dy * dy)) >> 6);
    return plasma_v2_source_tri(distance + runtime->phase1);
}

ss_u32 plasma_v2_source_noise(const plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y, ss_u32 stream)
{
    ss_u32 seed;

    if (runtime == 0) {
        return 0U;
    }
    seed = runtime->rng_state ^ (x * 374761393U) ^ (y * 668265263U) ^ (stream * 1274126177U);
    return plasma_v2_source_hash(seed) % 1001U;
}
