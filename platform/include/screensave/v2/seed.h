#ifndef SCREENSAVE_V2_SEED_H
#define SCREENSAVE_V2_SEED_H

#include "screensave/v2/base.h"

typedef struct ss_v2_seed_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 base_seed;
    ss_u32 stream_seed;
    ss_u32 deterministic;
} ss_v2_seed;

#endif /* SCREENSAVE_V2_SEED_H */
