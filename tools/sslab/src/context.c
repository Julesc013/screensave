#include "runtime.h"

#include <stdlib.h>
#include <string.h>

sslab_status sslab_create_context(const sslab_context_desc *desc, sslab_context **context)
{
    sslab_context *created;
    sslab_status status;

    if (context == 0 || desc == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    *context = 0;
    status = sslab_validate_desc(desc->size, sizeof(*desc), desc->abi_version);
    if (status != SSLAB_STATUS_OK) {
        return status;
    }
    status = sslab_check_host_abi();
    if (status != SSLAB_STATUS_OK) {
        return status;
    }

    created = (sslab_context *)calloc(1U, sizeof(*created));
    if (created == 0) {
        return SSLAB_STATUS_OUT_OF_MEMORY;
    }
    if (!sslab_copy_text(created->output_root, sizeof(created->output_root), desc->output_root)) {
        free(created);
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    if (!sslab_copy_text(created->catalog_root, sizeof(created->catalog_root), desc->catalog_root)) {
        free(created);
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    created->execution_path = SSLAB_EXECUTION_PATH_V1;

    *context = created;
    return SSLAB_STATUS_OK;
}

void sslab_destroy_context(sslab_context *context)
{
    free(context);
}

sslab_status sslab_set_execution_path(sslab_context *context, sslab_execution_path path)
{
    if (context == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    if (path != SSLAB_EXECUTION_PATH_V1 && path != SSLAB_EXECUTION_PATH_V2) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    context->execution_path = path;
    return SSLAB_STATUS_OK;
}

sslab_execution_path sslab_get_execution_path(const sslab_context *context)
{
    if (context == 0) {
        return SSLAB_EXECUTION_PATH_V1;
    }
    return context->execution_path;
}
