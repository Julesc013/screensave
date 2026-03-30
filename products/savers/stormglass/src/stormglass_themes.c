#include <string.h>

#include "stormglass_internal.h"

const screensave_theme_descriptor g_stormglass_themes[] = {
    {
        "blue_storm",
        "Blue Storm",
        "Cold deep-blue window tones with pale storm flashes and slate light bleed.",
        { 84, 132, 194, 255 },
        { 214, 238, 255, 255 }
    },
    {
        "sodium_vapor_city",
        "Sodium-Vapor City",
        "Warm amber urban window glow with distant street and signal lights.",
        { 218, 150, 70, 255 },
        { 255, 232, 178, 255 }
    },
    {
        "winter_pane",
        "Winter Pane",
        "Frosted blue-grey glass with pale cold-weather highlights.",
        { 166, 196, 222, 255 },
        { 245, 250, 255, 255 }
    },
    {
        "quiet_midnight_rain",
        "Quiet Midnight Rain",
        "Low-contrast midnight glass with restrained cool reflections.",
        { 102, 140, 186, 255 },
        { 178, 208, 236, 255 }
    },
    {
        "museum_weather",
        "Museum Weather",
        "Monochrome weather study with subdued pane and silhouette treatment.",
        { 152, 156, 162, 255 },
        { 228, 232, 236, 255 }
    },
    {
        "winter_streetlamp",
        "Winter Streetlamp",
        "Cold slate pane with warmer sodium streetlamp bleed behind the frost.",
        { 170, 184, 210, 255 },
        { 244, 206, 148, 255 }
    },
    {
        "monochrome_cold_pane",
        "Monochrome Cold Pane",
        "Near-black pane tones with pale graphite highlights and low-contrast weather motion.",
        { 134, 140, 148, 255 },
        { 214, 220, 228, 255 }
    }
};

const screensave_theme_descriptor *stormglass_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_stormglass_themes) / sizeof(g_stormglass_themes[0]));
    }

    return g_stormglass_themes;
}

const screensave_theme_descriptor *stormglass_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = stormglass_get_themes(&theme_count);
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
