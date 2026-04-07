#ifndef SCREENSAVE_PRIVATE_PRESENT_PATH_H
#define SCREENSAVE_PRIVATE_PRESENT_PATH_H

#include <windows.h>

#include "screensave/renderer_api.h"
#include "screensave/private/backend_caps.h"

#define SCREENSAVE_PRESENT_PATH_FLAG_NONE                0x00000000UL
#define SCREENSAVE_PRESENT_PATH_FLAG_USES_WINDOW_DC      0x00000001UL
#define SCREENSAVE_PRESENT_PATH_FLAG_ACCEPTS_EXTERNAL_DC 0x00000002UL
#define SCREENSAVE_PRESENT_PATH_FLAG_SWAP_BUFFERS        0x00000004UL
#define SCREENSAVE_PRESENT_PATH_FLAG_NOOP_PRESENT        0x00000008UL

typedef struct screensave_present_path_tag {
    screensave_backend_kind backend_kind;
    screensave_render_band active_band;
    unsigned long flags;
    const char *path_name;
} screensave_present_path;

void screensave_present_path_init(
    screensave_present_path *path,
    screensave_backend_kind backend_kind,
    screensave_render_band active_band,
    unsigned long descriptor_flags,
    const screensave_backend_caps *caps
);
int screensave_present_path_resize_renderer(
    screensave_renderer *renderer,
    const screensave_sizei *drawable_size
);
void screensave_present_path_set_present_dc(screensave_renderer *renderer, HDC present_dc);
void screensave_present_path_clear_present_dc(screensave_renderer *renderer);

#endif /* SCREENSAVE_PRIVATE_PRESENT_PATH_H */
