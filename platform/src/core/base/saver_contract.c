#include "screensave/saver_api.h"

int screensave_saver_module_is_valid(const screensave_saver_module *module)
{
    const screensave_saver_config_hooks *config_hooks;

    if (module == NULL) {
        return 0;
    }

    if (
        module->identity.product_key == NULL ||
        module->identity.display_name == NULL ||
        module->identity.product_key[0] == '\0' ||
        module->identity.display_name[0] == '\0'
    ) {
        return 0;
    }

    if ((module->capability_flags & SCREENSAVE_SAVER_CAP_UNIVERSAL_GDI) == 0UL) {
        return 0;
    }

    if (module->preset_count > 0U && module->presets == NULL) {
        return 0;
    }

    if (module->theme_count > 0U && module->themes == NULL) {
        return 0;
    }

    config_hooks = module->config_hooks;
    if (config_hooks != NULL) {
        if (config_hooks->set_defaults == NULL || config_hooks->clamp == NULL) {
            return 0;
        }
    }

    return 1;
}

const char *screensave_session_mode_name(screensave_session_mode mode)
{
    switch (mode) {
    case SCREENSAVE_SESSION_MODE_SCREEN:
        return "screen";

    case SCREENSAVE_SESSION_MODE_PREVIEW:
        return "preview";

    case SCREENSAVE_SESSION_MODE_WINDOWED:
        return "windowed";

    case SCREENSAVE_SESSION_MODE_CONFIG:
    default:
        return "config";
    }
}
