#include <string.h>

#include "transit_internal.h"

const screensave_theme_descriptor g_transit_themes[] = {
    {
        "wet_motorway",
        "Wet Motorway",
        "Cool roadway tones with restrained headlight and rain-sheen reflections.",
        { 88, 140, 198, 255 },
        { 210, 228, 248, 255 }
    },
    {
        "freight_night_run",
        "Freight Night Run",
        "Muted rail-corridor blues with warm distant signal lights.",
        { 130, 150, 182, 255 },
        { 238, 194, 122, 255 }
    },
    {
        "neon_tunnel",
        "Neon Tunnel",
        "Cool magenta-blue tunnel wash with brighter trim lighting.",
        { 104, 130, 214, 255 },
        { 238, 122, 212, 255 }
    },
    {
        "harbor_lights",
        "Harbor Lights",
        "Marine blue-black atmosphere with dock-light reflections and channel glow.",
        { 72, 148, 196, 255 },
        { 238, 212, 142, 255 }
    },
    {
        "quiet_industrial_edge",
        "Quiet Industrial Edge",
        "Low-intensity industrial corridor with sparse sodium and slate lighting.",
        { 118, 136, 156, 255 },
        { 210, 188, 136, 255 }
    },
    {
        "motorway_fog",
        "Motorway Fog",
        "Cool motorway mist with muted lane lights and softer white-blue reflections.",
        { 104, 134, 164, 255 },
        { 214, 226, 234, 255 }
    },
    {
        "harbor_midnight",
        "Harbor Midnight",
        "Deeper marine midnight palette with restrained amber channel lights.",
        { 78, 112, 150, 255 },
        { 216, 184, 118, 255 }
    }
};

const screensave_theme_descriptor *transit_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_transit_themes) / sizeof(g_transit_themes[0]));
    }

    return g_transit_themes;
}

const screensave_theme_descriptor *transit_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = transit_get_themes(&theme_count);
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
