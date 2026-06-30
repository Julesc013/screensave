#ifndef PLASMA_V3_TYPES_H
#define PLASMA_V3_TYPES_H

#include "screensave/v2/base.h"

#define PLASMA_V3_SCHEMA_ID "screensave.plasma.v3.visual-core-spike"
#define PLASMA_V3_SCHEMA_VERSION ((ss_u32)1U)

#define PLASMA_V3_KEY_LENGTH ((ss_u32)64U)
#define PLASMA_V3_CONTROL_MAX ((ss_u32)1000U)
#define PLASMA_V3_PRESENT_PIXEL_BYTES ((ss_u32)4U)

#define PLASMA_V3_RENDERER_SOFTWARE ((ss_u32)0U)
#define PLASMA_V3_RENDERER_GDI ((ss_u32)1U)
#define PLASMA_V3_RENDERER_GL11 ((ss_u32)2U)

#define PLASMA_V3_CAP_SOFTWARE_REFERENCE ((ss_u32)0x00000001U)
#define PLASMA_V3_CAP_GDI ((ss_u32)0x00000002U)
#define PLASMA_V3_CAP_GL11 ((ss_u32)0x00000004U)

#define PLASMA_V3_DEGRADE_NONE ((ss_u32)0U)
#define PLASMA_V3_DEGRADE_RENDERER ((ss_u32)0x00000001U)
#define PLASMA_V3_DEGRADE_SIZE ((ss_u32)0x00000002U)

#define PLASMA_V3_CONTROL_FIELD_SCALE ((ss_u32)0U)
#define PLASMA_V3_CONTROL_DETAIL ((ss_u32)1U)
#define PLASMA_V3_CONTROL_MOTION ((ss_u32)2U)
#define PLASMA_V3_CONTROL_WARP ((ss_u32)3U)
#define PLASMA_V3_CONTROL_PALETTE_SHIFT ((ss_u32)4U)
#define PLASMA_V3_CONTROL_CONTRAST ((ss_u32)5U)
#define PLASMA_V3_CONTROL_BRIGHTNESS ((ss_u32)6U)
#define PLASMA_V3_CONTROL_SOFTNESS ((ss_u32)7U)
#define PLASMA_V3_CONTROL_PHOSPHOR ((ss_u32)8U)

#define PLASMA_V3_MATERIAL_THERMAL_RIBBON ((ss_u32)0U)
#define PLASMA_V3_MATERIAL_COOL_SIGNAL ((ss_u32)1U)
#define PLASMA_V3_MATERIAL_GREEN_PHOSPHOR ((ss_u32)2U)

typedef struct plasma_v3_dimensions_tag {
    ss_u32 width;
    ss_u32 height;
} plasma_v3_dimensions;

typedef struct plasma_v3_spec_tag {
    ss_u32 struct_size;
    ss_u32 schema_version;
    ss_u32 material_kind;
    ss_u32 field_scale;
    ss_u32 detail;
    ss_u32 motion;
    ss_u32 warp;
    ss_u32 palette_shift;
    ss_u32 contrast;
    ss_u32 brightness;
    ss_u32 softness;
    ss_u32 phosphor;
    ss_u32 seed;
    ss_u32 deterministic;
    char material_key[PLASMA_V3_KEY_LENGTH];
    char profile_key[PLASMA_V3_KEY_LENGTH];
} plasma_v3_spec;

typedef struct plasma_v3_plan_request_tag {
    ss_u32 struct_size;
    const plasma_v3_spec *requested_spec;
    plasma_v3_dimensions drawable_size;
    ss_u32 requested_renderer;
    ss_u32 capability_flags;
    ss_u32 base_seed;
} plasma_v3_plan_request;

typedef struct plasma_v3_plan_tag {
    ss_u32 struct_size;
    ss_u32 schema_version;
    plasma_v3_spec requested_spec;
    plasma_v3_spec resolved_spec;
    plasma_v3_dimensions drawable_size;
    plasma_v3_dimensions field_size;
    plasma_v3_dimensions output_size;
    ss_u32 requested_renderer;
    ss_u32 active_renderer;
    ss_u32 capability_flags;
    ss_u32 degradation_flags;
    ss_u32 base_seed;
    ss_u32 frame_delta_millis;
    ss_u32 use_fixed_point;
    ss_u32 use_software_reference;
} plasma_v3_plan;

typedef struct plasma_v3_runtime_buffers_tag {
    ss_u32 struct_size;
    ss_u32 *field_buffer;
    ss_u8 *material_buffer;
    ss_u8 *treatment_buffer;
    ss_u8 *present_buffer;
    ss_u32 field_cell_count;
    ss_u32 material_byte_count;
    ss_u32 treatment_byte_count;
    ss_u32 present_byte_count;
} plasma_v3_runtime_buffers;

typedef struct plasma_v3_runtime_tag {
    ss_u32 struct_size;
    plasma_v3_plan resolved_plan;
    ss_u32 *field_buffer;
    ss_u8 *material_buffer;
    ss_u8 *treatment_buffer;
    ss_u8 *present_buffer;
    ss_u32 field_cell_count;
    ss_u32 material_byte_count;
    ss_u32 treatment_byte_count;
    ss_u32 present_byte_count;
    ss_u32 frame_index;
    ss_u32 elapsed_millis;
    ss_u32 phase0;
    ss_u32 phase1;
    ss_u32 phase2;
    ss_u32 rng_state;
} plasma_v3_runtime;

typedef struct plasma_v3_signature_tag {
    ss_u32 struct_size;
    ss_u32 hash;
    ss_u32 lit_pixels;
    ss_u32 byte_count;
} plasma_v3_signature;

#endif /* PLASMA_V3_TYPES_H */
