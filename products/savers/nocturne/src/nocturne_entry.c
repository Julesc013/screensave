/*
 * Series 05 keeps the first narrow saver/module contract wired into the product target.
 * This remains a no-op module shell. It does not implement saver visuals yet.
 */

#include <windows.h>

#include "screensave/saver_api.h"

static const screensave_saver_module g_nocturne_module = {
    {
        "nocturne",
        "Nocturne",
        "Series 05 no-op saver module shell."
    },
    SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI | SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE,
    NULL,
    0U,
    NULL,
    0U,
    NULL
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    return screensave_scr_main(instance, previous, command_line, show_code, &g_nocturne_module);
}
