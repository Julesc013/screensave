#include <string.h>

#include "ecosystems_internal.h"

const screensave_theme_descriptor g_ecosystems_themes[] = {
    {
        "aquarium_current",
        "Aquarium Current",
        "Cool blue water with pale aquatic highlights.",
        { 84, 164, 208, 255 },
        { 222, 246, 255, 255 }
    },
    {
        "aviary_silhouettes",
        "Aviary Silhouettes",
        "Soft sky tones with dark silhouette accents.",
        { 148, 164, 188, 255 },
        { 236, 228, 206, 255 }
    },
    {
        "dusk_fireflies",
        "Dusk Fireflies",
        "Warm dusk tones with bright firefly pulses.",
        { 212, 176, 96, 255 },
        { 255, 236, 152, 255 }
    },
    {
        "wetland_reeds",
        "Wetland Reeds",
        "Muted wetland greens with pale reed and insect highlights.",
        { 104, 140, 92, 255 },
        { 214, 224, 168, 255 }
    },
    {
        "dark_oceanic",
        "Dark Oceanic",
        "Low-light oceanic palette for subdued aquarium presentation.",
        { 72, 120, 162, 255 },
        { 170, 212, 242, 255 }
    },
    {
        "reef_current",
        "Reef Current",
        "Reef blues with brighter coral-like schooling highlights.",
        { 74, 172, 190, 255 },
        { 255, 220, 170, 255 }
    },
    {
        "night_marsh",
        "Night Marsh",
        "Dark marsh greens with pale firefly and reed highlights.",
        { 98, 126, 86, 255 },
        { 222, 234, 168, 255 }
    }
};

const screensave_theme_descriptor *ecosystems_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_ecosystems_themes) / sizeof(g_ecosystems_themes[0]));
    }

    return g_ecosystems_themes;
}

const screensave_theme_descriptor *ecosystems_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = ecosystems_get_themes(&theme_count);
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
