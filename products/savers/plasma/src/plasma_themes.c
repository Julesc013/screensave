#include "plasma_internal.h"

const screensave_theme_descriptor g_plasma_themes[] = {
    {
        "plasma_lava",
        "Plasma Lava",
        "Hotter ember reds with a brighter orange crest and a darker black floor.",
        { 204, 52, 12, 255 },
        { 255, 174, 48, 255 }
    },
    {
        "aurora_cool",
        "Aurora Cool",
        "Cold cyan foundations with sharper turquoise energy and a cleaner mint crest.",
        { 18, 124, 166, 255 },
        { 104, 248, 214, 255 }
    },
    {
        "oceanic_blue",
        "Oceanic Blue",
        "Deeper cobalt interference tones with brighter glacial crest highlights.",
        { 12, 54, 142, 255 },
        { 104, 198, 252, 255 }
    },
    {
        "museum_phosphor",
        "Museum Phosphor",
        "Measured green phosphor tuned for monochrome long-run presentation without washout.",
        { 20, 132, 44, 255 },
        { 172, 255, 150, 255 }
    },
    {
        "quiet_darkroom",
        "Quiet Darkroom",
        "Dark cinder warmth with lower-contrast highlights and calmer dark-room separation.",
        { 68, 30, 22, 255 },
        { 132, 70, 48, 255 }
    },
    {
        "midnight_interference",
        "Midnight Interference",
        "Blue-black interference tones with colder electric lift and less ocean overlap.",
        { 8, 24, 76, 255 },
        { 92, 170, 246, 255 }
    },
    {
        "amber_terminal",
        "Amber Terminal",
        "Amber phosphor warmth with darker brass foundations and bounded terminal crest highlights.",
        { 86, 64, 16, 255 },
        { 216, 168, 46, 255 }
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
