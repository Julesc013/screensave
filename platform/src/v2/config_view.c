#include "screensave/v2/config.h"

#include "screensave/v2/internal/validate.h"

ss_u32 ss_v2_config_view_is_valid(const ss_v2_config_view *view)
{
    ss_u32 status;

    status = ss_v2_check_prefix(view, (ss_u32)sizeof(*view), (ss_u32)sizeof(*view), SS_V2_ABI_VERSION);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    if (view->product_schema_id == 0 || view->product_schema_id[0] == '\0') {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (view->schema_version == 0U) {
        return SS_V2_STATUS_BAD_VERSION;
    }
    if (view->byte_count != 0U && view->bytes == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return SS_V2_STATUS_OK;
}
