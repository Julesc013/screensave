#include "plasma_v3_spec.h"

static ss_u32 plasma_v3_text_equals(const char *left, const char *right)
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

static void plasma_v3_copy_key(char *dst, ss_u32 dst_size, const char *src)
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

static ss_u32 plasma_v3_clamp_control(ss_u32 value)
{
    if (value > PLASMA_V3_CONTROL_MAX) {
        return PLASMA_V3_CONTROL_MAX;
    }
    return value;
}

static ss_u32 plasma_v3_key_is_valid(const char *text)
{
    ss_u32 index;

    if (text == 0 || text[0] == '\0') {
        return SS_V2_FALSE;
    }
    index = 0U;
    while (text[index] != '\0') {
        if (index + 1U >= PLASMA_V3_KEY_LENGTH) {
            return SS_V2_FALSE;
        }
        ++index;
    }
    return SS_V2_TRUE;
}

const char *plasma_v3_spec_schema_id(void)
{
    return PLASMA_V3_SCHEMA_ID;
}

void plasma_v3_spec_set_defaults(plasma_v3_spec *spec)
{
    if (spec == 0) {
        return;
    }
    spec->struct_size = (ss_u32)sizeof(*spec);
    spec->schema_version = PLASMA_V3_SCHEMA_VERSION;
    spec->material_kind = PLASMA_V3_MATERIAL_THERMAL_RIBBON;
    spec->field_scale = (ss_u32)460U;
    spec->detail = (ss_u32)360U;
    spec->motion = (ss_u32)220U;
    spec->warp = (ss_u32)260U;
    spec->palette_shift = (ss_u32)160U;
    spec->contrast = (ss_u32)240U;
    spec->brightness = (ss_u32)680U;
    spec->softness = (ss_u32)650U;
    spec->phosphor = (ss_u32)80U;
    spec->seed = (ss_u32)314159U;
    spec->deterministic = SS_V2_TRUE;
    plasma_v3_copy_key(spec->material_key, PLASMA_V3_KEY_LENGTH, "thermal_ribbon");
    plasma_v3_copy_key(spec->profile_key, PLASMA_V3_KEY_LENGTH, "plasma.v3.visual-spike");
}

void plasma_v3_spec_clamp(plasma_v3_spec *spec)
{
    if (spec == 0) {
        return;
    }
    spec->struct_size = (ss_u32)sizeof(*spec);
    spec->schema_version = PLASMA_V3_SCHEMA_VERSION;
    spec->field_scale = plasma_v3_clamp_control(spec->field_scale);
    spec->detail = plasma_v3_clamp_control(spec->detail);
    spec->motion = plasma_v3_clamp_control(spec->motion);
    spec->warp = plasma_v3_clamp_control(spec->warp);
    spec->palette_shift = plasma_v3_clamp_control(spec->palette_shift);
    spec->contrast = plasma_v3_clamp_control(spec->contrast);
    spec->brightness = plasma_v3_clamp_control(spec->brightness);
    spec->softness = plasma_v3_clamp_control(spec->softness);
    spec->phosphor = plasma_v3_clamp_control(spec->phosphor);
    if (plasma_v3_material_token(spec->material_kind) == 0) {
        spec->material_kind = PLASMA_V3_MATERIAL_THERMAL_RIBBON;
    }
    if (spec->deterministic != SS_V2_FALSE) {
        spec->deterministic = SS_V2_TRUE;
    }
    if (plasma_v3_key_is_valid(spec->material_key) == SS_V2_FALSE) {
        plasma_v3_copy_key(spec->material_key, PLASMA_V3_KEY_LENGTH, plasma_v3_material_token(spec->material_kind));
    }
    if (plasma_v3_key_is_valid(spec->profile_key) == SS_V2_FALSE) {
        plasma_v3_copy_key(spec->profile_key, PLASMA_V3_KEY_LENGTH, "plasma.v3.visual-spike");
    }
}

