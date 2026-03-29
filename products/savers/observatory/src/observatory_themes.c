#include <string.h>

#include "observatory_internal.h"

const screensave_theme_descriptor g_observatory_themes[] = {
    {
        "brass_observatory",
        "Brass Observatory",
        "Warm brass-and-ink exhibit tones with pale dial and body highlights.",
        { 206, 160, 94, 255 },
        { 252, 232, 188, 255 }
    },
    {
        "monochrome_chart_room",
        "Monochrome Chart Room",
        "Graphite and ivory chart tones with restrained astronomical marking.",
        { 180, 186, 196, 255 },
        { 246, 246, 240, 255 }
    },
    {
        "deep_blue_dome",
        "Deep Blue Dome",
        "Cool planetarium blue with pale dome and star-path highlights.",
        { 96, 132, 218, 255 },
        { 214, 234, 255, 255 }
    },
    {
        "eclipse_watch",
        "Eclipse Watch",
        "Dark exhibit palette with gold eclipse-marker accents.",
        { 160, 142, 110, 255 },
        { 244, 218, 132, 255 }
    },
    {
        "quiet_museum_astronomy",
        "Quiet Museum Astronomy",
        "Low-contrast slate astronomy exhibit for restrained long runs.",
        { 132, 142, 158, 255 },
        { 210, 220, 232, 255 }
    }
};

const screensave_theme_descriptor *observatory_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_observatory_themes) / sizeof(g_observatory_themes[0]));
    }

    return g_observatory_themes;
}

const screensave_theme_descriptor *observatory_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = observatory_get_themes(&theme_count);
    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < theme_count; ++index) {
        if (themes[index].theme_key != NULL && strcmp(themes[index].theme_key, theme_key) == 0) {
            return &themes[index];
        }
    }

    return NULL;
}
