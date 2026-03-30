#include <string.h>

#include "phosphor_internal.h"

const screensave_theme_descriptor g_phosphor_themes[] = {
    {
        "amber_scope",
        "Amber Scope",
        "Warm amber phosphor on a dark instrument background.",
        { 220, 148, 64, 255 },
        { 255, 214, 144, 255 }
    },
    {
        "green_vector",
        "Green Vector",
        "Classic green scope phosphor.",
        { 64, 220, 120, 255 },
        { 196, 255, 208, 255 }
    },
    {
        "blue_lab",
        "Blue Lab",
        "Cool laboratory-style blue phosphor.",
        { 88, 176, 255, 255 },
        { 212, 240, 255, 255 }
    },
    {
        "drafting_board",
        "Drafting Board",
        "Light drafting-board presentation with darker vector traces.",
        { 28, 80, 124, 255 },
        { 112, 160, 208, 255 }
    },
    {
        "museum_quiet",
        "Museum Quiet",
        "Restrained low-contrast trace palette for long quiet runs.",
        { 172, 188, 200, 255 },
        { 244, 248, 250, 255 }
    },
    {
        "amber_harmonics",
        "Amber Harmonics",
        "Warm amber phosphor with quieter laboratory-black contrast.",
        { 212, 146, 68, 255 },
        { 255, 214, 136, 255 }
    },
    {
        "white_instrument",
        "White Instrument",
        "Pale instrument-white trace for restrained laboratory and drafting-room runs.",
        { 220, 232, 236, 255 },
        { 255, 255, 255, 255 }
    }
};

const screensave_theme_descriptor *phosphor_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_phosphor_themes) / sizeof(g_phosphor_themes[0]));
    }

    return g_phosphor_themes;
}

const screensave_theme_descriptor *phosphor_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = phosphor_get_themes(&theme_count);
    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < theme_count; ++index) {
        if (
            themes[index].theme_key != NULL &&
            strcmp(themes[index].theme_key, theme_key) == 0
        ) {
            return &themes[index];
        }
    }

    return NULL;
}
