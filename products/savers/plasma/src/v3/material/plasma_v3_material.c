#include "plasma_v3_material.h"

typedef struct plasma_v3_color_ramp_tag {
    ss_u32 low_r;
    ss_u32 low_g;
    ss_u32 low_b;
    ss_u32 mid_r;
    ss_u32 mid_g;
    ss_u32 mid_b;
    ss_u32 high_r;
    ss_u32 high_g;
    ss_u32 high_b;
} plasma_v3_color_ramp;

static void plasma_v3_resolve_ramp(ss_u32 material_kind, plasma_v3_color_ramp *ramp)
{
    if (material_kind == PLASMA_V3_MATERIAL_COOL_SIGNAL) {
        ramp->low_r = 8U;
        ramp->low_g = 28U;
        ramp->low_b = 60U;
        ramp->mid_r = 44U;
        ramp->mid_g = 132U;
        ramp->mid_b = 188U;
        ramp->high_r = 186U;
        ramp->high_g = 242U;
        ramp->high_b = 214U;
        return;
    }
    if (material_kind == PLASMA_V3_MATERIAL_GREEN_PHOSPHOR) {
        ramp->low_r = 6U;
        ramp->low_g = 24U;
        ramp->low_b = 10U;
        ramp->mid_r = 42U;
        ramp->mid_g = 148U;
        ramp->mid_b = 52U;
        ramp->high_r = 190U;
        ramp->high_g = 255U;
        ramp->high_b = 170U;
        return;
    }
    ramp->low_r = 72U;
    ramp->low_g = 32U;
    ramp->low_b = 76U;
    ramp->mid_r = 144U;
    ramp->mid_g = 70U;
    ramp->mid_b = 154U;
    ramp->high_r = 255U;
    ramp->high_g = 196U;
    ramp->high_b = 92U;
}

static ss_u32 plasma_v3_clamp_byte(ss_i32 value)
{
    if (value < 0) {
        return 0U;
    }
    if (value > 255) {
        return 255U;
    }
    return (ss_u32)value;
}

static ss_u32 plasma_v3_lerp(ss_u32 left, ss_u32 right, ss_u32 amount)
{
    return (ss_u32)((ss_i32)left + (((ss_i32)right - (ss_i32)left) * (ss_i32)amount) / 1000);
}

static ss_u32 plasma_v3_curve_channel(ss_u32 low, ss_u32 mid, ss_u32 high, ss_u32 field_value, const plasma_v3_spec *spec)
{
    ss_u32 curved;
    ss_u32 channel;
    ss_i32 adjusted;

    curved = (field_value + spec->palette_shift) % 1001U;
    if (curved <= 500U) {
        channel = plasma_v3_lerp(low, mid, curved * 2U);
    } else {
        channel = plasma_v3_lerp(mid, high, (curved - 500U) * 2U);
    }
    adjusted = (ss_i32)channel;
    adjusted = (adjusted * ((ss_i32)spec->brightness + 500)) / 1000;
    adjusted = 128 + (((adjusted - 128) * ((ss_i32)spec->contrast + 500)) / 1000);
    return plasma_v3_clamp_byte(adjusted);
}

ss_u32 plasma_v3_material_map(plasma_v3_runtime *runtime)
{
    plasma_v3_color_ramp ramp;
    const plasma_v3_spec *spec;
    ss_u32 index;
    ss_u32 offset;
    ss_u32 field_value;

    if (runtime == 0 || runtime->field_buffer == 0 || runtime->material_buffer == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (runtime->material_byte_count < runtime->field_cell_count * PLASMA_V3_PRESENT_PIXEL_BYTES) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    spec = &runtime->resolved_plan.resolved_spec;
    plasma_v3_resolve_ramp(spec->material_kind, &ramp);
    for (index = 0U; index < runtime->field_cell_count; ++index) {
        field_value = runtime->field_buffer[index];
        offset = index * PLASMA_V3_PRESENT_PIXEL_BYTES;
        runtime->material_buffer[offset + 0U] = (ss_u8)plasma_v3_curve_channel(ramp.low_r, ramp.mid_r, ramp.high_r, field_value, spec);
        runtime->material_buffer[offset + 1U] = (ss_u8)plasma_v3_curve_channel(ramp.low_g, ramp.mid_g, ramp.high_g, field_value, spec);
        runtime->material_buffer[offset + 2U] = (ss_u8)plasma_v3_curve_channel(ramp.low_b, ramp.mid_b, ramp.high_b, field_value, spec);
        runtime->material_buffer[offset + 3U] = 255U;
    }
    return SS_V2_STATUS_OK;
}
