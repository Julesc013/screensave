#ifndef SCREENSAVE_DISPLAY_TEXT_H
#define SCREENSAVE_DISPLAY_TEXT_H

#include "screensave/config_api.h"
#include "screensave/renderer_api.h"

const char *screensave_display_detail_level(screensave_detail_level level);
const char *screensave_display_randomization_mode(screensave_randomization_mode mode);
const char *screensave_display_renderer_kind(screensave_renderer_kind kind);
void screensave_display_renderer_reason(
    const char *reason_code,
    char *buffer,
    unsigned int buffer_size
);
void screensave_display_renderer_status(
    const char *status_code,
    char *buffer,
    unsigned int buffer_size
);

#endif /* SCREENSAVE_DISPLAY_TEXT_H */
