#include "plasma_v2_spec.h"

static ss_u32 plasma_v2_token_equals(const char *left, const char *right)
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

static void plasma_v2_copy_key(char *dst, ss_u32 dst_size, const char *src)
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

static ss_u32 plasma_v2_is_normalized(ss_u32 value)
{
    return value <= PLASMA_V2_CONTROL_MAX ? SS_V2_TRUE : SS_V2_FALSE;
}

static ss_u32 plasma_v2_clamp_normalized(ss_u32 value)
{
    if (value > PLASMA_V2_CONTROL_MAX) {
        return PLASMA_V2_CONTROL_MAX;
    }
    return value;
}

static ss_u32 plasma_v2_key_is_valid(const char *text)
{
    ss_u32 index;

    if (text == 0 || text[0] == '\0') {
        return SS_V2_FALSE;
    }

    index = 0U;
    while (text[index] != '\0') {
        if (index + 1U >= PLASMA_V2_KEY_LENGTH) {
            return SS_V2_FALSE;
        }
        ++index;
    }

    return SS_V2_TRUE;
}

const char *plasma_v2_spec_schema_id(void)
{
    return PLASMA_V2_INSTRUMENT_SCHEMA_ID;
}

void plasma_v2_spec_set_defaults(plasma_v2_spec *spec)
{
    if (spec == 0) {
        return;
    }

    spec->struct_size = (ss_u32)sizeof(*spec);
    spec->schema_version = PLASMA_V2_INSTRUMENT_SCHEMA_VERSION;
    spec->field_family = PLASMA_V2_FIELD_CLASSIC;
    spec->motion_kind = PLASMA_V2_MOTION_FLOW;
    spec->output_kind = PLASMA_V2_OUTPUT_CONTINUOUS;
    spec->treatment_kind = PLASMA_V2_TREATMENT_SOFT;
    spec->quality_intent = PLASMA_V2_QUALITY_SAFE;
    spec->scale = (ss_u32)420U;
    spec->complexity = (ss_u32)520U;
    spec->speed = (ss_u32)280U;
    spec->warp = (ss_u32)180U;
    spec->feedback = (ss_u32)120U;
    spec->contrast = (ss_u32)640U;
    spec->brightness = (ss_u32)520U;
    spec->softness = (ss_u32)220U;
    spec->seed = (ss_u32)104729U;
    spec->deterministic = SS_V2_TRUE;
    plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "plasma_lava");
    plasma_v2_copy_key(spec->profile_key, PLASMA_V2_KEY_LENGTH, "plasma.v2.instrument.preview");
}

void plasma_v2_spec_clamp(plasma_v2_spec *spec)
{
    if (spec == 0) {
        return;
    }

    spec->struct_size = (ss_u32)sizeof(*spec);
    spec->schema_version = PLASMA_V2_INSTRUMENT_SCHEMA_VERSION;
    spec->scale = plasma_v2_clamp_normalized(spec->scale);
    spec->complexity = plasma_v2_clamp_normalized(spec->complexity);
    spec->speed = plasma_v2_clamp_normalized(spec->speed);
    spec->warp = plasma_v2_clamp_normalized(spec->warp);
    spec->feedback = plasma_v2_clamp_normalized(spec->feedback);
    spec->contrast = plasma_v2_clamp_normalized(spec->contrast);
    spec->brightness = plasma_v2_clamp_normalized(spec->brightness);
    spec->softness = plasma_v2_clamp_normalized(spec->softness);

    if (plasma_v2_field_family_token(spec->field_family) == 0) {
        spec->field_family = PLASMA_V2_FIELD_CLASSIC;
    }
    if (plasma_v2_motion_kind_token(spec->motion_kind) == 0) {
        spec->motion_kind = PLASMA_V2_MOTION_FLOW;
    }
    if (plasma_v2_output_kind_token(spec->output_kind) == 0) {
        spec->output_kind = PLASMA_V2_OUTPUT_CONTINUOUS;
    }
    if (plasma_v2_treatment_kind_token(spec->treatment_kind) == 0) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_SOFT;
    }
    if (plasma_v2_quality_intent_token(spec->quality_intent) == 0) {
        spec->quality_intent = PLASMA_V2_QUALITY_SAFE;
    }
    if (spec->deterministic != SS_V2_FALSE) {
        spec->deterministic = SS_V2_TRUE;
    }
    if (plasma_v2_key_is_valid(spec->material_key) == SS_V2_FALSE) {
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "plasma_lava");
    }
    if (plasma_v2_key_is_valid(spec->profile_key) == SS_V2_FALSE) {
        plasma_v2_copy_key(spec->profile_key, PLASMA_V2_KEY_LENGTH, "plasma.v2.instrument.preview");
    }
}

