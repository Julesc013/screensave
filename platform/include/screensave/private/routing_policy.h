#ifndef SCREENSAVE_PRIVATE_ROUTING_POLICY_H
#define SCREENSAVE_PRIVATE_ROUTING_POLICY_H

#include "screensave/config_api.h"
#include "screensave/private/backend_loader.h"
#include "screensave/saver_api.h"

int screensave_routing_prepare_backend_request(
    const screensave_saver_module *module,
    const screensave_common_config *common_config,
    screensave_renderer_kind requested_kind,
    HWND target_window,
    const screensave_sizei *drawable_size,
    screensave_diag_context *diagnostics,
    screensave_backend_request *request_out
);

#endif /* SCREENSAVE_PRIVATE_ROUTING_POLICY_H */
