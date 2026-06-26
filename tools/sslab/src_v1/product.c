#include "runtime_v1.h"

#include "nocturne_core.h"
#include "nocturne_v2_adapter.h"
#include "ricochet_core.h"
#include "ricochet_v2_adapter.h"

#include <stdlib.h>
#include <string.h>

static ss_u32 sslab_v1_nocturne_stream_seed(ss_u32 seed)
{
    (void)seed;
    return (ss_u32)0x0A1E0A1EU;
}

static ss_u32 sslab_v1_ricochet_stream_seed(ss_u32 seed)
{
    return seed ^ (ss_u32)0x9E3779B9U;
}

static const sslab_v1_product_def g_sslab_v1_products[] = {
    {
        "nocturne",
        NOCTURNE_DEFAULT_PRESET_KEY,
        SS_V2_SESSION_MODE_SCREEN,
        sslab_v1_nocturne_stream_seed,
        (ss_u32)sizeof(nocturne_config),
        nocturne_v2_product_descriptor
    },
    {
        "ricochet",
        RICOCHET_DEFAULT_PRESET_KEY,
        SS_V2_SESSION_MODE_WINDOWED,
        sslab_v1_ricochet_stream_seed,
        (ss_u32)sizeof(ricochet_config),
        ricochet_v2_product_descriptor
    }
};

const sslab_v1_product_def *sslab_v1_find_product_def(const char *product_key)
{
    unsigned int index;

    if (product_key == 0) {
        return 0;
    }
    for (index = 0U; index < (unsigned int)(sizeof(g_sslab_v1_products) / sizeof(g_sslab_v1_products[0])); ++index) {
        if (strcmp(g_sslab_v1_products[index].product_key, product_key) == 0) {
            return &g_sslab_v1_products[index];
        }
    }
    return 0;
}

sslab_v1_status sslab_v1_configure_product(
    const sslab_v1_product_def *definition,
    const char *preset_key,
    ss_u8 *config_bytes,
    ss_u32 config_capacity,
    ss_u32 *config_size)
{
    nocturne_config nocturne;
    ricochet_config ricochet;

    if (definition == 0 || config_bytes == 0 || config_size == 0 || config_capacity < definition->config_size) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    (void)preset_key;
    memset(config_bytes, 0, config_capacity);
    if (strcmp(definition->product_key, "nocturne") == 0) {
        nocturne.motion_mode = NOCTURNE_MOTION_MONOLITH;
        nocturne.fade_speed = NOCTURNE_FADE_GENTLE;
        nocturne.motion_strength = NOCTURNE_STRENGTH_SUBTLE;
        memcpy(config_bytes, &nocturne, sizeof(nocturne));
    } else if (strcmp(definition->product_key, "ricochet") == 0) {
        ricochet.object_mode = RICOCHET_OBJECT_BLOCK;
        ricochet.object_count = 1;
        ricochet.speed_mode = RICOCHET_SPEED_STANDARD;
        ricochet.trail_mode = RICOCHET_TRAIL_SHORT;
        memcpy(config_bytes, &ricochet, sizeof(ricochet));
    } else {
        return SSLAB_V1_STATUS_NOT_FOUND;
    }
    *config_size = definition->config_size;
    return SSLAB_V1_STATUS_OK;
}

sslab_v1_status sslab_v1_open_product(sslab_v1_context *context, const char *product_key, sslab_v1_product **product)
{
    sslab_v1_product *opened;
    const sslab_v1_product_def *definition;

    if (context == 0 || product_key == 0 || product == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    *product = 0;
    definition = sslab_v1_find_product_def(product_key);
    if (definition == 0) {
        return SSLAB_V1_STATUS_NOT_FOUND;
    }
    opened = (sslab_v1_product *)calloc(1U, sizeof(*opened));
    if (opened == 0) {
        return SSLAB_V1_STATUS_OUT_OF_MEMORY;
    }
    opened->context = context;
    opened->definition = definition;
    *product = opened;
    return SSLAB_V1_STATUS_OK;
}

void sslab_v1_close_product(sslab_v1_product *product)
{
    free(product);
}
