#include "../product_adapter.h"

#include "nocturne_internal.h"
#include "nocturne_v2_adapter.h"

#include <string.h>

static unsigned long sslab_nocturne_stream_seed(unsigned long seed)
{
    (void)seed;
    return 0x0A1E0A1EUL;
}

static sslab_status sslab_nocturne_configure(
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size)
{
    nocturne_config *config;

    if (common_config == 0 || product_config == 0 || product_config_size < sizeof(nocturne_config)) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }

    config = (nocturne_config *)product_config;
    memset(common_config, 0, sizeof(*common_config));
    memset(config, 0, sizeof(*config));
    common_config->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    common_config->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    common_config->preset_key = NOCTURNE_DEFAULT_PRESET_KEY;
    common_config->theme_key = NOCTURNE_DEFAULT_THEME_KEY;
    config->motion_mode = NOCTURNE_MOTION_MONOLITH;
    config->fade_speed = NOCTURNE_FADE_GENTLE;
    config->motion_strength = NOCTURNE_STRENGTH_SUBTLE;
    nocturne_apply_preset_to_config(preset_key, common_config, config);
    return SSLAB_STATUS_OK;
}

static sslab_status sslab_nocturne_create(
    const sslab_product_adapter_v0 *adapter,
    screensave_saver_session **session,
    const screensave_saver_environment *environment)
{
    (void)adapter;
    return nocturne_create_session(0, session, environment) ? SSLAB_STATUS_OK : SSLAB_STATUS_FAIL;
}

static const sslab_product_adapter_v0 g_sslab_nocturne_adapter = {
    sizeof(sslab_product_adapter_v0),
    SSLAB_ABI_VERSION,
    "nocturne",
    NOCTURNE_DEFAULT_PRESET_KEY,
    sizeof(nocturne_config),
    SCREENSAVE_SESSION_MODE_SCREEN,
    sslab_nocturne_stream_seed,
    sslab_nocturne_configure,
    sslab_nocturne_create,
    nocturne_resize_session,
    nocturne_step_session,
    nocturne_render_session,
    nocturne_destroy_session,
    nocturne_v2_product_descriptor
};

const sslab_product_adapter_v0 *sslab_nocturne_adapter(void)
{
    return &g_sslab_nocturne_adapter;
}
