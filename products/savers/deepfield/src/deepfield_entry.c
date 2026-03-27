#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "deepfield_internal.h"

const screensave_saver_module *nocturne_get_module(void);
const screensave_saver_module *ricochet_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    const screensave_saver_module *modules[3];

    modules[0] = nocturne_get_module();
    modules[1] = ricochet_get_module();
    modules[2] = deepfield_get_module();
    return screensave_scr_main_with_registry(
        instance,
        previous,
        command_line,
        show_code,
        deepfield_get_module(),
        modules,
        (unsigned int)(sizeof(modules) / sizeof(modules[0]))
    );
}
