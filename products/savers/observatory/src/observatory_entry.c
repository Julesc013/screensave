#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "observatory_internal.h"

const screensave_saver_module *nocturne_get_module(void);
const screensave_saver_module *ricochet_get_module(void);
const screensave_saver_module *deepfield_get_module(void);
const screensave_saver_module *plasma_get_module(void);
const screensave_saver_module *phosphor_get_module(void);
const screensave_saver_module *pipeworks_get_module(void);
const screensave_saver_module *lifeforms_get_module(void);
const screensave_saver_module *signals_get_module(void);
const screensave_saver_module *mechanize_get_module(void);
const screensave_saver_module *ecosystems_get_module(void);
const screensave_saver_module *stormglass_get_module(void);
const screensave_saver_module *transit_get_module(void);
const screensave_saver_module *vector_get_module(void);
const screensave_saver_module *explorer_get_module(void);
const screensave_saver_module *city_get_module(void);
const screensave_saver_module *atlas_get_module(void);
const screensave_saver_module *gallery_get_module(void);
const screensave_saver_module *observatory_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    const screensave_saver_module *modules[18];

    modules[0] = nocturne_get_module();
    modules[1] = ricochet_get_module();
    modules[2] = deepfield_get_module();
    modules[3] = plasma_get_module();
    modules[4] = phosphor_get_module();
    modules[5] = pipeworks_get_module();
    modules[6] = lifeforms_get_module();
    modules[7] = signals_get_module();
    modules[8] = mechanize_get_module();
    modules[9] = ecosystems_get_module();
    modules[10] = stormglass_get_module();
    modules[11] = transit_get_module();
    modules[12] = observatory_get_module();
    modules[13] = vector_get_module();
    modules[14] = explorer_get_module();
    modules[15] = city_get_module();
    modules[16] = atlas_get_module();
    modules[17] = gallery_get_module();
    return screensave_scr_main_with_registry(
        instance,
        previous,
        command_line,
        show_code,
        observatory_get_module(),
        modules,
        (unsigned int)(sizeof(modules) / sizeof(modules[0]))
    );
}
