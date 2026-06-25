#include "runtime.h"

#include <string.h>

int sslab_copy_text(char *target, unsigned int target_size, const char *source)
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

sslab_status sslab_validate_desc(unsigned long size, unsigned long expected_size, unsigned long abi_version)
{
    if (size < expected_size || abi_version != SSLAB_ABI_VERSION) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    return SSLAB_STATUS_OK;
}

sslab_status sslab_get_abi_info(sslab_abi_info *info)
{
    if (info == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    memset(info, 0, sizeof(*info));
    info->size = (sslab_u32)sizeof(*info);
    info->abi_version = SSLAB_ABI_VERSION;
    info->required_unsigned_long_bytes = SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES;
    info->observed_unsigned_long_bytes = (sslab_u32)sizeof(unsigned long);
    info->max_capture_frames = SSLAB_MAX_CAPTURE_FRAMES;
    return SSLAB_STATUS_OK;
}

sslab_status sslab_check_host_abi(void)
{
    if (sizeof(unsigned long) != SSLAB_ABI_REQUIRED_UNSIGNED_LONG_BYTES) {
        return SSLAB_STATUS_UNSUPPORTED_HOST;
    }
    return SSLAB_STATUS_OK;
}
