#ifndef SCREENSAVE_V2_CONFIG_H
#define SCREENSAVE_V2_CONFIG_H

#include "screensave/v2/base.h"

typedef struct ss_v2_config_view_tag {
    ss_u32 struct_size;
    ss_u32 abi_version;
    const char *product_schema_id;
    ss_u32 schema_version;
    const void *bytes;
    ss_u32 byte_count;
} ss_v2_config_view;

#endif /* SCREENSAVE_V2_CONFIG_H */
