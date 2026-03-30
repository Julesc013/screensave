#include <string.h>

#include "lifeforms_internal.h"

const screensave_theme_descriptor g_lifeforms_themes[] = {
    {
        "classic_mono",
        "Classic Mono",
        "Monochrome classic presentation with restrained white-on-black cells.",
        { 212, 212, 212, 255 },
        { 255, 255, 255, 255 }
    },
    {
        "laboratory_age",
        "Laboratory Age",
        "Cool laboratory tones that make older colonies read more clearly.",
        { 96, 180, 232, 255 },
        { 244, 220, 112, 255 }
    },
    {
        "amber_phosphor",
        "Amber Phosphor",
        "Warm amber phosphor field with soft high-age highlights.",
        { 236, 162, 74, 255 },
        { 255, 226, 170, 255 }
    },
    {
        "green_phosphor",
        "Green Phosphor",
        "Terminal green cells with bright phosphor accents.",
        { 92, 214, 116, 255 },
        { 220, 255, 214, 255 }
    },
    {
        "museum_quiet",
        "Museum Quiet",
        "Low-contrast dark-room presentation for calmer long runs.",
        { 154, 170, 184, 255 },
        { 222, 228, 234, 255 }
    },
    {
        "garden_green",
        "Garden Green",
        "Soft garden phosphor green with quieter bloom highlights.",
        { 110, 188, 104, 255 },
        { 228, 252, 196, 255 }
    },
    {
        "blue_archive",
        "Blue Archive",
        "Cool archive blues with pale specimen highlights for calmer study.",
        { 112, 150, 204, 255 },
        { 228, 238, 255, 255 }
    }
};

const screensave_theme_descriptor *lifeforms_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_lifeforms_themes) / sizeof(g_lifeforms_themes[0]));
    }

    return g_lifeforms_themes;
}

const screensave_theme_descriptor *lifeforms_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = lifeforms_get_themes(&theme_count);
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
