#include "../product_adapter.h"

#include "plasma_migration_v2.h"
#include "plasma_v2_adapter.h"

#include <string.h>

static unsigned long sslab_plasma_stream_seed(unsigned long seed)
{
    return seed ^ 0x506c5632UL;
}

static sslab_status sslab_plasma_configure(
    const char *preset_key,
    screensave_common_config *common_config,
    void *product_config,
    unsigned int product_config_size)
{
    plasma_spec_v2 spec;
    ss_u32 status;

    if (common_config == 0 || product_config == 0 || product_config_size < sizeof(plasma_spec_v2)) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }

    memset(common_config, 0, sizeof(*common_config));
    common_config->schema_version = SCREENSAVE_CONFIG_SCHEMA_VERSION;
    common_config->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;
    common_config->preset_key = "plasma_lava";
    common_config->theme_key = "plasma_lava";

    status = plasma_migration_v2_apply_u09(preset_key, 0, &spec);
    if (status != SS_V2_STATUS_OK) {
        status = plasma_migration_v2_apply_u09("plasma_lava", 0, &spec);
        if (status != SS_V2_STATUS_OK) {
            return SSLAB_STATUS_INVALID_ARGUMENT;
        }
    }
    if (preset_key != 0 && strcmp(preset_key, "plasma_lava_material_matrix") == 0) {
        spec.material_id = PLASMA_V2_MATERIAL_AURORA_COOL;
        spec.treatment_flags = PLASMA_V2_TREATMENT_RESTRAINED_DITHER;
    } else if (preset_key != 0 && strcmp(preset_key, "plasma_lava_controls_matrix") == 0) {
        spec.field_family = PLASMA_V2_FIELD_RADIAL_WARPED;
        spec.scale = (ss_u32)70U;
        spec.complexity = (ss_u32)88U;
        spec.motion_speed = (ss_u32)85U;
        spec.warp_amount = (ss_u32)60U;
        spec.feedback_amount = (ss_u32)45U;
        spec.output_style = PLASMA_V2_OUTPUT_CONTOUR;
        spec.brightness = (ss_u32)70U;
        spec.contrast = (ss_u32)60U;
        spec.treatment_flags = PLASMA_V2_TREATMENT_RESTRAINED_CRT;
    } else if (preset_key != 0 && strcmp(preset_key, "plasma_visualintent_candidate_matrix") == 0) {
        spec.field_family = PLASMA_V2_FIELD_RADIAL_WARPED;
        spec.scale = (ss_u32)92U;
        spec.complexity = (ss_u32)72U;
        spec.motion_speed = (ss_u32)24U;
        spec.warp_amount = (ss_u32)36U;
        spec.feedback_amount = (ss_u32)10U;
        spec.output_style = PLASMA_V2_OUTPUT_BANDED;
        spec.material_id = PLASMA_V2_MATERIAL_MUSEUM_PHOSPHOR;
        spec.brightness = (ss_u32)42U;
        spec.contrast = (ss_u32)74U;
        spec.treatment_flags = PLASMA_V2_TREATMENT_RESTRAINED_CRT;
    }
    plasma_spec_v2_clamp(&spec);
    memcpy(product_config, &spec, sizeof(spec));
    return SSLAB_STATUS_OK;
}

static sslab_status sslab_plasma_create_unavailable(
    const sslab_product_adapter_v0 *adapter,
    screensave_saver_session **session,
    const screensave_saver_environment *environment)
{
    (void)adapter;
    (void)session;
    (void)environment;
    return SSLAB_STATUS_NOT_FOUND;
}

static void sslab_plasma_noop_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment)
{
    (void)session;
    (void)environment;
}

static void sslab_plasma_destroy_noop(screensave_saver_session *session)
{
    (void)session;
}

static const sslab_product_adapter_v0 g_sslab_plasma_adapter = {
    sizeof(sslab_product_adapter_v0),
    SSLAB_ABI_VERSION,
    "plasma",
    "plasma_lava",
    sizeof(plasma_spec_v2),
    SCREENSAVE_SESSION_MODE_WINDOWED,
    sslab_plasma_stream_seed,
    sslab_plasma_configure,
    sslab_plasma_create_unavailable,
    sslab_plasma_noop_session,
    sslab_plasma_noop_session,
    sslab_plasma_noop_session,
    sslab_plasma_destroy_noop,
    plasma_v2_product_descriptor
};

const sslab_product_adapter_v0 *sslab_plasma_adapter(void)
{
    return &g_sslab_plasma_adapter;
}
