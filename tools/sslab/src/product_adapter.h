#ifndef SCREENSAVE_SSLAB_PRODUCT_ADAPTER_H
#define SCREENSAVE_SSLAB_PRODUCT_ADAPTER_H

#include "screensave/sslab.h"
#include "screensave/saver_api.h"
#include "screensave/v2/product.h"

typedef struct sslab_product_adapter_v0 {
    unsigned long size;
    unsigned long version;
    const char *product_key;
    const char *default_preset_key;
    unsigned int product_config_size;
    screensave_session_mode session_mode;
    unsigned long (*stream_seed)(unsigned long seed);
    sslab_status (*configure)(
        const char *preset_key,
        screensave_common_config *common_config,
        void *product_config,
        unsigned int product_config_size);
    sslab_status (*create)(
        const struct sslab_product_adapter_v0 *adapter,
        screensave_saver_session **session,
        const screensave_saver_environment *environment);
    void (*resize)(
        screensave_saver_session *session,
        const screensave_saver_environment *environment);
    void (*step)(
        screensave_saver_session *session,
        const screensave_saver_environment *environment);
    void (*render)(
        screensave_saver_session *session,
        const screensave_saver_environment *environment);
    void (*destroy)(screensave_saver_session *session);
    const ss_v2_product_descriptor *(*v2_descriptor)(void);
} sslab_product_adapter_v0;

const sslab_product_adapter_v0 *sslab_nocturne_adapter(void);
const sslab_product_adapter_v0 *sslab_ricochet_adapter(void);
const sslab_product_adapter_v0 *sslab_plasma_adapter(void);

#endif
