#ifndef SCREENSAVE_PRIVATE_V1_V2_COMPAT_ADAPTER_H
#define SCREENSAVE_PRIVATE_V1_V2_COMPAT_ADAPTER_H

#include "screensave/saver_api.h"
#include "screensave/v2/product.h"

typedef struct ss_v1_v2_compat_session_tag ss_v1_v2_compat_session;

ss_u32 ss_v1_v2_compat_create(
    const ss_v2_product_descriptor *product,
    const screensave_saver_environment *environment,
    const void *product_config,
    ss_u32 product_config_size,
    ss_v1_v2_compat_session **session_out
);

void ss_v1_v2_compat_destroy(ss_v1_v2_compat_session *session);

ss_u32 ss_v1_v2_compat_resize(
    ss_v1_v2_compat_session *session,
    const screensave_saver_environment *environment
);

ss_u32 ss_v1_v2_compat_advance(
    ss_v1_v2_compat_session *session,
    const screensave_saver_environment *environment
);

ss_u32 ss_v1_v2_compat_render(
    ss_v1_v2_compat_session *session,
    const screensave_saver_environment *environment
);

#endif /* SCREENSAVE_PRIVATE_V1_V2_COMPAT_ADAPTER_H */