ss_u32 plasma_v2_spec_is_valid(const plasma_v2_spec *spec)
{
    if (spec == 0) {
        return SS_V2_FALSE;
    }
    if (spec->struct_size < (ss_u32)sizeof(*spec)) {
        return SS_V2_FALSE;
    }
    if (spec->schema_version != PLASMA_V2_INSTRUMENT_SCHEMA_VERSION) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_field_family_token(spec->field_family) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_motion_kind_token(spec->motion_kind) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_output_kind_token(spec->output_kind) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_treatment_kind_token(spec->treatment_kind) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_quality_intent_token(spec->quality_intent) == 0) {
        return SS_V2_FALSE;
    }
    if (
        plasma_v2_is_normalized(spec->scale) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->complexity) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->speed) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->warp) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->feedback) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->contrast) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->brightness) == SS_V2_FALSE ||
        plasma_v2_is_normalized(spec->softness) == SS_V2_FALSE
    ) {
        return SS_V2_FALSE;
    }
    if (spec->deterministic != SS_V2_FALSE && spec->deterministic != SS_V2_TRUE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_key_is_valid(spec->material_key) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_key_is_valid(spec->profile_key) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }

    return SS_V2_TRUE;
}

ss_u32 plasma_v2_spec_apply_control(plasma_v2_spec *spec, ss_u32 control_id, ss_u32 value)
{
    if (spec == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    if (control_id == PLASMA_V2_CONTROL_FIELD) {
        if (plasma_v2_field_family_token(value) == 0) {
            return SS_V2_STATUS_UNSUPPORTED;
        }
        spec->field_family = value;
    } else if (control_id == PLASMA_V2_CONTROL_SCALE) {
        spec->scale = value;
    } else if (control_id == PLASMA_V2_CONTROL_COMPLEXITY) {
        spec->complexity = value;
    } else if (control_id == PLASMA_V2_CONTROL_MOTION) {
        if (plasma_v2_motion_kind_token(value) == 0) {
            return SS_V2_STATUS_UNSUPPORTED;
        }
        spec->motion_kind = value;
    } else if (control_id == PLASMA_V2_CONTROL_SPEED) {
        spec->speed = value;
    } else if (control_id == PLASMA_V2_CONTROL_WARP) {
        spec->warp = value;
    } else if (control_id == PLASMA_V2_CONTROL_FEEDBACK) {
        spec->feedback = value;
    } else if (control_id == PLASMA_V2_CONTROL_OUTPUT) {
        if (plasma_v2_output_kind_token(value) == 0) {
            return SS_V2_STATUS_UNSUPPORTED;
        }
        spec->output_kind = value;
    } else if (control_id == PLASMA_V2_CONTROL_CONTRAST) {
        spec->contrast = value;
    } else if (control_id == PLASMA_V2_CONTROL_BRIGHTNESS) {
        spec->brightness = value;
    } else if (control_id == PLASMA_V2_CONTROL_SOFTNESS) {
        spec->softness = value;
    } else if (control_id == PLASMA_V2_CONTROL_TREATMENT) {
        if (plasma_v2_treatment_kind_token(value) == 0) {
            return SS_V2_STATUS_UNSUPPORTED;
        }
        spec->treatment_kind = value;
    } else if (control_id == PLASMA_V2_CONTROL_QUALITY) {
        if (plasma_v2_quality_intent_token(value) == 0) {
            return SS_V2_STATUS_UNSUPPORTED;
        }
        spec->quality_intent = value;
    } else {
        return SS_V2_STATUS_UNSUPPORTED;
    }

    plasma_v2_spec_clamp(spec);
    return plasma_v2_spec_is_valid(spec) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_BAD_ARGUMENT;
}

ss_u32 plasma_v2_spec_set_material_key(plasma_v2_spec *spec, const char *material_key)
{
    if (spec == 0 || plasma_v2_key_is_valid(material_key) == SS_V2_FALSE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, material_key);
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_spec_set_profile_key(plasma_v2_spec *spec, const char *profile_key)
{
    if (spec == 0 || plasma_v2_key_is_valid(profile_key) == SS_V2_FALSE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    plasma_v2_copy_key(spec->profile_key, PLASMA_V2_KEY_LENGTH, profile_key);
    return SS_V2_STATUS_OK;
}

static void plasma_v2_apply_legacy_effect(ss_i32 effect_mode, plasma_v2_spec *spec)
{
    if (effect_mode == (ss_i32)1) {
        spec->field_family = PLASMA_V2_FIELD_FIRE;
        spec->motion_kind = PLASMA_V2_MOTION_PULSE;
        spec->warp = (ss_u32)140U;
        spec->feedback = (ss_u32)180U;
    } else if (effect_mode == (ss_i32)2 || effect_mode == (ss_i32)5) {
        spec->field_family = PLASMA_V2_FIELD_INTERFERENCE;
        spec->motion_kind = PLASMA_V2_MOTION_FLOW;
        spec->warp = (ss_u32)260U;
    } else if (effect_mode == (ss_i32)4) {
        spec->field_family = PLASMA_V2_FIELD_LATTICE;
        spec->motion_kind = PLASMA_V2_MOTION_DRIFT;
        spec->complexity = (ss_u32)620U;
    } else if (effect_mode == (ss_i32)6) {
        spec->field_family = PLASMA_V2_FIELD_AURORA;
        spec->motion_kind = PLASMA_V2_MOTION_SWIRL;
        spec->scale = (ss_u32)560U;
        spec->warp = (ss_u32)320U;
    } else if (effect_mode == (ss_i32)3 || effect_mode == (ss_i32)7) {
        spec->field_family = PLASMA_V2_FIELD_CURL;
        spec->motion_kind = PLASMA_V2_MOTION_FLOW;
        spec->complexity = (ss_u32)680U;
    } else {
        spec->field_family = PLASMA_V2_FIELD_CLASSIC;
    }
}

static void plasma_v2_apply_legacy_speed(ss_i32 speed_mode, plasma_v2_spec *spec)
{
    if (speed_mode == (ss_i32)0) {
        spec->speed = (ss_u32)220U;
    } else if (speed_mode == (ss_i32)2) {
        spec->speed = (ss_u32)700U;
    } else {
        spec->speed = (ss_u32)420U;
    }
}

static void plasma_v2_apply_legacy_resolution(ss_i32 resolution_mode, plasma_v2_spec *spec)
{
    if (resolution_mode == (ss_i32)0) {
        spec->scale = (ss_u32)680U;
        spec->complexity = (ss_u32)360U;
        spec->softness = (ss_u32)340U;
    } else if (resolution_mode == (ss_i32)2) {
        spec->scale = (ss_u32)320U;
        spec->complexity = (ss_u32)640U;
        spec->softness = (ss_u32)120U;
    }
}

static void plasma_v2_apply_legacy_smoothing(ss_i32 smoothing_mode, plasma_v2_spec *spec)
{
    if (smoothing_mode == (ss_i32)0) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_NONE;
        spec->softness = (ss_u32)0U;
    } else if (smoothing_mode == (ss_i32)2) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_PHOSPHOR;
        spec->softness = (ss_u32)560U;
    } else {
        spec->treatment_kind = PLASMA_V2_TREATMENT_SOFT;
        if (spec->softness < (ss_u32)220U) {
            spec->softness = (ss_u32)220U;
        }
    }
}

static void plasma_v2_apply_legacy_preset(const char *preset_key, plasma_v2_spec *spec)
{
    if (plasma_v2_token_equals(preset_key, "aurora_plasma") == SS_V2_TRUE) {
        spec->field_family = PLASMA_V2_FIELD_AURORA;
        spec->motion_kind = PLASMA_V2_MOTION_SWIRL;
        spec->scale = (ss_u32)560U;
        spec->complexity = (ss_u32)600U;
        spec->warp = (ss_u32)320U;
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "aurora_cool");
    } else if (plasma_v2_token_equals(preset_key, "ocean_interference") == SS_V2_TRUE) {
        spec->field_family = PLASMA_V2_FIELD_INTERFERENCE;
        spec->motion_kind = PLASMA_V2_MOTION_FLOW;
        spec->scale = (ss_u32)360U;
        spec->complexity = (ss_u32)700U;
        spec->warp = (ss_u32)280U;
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "oceanic_blue");
    } else if (plasma_v2_token_equals(preset_key, "museum_phosphor") == SS_V2_TRUE) {
        spec->field_family = PLASMA_V2_FIELD_INTERFERENCE;
        spec->motion_kind = PLASMA_V2_MOTION_DRIFT;
        spec->speed = (ss_u32)220U;
        spec->treatment_kind = PLASMA_V2_TREATMENT_PHOSPHOR;
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "museum_phosphor");
    } else if (plasma_v2_token_equals(preset_key, "quiet_darkroom") == SS_V2_TRUE) {
        spec->field_family = PLASMA_V2_FIELD_FIRE;
        spec->motion_kind = PLASMA_V2_MOTION_DRIFT;
        spec->speed = (ss_u32)180U;
        spec->brightness = (ss_u32)320U;
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "quiet_darkroom");
    } else if (
        plasma_v2_token_equals(preset_key, "plasma_lava") == SS_V2_TRUE ||
        plasma_v2_token_equals(preset_key, "ember_lava") == SS_V2_TRUE
    ) {
        spec->field_family = PLASMA_V2_FIELD_CLASSIC;
        spec->motion_kind = PLASMA_V2_MOTION_FLOW;
        spec->scale = (ss_u32)420U;
        spec->complexity = (ss_u32)520U;
        spec->warp = (ss_u32)180U;
        spec->feedback = (ss_u32)120U;
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, "plasma_lava");
    }
}

