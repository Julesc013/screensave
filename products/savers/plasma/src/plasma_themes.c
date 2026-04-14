#include "plasma_internal.h"

const screensave_theme_descriptor g_plasma_themes[] = {
    {
        "plasma_lava",
        "Plasma Lava",
        "Classic lava reds with brighter amber lift and a deeper black floor.",
        { 184, 60, 18, 255 },
        { 255, 214, 86, 255 }
    },
    {
        "aurora_cool",
        "Aurora Cool",
        "Cold cyan foundations with brighter mint edge energy.",
        { 28, 132, 156, 255 },
        { 90, 248, 184, 255 }
    },
    {
        "oceanic_blue",
        "Oceanic Blue",
        "Deeper blue interference tones with glacial crest highlights.",
        { 18, 66, 154, 255 },
        { 132, 220, 255, 255 }
    },
    {
        "museum_phosphor",
        "Museum Phosphor",
        "Restrained green phosphor tuned for monochrome long-run presentation.",
        { 32, 126, 54, 255 },
        { 188, 255, 158, 255 }
    },
    {
        "quiet_darkroom",
        "Quiet Darkroom",
        "Dark cinder warmth with lower-contrast highlights for quiet rooms.",
        { 84, 42, 30, 255 },
        { 176, 96, 72, 255 }
    },
    {
        "midnight_interference",
        "Midnight Interference",
        "Blue-black interference tones with colder electric lift.",
        { 16, 34, 92, 255 },
        { 120, 188, 255, 255 }
    },
    {
        "amber_terminal",
        "Amber Terminal",
        "Amber phosphor warmth with brighter terminal-style crest highlights.",
        { 146, 88, 30, 255 },
        { 255, 188, 88, 255 }
    }
};

const screensave_theme_descriptor *plasma_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = plasma_theme_count();
    }

    return g_plasma_themes;
}

unsigned int plasma_theme_count(void)
{
    return (unsigned int)(sizeof(g_plasma_themes) / sizeof(g_plasma_themes[0]));
}

unsigned int plasma_classic_theme_count(void)
{
    return plasma_theme_count();
}

int plasma_is_known_theme_key(const char *theme_key)
{
    return plasma_find_theme_descriptor(theme_key) != NULL;
}

int plasma_classic_is_known_theme_key(const char *theme_key)
{
    return plasma_is_known_theme_key(theme_key);
}

const screensave_theme_descriptor *plasma_find_theme_descriptor(const char *theme_key)
{
    const plasma_content_theme_entry *entry;

    entry = plasma_content_find_theme_entry(theme_key);
    return entry != NULL ? entry->descriptor : NULL;
}
