#include <string.h>

#include "city_internal.h"

const screensave_theme_descriptor g_city_themes[] = {
    {
        "quiet_city_sleep",
        "Quiet City Sleep",
        "Deep blue-black city night with subdued amber windows and low motion.",
        { 60, 72, 118, 255 },
        { 200, 168, 236, 255 }
    },
    {
        "skyline_vista",
        "Skyline Vista",
        "Tall skyline silhouettes with clean indigo light and muted glass reflections.",
        { 74, 100, 160, 255 },
        { 232, 180, 110, 255 }
    },
    {
        "harbor_sodium_night",
        "Harbor Sodium Night",
        "Warm sodium harbor light against cool water and steel-blue tower outlines.",
        { 116, 126, 146, 255 },
        { 236, 176, 86, 255 }
    },
    {
        "wet_rooftop",
        "Wet Rooftop",
        "Rain-sheened rooftop blues with reflective cyan and window-glow accents.",
        { 66, 106, 168, 255 },
        { 120, 210, 228, 255 }
    },
    {
        "industrial_port",
        "Industrial Port",
        "Muted industrial night palette with steel gray structures and warm dock lights.",
        { 114, 124, 138, 255 },
        { 226, 192, 110, 255 }
    }
};

const screensave_theme_descriptor *city_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_city_themes) / sizeof(g_city_themes[0]));
    }

    return g_city_themes;
}

const screensave_theme_descriptor *city_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = city_get_themes(&theme_count);
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
