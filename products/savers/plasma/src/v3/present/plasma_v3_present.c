#include "plasma_v3_present.h"

ss_u32 plasma_v3_present_flat(plasma_v3_runtime *runtime)
{
    ss_u32 index;

    if (runtime == 0 || runtime->treatment_buffer == 0 || runtime->present_buffer == 0) {
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
