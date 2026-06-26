#ifndef SCREENSAVE_V2_INTERNAL_VALIDATE_H
#define SCREENSAVE_V2_INTERNAL_VALIDATE_H

#include "screensave/v2/base.h"

typedef struct ss_v2_prefix_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
} ss_v2_prefix;

ss_u32 ss_v2_check_prefix(
    const void *value,
    ss_u32 observed_size,
    ss_u32 minimum_size,
    ss_u32 expected_abi_version
);

#endif /* SCREENSAVE_V2_INTERNAL_VALIDATE_H */
