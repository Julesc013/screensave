#include "runtime_v1.h"

#include <stdlib.h>
#include <string.h>

int sslab_v1_copy_text(char *target, unsigned int target_size, const char *source)
{
    unsigned int index;

    if (target == 0 || target_size == 0U) {
        return 0;
    }
    if (source == 0) {
        target[0] = '\0';
        return 1;
    }
    index = 0U;
    while (index + 1U < target_size && source[index] != '\0') {
        target[index] = source[index];
        ++index;
    }
    target[index] = '\0';
    return source[index] == '\0';
}

sslab_v1_status sslab_v1_check_prefix(ss_u32 struct_size, ss_u32 minimum_size, ss_u32 abi_version)
{
    if (struct_size < minimum_size || abi_version != SSLAB_V1_ABI_VERSION) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    return SSLAB_V1_STATUS_OK;
}

sslab_v1_status sslab_v1_status_from_v2(ss_u32 status)
{
    switch (status) {
    case SS_V2_STATUS_OK:
        return SSLAB_V1_STATUS_OK;
    case SS_V2_STATUS_BAD_ARGUMENT:
    case SS_V2_STATUS_BAD_SIZE:
    case SS_V2_STATUS_BAD_VERSION:
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    case SS_V2_STATUS_UNSUPPORTED:
        return SSLAB_V1_STATUS_UNSUPPORTED_HOST;
    case SS_V2_STATUS_FAIL:
    default:
        return SSLAB_V1_STATUS_FAIL;
    }
}

sslab_v1_status sslab_v1_get_abi_info(sslab_v1_abi_info *info)
{
    if (info == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    memset(info, 0, sizeof(*info));
    info->struct_size = (ss_u32)sizeof(*info);
    info->abi_version = SSLAB_V1_ABI_VERSION;
    info->max_capture_frames = SSLAB_V1_MAX_CAPTURE_FRAMES;
    info->portable_v2_abi_version = SS_V2_ABI_VERSION;
    return SSLAB_V1_STATUS_OK;
}

sslab_v1_status sslab_v1_create_context(const sslab_v1_context_desc *desc, sslab_v1_context **context)
{
    sslab_v1_context *created;
    sslab_v1_status status;

    if (desc == 0 || context == 0) {
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    *context = 0;
    status = sslab_v1_check_prefix(desc->struct_size, (ss_u32)sizeof(*desc), desc->abi_version);
    if (status != SSLAB_V1_STATUS_OK) {
        return status;
    }
    created = (sslab_v1_context *)calloc(1U, sizeof(*created));
    if (created == 0) {
        return SSLAB_V1_STATUS_OUT_OF_MEMORY;
    }
    if (!sslab_v1_copy_text(created->output_root, sizeof(created->output_root), desc->output_root)) {
        free(created);
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    if (!sslab_v1_copy_text(created->catalog_root, sizeof(created->catalog_root), desc->catalog_root)) {
        free(created);
        return SSLAB_V1_STATUS_INVALID_ARGUMENT;
    }
    *context = created;
    return SSLAB_V1_STATUS_OK;
}

void sslab_v1_destroy_context(sslab_v1_context *context)
{
    free(context);
}
