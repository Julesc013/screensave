#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "lifeforms_internal.h"

const screensave_saver_module *nocturne_get_module(void);
const screensave_saver_module *ricochet_get_module(void);
const screensave_saver_module *deepfield_get_module(void);
const screensave_saver_module *ember_get_module(void);
const screensave_saver_module *oscilloscope_dreams_get_module(void);
const screensave_saver_module *pipeworks_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    const screensave_saver_module *modules[7];

    modules[0] = nocturne_get_module();
    modules[1] = ricochet_get_module();
    modules[2] = deepfield_get_module();
    modules[3] = ember_get_module();
    modules[4] = oscilloscope_dreams_get_module();
    modules[5] = pipeworks_get_module();
    modules[6] = lifeforms_get_module();
    return screensave_scr_main_with_registry(
        instance,
        previous,
        command_line,
        show_code,
        lifeforms_get_module(),
        modules,
        (unsigned int)(sizeof(modules) / sizeof(modules[0]))
    );
}
