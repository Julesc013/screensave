#include "plasma_v3_treatment.h"

static ss_u8 plasma_v3_mix_channel(ss_u8 value, ss_u8 neighbor, ss_u32 amount)
{
    ss_u32 mixed;

    mixed = ((ss_u32)value * (1000U - amount)) + ((ss_u32)neighbor * amount);
    return (ss_u8)(mixed / 1000U);
}

static ss_u8 plasma_v3_phosphor_channel(ss_u8 value, ss_u32 phosphor, ss_u32 green_bias)
{
    ss_u32 lifted;

    lifted = (ss_u32)value;
    lifted = (lifted * (1000U - (phosphor / 3U))) / 1000U;
    lifted += (green_bias * phosphor) / 1000U;
    if (lifted > 255U) {
        lifted = 255U;
    }
    return (ss_u8)lifted;
}

ss_u32 plasma_v3_treatment_apply(plasma_v3_runtime *runtime)
{
    const plasma_v3_spec *spec;
    ss_u32 width;
    ss_u32 height;
    ss_u32 x;
    ss_u32 y;
    ss_u32 offset;
    ss_u32 left_offset;
    ss_u32 soft;
    ss_u8 red;
    ss_u8 green;
    ss_u8 blue;

    if (runtime == 0 || runtime->material_buffer == 0 || runtime->treatment_buffer == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (runtime->treatment_byte_count < runtime->material_byte_count) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    spec = &runtime->resolved_plan.resolved_spec;
    width = runtime->resolved_plan.output_size.width;
    height = runtime->resolved_plan.output_size.height;
    soft = spec->softness / 3U;
    for (y = 0U; y < height; ++y) {
        for (x = 0U; x < width; ++x) {
            offset = ((y * width) + x) * PLASMA_V3_PRESENT_PIXEL_BYTES;
            left_offset = x == 0U ? offset : offset - PLASMA_V3_PRESENT_PIXEL_BYTES;
            red = plasma_v3_mix_channel(runtime->material_buffer[offset + 0U], runtime->material_buffer[left_offset + 0U], soft);
            green = plasma_v3_mix_channel(runtime->material_buffer[offset + 1U], runtime->material_buffer[left_offset + 1U], soft);
            blue = plasma_v3_mix_channel(runtime->material_buffer[offset + 2U], runtime->material_buffer[left_offset + 2U], soft);
            runtime->treatment_buffer[offset + 0U] = plasma_v3_phosphor_channel(red, spec->phosphor, 12U);
            runtime->treatment_buffer[offset + 1U] = plasma_v3_phosphor_channel(green, spec->phosphor, 56U);
            runtime->treatment_buffer[offset + 2U] = plasma_v3_phosphor_channel(blue, spec->phosphor, 18U);
            runtime->treatment_buffer[offset + 3U] = 255U;
        }
    }
    return SS_V2_STATUS_OK;
}
