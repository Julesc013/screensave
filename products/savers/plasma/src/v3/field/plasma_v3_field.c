#include "plasma_v3_field.h"

static ss_i32 plasma_v3_abs_i32(ss_i32 value)
{
    return value < 0 ? -value : value;
}

static ss_u32 plasma_v3_wave(ss_i32 value)
{
    ss_i32 wrapped;
    ss_i32 smooth;

    wrapped = value % 1024;
    if (wrapped < 0) {
        wrapped += 1024;
    }
    if (wrapped > 511) {
        wrapped = 1023 - wrapped;
    }
    wrapped = (wrapped * 1000) / 511;
    smooth = (wrapped * wrapped * (3000 - (2 * wrapped))) / 1000000;
    return (ss_u32)smooth;
}

static ss_u32 plasma_v3_average4(ss_u32 a, ss_u32 b, ss_u32 c, ss_u32 d)
{
    return (a + b + c + d) / 4U;
}

ss_u32 plasma_v3_field_render(plasma_v3_runtime *runtime)
{
    const plasma_v3_spec *spec;
    ss_u32 width;
    ss_u32 height;
    ss_u32 x;
    ss_u32 y;
    ss_u32 index;
    ss_u32 freq;
    ss_u32 warp_term;
    ss_i32 nx;
    ss_i32 ny;
    ss_i32 center;
    ss_u32 wave0;
    ss_u32 wave1;
    ss_u32 wave2;
    ss_u32 wave3;
    ss_u32 value;

    if (runtime == 0 || runtime->field_buffer == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    spec = &runtime->resolved_plan.resolved_spec;
    width = runtime->resolved_plan.field_size.width;
    height = runtime->resolved_plan.field_size.height;
    if (width == 0U || height == 0U || runtime->field_cell_count < width * height) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    freq = 1U + (spec->detail / 300U) + (spec->field_scale / 500U);
    center = 512;
    index = 0U;
    for (y = 0U; y < height; ++y) {
        ny = height > 1U ? (ss_i32)((y * 1024U) / (height - 1U)) : 0;
        for (x = 0U; x < width; ++x) {
            nx = width > 1U ? (ss_i32)((x * 1024U) / (width - 1U)) : 0;
            warp_term = (plasma_v3_wave(ny + (ss_i32)runtime->phase1) * spec->warp) / 1000U;
            wave0 = plasma_v3_wave((nx * (ss_i32)freq) + (ss_i32)runtime->phase0 + (ss_i32)warp_term);
            wave1 = plasma_v3_wave((ny * (ss_i32)(freq + 1U)) - (ss_i32)runtime->phase1);
            wave2 = plasma_v3_wave(((nx + ny) * (ss_i32)(freq + 2U)) / 2 + (ss_i32)runtime->phase2);
            wave3 = plasma_v3_wave(
                ((plasma_v3_abs_i32(nx - center) + plasma_v3_abs_i32(ny - center)) * (ss_i32)(freq + 1U)) +
                (ss_i32)((runtime->rng_state >> 8) & 1023U)
            );
            value = plasma_v3_average4(wave0, wave1, wave2, wave3);
            value = 300U + ((value * 700U) / 1000U);
            runtime->field_buffer[index] = value;
            ++index;
        }
    }
    return SS_V2_STATUS_OK;
}
