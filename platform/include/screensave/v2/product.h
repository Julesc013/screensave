#ifndef SCREENSAVE_V2_PRODUCT_H
#define SCREENSAVE_V2_PRODUCT_H

#include "screensave/v2/session.h"

#define SS_V2_PRODUCT_CAP_REFERENCE_CPU ((ss_u32)0x00000001U)
#define SS_V2_PRODUCT_CAP_RGBA8_SURFACE ((ss_u32)0x00000002U)
#define SS_V2_PRODUCT_CAP_FIXED_STEP ((ss_u32)0x00000004U)
#define SS_V2_PRODUCT_CAP_DETERMINISTIC_SEED ((ss_u32)0x00000008U)

typedef struct ss_v2_product_descriptor_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    const char *product_key;
    const char *display_name;
    const char *product_version;
    const char *config_schema_id;
    ss_u32 config_schema_version;
    ss_u32 capability_flags;
    const ss_v2_session_ops *session_ops;
} ss_v2_product_descriptor;

ss_u32 ss_v2_product_descriptor_is_valid(const ss_v2_product_descriptor *product);

#endif /* SCREENSAVE_V2_PRODUCT_H */
