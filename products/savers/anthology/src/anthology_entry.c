#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "anthology_internal.h"

const screensave_saver_module *anthology_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    return screensave_scr_main(instance, previous, command_line, show_code, anthology_get_module());
}
