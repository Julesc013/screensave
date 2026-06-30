#ifndef PLASMA_V3_SPEC_H
#define PLASMA_V3_SPEC_H

#include "plasma_v3_types.h"

const char *plasma_v3_spec_schema_id(void);
void plasma_v3_spec_set_defaults(plasma_v3_spec *spec);
void plasma_v3_spec_clamp(plasma_v3_spec *spec);
ss_u32 plasma_v3_spec_is_valid(const plasma_v3_spec *spec);
ss_u32 plasma_v3_spec_apply_control(plasma_v3_spec *spec, ss_u32 control_id, ss_u32 value);
ss_u32 plasma_v3_spec_set_material_key(plasma_v3_spec *spec, const char *material_key);
const char *plasma_v3_control_token(ss_u32 control_id);
const char *plasma_v3_material_token(ss_u32 material_kind);
ss_u32 plasma_v3_control_from_token(const char *token, ss_u32 *control_id_out);
ss_u32 plasma_v3_renderer_from_token(const char *token, ss_u32 *renderer_out);
const char *plasma_v3_renderer_token(ss_u32 renderer);

#endif /* PLASMA_V3_SPEC_H */
