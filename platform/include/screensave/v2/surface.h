#ifndef SCREENSAVE_V2_SURFACE_H
#define SCREENSAVE_V2_SURFACE_H

#include "screensave/v2/types.h"

#define SS_V2_SURFACE_FORMAT_RGBA8 ((ss_u32)1U)
#define SS_V2_SURFACE_ORIGIN_TOP_LEFT ((ss_u32)1U)

typedef struct ss_v2_surface_desc_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 width;
    ss_u32 height;
    ss_u32 stride_bytes;
    ss_u32 format;
    ss_u32 origin;
    ss_u8 *pixels;
} ss_v2_surface_desc;

#endif /* SCREENSAVE_V2_SURFACE_H */
