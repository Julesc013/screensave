/*
 * Series 03 wires the first real host skeleton into the product target.
 * This file provides product identity only. It does not implement saver logic.
 */

#include <windows.h>

#include "screensave/scr_entry.h"

static const screensave_product_identity g_nocturne_identity = {
    "nocturne",
    "Nocturne"
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    return screensave_scr_main(instance, previous, command_line, show_code, &g_nocturne_identity);
}
