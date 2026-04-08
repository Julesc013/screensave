#ifndef SCREENSAVE_PRIVATE_BACKEND_CAPS_H
#define SCREENSAVE_PRIVATE_BACKEND_CAPS_H

#include "screensave/renderer_api.h"
#include "screensave/private/backend_kind.h"
#include "screensave/private/render_band.h"

#define SCREENSAVE_BACKEND_CAPS_STATE_AVAILABLE        0x00000001UL
#define SCREENSAVE_BACKEND_CAPS_STATE_PLACEHOLDER      0x00000002UL
#define SCREENSAVE_BACKEND_CAPS_STATE_DOUBLE_BUFFERED  0x00000004UL
#define SCREENSAVE_BACKEND_CAPS_STATE_SUPPORT_GDI      0x00000008UL
#define SCREENSAVE_BACKEND_CAPS_STATE_GENERIC_FORMAT   0x00000010UL
#define SCREENSAVE_BACKEND_CAPS_STATE_ADVANCED_CONTEXT 0x00000020UL
#define SCREENSAVE_BACKEND_CAPS_STATE_COMPAT_PROFILE   0x00000040UL
#define SCREENSAVE_BACKEND_CAPS_STATE_MODERN_CONTEXT   0x00000080UL

typedef struct screensave_backend_caps_tag {
    screensave_backend_kind backend_kind;
    screensave_render_band band;
    unsigned long state_flags;
    unsigned long public_capability_flags;
    unsigned long private_capability_flags;
    unsigned long required_private_capability_flags;
    unsigned long preferred_private_capability_flags;
    unsigned long missing_private_capability_flags;
    int rgba_bits;
    int depth_bits;
    int major_version;
    int minor_version;
    char vendor[64];
    char renderer[96];
    char version[64];
    char failure_reason[64];
} screensave_backend_caps;

void screensave_backend_caps_init(
    screensave_backend_caps *caps,
    screensave_backend_kind backend_kind,
    screensave_render_band band
);
void screensave_backend_caps_apply_renderer_info(
    screensave_backend_caps *caps,
    const screensave_renderer_info *info
);
void screensave_backend_caps_mark_failure(
    screensave_backend_caps *caps,
    const char *failure_reason,
    int placeholder_only
);

#endif /* SCREENSAVE_PRIVATE_BACKEND_CAPS_H */
