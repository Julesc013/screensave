#include <string.h>

#include "template_saver_internal.h"

const screensave_theme_descriptor g_template_saver_themes[TEMPLATE_SAVER_THEME_COUNT] = {
    {
        "starter_blue",
        "Starter Blue",
        "Cool blue starter theme for the default template preset.",
        { 0xA4, 0xC4, 0xFF, 0xFF },
        { 0x36, 0x5E, 0x9A, 0xFF }
    },
    {
        "starter_amber",
        "Starter Amber",
        "Warmer accent theme for higher-contrast template variants.",
        { 0xFF, 0xC1, 0x7A, 0xFF },
        { 0x8E, 0x53, 0x12, 0xFF }
    },
    {
        "starter_mono",
        "Starter Mono",
        "Monochrome fallback theme for calm or low-distraction products.",
        { 0xC8, 0xC8, 0xC8, 0xFF },
        { 0x48, 0x48, 0x48, 0xFF }
    }
};

const screensave_theme_descriptor *template_saver_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = TEMPLATE_SAVER_THEME_COUNT;
    }

    return g_template_saver_themes;
}

const screensave_theme_descriptor *template_saver_find_theme_descriptor(const char *theme_key)
{
    unsigned int index;

    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < TEMPLATE_SAVER_THEME_COUNT; ++index) {
        if (strcmp(g_template_saver_themes[index].theme_key, theme_key) == 0) {
            return &g_template_saver_themes[index];
        }
    }

    return NULL;
}
