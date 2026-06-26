#include "runtime_v1.h"

const char *sslab_v1_status_name(sslab_v1_status status)
{
    switch (status) {
    case SSLAB_V1_STATUS_OK:
        return "pass";
    case SSLAB_V1_STATUS_BLOCKED:
        return "blocked";
    case SSLAB_V1_STATUS_UNSUPPORTED_HOST:
        return "unsupported-host";
    case SSLAB_V1_STATUS_INVALID_ARGUMENT:
        return "invalid-argument";
    case SSLAB_V1_STATUS_NOT_FOUND:
        return "not-found";
    case SSLAB_V1_STATUS_OUT_OF_MEMORY:
        return "out-of-memory";
    case SSLAB_V1_STATUS_FAIL:
    default:
        return "fail";
    }
}
