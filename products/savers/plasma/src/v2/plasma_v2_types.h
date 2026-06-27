#ifndef PLASMA_V2_TYPES_H
#define PLASMA_V2_TYPES_H

#include "screensave/v2/base.h"

#define PLASMA_V2_INSTRUMENT_SCHEMA_ID "screensave.plasma.v2.instrument-spec"
#define PLASMA_V2_INSTRUMENT_SCHEMA_VERSION ((ss_u32)1U)

#define PLASMA_V2_KEY_LENGTH ((ss_u32)64U)
#define PLASMA_V2_CONTROL_MIN ((ss_u32)0U)
#define PLASMA_V2_CONTROL_MAX ((ss_u32)1000U)

#define PLASMA_V2_FIELD_CLASSIC ((ss_u32)0U)
#define PLASMA_V2_FIELD_INTERFERENCE ((ss_u32)1U)
#define PLASMA_V2_FIELD_FIRE ((ss_u32)2U)
#define PLASMA_V2_FIELD_AURORA ((ss_u32)3U)
#define PLASMA_V2_FIELD_CURL ((ss_u32)4U)
#define PLASMA_V2_FIELD_LATTICE ((ss_u32)5U)

#define PLASMA_V2_MOTION_STILL ((ss_u32)0U)
#define PLASMA_V2_MOTION_DRIFT ((ss_u32)1U)
#define PLASMA_V2_MOTION_FLOW ((ss_u32)2U)
#define PLASMA_V2_MOTION_PULSE ((ss_u32)3U)
#define PLASMA_V2_MOTION_SWIRL ((ss_u32)4U)

#define PLASMA_V2_OUTPUT_CONTINUOUS ((ss_u32)0U)
#define PLASMA_V2_OUTPUT_BANDED ((ss_u32)1U)
#define PLASMA_V2_OUTPUT_CONTOUR ((ss_u32)2U)
#define PLASMA_V2_OUTPUT_GLYPH ((ss_u32)3U)

#define PLASMA_V2_TREATMENT_NONE ((ss_u32)0U)
#define PLASMA_V2_TREATMENT_SOFT ((ss_u32)1U)
#define PLASMA_V2_TREATMENT_PHOSPHOR ((ss_u32)2U)
#define PLASMA_V2_TREATMENT_CRT ((ss_u32)3U)
#define PLASMA_V2_TREATMENT_DITHER ((ss_u32)4U)
#define PLASMA_V2_TREATMENT_BLOOM ((ss_u32)5U)

#define PLASMA_V2_QUALITY_SAFE ((ss_u32)0U)
#define PLASMA_V2_QUALITY_PREVIEW ((ss_u32)1U)
#define PLASMA_V2_QUALITY_LABS ((ss_u32)2U)

#define PLASMA_V2_CONTROL_FIELD ((ss_u32)0U)
#define PLASMA_V2_CONTROL_SCALE ((ss_u32)1U)
#define PLASMA_V2_CONTROL_COMPLEXITY ((ss_u32)2U)
#define PLASMA_V2_CONTROL_MOTION ((ss_u32)3U)
#define PLASMA_V2_CONTROL_SPEED ((ss_u32)4U)
#define PLASMA_V2_CONTROL_WARP ((ss_u32)5U)
#define PLASMA_V2_CONTROL_FEEDBACK ((ss_u32)6U)
#define PLASMA_V2_CONTROL_OUTPUT ((ss_u32)7U)
#define PLASMA_V2_CONTROL_CONTRAST ((ss_u32)8U)
#define PLASMA_V2_CONTROL_BRIGHTNESS ((ss_u32)9U)
#define PLASMA_V2_CONTROL_SOFTNESS ((ss_u32)10U)
#define PLASMA_V2_CONTROL_TREATMENT ((ss_u32)11U)
#define PLASMA_V2_CONTROL_QUALITY ((ss_u32)12U)

typedef struct plasma_v2_spec_tag {
    ss_u32 struct_size;
    ss_u32 schema_version;
    ss_u32 field_family;
    ss_u32 motion_kind;
    ss_u32 output_kind;
    ss_u32 treatment_kind;
    ss_u32 quality_intent;
    ss_u32 scale;
    ss_u32 complexity;
    ss_u32 speed;
    ss_u32 warp;
    ss_u32 feedback;
    ss_u32 contrast;
    ss_u32 brightness;
    ss_u32 softness;
    ss_u32 seed;
    ss_u32 deterministic;
    char material_key[PLASMA_V2_KEY_LENGTH];
    char profile_key[PLASMA_V2_KEY_LENGTH];
} plasma_v2_spec;

typedef struct plasma_v2_legacy_config_view_tag {
    ss_u32 struct_size;
    ss_i32 effect_mode;
    ss_i32 speed_mode;
    ss_i32 resolution_mode;
    ss_i32 smoothing_mode;
    const char *preset_key;
    const char *theme_key;
    ss_u32 seed;
    ss_u32 deterministic;
} plasma_v2_legacy_config_view;

#endif /* PLASMA_V2_TYPES_H */
