#ifndef SCREENSAVE_V2_TYPES_H
#define SCREENSAVE_V2_TYPES_H

#include "screensave/v2/base.h"

typedef struct ss_v2_u64_parts_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 low;
    ss_u32 high;
} ss_v2_u64_parts;

typedef struct ss_v2_pointi_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_i32 x;
    ss_i32 y;
} ss_v2_pointi;

typedef struct ss_v2_size_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 width;
    ss_u32 height;
} ss_v2_size;

typedef struct ss_v2_recti_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_i32 x;
    ss_i32 y;
    ss_u32 width;
    ss_u32 height;
} ss_v2_recti;

typedef struct ss_v2_color_rgba8_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u8 red;
    ss_u8 green;
    ss_u8 blue;
    ss_u8 alpha;
} ss_v2_color_rgba8;

#endif /* SCREENSAVE_V2_TYPES_H */
