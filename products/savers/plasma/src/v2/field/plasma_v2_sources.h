#ifndef PLASMA_V2_SOURCES_H
#define PLASMA_V2_SOURCES_H

#include "../plasma_v2_runtime.h"

ss_u32 plasma_v2_source_wave(const plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y, ss_u32 stream);
ss_u32 plasma_v2_source_radial(const plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y);
ss_u32 plasma_v2_source_noise(const plasma_v2_runtime *runtime, ss_u32 x, ss_u32 y, ss_u32 stream);

#endif /* PLASMA_V2_SOURCES_H */
