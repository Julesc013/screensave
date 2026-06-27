#ifndef PLASMA_V2_SPEC_H
#define PLASMA_V2_SPEC_H

#include "plasma_v2_types.h"

const char *plasma_v2_spec_schema_id(void);

void plasma_v2_spec_set_defaults(plasma_v2_spec *spec);
void plasma_v2_spec_clamp(plasma_v2_spec *spec);
ss_u32 plasma_v2_spec_is_valid(const plasma_v2_spec *spec);

ss_u32 plasma_v2_spec_apply_control(plasma_v2_spec *spec, ss_u32 control_id, ss_u32 value);
ss_u32 plasma_v2_spec_set_material_key(plasma_v2_spec *spec, const char *material_key);
ss_u32 plasma_v2_spec_set_profile_key(plasma_v2_spec *spec, const char *profile_key);
ss_u32 plasma_v2_spec_from_legacy_config_view(
    const plasma_v2_legacy_config_view *legacy,
    plasma_v2_spec *spec_out
);

const char *plasma_v2_field_family_token(ss_u32 value);
const char *plasma_v2_motion_kind_token(ss_u32 value);
const char *plasma_v2_output_kind_token(ss_u32 value);
const char *plasma_v2_treatment_kind_token(ss_u32 value);
const char *plasma_v2_quality_intent_token(ss_u32 value);
const char *plasma_v2_control_token(ss_u32 value);

ss_u32 plasma_v2_field_family_from_token(const char *token, ss_u32 *value_out);
ss_u32 plasma_v2_motion_kind_from_token(const char *token, ss_u32 *value_out);
ss_u32 plasma_v2_output_kind_from_token(const char *token, ss_u32 *value_out);
ss_u32 plasma_v2_treatment_kind_from_token(const char *token, ss_u32 *value_out);
ss_u32 plasma_v2_quality_intent_from_token(const char *token, ss_u32 *value_out);

#endif /* PLASMA_V2_SPEC_H */