static void plasma_v2_apply_legacy_theme(const char *theme_key, plasma_v2_spec *spec)
{
    if (
        plasma_v2_token_equals(theme_key, "plasma_lava") == SS_V2_TRUE ||
        plasma_v2_token_equals(theme_key, "aurora_cool") == SS_V2_TRUE ||
        plasma_v2_token_equals(theme_key, "oceanic_blue") == SS_V2_TRUE ||
        plasma_v2_token_equals(theme_key, "museum_phosphor") == SS_V2_TRUE ||
        plasma_v2_token_equals(theme_key, "quiet_darkroom") == SS_V2_TRUE
    ) {
        plasma_v2_copy_key(spec->material_key, PLASMA_V2_KEY_LENGTH, theme_key);
    }
    if (plasma_v2_token_equals(theme_key, "museum_phosphor") == SS_V2_TRUE) {
        spec->treatment_kind = PLASMA_V2_TREATMENT_PHOSPHOR;
    }
    if (plasma_v2_token_equals(theme_key, "quiet_darkroom") == SS_V2_TRUE) {
        spec->brightness = (ss_u32)320U;
    }
}

ss_u32 plasma_v2_spec_from_legacy_config_view(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_spec *spec_out
)
{
    if (legacy == 0 || spec_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (legacy->struct_size < (ss_u32)sizeof(*legacy)) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    plasma_v2_spec_set_defaults(spec_out);
    plasma_v2_apply_legacy_effect(legacy->effect_mode, spec_out);
    plasma_v2_apply_legacy_speed(legacy->speed_mode, spec_out);
    plasma_v2_apply_legacy_resolution(legacy->resolution_mode, spec_out);
    plasma_v2_apply_legacy_smoothing(legacy->smoothing_mode, spec_out);
    plasma_v2_apply_legacy_preset(legacy->preset_key, spec_out);
    plasma_v2_apply_legacy_theme(legacy->theme_key, spec_out);
    spec_out->seed = legacy->seed;
    spec_out->deterministic = legacy->deterministic == SS_V2_FALSE ? SS_V2_FALSE : SS_V2_TRUE;
    plasma_v2_spec_clamp(spec_out);

    return plasma_v2_spec_is_valid(spec_out) == SS_V2_TRUE ? SS_V2_STATUS_OK : SS_V2_STATUS_FAIL;
}

const char *plasma_v2_field_family_token(ss_u32 value)
{
    if (value == PLASMA_V2_FIELD_CLASSIC) {
        return "classic";
    }
    if (value == PLASMA_V2_FIELD_INTERFERENCE) {
        return "interference";
    }
    if (value == PLASMA_V2_FIELD_FIRE) {
        return "fire";
    }
    if (value == PLASMA_V2_FIELD_AURORA) {
        return "aurora";
    }
    if (value == PLASMA_V2_FIELD_CURL) {
        return "curl";
    }
    if (value == PLASMA_V2_FIELD_LATTICE) {
        return "lattice";
    }
    return 0;
}

const char *plasma_v2_motion_kind_token(ss_u32 value)
{
    if (value == PLASMA_V2_MOTION_STILL) {
        return "still";
    }
    if (value == PLASMA_V2_MOTION_DRIFT) {
        return "drift";
    }
    if (value == PLASMA_V2_MOTION_FLOW) {
        return "flow";
    }
    if (value == PLASMA_V2_MOTION_PULSE) {
        return "pulse";
    }
    if (value == PLASMA_V2_MOTION_SWIRL) {
        return "swirl";
    }
    return 0;
}

const char *plasma_v2_output_kind_token(ss_u32 value)
{
    if (value == PLASMA_V2_OUTPUT_CONTINUOUS) {
        return "continuous";
    }
    if (value == PLASMA_V2_OUTPUT_BANDED) {
        return "banded";
    }
    if (value == PLASMA_V2_OUTPUT_CONTOUR) {
        return "contour";
    }
    if (value == PLASMA_V2_OUTPUT_GLYPH) {
        return "glyph";
    }
    return 0;
}

const char *plasma_v2_treatment_kind_token(ss_u32 value)
{
    if (value == PLASMA_V2_TREATMENT_NONE) {
        return "none";
    }
    if (value == PLASMA_V2_TREATMENT_SOFT) {
        return "soft";
    }
    if (value == PLASMA_V2_TREATMENT_PHOSPHOR) {
        return "phosphor";
    }
    if (value == PLASMA_V2_TREATMENT_CRT) {
        return "crt";
    }
    if (value == PLASMA_V2_TREATMENT_DITHER) {
        return "dither";
    }
    if (value == PLASMA_V2_TREATMENT_BLOOM) {
        return "bloom";
    }
    return 0;
}

const char *plasma_v2_quality_intent_token(ss_u32 value)
{
    if (value == PLASMA_V2_QUALITY_SAFE) {
        return "safe";
    }
    if (value == PLASMA_V2_QUALITY_PREVIEW) {
        return "preview";
    }
    if (value == PLASMA_V2_QUALITY_LABS) {
        return "labs";
    }
    return 0;
}

const char *plasma_v2_control_token(ss_u32 value)
{
    if (value == PLASMA_V2_CONTROL_FIELD) {
        return "field";
    }
    if (value == PLASMA_V2_CONTROL_SCALE) {
        return "scale";
    }
    if (value == PLASMA_V2_CONTROL_COMPLEXITY) {
        return "complexity";
    }
    if (value == PLASMA_V2_CONTROL_MOTION) {
        return "motion";
    }
    if (value == PLASMA_V2_CONTROL_SPEED) {
        return "speed";
    }
    if (value == PLASMA_V2_CONTROL_WARP) {
        return "warp";
    }
    if (value == PLASMA_V2_CONTROL_FEEDBACK) {
        return "feedback";
    }
    if (value == PLASMA_V2_CONTROL_OUTPUT) {
        return "output";
    }
    if (value == PLASMA_V2_CONTROL_CONTRAST) {
        return "contrast";
    }
    if (value == PLASMA_V2_CONTROL_BRIGHTNESS) {
        return "brightness";
    }
    if (value == PLASMA_V2_CONTROL_SOFTNESS) {
        return "softness";
    }
    if (value == PLASMA_V2_CONTROL_TREATMENT) {
        return "treatment";
    }
    if (value == PLASMA_V2_CONTROL_QUALITY) {
        return "quality";
    }
    return 0;
}

static ss_u32 plasma_v2_token_to_value(
    const char *token,
    const char *(*token_fn)(ss_u32),
    ss_u32 last_value,
    ss_u32 *value_out
)
{
    ss_u32 value;

    if (token == 0 || value_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    for (value = 0U; value <= last_value; ++value) {
        if (plasma_v2_token_equals(token, token_fn(value)) == SS_V2_TRUE) {
            *value_out = value;
            return SS_V2_STATUS_OK;
        }
    }

    return SS_V2_STATUS_UNSUPPORTED;
}

ss_u32 plasma_v2_field_family_from_token(const char *token, ss_u32 *value_out)
{
    return plasma_v2_token_to_value(token, plasma_v2_field_family_token, PLASMA_V2_FIELD_LATTICE, value_out);
}

ss_u32 plasma_v2_motion_kind_from_token(const char *token, ss_u32 *value_out)
{
    return plasma_v2_token_to_value(token, plasma_v2_motion_kind_token, PLASMA_V2_MOTION_SWIRL, value_out);
}

ss_u32 plasma_v2_output_kind_from_token(const char *token, ss_u32 *value_out)
{
    return plasma_v2_token_to_value(token, plasma_v2_output_kind_token, PLASMA_V2_OUTPUT_GLYPH, value_out);
}

ss_u32 plasma_v2_treatment_kind_from_token(const char *token, ss_u32 *value_out)
{
    return plasma_v2_token_to_value(token, plasma_v2_treatment_kind_token, PLASMA_V2_TREATMENT_BLOOM, value_out);
}

ss_u32 plasma_v2_quality_intent_from_token(const char *token, ss_u32 *value_out)
{
    return plasma_v2_token_to_value(token, plasma_v2_quality_intent_token, PLASMA_V2_QUALITY_LABS, value_out);
}
