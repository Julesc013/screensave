#ifndef PLASMA_SPEC_V2_H
#define PLASMA_SPEC_V2_H

#include "screensave/v2/base.h"

#define PLASMA_SPEC_V2_SCHEMA_ID "screensave.plasma.spec.v2"
#define PLASMA_SPEC_V2_SCHEMA_VERSION ((ss_u32)2U)

#define PLASMA_V2_FIELD_CLASSIC_INTERFERENCE ((ss_u32)0U)
#define PLASMA_V2_FIELD_RADIAL_WARPED ((ss_u32)1U)

#define PLASMA_V2_OUTPUT_CONTINUOUS ((ss_u32)0U)
#define PLASMA_V2_OUTPUT_BANDED ((ss_u32)1U)
#define PLASMA_V2_OUTPUT_CONTOUR ((ss_u32)2U)

#define PLASMA_V2_MATERIAL_PLASMA_LAVA ((ss_u32)0U)
#define PLASMA_V2_MATERIAL_AURORA_COOL ((ss_u32)1U)
#define PLASMA_V2_MATERIAL_OCEANIC_BLUE ((ss_u32)2U)
#define PLASMA_V2_MATERIAL_MUSEUM_PHOSPHOR ((ss_u32)3U)
#define PLASMA_V2_MATERIAL_QUIET_DARKROOM ((ss_u32)4U)

#define PLASMA_V2_PRESENTATION_FLAT ((ss_u32)0U)

#define PLASMA_V2_TREATMENT_NONE ((ss_u32)0U)
#define PLASMA_V2_TREATMENT_RESTRAINED_DITHER ((ss_u32)1U)
#define PLASMA_V2_TREATMENT_RESTRAINED_CRT ((ss_u32)2U)

#define PLASMA_V2_QUALITY_SAFE ((ss_u32)0U)

#define PLASMA_V2_SEED_SESSION ((ss_u32)0U)
#define PLASMA_V2_SEED_FIXED ((ss_u32)1U)
#define PLASMA_V2_SEED_RANDOMIZE ((ss_u32)2U)

#define PLASMA_V2_BASIC_CONTROL_FIELD_FAMILY ((ss_u32)0U)
#define PLASMA_V2_BASIC_CONTROL_SCALE ((ss_u32)1U)
#define PLASMA_V2_BASIC_CONTROL_COMPLEXITY ((ss_u32)2U)
#define PLASMA_V2_BASIC_CONTROL_MOTION_SPEED ((ss_u32)3U)
#define PLASMA_V2_BASIC_CONTROL_WARP_AMOUNT ((ss_u32)4U)
#define PLASMA_V2_BASIC_CONTROL_FEEDBACK_AMOUNT ((ss_u32)5U)
#define PLASMA_V2_BASIC_CONTROL_OUTPUT_STYLE ((ss_u32)6U)
#define PLASMA_V2_BASIC_CONTROL_MATERIAL ((ss_u32)7U)
#define PLASMA_V2_BASIC_CONTROL_BRIGHTNESS ((ss_u32)8U)
#define PLASMA_V2_BASIC_CONTROL_CONTRAST ((ss_u32)9U)
#define PLASMA_V2_BASIC_CONTROL_TREATMENT ((ss_u32)10U)
#define PLASMA_V2_BASIC_CONTROL_SEED_POLICY ((ss_u32)11U)
#define PLASMA_V2_BASIC_CONTROL_QUALITY_INTENT ((ss_u32)12U)

#define PLASMA_V2_MIN_PERCENT ((ss_u32)0U)
#define PLASMA_V2_MAX_PERCENT ((ss_u32)100U)

typedef struct plasma_spec_v2_tag {
    ss_u32 struct_size;
    ss_u32 schema_version;
    ss_u32 field_family;
    ss_u32 scale;
    ss_u32 complexity;
    ss_u32 motion_speed;
    ss_u32 warp_amount;
    ss_u32 feedback_amount;
    ss_u32 output_style;
    ss_u32 material_id;
    ss_u32 brightness;
    ss_u32 contrast;
    ss_u32 treatment_flags;
    ss_u32 quality_intent;
    ss_u32 seed_policy;
    ss_u32 presentation;
} plasma_spec_v2;

const char *plasma_spec_v2_schema_id(void);
void plasma_spec_v2_set_defaults(plasma_spec_v2 *spec);
void plasma_spec_v2_clamp(plasma_spec_v2 *spec);
ss_u32 plasma_spec_v2_is_valid(const plasma_spec_v2 *spec);
ss_u32 plasma_spec_v2_apply_basic_control(plasma_spec_v2 *spec, ss_u32 control_id, ss_u32 value);

const char *plasma_spec_v2_basic_control_token(ss_u32 value);
const char *plasma_spec_v2_field_token(ss_u32 value);
const char *plasma_spec_v2_output_token(ss_u32 value);
const char *plasma_spec_v2_material_token(ss_u32 value);
const char *plasma_spec_v2_treatment_token(ss_u32 value);
const char *plasma_spec_v2_seed_policy_token(ss_u32 value);

#endif /* PLASMA_SPEC_V2_H */
