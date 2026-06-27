#include "plasma_v2_spec.h"

static int plasma_smoke_key_equals(const char *left, const char *right)
{
    if (left == 0 || right == 0) {
        return 0;
    }
    while (*left != '\0' && *right != '\0') {
        if (*left != *right) {
            return 0;
        }
        ++left;
        ++right;
    }
    return *left == *right;
}

int main(void)
{
    plasma_v2_spec spec;
    plasma_v2_legacy_config_view legacy;
    ss_u32 value;

    plasma_v2_spec_set_defaults(&spec);
    if (plasma_v2_spec_is_valid(&spec) != SS_V2_TRUE) {
        return 1;
    }
    if (!plasma_smoke_key_equals(plasma_v2_spec_schema_id(), PLASMA_V2_INSTRUMENT_SCHEMA_ID)) {
        return 2;
    }
    if (!plasma_smoke_key_equals(spec.material_key, "plasma_lava")) {
        return 3;
    }
    if (plasma_v2_field_family_from_token("interference", &value) != SS_V2_STATUS_OK) {
        return 4;
    }
    if (value != PLASMA_V2_FIELD_INTERFERENCE) {
        return 5;
    }
    if (!plasma_smoke_key_equals(plasma_v2_treatment_kind_token(PLASMA_V2_TREATMENT_PHOSPHOR), "phosphor")) {
        return 6;
    }
    if (plasma_v2_motion_kind_from_token("unknown", &value) != SS_V2_STATUS_UNSUPPORTED) {
        return 7;
    }

    if (plasma_v2_spec_apply_control(&spec, PLASMA_V2_CONTROL_SCALE, (ss_u32)1200U) != SS_V2_STATUS_OK) {
        return 8;
    }
    if (spec.scale != PLASMA_V2_CONTROL_MAX) {
        return 9;
    }
    if (plasma_v2_spec_apply_control(&spec, PLASMA_V2_CONTROL_FIELD, PLASMA_V2_FIELD_FIRE) != SS_V2_STATUS_OK) {
        return 10;
    }
    if (spec.field_family != PLASMA_V2_FIELD_FIRE) {
        return 11;
    }
    if (plasma_v2_spec_set_material_key(&spec, "quiet_darkroom") != SS_V2_STATUS_OK) {
        return 12;
    }
    if (!plasma_smoke_key_equals(spec.material_key, "quiet_darkroom")) {
        return 13;
    }

    legacy.struct_size = (ss_u32)sizeof(legacy);
    legacy.effect_mode = (ss_i32)1;
    legacy.speed_mode = (ss_i32)2;
    legacy.resolution_mode = (ss_i32)2;
    legacy.smoothing_mode = (ss_i32)2;
    legacy.preset_key = 0;
    legacy.theme_key = 0;
    legacy.seed = (ss_u32)4096U;
    legacy.deterministic = SS_V2_TRUE;
    if (plasma_v2_spec_from_legacy_config_view(&legacy, &spec) != SS_V2_STATUS_OK) {
        return 14;
    }
    if (spec.field_family != PLASMA_V2_FIELD_FIRE || spec.speed != (ss_u32)700U) {
        return 15;
    }
    if (spec.treatment_kind != PLASMA_V2_TREATMENT_PHOSPHOR) {
        return 16;
    }
    if (spec.seed != (ss_u32)4096U || spec.deterministic != SS_V2_TRUE) {
        return 17;
    }

    legacy.effect_mode = (ss_i32)0;
    legacy.speed_mode = (ss_i32)1;
    legacy.resolution_mode = (ss_i32)1;
    legacy.smoothing_mode = (ss_i32)1;
    legacy.preset_key = "ocean_interference";
    legacy.theme_key = "museum_phosphor";
    legacy.seed = (ss_u32)123U;
    legacy.deterministic = SS_V2_FALSE;
    if (plasma_v2_spec_from_legacy_config_view(&legacy, &spec) != SS_V2_STATUS_OK) {
        return 18;
    }
    if (spec.field_family != PLASMA_V2_FIELD_INTERFERENCE) {
        return 19;
    }
    if (!plasma_smoke_key_equals(spec.material_key, "museum_phosphor")) {
        return 20;
    }
    if (spec.deterministic != SS_V2_FALSE) {
        return 21;
    }

    return 0;
}
