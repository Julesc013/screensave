#ifndef SCREENSAVE_V2_CLOCK_H
#define SCREENSAVE_V2_CLOCK_H

#include "screensave/v2/types.h"

typedef struct ss_v2_clock_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_u64_parts frame_index;
    ss_v2_u64_parts elapsed_ms;
    ss_u32 delta_ms;
    ss_u32 fixed_step_ms;
} ss_v2_clock;

#endif /* SCREENSAVE_V2_CLOCK_H */
