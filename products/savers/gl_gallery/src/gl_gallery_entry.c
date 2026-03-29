#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "gl_gallery_internal.h"

const screensave_saver_module *gl_gallery_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    const screensave_saver_module *modules[1];

    modules[0] = gl_gallery_get_module();
    return screensave_scr_main_with_registry(
        instance,
        previous,
        command_line,
        show_code,
        gl_gallery_get_module(),
        modules,
        1U
    );
}
