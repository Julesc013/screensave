#include "screensave/version.h"

static const screensave_version_info g_screensave_version_info = {
    SCREENSAVE_VERSION_MAJOR,
    SCREENSAVE_VERSION_MINOR,
    SCREENSAVE_VERSION_PATCH,
    SCREENSAVE_VERSION_TEXT,
    SCREENSAVE_VERSION_SERIES
};

const screensave_version_info *screensave_version_get_info(void)
{
    return &g_screensave_version_info;
}

const char *screensave_version_get_text(void)
{
    return g_screensave_version_info.version_text;
}
