#include "screensave/v2/product.h"

#include "screensave/v2/internal/validate.h"

ss_u32 ss_v2_session_desc_is_valid(const ss_v2_session_desc *desc)
{
    ss_u32 status;

    status = ss_v2_check_prefix(desc, (ss_u32)sizeof(*desc), (ss_u32)sizeof(*desc), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = ss_v2_check_prefix(&desc->dimensions, (ss_u32)sizeof(desc->dimensions), (ss_u32)sizeof(desc->dimensions), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = ss_v2_check_prefix(&desc->seed, (ss_u32)sizeof(desc->seed), (ss_u32)sizeof(desc->seed), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = ss_v2_check_prefix(&desc->clock, (ss_u32)sizeof(desc->clock), (ss_u32)sizeof(desc->clock), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return ss_v2_config_view_is_valid(&desc->product_config);
}

ss_u32 ss_v2_resize_desc_is_valid(const ss_v2_resize_desc *desc)
{
    ss_u32 status;

    status = ss_v2_check_prefix(desc, (ss_u32)sizeof(*desc), (ss_u32)sizeof(*desc), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return ss_v2_check_prefix(&desc->dimensions, (ss_u32)sizeof(desc->dimensions), (ss_u32)sizeof(desc->dimensions), SS_V2_ABI_VERSION);
}

ss_u32 ss_v2_advance_desc_is_valid(const ss_v2_advance_desc *desc)
{
    ss_u32 status;

    status = ss_v2_check_prefix(desc, (ss_u32)sizeof(*desc), (ss_u32)sizeof(*desc), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    return ss_v2_check_prefix(&desc->clock, (ss_u32)sizeof(desc->clock), (ss_u32)sizeof(desc->clock), SS_V2_ABI_VERSION);
}

ss_u32 ss_v2_render_desc_is_valid(const ss_v2_render_desc *desc)
{
    ss_u32 status;

    status = ss_v2_check_prefix(desc, (ss_u32)sizeof(*desc), (ss_u32)sizeof(*desc), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    status = ss_v2_check_prefix(&desc->clock, (ss_u32)sizeof(desc->clock), (ss_u32)sizeof(desc->clock), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (desc->draw_target == 0 && desc->surface == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (desc->draw_target != 0) {
        status = ss_v2_draw_target_is_valid(desc->draw_target);
        if (status != SS_V2_STATUS_OK) {
            return status;
        }
    }
    if (desc->surface != 0) {
        status = ss_v2_surface_desc_is_valid(desc->surface);
        if (status != SS_V2_STATUS_OK) {
            return status;
        }
    }
    return SS_V2_STATUS_OK;
}

ss_u32 ss_v2_session_ops_is_valid(const ss_v2_session_ops *ops)
{
    ss_u32 status;

    status = ss_v2_check_prefix(ops, (ss_u32)sizeof(*ops), (ss_u32)sizeof(*ops), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (ops->create == 0 || ops->destroy == 0 || ops->resize == 0 || ops->advance == 0 || ops->render == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return SS_V2_STATUS_OK;
}

ss_u32 ss_v2_product_descriptor_is_valid(const ss_v2_product_descriptor *product)
{
    ss_u32 status;

    status = ss_v2_check_prefix(product, (ss_u32)sizeof(*product), (ss_u32)sizeof(*product), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (product->product_key == 0 || product->product_key[0] == '\0') {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (product->config_schema_id == 0 || product->config_schema_id[0] == '\0') {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (product->config_schema_version == 0U || product->session_ops == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return ss_v2_session_ops_is_valid(product->session_ops);
}
