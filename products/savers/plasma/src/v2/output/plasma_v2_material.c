#include "plasma_v2_material.h"

typedef struct plasma_v2_material_colors_tag {
    ss_u32 low_r;
    ss_u32 low_g;
    ss_u32 low_b;
    ss_u32 high_r;
    ss_u32 high_g;
    ss_u32 high_b;
} plasma_v2_material_colors;

static ss_u32 plasma_v2_material_key_equals(const char *left, const char *right)
{
    if (left == 0 || right == 0) {
        return SS_V2_FALSE;
    }
    while (*left != '\0' && *right != '\0') {
        if (*left != *right) {
            return SS_V2_FALSE;
        }
        ++left;
        ++right;
    }
    return *left == *right ? SS_V2_TRUE : SS_V2_FALSE;
}

static void plasma_v2_material_resolve(const char *key, plasma_v2_material_colors *colors)
{
    if (plasma_v2_material_key_equals(key, "aurora_cool") == SS_V2_TRUE) {
        colors->low_r = 4U;
        colors->low_g = 18U;
        colors->low_b = 42U;
        colors->high_r = 124U;
        colors->high_g = 246U;
        colors->high_b = 218U;
        return;
    }
    if (plasma_v2_material_key_equals(key, "oceanic_blue") == SS_V2_TRUE) {
        colors->low_r = 5U;
        colors->low_g = 14U;
        colors->low_b = 55U;
        colors->high_r = 92U;
        colors->high_g = 190U;
        colors->high_b = 252U;
        return;
    }
    if (plasma_v2_material_key_equals(key, "museum_phosphor") == SS_V2_TRUE) {
        colors->low_r = 3U;
        colors->low_g = 24U;
        colors->low_b = 8U;
        colors->high_r = 176U;
        colors->high_g = 255U;
        colors->high_b = 154U;
        return;
    }
    if (plasma_v2_material_key_equals(key, "quiet_darkroom") == SS_V2_TRUE) {
        colors->low_r = 10U;
        colors->low_g = 7U;
        colors->low_b = 6U;
        colors->high_r = 126U;
        colors->high_g = 64U;
        colors->high_b = 42U;
        return;
    }
    colors->low_r = 16U;
    colors->low_g = 3U;
    colors->low_b = 1U;
    colors->high_r = 255U;
    colors->high_g = 166U;
    colors->high_b = 44U;
}

static ss_u32 plasma_v2_material_clamp_byte(ss_i32 value)
{
    if (value < 0) {
        return 0U;
    }
    if (value > 255) {
        return 255U;
    }
    return (ss_u32)value;
}

static ss_u32 plasma_v2_material_channel(ss_u32 low, ss_u32 high, ss_u32 field_value, const plasma_v2_spec *spec)
{
    ss_i32 value;

    value = (ss_i32)low + (((ss_i32)high - (ss_i32)low) * (ss_i32)field_value) / 1000;
    value = (value * ((ss_i32)spec->brightness + 500)) / 1000;
    value = 128 + (((value - 128) * ((ss_i32)spec->contrast + 500)) / 1000);
    return plasma_v2_material_clamp_byte(value);
}

ss_u32 plasma_v2_material_map(plasma_v2_runtime *runtime)
{
    plasma_v2_material_colors colors;
    ss_u32 index;
    ss_u32 field_value;
    ss_u32 offset;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (runtime->material_byte_count < runtime->field_cell_count * 4U) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    plasma_v2_material_resolve(runtime->resolved_plan.resolved_spec.material_key, &colors);
    for (index = 0U; index < runtime->field_cell_count; ++index) {
        field_value = runtime->field_b[index];
        offset = index * 4U;
        runtime->material_buffer[offset + 0U] = (ss_u8)plasma_v2_material_channel(colors.low_r, colors.high_r, field_value, &runtime->resolved_plan.resolved_spec);
        runtime->material_buffer[offset + 1U] = (ss_u8)plasma_v2_material_channel(colors.low_g, colors.high_g, field_value, &runtime->resolved_plan.resolved_spec);
        runtime->material_buffer[offset + 2U] = (ss_u8)plasma_v2_material_channel(colors.low_b, colors.high_b, field_value, &runtime->resolved_plan.resolved_spec);
        runtime->material_buffer[offset + 3U] = 255U;
    }
    return SS_V2_STATUS_OK;
}
