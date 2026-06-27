#include "plasma_v2_present.h"

ss_u32 plasma_v2_present_flat(plasma_v2_runtime *runtime)
{
    ss_u32 index;

    if (plasma_v2_runtime_is_valid(runtime) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (runtime->present_byte_count < runtime->treatment_byte_count) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    for (index = 0U; index < runtime->treatment_byte_count; ++index) {
        runtime->present_buffer[index] = runtime->treatment_buffer[index];
    }
    return SS_V2_STATUS_OK;
}
