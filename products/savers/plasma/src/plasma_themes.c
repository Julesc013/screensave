#include <string.h>

#include "plasma_internal.h"

static const char *plasma_canonical_theme_key(const char *theme_key)
{
    if (theme_key != NULL && lstrcmpiA(theme_key, "ember_lava") == 0) {
        return "plasma_lava";
    }

    return theme_key;
}

const screensave_theme_descriptor g_plasma_themes[] = {
    {
        "plasma_lava",
        "Plasma Lava",
        "Warm plasma reds with amber highlights.",
        { 192, 74, 20, 255 },
        { 255, 194, 72, 255 }
    },
    {
        "aurora_cool",
        "Aurora Cool",
        "Cool cyan and green plasma tones.",
        { 36, 124, 164, 255 },
        { 108, 236, 188, 255 }
    },
    {
        "oceanic_blue",
        "Oceanic Blue",
        "Deep blue interference tones with pale highlights.",
        { 24, 72, 164, 255 },
        { 120, 204, 255, 255 }
    },
    {
        "museum_phosphor",
        "Museum Phosphor",
        "Restrained green monochrome for terminal-like presentation.",
        { 44, 148, 64, 255 },
        { 180, 255, 170, 255 }
    },
    {
        "quiet_darkroom",
        "Quiet Darkroom",
        "Muted plasma tones for low-distraction rooms.",
        { 112, 58, 40, 255 },
        { 204, 132, 96, 255 }
    }
};

const screensave_theme_descriptor *plasma_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_plasma_themes) / sizeof(g_plasma_themes[0]));
    }

    return g_plasma_themes;
}

const screensave_theme_descriptor *plasma_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = plasma_get_themes(&theme_count);
    theme_key = plasma_canonical_theme_key(theme_key);
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
