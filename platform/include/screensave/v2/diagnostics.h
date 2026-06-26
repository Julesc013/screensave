#ifndef SCREENSAVE_V2_DIAGNOSTICS_H
#define SCREENSAVE_V2_DIAGNOSTICS_H

#include "screensave/v2/base.h"

#define SS_V2_DIAG_LEVEL_ERROR ((ss_u32)0U)
#define SS_V2_DIAG_LEVEL_WARNING ((ss_u32)1U)
#define SS_V2_DIAG_LEVEL_INFO ((ss_u32)2U)
#define SS_V2_DIAG_LEVEL_DEBUG ((ss_u32)3U)

#define SS_V2_DIAG_DOMAIN_PLATFORM ((ss_u32)0U)
#define SS_V2_DIAG_DOMAIN_HOST ((ss_u32)1U)
#define SS_V2_DIAG_DOMAIN_CONFIG ((ss_u32)2U)
#define SS_V2_DIAG_DOMAIN_RENDERER ((ss_u32)3U)
#define SS_V2_DIAG_DOMAIN_PRODUCT ((ss_u32)4U)
#define SS_V2_DIAG_DOMAIN_PROOF ((ss_u32)5U)

typedef struct ss_v2_diag_message_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_u32 level;
    ss_u32 domain;
    ss_u32 code;
    const char *origin;
    const char *text;
} ss_v2_diag_message;

typedef void (*ss_v2_diag_sink_fn)(void *user_data, const ss_v2_diag_message *message);

typedef struct ss_v2_diag_context_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    ss_v2_diag_sink_fn sink;
    void *user_data;
    ss_u32 minimum_level;
} ss_v2_diag_context;

#endif /* SCREENSAVE_V2_DIAGNOSTICS_H */
