#include "../product_adapter.h"

#include "ricochet_internal.h"
#include "ricochet_v2_adapter.h"

static unsigned long sslab_ricochet_stream_seed(unsigned long seed)
{
    return seed ^ 0x9E3779B9UL;
}

static sslab_status sslab_ricochet_configure(
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size)
{
    if (common_config == 0 || product_config == 0 || product_config_size < sizeof(ricochet_config)) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }

    screensave_common_config_set_defaults(common_config);
    ricochet_apply_preset_to_config(preset_key, common_config, (ricochet_config *)product_config);
    return SSLAB_STATUS_OK;
}

static sslab_status sslab_ricochet_create(
    const sslab_product_adapter_v0 *adapter,
    screensave_saver_session **session,
    const screensave_saver_environment *environment)
{
    (void)adapter;
    return ricochet_create_session(0, session, environment) ? SSLAB_STATUS_OK : SSLAB_STATUS_FAIL;
}

static const sslab_product_adapter_v0 g_sslab_ricochet_adapter = {
    sizeof(sslab_product_adapter_v0),
    SSLAB_ABI_VERSION,
    "ricochet",
    RICOCHET_DEFAULT_PRESET_KEY,
    sizeof(ricochet_config),
    SCREENSAVE_SESSION_MODE_WINDOWED,
    sslab_ricochet_stream_seed,
    sslab_ricochet_configure,
    sslab_ricochet_create,
    ricochet_resize_session,
    ricochet_step_session,
    ricochet_render_session,
    ricochet_destroy_session,
    ricochet_v2_product_descriptor
};

const sslab_product_adapter_v0 *sslab_ricochet_adapter(void)
{
    return &g_sslab_ricochet_adapter;
}
