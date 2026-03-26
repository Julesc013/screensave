#include <windows.h>

#include "nocturne_internal.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    return screensave_scr_main(instance, previous, command_line, show_code, nocturne_get_module());
}
