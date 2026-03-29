#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "lifeforms_internal.h"

const screensave_saver_module *lifeforms_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    return screensave_scr_main(instance, previous, command_line, show_code, lifeforms_get_module());
}
