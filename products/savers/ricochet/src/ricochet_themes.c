#include "ricochet_internal.h"

const screensave_theme_descriptor g_ricochet_themes[] = {
    {
        "classic_clean",
        "Classic Clean",
        "Bright neutral motion with a cool accent edge.",
        { 232, 232, 228, 255 },
        { 80, 180, 255, 255 }
    },
    {
        "phosphor_green",
        "Phosphor Green",
        "Muted green motion for longer phosphor-style runs.",
        { 32, 180, 112, 255 },
        { 64, 255, 156, 255 }
    },
    {
        "quiet_modern",
        "Quiet Modern",
        "Cool blue-gray motion with restrained contrast.",
        { 112, 164, 212, 255 },
        { 56, 108, 168, 255 }
    },
    {
        "minimal_slate",
        "Minimal Slate",
        "Low-contrast neutral presentation for lobby or office use.",
        { 164, 168, 176, 255 },
        { 92, 104, 120, 255 }
    }
};

const screensave_theme_descriptor *ricochet_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_ricochet_themes) / sizeof(g_ricochet_themes[0]));
    }

    return g_ricochet_themes;
}

const screensave_theme_descriptor *ricochet_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;

    return screensave_find_theme(ricochet_get_themes(&theme_count), theme_count, theme_key);
}
