#include "../../../../platform/src/host/win32_scr/scr_host_entry.h"
#include "fractal_atlas_internal.h"

const screensave_saver_module *nocturne_get_module(void);
const screensave_saver_module *ricochet_get_module(void);
const screensave_saver_module *deepfield_get_module(void);
const screensave_saver_module *ember_get_module(void);
const screensave_saver_module *oscilloscope_dreams_get_module(void);
const screensave_saver_module *pipeworks_get_module(void);
const screensave_saver_module *lifeforms_get_module(void);
const screensave_saver_module *signal_lab_get_module(void);
const screensave_saver_module *mechanical_dreams_get_module(void);
const screensave_saver_module *ecosystems_get_module(void);
const screensave_saver_module *stormglass_get_module(void);
const screensave_saver_module *night_transit_get_module(void);
const screensave_saver_module *observatory_get_module(void);
const screensave_saver_module *vector_worlds_get_module(void);
const screensave_saver_module *retro_explorer_get_module(void);
const screensave_saver_module *city_nocturne_get_module(void);
const screensave_saver_module *fractal_atlas_get_module(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    const screensave_saver_module *modules[17];

    modules[0] = nocturne_get_module();
    modules[1] = ricochet_get_module();
    modules[2] = deepfield_get_module();
    modules[3] = ember_get_module();
    modules[4] = oscilloscope_dreams_get_module();
    modules[5] = pipeworks_get_module();
    modules[6] = lifeforms_get_module();
    modules[7] = signal_lab_get_module();
    modules[8] = mechanical_dreams_get_module();
    modules[9] = ecosystems_get_module();
    modules[10] = stormglass_get_module();
    modules[11] = night_transit_get_module();
    modules[12] = observatory_get_module();
    modules[13] = vector_worlds_get_module();
    modules[14] = retro_explorer_get_module();
    modules[15] = city_nocturne_get_module();
    modules[16] = fractal_atlas_get_module();
    return screensave_scr_main_with_registry(
        instance,
        previous,
        command_line,
        show_code,
        fractal_atlas_get_module(),
        modules,
        (unsigned int)(sizeof(modules) / sizeof(modules[0]))
    );
}
