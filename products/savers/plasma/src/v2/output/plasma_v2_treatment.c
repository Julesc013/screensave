#include "plasma_v2_treatment.h"

static ss_u8 plasma_v2_treatment_soften(ss_u8 value, ss_u32 softness)
{
    ss_u32 mixed;

    mixed = ((ss_u32)value * (1000U - softness)) + (128U * softness);
    return (ss_u8)(mixed / 1000U);
}

ss_u32 plasma_v2_treatment_apply(plasma_v2_runtime *runtime)
{
    ss_u32 index;
    ss_u32 offset;
    ss_u32 treatment;
    ss_u32 softness;
    ss_u8 red;
    ss_u8 green;
    ss_u8 blue;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (runtime->treatment_byte_count < runtime->material_byte_count) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    treatment = runtime->resolved_plan.resolved_spec.treatment_kind;
    softness = runtime->resolved_plan.resolved_spec.softness;
    for (index = 0U; index < runtime->field_cell_count; ++index) {
        offset = index * 4U;
        red = runtime->material_buffer[offset + 0U];
        green = runtime->material_buffer[offset + 1U];
        blue = runtime->material_buffer[offset + 2U];
        if (treatment == PLASMA_V2_TREATMENT_SOFT) {
            red = plasma_v2_treatment_soften(red, softness);
            green = plasma_v2_treatment_soften(green, softness);
            blue = plasma_v2_treatment_soften(blue, softness);
        } else if (treatment == PLASMA_V2_TREATMENT_PHOSPHOR) {
            red = (ss_u8)(((ss_u32)red * 80U) / 100U);
            green = (ss_u8)(((ss_u32)green * 112U) / 100U);
            blue = (ss_u8)(((ss_u32)blue * 76U) / 100U);
        }
        runtime->treatment_buffer[offset + 0U] = red;
        runtime->treatment_buffer[offset + 1U] = green;
        runtime->treatment_buffer[offset + 2U] = blue;
        runtime->treatment_buffer[offset + 3U] = 255U;
    }
    return SS_V2_STATUS_OK;
}
