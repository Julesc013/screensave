#include "deepfield_internal.h"

const screensave_theme_descriptor g_deepfield_themes[] = {
    {
        "quiet_observatory",
        "Quiet Observatory",
        "Neutral star tones with a gentle cool accent.",
        { 196, 204, 220, 255 },
        { 96, 132, 184, 255 }
    },
    {
        "warp_travel",
        "Warp Travel",
        "Bright white-blue travel tones for restrained acceleration scenes.",
        { 224, 232, 255, 255 },
        { 92, 172, 255, 255 }
    },
    {
        "phosphor_space",
        "Phosphor Space",
        "Monochrome green depth cueing with a terminal-inspired feel.",
        { 88, 220, 156, 255 },
        { 24, 120, 88, 255 }
    },
    {
        "blue_vault",
        "Blue Vault",
        "Deep blue-black star tones for cool fly-through runs.",
        { 132, 164, 224, 255 },
        { 44, 88, 164, 255 }
    },
    {
        "museum_terminal",
        "Museum Terminal",
        "Muted gray-green presentation tuned for quiet exhibits.",
        { 168, 176, 168, 255 },
        { 88, 108, 96, 255 }
    },
    {
        "deep_midnight",
        "Deep Midnight",
        "Deep blue-black star tones tuned for long dark-room runs and quieter travel.",
        { 120, 148, 216, 255 },
        { 52, 88, 168, 255 }
    }
};

const screensave_theme_descriptor *deepfield_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_deepfield_themes) / sizeof(g_deepfield_themes[0]));
    }

    return g_deepfield_themes;
}

const screensave_theme_descriptor *deepfield_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;

    themes = deepfield_get_themes(&theme_count);
    return screensave_find_theme(themes, theme_count, theme_key);
}