ss_u32 plasma_v3_spec_is_valid(const plasma_v3_spec *spec)
{
    if (spec == 0) {
        return SS_V2_FALSE;
    }
    if (spec->struct_size < (ss_u32)sizeof(*spec)) {
        return SS_V2_FALSE;
    }
    if (spec->schema_version != PLASMA_V3_SCHEMA_VERSION) {
        return SS_V2_FALSE;
    }
    if (plasma_v3_material_token(spec->material_kind) == 0) {
        return SS_V2_FALSE;
    }
    if (
        spec->field_scale > PLASMA_V3_CONTROL_MAX ||
        spec->detail > PLASMA_V3_CONTROL_MAX ||
        spec->motion > PLASMA_V3_CONTROL_MAX ||
        spec->warp > PLASMA_V3_CONTROL_MAX ||
        spec->palette_shift > PLASMA_V3_CONTROL_MAX ||
        spec->contrast > PLASMA_V3_CONTROL_MAX ||
        spec->brightness > PLASMA_V3_CONTROL_MAX ||
        spec->softness > PLASMA_V3_CONTROL_MAX ||
        spec->phosphor > PLASMA_V3_CONTROL_MAX
    ) {
        return SS_V2_FALSE;
    }
    if (spec->deterministic != SS_V2_FALSE && spec->deterministic != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plasma_v3_key_is_valid(spec->material_key) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v3_key_is_valid(spec->profile_key) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    return SS_V2_TRUE;
}

ss_u32 plasma_v3_spec_apply_control(plasma_v3_spec *spec, ss_u32 control_id, ss_u32 value)
{
    if (spec == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (control_id == PLASMA_V3_CONTROL_FIELD_SCALE) {
        spec->field_scale = value;
    } else if (control_id == PLASMA_V3_CONTROL_DETAIL) {
        spec->detail = value;
    } else if (control_id == PLASMA_V3_CONTROL_MOTION) {
        spec->motion = value;
    } else if (control_id == PLASMA_V3_CONTROL_WARP) {
        spec->warp = value;
    } else if (control_id == PLASMA_V3_CONTROL_PALETTE_SHIFT) {
        spec->palette_shift = value;
    } else if (control_id == PLASMA_V3_CONTROL_CONTRAST) {
        spec->contrast = value;
    } else if (control_id == PLASMA_V3_CONTROL_BRIGHTNESS) {
        spec->brightness = value;
    } else if (control_id == PLASMA_V3_CONTROL_SOFTNESS) {
        spec->softness = value;
    } else if (control_id == PLASMA_V3_CONTROL_PHOSPHOR) {
        spec->phosphor = value;
    } else {
        return SS_V2_STATUS_UNSUPPORTED;
    }
    plasma_v3_spec_clamp(spec);
    return plasma_v3_spec_is_valid(spec) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_FAIL;
}

ss_u32 plasma_v3_spec_set_material_key(plasma_v3_spec *spec, const char *material_key)
{
    if (spec == 0 || material_key == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_text_equals(material_key, "thermal_ribbon") == SS_V2_TRUE) {
        spec->material_kind = PLASMA_V3_MATERIAL_THERMAL_RIBBON;
    } else if (plasma_v3_text_equals(material_key, "cool_signal") == SS_V2_TRUE) {
        spec->material_kind = PLASMA_V3_MATERIAL_COOL_SIGNAL;
    } else if (plasma_v3_text_equals(material_key, "green_phosphor") == SS_V2_TRUE) {
        spec->material_kind = PLASMA_V3_MATERIAL_GREEN_PHOSPHOR;
    } else {
        return SS_V2_STATUS_UNSUPPORTED;
    }
    plasma_v3_copy_key(spec->material_key, PLASMA_V3_KEY_LENGTH, material_key);
    return SS_V2_STATUS_OK;
}

const char *plasma_v3_control_token(ss_u32 control_id)
{
    if (control_id == PLASMA_V3_CONTROL_FIELD_SCALE) {
        return "field_scale";
    }
    if (control_id == PLASMA_V3_CONTROL_DETAIL) {
        return "detail";
    }
    if (control_id == PLASMA_V3_CONTROL_MOTION) {
        return "motion";
    }
    if (control_id == PLASMA_V3_CONTROL_WARP) {
        return "warp";
    }
    if (control_id == PLASMA_V3_CONTROL_PALETTE_SHIFT) {
        return "palette_shift";
    }
    if (control_id == PLASMA_V3_CONTROL_CONTRAST) {
        return "contrast";
    }
    if (control_id == PLASMA_V3_CONTROL_BRIGHTNESS) {
        return "brightness";
    }
    if (control_id == PLASMA_V3_CONTROL_SOFTNESS) {
        return "softness";
    }
    if (control_id == PLASMA_V3_CONTROL_PHOSPHOR) {
        return "phosphor";
    }
    return 0;
}

const char *plasma_v3_material_token(ss_u32 material_kind)
{
    if (material_kind == PLASMA_V3_MATERIAL_THERMAL_RIBBON) {
        return "thermal_ribbon";
    }
    if (material_kind == PLASMA_V3_MATERIAL_COOL_SIGNAL) {
        return "cool_signal";
    }
    if (material_kind == PLASMA_V3_MATERIAL_GREEN_PHOSPHOR) {
        return "green_phosphor";
    }
    return 0;
}

ss_u32 plasma_v3_control_from_token(const char *token, ss_u32 *control_id_out)
{
    ss_u32 control_id;

    if (token == 0 || control_id_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    for (control_id = 0U; control_id <= PLASMA_V3_CONTROL_PHOSPHOR; ++control_id) {
        if (plasma_v3_text_equals(token, plasma_v3_control_token(control_id)) == SS_V2_TRUE) {
            *control_id_out = control_id;
            return SS_V2_STATUS_OK;
        }
    }
    return SS_V2_STATUS_UNSUPPORTED;
}

ss_u32 plasma_v3_renderer_from_token(const char *token, ss_u32 *renderer_out)
{
    if (token == 0 || renderer_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v3_text_equals(token, "software") == SS_V2_TRUE) {
        *renderer_out = PLASMA_V3_RENDERER_SOFTWARE;
        return SS_V2_STATUS_OK;
    }
    if (plasma_v3_text_equals(token, "gdi") == SS_V2_TRUE) {
        *renderer_out = PLASMA_V3_RENDERER_GDI;
        return SS_V2_STATUS_OK;
    }
    if (plasma_v3_text_equals(token, "gl11") == SS_V2_TRUE) {
        *renderer_out = PLASMA_V3_RENDERER_GL11;
        return SS_V2_STATUS_OK;
    }
    return SS_V2_STATUS_UNSUPPORTED;
}

const char *plasma_v3_renderer_token(ss_u32 renderer)
{
    if (renderer == PLASMA_V3_RENDERER_SOFTWARE) {
        return "software";
    }
    if (renderer == PLASMA_V3_RENDERER_GDI) {
        return "gdi";
    }
    if (renderer == PLASMA_V3_RENDERER_GL11) {
        return "gl11";
    }
    return 0;
}
