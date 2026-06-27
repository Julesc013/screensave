#ifndef PLASMA_V2_RUNTIME_H
#define PLASMA_V2_RUNTIME_H

#include "plasma_v2_plan.h"

#define PLASMA_V2_PRESENT_PIXEL_BYTES ((ss_u32)4U)

typedef struct plasma_v2_runtime_buffers_tag {
    ss_u32 struct_size;
    ss_u32 *field_a;
    ss_u32 *field_b;
    ss_u32 *field_history;
    ss_u8 *material_buffer;
    ss_u8 *treatment_buffer;
    ss_u8 *present_buffer;
    ss_u32 field_cell_count;
    ss_u32 material_byte_count;
    ss_u32 treatment_byte_count;
    ss_u32 present_byte_count;
} plasma_v2_runtime_buffers;

typedef struct plasma_v2_runtime_tag {
    ss_u32 struct_size;
    plasma_v2_plan resolved_plan;
    ss_u32 *field_a;
    ss_u32 *field_b;
    ss_u32 *field_history;
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
} plasma_v2_runtime;

ss_u32 plasma_v2_runtime_required_field_cells(const plasma_v2_plan *plan, ss_u32 *cell_count_out);
ss_u32 plasma_v2_runtime_required_pixel_bytes(const plasma_v2_plan *plan, ss_u32 *byte_count_out);
ss_u32 plasma_v2_runtime_bind(
    const plasma_v2_plan *plan,
    const plasma_v2_runtime_buffers *buffers,
    plasma_v2_runtime *runtime_out
);
ss_u32 plasma_v2_runtime_reset(plasma_v2_runtime *runtime);
ss_u32 plasma_v2_runtime_advance(plasma_v2_runtime *runtime, ss_u32 delta_millis);
ss_u32 plasma_v2_runtime_is_valid(const plasma_v2_runtime *runtime);

#endif /* PLASMA_V2_RUNTIME_H */
