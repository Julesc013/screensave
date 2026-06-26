#ifndef SCREENSAVE_V2_BASE_H
#define SCREENSAVE_V2_BASE_H

#define SS_V2_ABI_VERSION 2U

typedef unsigned char ss_u8;
typedef unsigned short ss_u16;
typedef unsigned int ss_u32;
typedef signed int ss_i32;

typedef char ss_v2_require_u8[(sizeof(ss_u8) == 1U) ? 1 : -1];
typedef char ss_v2_require_u16[(sizeof(ss_u16) == 2U) ? 1 : -1];
typedef char ss_v2_require_u32[(sizeof(ss_u32) == 4U) ? 1 : -1];
typedef char ss_v2_require_i32[(sizeof(ss_i32) == 4U) ? 1 : -1];

#define SS_V2_FALSE ((ss_u32)0U)
#define SS_V2_TRUE ((ss_u32)1U)

#define SS_V2_STATUS_OK ((ss_u32)0U)
#define SS_V2_STATUS_BAD_ARGUMENT ((ss_u32)1U)
#define SS_V2_STATUS_BAD_SIZE ((ss_u32)2U)
#define SS_V2_STATUS_BAD_VERSION ((ss_u32)3U)
#define SS_V2_STATUS_UNSUPPORTED ((ss_u32)4U)
#define SS_V2_STATUS_FAIL ((ss_u32)5U)

#endif /* SCREENSAVE_V2_BASE_H */
