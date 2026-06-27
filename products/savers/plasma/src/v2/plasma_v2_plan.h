#ifndef PLASMA_V2_PLAN_H
#define PLASMA_V2_PLAN_H

#include "plasma_v2_spec.h"

#define PLASMA_V2_PLAN_SCHEMA_VERSION ((ss_u32)1U)
#define PLASMA_V2_PLAN_DEGRADE_REASON_LENGTH ((ss_u32)96U)

#define PLASMA_V2_RENDERER_SOFTWARE ((ss_u32)0U)
#define PLASMA_V2_RENDERER_GDI ((ss_u32)1U)
#define PLASMA_V2_RENDERER_GL11 ((ss_u32)2U)

#define PLASMA_V2_CAP_SOFTWARE_REFERENCE ((ss_u32)0x00000001U)
#define PLASMA_V2_CAP_GDI ((ss_u32)0x00000002U)
#define PLASMA_V2_CAP_GL11 ((ss_u32)0x00000004U)
#define PLASMA_V2_CAP_FEEDBACK_BUFFER ((ss_u32)0x00000008U)

#define PLASMA_V2_DEGRADE_NONE ((ss_u32)0U)
#define PLASMA_V2_DEGRADE_RENDERER ((ss_u32)0x00000001U)
#define PLASMA_V2_DEGRADE_FIELD_SIZE ((ss_u32)0x00000002U)
#define PLASMA_V2_DEGRADE_OUTPUT_SIZE ((ss_u32)0x00000004U)
#define PLASMA_V2_DEGRADE_EXPERIMENTAL ((ss_u32)0x00000008U)

typedef struct plasma_v2_dimensions_tag {
    ss_u32 width;
    ss_u32 height;
} plasma_v2_dimensions;

typedef struct plasma_v2_plan_request_tag {
    ss_u32 struct_size;
    const plasma_v2_spec *requested_spec;
    plasma_v2_dimensions drawable_size;
    ss_u32 requested_renderer;
    ss_u32 capability_flags;
    ss_u32 base_seed;
    ss_u32 stream_seed;
    ss_u32 allow_experimental;
} plasma_v2_plan_request;

typedef struct plasma_v2_plan_tag {
    ss_u32 struct_size;
    ss_u32 schema_version;
    plasma_v2_spec requested_spec;
    plasma_v2_spec resolved_spec;
    plasma_v2_dimensions drawable_size;
    plasma_v2_dimensions field_size;
    plasma_v2_dimensions output_size;
    ss_u32 requested_renderer;
    ss_u32 active_renderer;
    ss_u32 capability_flags;
    ss_u32 degradation_flags;
    ss_u32 base_seed;
    ss_u32 stream_seed;
    ss_u32 use_fixed_point;
    ss_u32 use_software_reference;
    ss_u32 use_feedback_buffer;
    ss_u32 allow_experimental;
    char degrade_reason[PLASMA_V2_PLAN_DEGRADE_REASON_LENGTH];
} plasma_v2_plan;

void plasma_v2_plan_set_defaults(plasma_v2_plan *plan);
ss_u32 plasma_v2_plan_compile(const plasma_v2_plan_request *request, plasma_v2_plan *plan_out);
ss_u32 plasma_v2_plan_is_valid(const plasma_v2_plan *plan);
const char *plasma_v2_plan_renderer_token(ss_u32 renderer);

#endif /* PLASMA_V2_PLAN_H */
