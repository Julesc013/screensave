#include <string.h>

#include "atlas_internal.h"

const screensave_theme_descriptor g_atlas_themes[] = {
    {
        "atlas_monochrome",
        "Atlas Monochrome",
        "Neutral atlas tones with paper-white highlights.",
        { 124, 124, 132, 255 },
        { 236, 236, 240, 255 }
    },
    {
        "liquid_nebula",
        "Liquid Nebula",
        "Blue-magenta palette for richer voyage presentations.",
        { 70, 84, 190, 255 },
        { 248, 92, 218, 255 }
    },
    {
        "terminal_mathematics",
        "Terminal Mathematics",
        "Green-on-black mathematics room palette.",
        { 42, 148, 72, 255 },
        { 176, 255, 168, 255 }
    },
    {
        "deep_ultraviolet",
        "Deep Ultraviolet",
        "Dark violet structure tones with pale ultraviolet highlights.",
        { 92, 48, 188, 255 },
        { 212, 174, 255, 255 }
    },
    {
        "museum_print_room",
        "Museum Print Room",
        "Muted print-room palette for restrained long runs.",
        { 120, 94, 78, 255 },
        { 222, 208, 190, 255 }
    }
};

const screensave_theme_descriptor *atlas_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_atlas_themes) / sizeof(g_atlas_themes[0]));
    }

    return g_atlas_themes;
}

const screensave_theme_descriptor *atlas_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = atlas_get_themes(&theme_count);
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
