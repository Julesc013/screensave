#include "timing_internal.h"

void screensave_timebase_reset(screensave_timebase *timebase)
{
    DWORD tick;

    if (timebase == NULL) {
        return;
    }

    tick = GetTickCount();
    timebase->start_tick = tick;
    timebase->last_tick = tick;
    timebase->frame_index = 0UL;
}

void screensave_timebase_sample(screensave_timebase *timebase, screensave_runtime_clock *clock)
{
    DWORD tick;

    if (timebase == NULL || clock == NULL) {
        return;
    }

    tick = GetTickCount();
    clock->session_start_millis = (unsigned long)timebase->start_tick;
    clock->elapsed_millis = (unsigned long)(tick - timebase->start_tick);
    clock->delta_millis = (unsigned long)(tick - timebase->last_tick);
    clock->frame_index = timebase->frame_index;

    timebase->last_tick = tick;
    timebase->frame_index += 1UL;
}
