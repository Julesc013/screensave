#include "plasma_v2_field.h"
#include "plasma_v2_generators.h"
#include "plasma_v2_modifiers.h"

ss_u32 plasma_v2_field_step(plasma_v2_runtime *runtime)
{
    ss_u32 status;

    status = plasma_v2_generate_field(runtime);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return plasma_v2_apply_modifiers(runtime);
}
