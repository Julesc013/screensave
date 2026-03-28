#include "nocturne_internal.h"

const screensave_theme_descriptor g_nocturne_themes[] = {
    {
        "pure_black",
        "Pure Black",
        "An intentionally blank black-room profile with no visible accent.",
        { 0, 0, 0, 255 },
        { 0, 0, 0, 255 }
    },
    {
        "amber_black",
        "Amber Black",
        "Dim amber on a black field.",
        { 10, 6, 0, 255 },
        { 26, 16, 0, 255 }
    },
    {
        "green_black",
        "Green Black",
        "Muted phosphor green with a dark-room floor.",
        { 0, 10, 4, 255 },
        { 0, 24, 10, 255 }
    },
    {
        "blue_black",
        "Blue Black",
        "Cool blue for quiet night viewing.",
        { 0, 6, 10, 255 },
        { 0, 16, 28, 255 }
    },
    {
        "gray_black",
        "Gray Black",
        "Neutral near-black museum lighting.",
        { 8, 8, 8, 255 },
        { 20, 20, 20, 255 }
    }
};

const screensave_theme_descriptor *nocturne_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_nocturne_themes) / sizeof(g_nocturne_themes[0]));
    }

    return g_nocturne_themes;
}

const screensave_theme_descriptor *nocturne_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;

    themes = nocturne_get_themes(&theme_count);
    return screensave_find_theme(themes, theme_count, theme_key);
}
