#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "atlas_internal.h"

const screensave_saver_module *atlas_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    return screensave_scr_main(instance, previous, command_line, show_code, atlas_get_module());
}
