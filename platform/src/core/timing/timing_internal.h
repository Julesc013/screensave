#ifndef SCREENSAVE_TIMING_INTERNAL_H
#define SCREENSAVE_TIMING_INTERNAL_H

#include <windows.h>

#include "screensave/saver_api.h"

typedef struct screensave_timebase_tag {
    DWORD start_tick;
    DWORD last_tick;
    unsigned long frame_index;
} screensave_timebase;

void screensave_timebase_reset(screensave_timebase *timebase);
void screensave_timebase_sample(screensave_timebase *timebase, screensave_runtime_clock *clock);

#endif /* SCREENSAVE_TIMING_INTERNAL_H */
