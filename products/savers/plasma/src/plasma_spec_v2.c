#include "plasma_spec_v2.h"

static ss_u32 plasma_v2_clamp_percent(ss_u32 value)
{
    if (value > PLASMA_V2_MAX_PERCENT) {
        return PLASMA_V2_MAX_PERCENT;
    }

    return value;
}

static ss_u32 plasma_v2_is_percent(ss_u32 value)
{
    return value <= PLASMA_V2_MAX_PERCENT ? SS_V2_TRUE : SS_V2_FALSE;
}

const char *plasma_spec_v2_schema_id(void)
{
    return PLASMA_SPEC_V2_SCHEMA_ID;
}

void plasma_spec_v2_set_defaults(plasma_spec_v2 *spec)
{
    if (spec == 0) {
        return;
    }

    spec->struct_size = (ss_u32)sizeof(*spec);
    spec->schema_version = PLASMA_SPEC_V2_SCHEMA_VERSION;
    spec->field_family = PLASMA_V2_FIELD_CLASSIC_INTERFERENCE;
    spec->scale = (ss_u32)100U;
    spec->complexity = (ss_u32)56U;
    spec->motion_speed = (ss_u32)32U;
    spec->warp_amount = (ss_u32)18U;
    spec->feedback_amount = (ss_u32)8U;
    spec->output_style = PLASMA_V2_OUTPUT_CONTINUOUS;
    spec->material_id = PLASMA_V2_MATERIAL_PLASMA_LAVA;
    spec->brightness = (ss_u32)100U;
    spec->contrast = (ss_u32)100U;
    spec->treatment_flags = PLASMA_V2_TREATMENT_NONE;
    spec->quality_intent = PLASMA_V2_QUALITY_SAFE;
    spec->seed_policy = PLASMA_V2_SEED_SESSION;
    spec->presentation = PLASMA_V2_PRESENTATION_FLAT;
}

void plasma_spec_v2_clamp(plasma_spec_v2 *spec)
{
    if (spec == 0) {
        return;
    }

    spec->struct_size = (ss_u32)sizeof(*spec);
    spec->schema_version = PLASMA_SPEC_V2_SCHEMA_VERSION;
    spec->scale = plasma_v2_clamp_percent(spec->scale);
    spec->complexity = plasma_v2_clamp_percent(spec->complexity);
    spec->motion_speed = plasma_v2_clamp_percent(spec->motion_speed);
    spec->warp_amount = plasma_v2_clamp_percent(spec->warp_amount);
    spec->feedback_amount = plasma_v2_clamp_percent(spec->feedback_amount);
    spec->brightness = plasma_v2_clamp_percent(spec->brightness);
    spec->contrast = plasma_v2_clamp_percent(spec->contrast);

    if (plasma_spec_v2_field_token(spec->field_family) == 0) {
        spec->field_family = PLASMA_V2_FIELD_CLASSIC_INTERFERENCE;
    }
    if (plasma_spec_v2_output_token(spec->output_style) == 0) {
        spec->output_style = PLASMA_V2_OUTPUT_CONTINUOUS;
    }
    if (plasma_spec_v2_material_token(spec->material_id) == 0) {
        spec->material_id = PLASMA_V2_MATERIAL_PLASMA_LAVA;
    }
    if (plasma_spec_v2_treatment_token(spec->treatment_flags) == 0) {
        spec->treatment_flags = PLASMA_V2_TREATMENT_NONE;
    }
    if (spec->quality_intent != PLASMA_V2_QUALITY_SAFE) {
        spec->quality_intent = PLASMA_V2_QUALITY_SAFE;
    }
    if (plasma_spec_v2_seed_policy_token(spec->seed_policy) == 0) {
        spec->seed_policy = PLASMA_V2_SEED_SESSION;
    }
    spec->presentation = PLASMA_V2_PRESENTATION_FLAT;
}

ss_u32 plasma_spec_v2_is_valid(const plasma_spec_v2 *spec)
{
    if (spec == 0) {
        return SS_V2_FALSE;
    }
    if (spec->struct_size < (ss_u32)sizeof(*spec)) {
        return SS_V2_FALSE;
    }
    if (spec->schema_version != PLASMA_SPEC_V2_SCHEMA_VERSION) {
        return SS_V2_FALSE;
    }
    if (plasma_spec_v2_field_token(spec->field_family) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->scale) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->complexity) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->motion_speed) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->warp_amount) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->feedback_amount) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_spec_v2_output_token(spec->output_style) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_spec_v2_material_token(spec->material_id) == 0) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->brightness) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_v2_is_percent(spec->contrast) == SS_V2_FALSE) {
        return SS_V2_FALSE;
    }
    if (plasma_spec_v2_treatment_token(spec->treatment_flags) == 0) {
        return SS_V2_FALSE;
    }
    if (spec->quality_intent != PLASMA_V2_QUALITY_SAFE) {
        return SS_V2_FALSE;
    }
    if (plasma_spec_v2_seed_policy_token(spec->seed_policy) == 0) {
        return SS_V2_FALSE;
    }
    if (spec->presentation != PLASMA_V2_PRESENTATION_FLAT) {
        return SS_V2_FALSE;
    }

    return SS_V2_TRUE;
}

const char *plasma_spec_v2_field_token(ss_u32 value)
{
    if (value == PLASMA_V2_FIELD_CLASSIC_INTERFERENCE) {
        return "classic_interference";
    }
    if (value == PLASMA_V2_FIELD_RADIAL_WARPED) {
        return "radial_warped";
    }

    return 0;
}

const char *plasma_spec_v2_output_token(ss_u32 value)
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

    return 0;
}

const char *plasma_spec_v2_material_token(ss_u32 value)
{
    if (value == PLASMA_V2_MATERIAL_PLASMA_LAVA) {
        return "plasma_lava";
    }
    if (value == PLASMA_V2_MATERIAL_AURORA_COOL) {
        return "aurora_cool";
    }
    if (value == PLASMA_V2_MATERIAL_OCEANIC_BLUE) {
        return "oceanic_blue";
    }
    if (value == PLASMA_V2_MATERIAL_MUSEUM_PHOSPHOR) {
        return "museum_phosphor";
    }
    if (value == PLASMA_V2_MATERIAL_QUIET_DARKROOM) {
        return "quiet_darkroom";
    }

    return 0;
}

const char *plasma_spec_v2_treatment_token(ss_u32 value)
{
    if (value == PLASMA_V2_TREATMENT_NONE) {
        return "none";
    }
    if (value == PLASMA_V2_TREATMENT_RESTRAINED_DITHER) {
        return "restrained_dither";
    }
    if (value == PLASMA_V2_TREATMENT_RESTRAINED_CRT) {
        return "restrained_crt";
    }

    return 0;
}

const char *plasma_spec_v2_seed_policy_token(ss_u32 value)
{
    if (value == PLASMA_V2_SEED_SESSION) {
        return "session";
    }
    if (value == PLASMA_V2_SEED_FIXED) {
        return "fixed";
    }
    if (value == PLASMA_V2_SEED_RANDOMIZE) {
        return "randomize";
    }

    return 0;
}
