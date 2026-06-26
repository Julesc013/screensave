#include "screensave/v2/internal/validate.h"

ss_u32 ss_v2_check_prefix(
    const void *value,
    ss_u32 observed_size,
    ss_u32 minimum_size,
    ss_u32 expected_abi_version
)
{
    const ss_v2_prefix *prefix;

    if (value == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (observed_size < (ss_u32)sizeof(ss_v2_prefix) || minimum_size < (ss_u32)sizeof(ss_v2_prefix)) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    prefix = (const ss_v2_prefix *)value;
    if (prefix->struct_size < minimum_size) {
        return SS_V2_STATUS_BAD_SIZE;
    }
    if (prefix->abi_version != expected_abi_version) {
        return SS_V2_STATUS_BAD_VERSION;
    }
    return SS_V2_STATUS_OK;
}
